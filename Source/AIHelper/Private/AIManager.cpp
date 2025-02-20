// Fill out your copyright notice in the Description page of Project Settings.


#include "AIManager.h"

#include "Models/BaiLian/BaiLian_DeepSeek.h"
#include "Models/BaiLian/BaiLian_TongYiQianWen.h"

void UAIManager::Initialize(FSubsystemCollectionBase& Collection)
{
	StartTime = FDateTime::Now().ToString(TEXT("%Y_%m_%d_%H_%M_%S"));
	if (UBaiLian_DeepSeek* BaiLianDeepSeekR1 = NewObject<UBaiLian_DeepSeek>(this))
	{
		BaiLianDeepSeekR1->AIManager = this;
		AIMaps.Emplace(BAILIAN_DeepSeek_CATEGORY,BaiLianDeepSeekR1);
	}
	if (UBaiLian_TongYiQianWen* TongYi_Omni_Turbo = NewObject<UBaiLian_TongYiQianWen>(this))
	{
		TongYi_Omni_Turbo->AIManager = this;
		AIMaps.Emplace(BAILIAN_TongYiQianWen_CATEGORY,TongYi_Omni_Turbo);
	}
}

void UAIManager::Deinitialize()
{
	AIMaps.Empty();
}

void UAIManager::BP_SetCurrentModelCategory(FString InModelCategory)
{
	// remove old model
	UModelBase* oldModel = AIMaps.FindRef(InModelCategory);
	if (oldModel)
	{
		oldModel->UnUseModel();
	}
	// change model
	CurrentModelCategory = InModelCategory;
	// join new model
	UModelBase* newModel = AIMaps.FindRef(CurrentModelCategory);
	if (newModel)
	{
		newModel->UseModel();
	}
}

void UAIManager::BP_SetCurrentModelName(FString InModelName)
{
	UModelBase* newModel = AIMaps.FindRef(CurrentModelCategory);
	if (newModel)
	{
		CurrentModelName = InModelName;
		newModel->SetCurrentModel(InModelName);
	}
}

void UAIManager::BP_Chat(FString Message)
{
	UModelBase* currentModel = AIMaps.FindRef(CurrentModelCategory);
	if (currentModel)
	{
		currentModel->Chat(Message);
	}
}

FString UAIManager::BP_GetChatMessage()
{
	UModelBase* currentModel = AIMaps.FindRef(CurrentModelCategory);
	if (currentModel)
	{
		return currentModel->GetChatMessage();
	}
	return TEXT("Cant Find This Model");
}

FString UAIManager::BP_GetChatReason()
{
	UModelBase* currentModel = AIMaps.FindRef(CurrentModelCategory);
	if (currentModel)
	{
		return currentModel->GetChatReason();
	}
	return TEXT("Cant Find This Model");
}

TArray<FString> UAIManager::BP_GetAllModelCategory()
{
	TArray<FString> Tmp;
	AIMaps.GetKeys(Tmp);
	return Tmp;
}

TArray<FString> UAIManager::BP_GetAllModelName()
{
	UModelBase* currentModel = AIMaps.FindRef(CurrentModelCategory);
	if (currentModel)
	{
		return currentModel->GetSupportModels();
	}
	return {};
}
