// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "AIDeveloperSettings.generated.h"

/**
 * 
 */
UCLASS(Config=Game,DefaultConfig)
class AIHELPER_API UAIDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	virtual FName GetCategoryName() const override;
	virtual FName GetContainerName() const override;

	// sk-f90ccdea899147bcb2039cd84d20eaf2
	UPROPERTY(Config,EditAnywhere,Category="ALi|BaiLian")
	FString BaiLianAPIKey;

	
	
#if WITH_EDITOR
	virtual FText GetSectionDescription() const override;
	virtual FText GetSectionText() const override;
#endif
};
