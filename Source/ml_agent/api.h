// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Http.h"
#include "api.generated.h"

UCLASS()
class ML_AGENT_API Aapi : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	Aapi();

	// Function to send audio file to server
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void SendAudioFileToServer(const FString& FilePath);

	// Variable to store server response option
	UPROPERTY(BlueprintReadWrite, Category = "Audio")
	int32 agentOption;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	// HTTP request callback
	void OnRequestCompleted(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSuccess);

	// Helper function to read file contents
	bool ReadFile(const FString& FilePath, TArray<uint8>& OutFileData);

	FHttpModule* Http;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
