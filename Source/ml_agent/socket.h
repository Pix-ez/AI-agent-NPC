// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IWebSocket.h"
#include "Misc/Base64.h"
#include "Sound/SoundWave.h"
#include "Sound/SoundBase.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundWaveProcedural.h"

#include "WebSocketsModule.h"
#include "socket.generated.h"


UCLASS()
class ML_AGENT_API Asocket : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	Asocket();
	// Function to send data on websocket
	UFUNCTION(BlueprintCallable, Category = "Network")
	void SendWebSocketMessage(const FString& Message);

	//Function to send audio data to server.
	UFUNCTION(BlueprintCallable, Category = "WebSocket")
	void SendAudioDataBinary(const TArray<float> AudioData);

	UFUNCTION(BlueprintCallable, Category = "WebSocket")
	void SendAudioData(const FString& FilePath);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	FString FileSavePath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WebSocket")
	FString Succes_code= "0";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WebSocket")
	int32 code = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	TArray<uint8> AccumulatedAudioData;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


private:
	TSharedPtr<IWebSocket> WebSocket;
	FString ServerURL;

	void OnWebSocketConnected();
	void OnWebSocketClosed(int32 StatusCode, const FString& Reason, bool bWasClean);
	void OnWebSocketError(const FString& Error);
	void OnWebSocketMessage(const FString& Message);

	void OnWebSocketRawMessage(const void* Data, SIZE_T Length, SIZE_T BytesRemaining);

	void ConnectWebSocket();
	
	void HandleError(const FString& Error);



	// Helper function to read file contents
	bool ReadFile(const FString& FilePath, TArray<uint8>& OutFileData);
};
