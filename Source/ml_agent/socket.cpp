// Fill out your copyright notice in the Description page of Project Settings.


#include "socket.h"
#include "Sound/SoundWave.h"
#include "Sound/SoundBase.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundWaveProcedural.h"

#include "Misc/Base64.h"
#include "Kismet/GameplayStatics.h"
#include "IWebSocket.h"
// Sets default values
Asocket::Asocket()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Set your Flask SocketIO server URL here
	ServerURL = TEXT("ws://127.0.0.1:6969");

	

	// Connect to WebSocket server
	ConnectWebSocket();

}

void Asocket::OnWebSocketConnected()
{
	UE_LOG(LogTemp, Warning, TEXT("WebSocket connected successfully"));
	
}

void Asocket::OnWebSocketClosed(int32 StatusCode, const FString& Reason, bool bWasClean)
{
	UE_LOG(LogTemp, Warning, TEXT("WebSocket closed: %s"), *Reason);

}

void Asocket::OnWebSocketError(const FString& Error)
{
	UE_LOG(LogTemp, Error, TEXT("WebSocket error: %s"), *Error);
}


void Asocket::OnWebSocketMessage(const FString& Message)
{

  UE_LOG(LogTemp, Warning, TEXT("Succes code: %s"), *Message);
  Succes_code = *Message;
  

	 // Assuming Message contains Base64-encoded audio data as received from the server

	// // Decode Base64-encoded audio data
	//TArray<uint8> DecodedData;
	//bool bSuccess = FBase64::Decode(Message, DecodedData); // Pass Message directly to Decode

	//if (bSuccess)
	//{
	//	// Form the complete file path
	//	FString CompleteFilePath = FileSavePath + TEXT("/AudioFilesave.wav");

	//	// Save the audio data to the complete file path
	//	if (FFileHelper::SaveArrayToFile(DecodedData, *CompleteFilePath))
	//	{
	//		
	//		UE_LOG(LogTemp, Log, TEXT("Audio data saved to file: %s"), *CompleteFilePath);
	//	}
	//	else
	//	{
	//		UE_LOG(LogTemp, Error, TEXT("Failed to save audio data to file: %s"), *CompleteFilePath);
	//	}
	//}
	//else
	//{
	//	// Handle decoding failure
	//	UE_LOG(LogTemp, Error, TEXT("Failed to decode Base64-encoded audio data"));
	//}
}

// Declare AccumulatedAudioData as a member variable of the class to maintain its state across function calls
//TArray<uint8> AccumulatedAudioData;
//void Asocket::OnWebSocketRawMessage(const void* Data, SIZE_T Length, SIZE_T BytesRemaining)
//{
//	
//	// Convert the raw binary data to a TArray<uint8>
//	const uint8* BinaryData = reinterpret_cast<const uint8*>(Data);
//
//	// Append the binary data to the accumulated audio data
//	AccumulatedAudioData.Append(BinaryData, Length);
//
//	// If there are no more bytes remaining, save the accumulated data to the file
//	if (BytesRemaining == 0)
//	{
//		// Form the complete file path
//		FString CompleteFilePath = FileSavePath + TEXT("/AudioFile.wav");
//
//		// Save the accumulated audio data to the complete file path
//		if (FFileHelper::SaveArrayToFile(AccumulatedAudioData, *CompleteFilePath))
//		{
//			UE_LOG(LogTemp, Log, TEXT("Audio data saved to file: %s"), *CompleteFilePath);
//		}
//		else
//		{
//			UE_LOG(LogTemp, Error, TEXT("Failed to save audio data to file: %s"), *CompleteFilePath);
//		}
//
//		// Clear the accumulated audio data for the next transmission
//		AccumulatedAudioData.Empty();
//	}
//}

