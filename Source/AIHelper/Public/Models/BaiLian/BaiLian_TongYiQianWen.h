// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIType.h"
#include "HttpFwd.h"
#include "Models/ModelBase.h"
#include "BaiLian_TongYiQianWen.generated.h"

class UAIManager;

namespace TongYi_Omni_Turbo_API
{
	struct FContent
	{
		FString type;
		FString text;
	};

	struct FMessage
	{
		FString role;
		FContent content;
	};
}



UCLASS()
class AIHELPER_API UBaiLian_TongYiQianWen : public UModelBase
{
	GENERATED_BODY()
public:
	virtual FString GetModelType() override;
	virtual void UseModel() override;
	virtual void UnUseModel() override;
	virtual void Chat(FString Message) override;
	virtual FString GetChatMessage() override;
	virtual void ResetModel() override;
	virtual TArray<FString> GetSupportModels() override;
	virtual void SetCurrentModel(FString ModelName) override;

	void Bind_OnRequestProgress64(FHttpRequestPtr Request, uint64 BytesSent, uint64 BytesReceived);
	void Bind_OnRequestProcessComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);
	bool Bind_ResponseBodyReceiveStreamDelegate(void* Ptr, int64 Length);
	//
	UPROPERTY()
	UAIManager* AIManager = nullptr;

private:
	
	FString CacheMessage;
	FString CurrentUseModel;
	
	TArray<TongYi_Omni_Turbo_API::FMessage> SavedMessageList;
	void CF_SaveChatMessage(const FString& Message,FString Role);
	FString CF_GetChatHistoryString() const;
};
