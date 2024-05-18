// Fill out your copyright notice in the Description page of Project Settings.


#include "api.h"

// Sets default values
Aapi::Aapi()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Http = &FHttpModule::Get();
	agentOption = -1; // Initialize agentOption to a default value

}

void Aapi::SendAudioFileToServer(const FString& FilePath)
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


	// Create HTTP request
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb("POST");
	HttpRequest->SetURL("http://127.0.0.1:5002/process_audio");
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/octet-stream"));
	HttpRequest->SetContent(AudioData);



	// Set up callback function
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &Aapi::OnRequestCompleted);

	// Send request
	UE_LOG(LogTemp, Log, TEXT("Sending HTTP request to server"));
	HttpRequest->ProcessRequest();
}

// Called when the game starts or when spawned
void Aapi::BeginPlay()
{
	Super::BeginPlay();
	
}

void Aapi::OnRequestCompleted(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSuccess)
{
	if (bSuccess && HttpResponse.IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("Request succeeded with response code: %d"), HttpResponse->GetResponseCode());
		FString ResponseContent = HttpResponse->GetContentAsString();
		UE_LOG(LogTemp, Log, TEXT("Response content: %s"), *ResponseContent);

		// Parse JSON response
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);
		if (FJsonSerializer::Deserialize(Reader, JsonObject))
		{
			// Get the value of the "option" key and set it to agentOption variable
			if (JsonObject->HasField("option") && JsonObject->TryGetNumberField("option", agentOption))
			{
				UE_LOG(LogTemp, Log, TEXT("agentOption set to: %d"), agentOption);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to get 'option' field from JSON"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to deserialize JSON response"));
		}
	}
	else
	{
		FString ErrorMsg = bSuccess ? TEXT("Server response is invalid") : TEXT("Request failed");
		UE_LOG(LogTemp, Error, TEXT("%s"), *ErrorMsg);
	}
}

bool Aapi::ReadFile(const FString& FilePath, TArray<uint8>& OutFileData)
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

// Called every frame
void Aapi::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

