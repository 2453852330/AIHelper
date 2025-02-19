// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AIType.h"
#include "AIUtils.generated.h"

class UAIManager;
/**
 * 
 */
UCLASS()
class AIHELPER_API UAIUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// ** base functions
	
	UFUNCTION(BlueprintCallable,Category="AI Utils")
	static void BPL_WriteDataToFile(FString FolderName,FString Data,FString FileName,bool bUseRealTime = true,FString Time = TEXT(""));

	// ** pure functions
	
};
