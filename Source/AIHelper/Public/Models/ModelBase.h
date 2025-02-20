// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IAIModelInterface.h"

#include "UObject/Object.h"
#include "ModelBase.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class AIHELPER_API UModelBase : public UObject, public IAIModelInterface
{
	GENERATED_BODY()
public:
	virtual FString GetModelType() override{return TEXT("ModelBase"); };
	virtual void UseModel() override{};
	virtual void UnUseModel() override{};
	virtual void Chat(FString Message) override{};
	virtual FString GetChatMessage() override{ return TEXT(""); };
	virtual void ResetModel() override{};
	virtual FString GetChatReason() override{return TEXT(""); };
	virtual TArray<FString> GetSupportModels() override{return {};};
	virtual void SetCurrentModel(FString ModelName) override {};
};
