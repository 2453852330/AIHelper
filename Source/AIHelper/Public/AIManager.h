// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AIManager.generated.h"


struct FChatResponseData;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnChatFinish, bool , bStream, const FString & , CreateId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnChatReason, bool , bStream, const FString & , CreateId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnModelError,const FString & ,ErrorMessage);

#define BAILIAN_DeepSeek_CATEGORY TEXT("MODEL_DEEPSEEK_CATEGORY")
#define BAILIAN_TongYiQianWen_CATEGORY TEXT("MODEL_TYQW_CATEGORY")

class UModelBase;

UCLASS()
class AIHELPER_API UAIManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	FString StartTime;
	FString CurrentModelName;
	FString CurrentModelCategory;
	
	UPROPERTY()
	TMap<FString,UModelBase*> AIMaps;

	UFUNCTION(BlueprintCallable,Category="AIManager")
	void BP_SetCurrentModelCategory(FString InModelCategory);
	UFUNCTION(BlueprintCallable,Category="AIManager")
	void BP_SetCurrentModelName(FString InModelName);
	UFUNCTION(BlueprintCallable,Category="AIManager")
	void BP_Chat(FString Message);	
	UFUNCTION(BlueprintPure,Category="AIManager")
	FString BP_GetChatMessage();
	UFUNCTION(BlueprintPure,Category="AIManager")
	FString BP_GetChatReason();
	UFUNCTION(BlueprintPure,Category="AIManager")
	TArray<FString> BP_GetAllModelCategory();
	UFUNCTION(BlueprintPure,Category="AIManager")
	TArray<FString> BP_GetAllModelName();	
	
	UPROPERTY(BlueprintAssignable,Category="AIManager")
	FOnChatFinish OnChatFinish;
	UPROPERTY(BlueprintAssignable,Category="AIManager")
	FOnModelError OnModelError;
	UPROPERTY(BlueprintAssignable,Category="AIManager")
	FOnChatReason OnChatReason;
};