void Asocket::OnWebSocketRawMessage(const void* Data, SIZE_T Length, SIZE_T BytesRemaining)
{
	

	// Convert the raw binary data to a TArray<uint8>
	const uint8* BinaryData = reinterpret_cast<const uint8*>(Data);

	// Append the binary data to the accumulated audio data
	AccumulatedAudioData.Append(BinaryData, Length);

	//// If there are no more bytes remaining, save the accumulated data to the file
	//if (BytesRemaining == 0)
	//{
	//	// Form the complete file path
	//	FString CompleteFilePath = FileSavePath + TEXT("/AudioFile.wav");

	//	// Save the accumulated audio data to the complete file path
	//	if (FFileHelper::SaveArrayToFile(AccumulatedAudioData, *CompleteFilePath))
	//	{
	//		UE_LOG(LogTemp, Log, TEXT("Audio data saved to file: %s"), *CompleteFilePath);
	//	}
	//	else
	//	{
	//		UE_LOG(LogTemp, Error, TEXT("Failed to save audio data to file: %s"), *CompleteFilePath);
	//	}

	//	
	//}
}




void Asocket::ConnectWebSocket()
{
	WebSocket = FWebSocketsModule::Get().CreateWebSocket(ServerURL);

	if (WebSocket.IsValid())
	{
		// Bind connection event handlers
		WebSocket->OnConnected().AddUObject(this, &Asocket::OnWebSocketConnected);
		WebSocket->OnClosed().AddUObject(this, &Asocket::OnWebSocketClosed);
		
		WebSocket->OnMessage().AddUObject(this, &Asocket::OnWebSocketMessage);
		WebSocket->OnRawMessage().AddUObject(this, &Asocket::OnWebSocketRawMessage);


		// Connect to the WebSocket server
		WebSocket->Connect();
	}
}

void Asocket::SendWebSocketMessage(const FString& Message)
{
	if (WebSocket.IsValid() && WebSocket->IsConnected())
	{
		WebSocket->Send(Message);
	}

}

void Asocket::SendAudioData(const FString& FilePath)
{
	// Read audio file data
	UE_LOG(LogTemp, Log, TEXT("Attempting to read audio file: %s"), *FilePath);
	// Read audio file data
	TArray<uint8> AudioData;
	if (!ReadFile(FilePath, AudioData))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to read audio file: %s"), *FilePath);
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("Audio file read successfully: %s, Size: %d bytes"), *FilePath, AudioData.Num());

	if (WebSocket.IsValid() && WebSocket->IsConnected())
	{
		// Convert binary data to Base64-encoded string
		FString Base64String = FBase64::Encode(AudioData);

		// Send audio data over WebSocket connection
		WebSocket->Send(Base64String);
	}
}

void Asocket::SendAudioDataBinary(const TArray<float> AudioData)
{
	
	UE_LOG(LogTemp, Log, TEXT("Audio Buffer from mic:  Size: %d bytes"),  AudioData.Num());

	if (WebSocket.IsValid() && WebSocket->IsConnected())
	{
		//// Convert TArray<float> to TArray<uint8>
		//TArray<uint8> AudioDataUint8;
		//for (float Sample : AudioData)
		//{
		//	// Scale and convert each float sample to uint8
		//	uint8 SampleUint8 = FMath::Clamp<uint8>(FMath::RoundToInt(Sample * 127.5f + 127.5f), 0, 255);
		//	AudioDataUint8.Add(SampleUint8);
		//}

		//// Encode the TArray<uint8> to Base64
		//FString Base64String = FBase64::Encode(AudioDataUint8);
		UE_LOG(LogTemp, Log, TEXT("send audio success"));

		/*WebSocket->Send(Base64String);*/

		//Sending Raw Audio Binray data to server.
		WebSocket->Send(&AudioData, AudioData.Num(), true);
	}
}

void Asocket::HandleError(const FString& Error)
{

	UE_LOG(LogTemp, Error, TEXT("WebSocket error: %s"), *Error);
}

bool Asocket::ReadFile(const FString& FilePath, TArray<uint8>& OutFileData)
{
	// Read file contents
	if (FFileHelper::LoadFileToArray(OutFileData, *FilePath))
	{
		UE_LOG(LogTemp, Log, TEXT("File read successfully: %s, Size: %d bytes"), *FilePath, OutFileData.Num());
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to read file: %s"), *FilePath);
		return false;
	}
}





// Called when the game starts or when spawned
void Asocket::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void Asocket::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

