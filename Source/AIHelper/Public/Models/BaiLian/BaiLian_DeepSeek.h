// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HttpFwd.h"
#include "AIType.h"

#include "Models/ModelBase.h"

#include "UObject/NoExportTypes.h"
#include "BaiLian_DeepSeek.generated.h"


class UAIManager;

namespace BaiLian_DeepSeek_R1_API
{
	struct FMessage
	{
		FString content;
		FString reasoning_content;
		FString role;
	};
}

/**
 * 
 */
UCLASS()
class AIHELPER_API UBaiLian_DeepSeek : public UModelBase
{
	GENERATED_BODY()
public:
	virtual FString GetModelType() override;

	// init and reset
	virtual void UseModel() override;
	virtual void UnUseModel() override;
	virtual void Chat(FString Message) override;
	virtual FString GetChatMessage() override;
	virtual FString GetChatReason() override;
	virtual void ResetModel() override;
	virtual TArray<FString> GetSupportModels() override;
	virtual void SetCurrentModel(FString ModelName) override;
	//
	UPROPERTY()
	UAIManager* AIManager = nullptr;

private:
	void Bind_OnRequestProcessComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);
	bool Bind_ResponseBodyReceiveStreamDelegate(void* Ptr, int64 Length);

	FString CacheMessage;
	FString CacheReason;
	FString CurrentModelName;
	TArray<BaiLian_DeepSeek_R1_API::FMessage> MessageList;
	void CF_SaveChatMessage(const FString& Message,FString Role);
	FString CF_GetChatHistoryString() const;
	
};
