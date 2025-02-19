// Fill out your copyright notice in the Description page of Project Settings.


#include "AIManager.h"

#include "Models/BaiLian/BaiLian_DeepSeek_R1.h"
#include "Models/BaiLian/TongYi_Omni_Turbo.h"

void UAIManager::Initialize(FSubsystemCollectionBase& Collection)
{
	StartTime = FDateTime::Now().ToString(TEXT("%Y_%m_%d_%H_%M_%S"));
	if (UBaiLian_DeepSeek_R1* BaiLianDeepSeekR1 = NewObject<UBaiLian_DeepSeek_R1>(this))
	{
		BaiLianDeepSeekR1->AIManager = this;
		AIMaps.Emplace(BAILIAN_DeepSeek_R1,BaiLianDeepSeekR1);
	}
	if (UTongYi_Omni_Turbo* TongYi_Omni_Turbo = NewObject<UTongYi_Omni_Turbo>(this))
	{
		TongYi_Omni_Turbo->AIManager = this;
		AIMaps.Emplace(TONGYI_Omni_Turbo,TongYi_Omni_Turbo);
	}
}

void UAIManager::Deinitialize()
{
	AIMaps.Empty();
}


void UAIManager::BP_SetCurrentModel(FString InModelName)
{
	// remove old model
	UModelBase* oldModel = AIMaps.FindRef(CurrentModelName);
	if (oldModel)
	{
		oldModel->UnUseModel();
	}
	// change model
	CurrentModelName = InModelName;
	// join new model
	UModelBase* newModel = AIMaps.FindRef(CurrentModelName);
	if (newModel)
	{
		newModel->UseModel();
	}
}

void UAIManager::BP_Chat(FString Message)
{
	UModelBase* currentModel = AIMaps.FindRef(CurrentModelName);
	if (currentModel)
	{
		currentModel->Chat(Message);
	}
}

FString UAIManager::BP_GetChatMessage()
{
	UModelBase* currentModel = AIMaps.FindRef(CurrentModelName);
	if (currentModel)
	{
		return currentModel->GetChatMessage();
	}
	return TEXT("Cant Find This Model");
}

FString UAIManager::BP_GetChatReason()
{
	UModelBase* currentModel = AIMaps.FindRef(CurrentModelName);
	if (currentModel)
	{
		return currentModel->GetChatReason();
	}
	return TEXT("Cant Find This Model");
}

TArray<FString> UAIManager::BP_GetAllModels()
{
	TArray<FString> Tmp;
	AIMaps.GetKeys(Tmp);
	return Tmp;
}
