// Fill out your copyright notice in the Description page of Project Settings.


#include "Models/BaiLian/BaiLian_DeepSeek.h"

#include "AIManager.h"
#include "HttpModule.h"
#include "LogHelper.h"

#include "Settings/AIDeveloperSettings.h"

FString UBaiLian_DeepSeek::GetModelType()
{
	return TEXT("Chat");
}

void UBaiLian_DeepSeek::UseModel()
{
	LogDefault("Use BaiLianDeepSeekR1 Model")
}

void UBaiLian_DeepSeek::UnUseModel()
{
	LogDefault("UnUse BaiLianDeepSeekR1 Model")
}

void UBaiLian_DeepSeek::Chat(FString Message)
{
	LogDefault("Start Chat")
	CacheMessage = TEXT("");
	CacheReason = TEXT("");
	CF_SaveChatMessage(Message,TEXT("user"));

	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(TEXT("https://dashscope.aliyuncs.com/compatible-mode/v1/chat/completions"));
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetHeader(TEXT("Authorization"), TEXT(" Bearer ") + GetDefault<UAIDeveloperSettings>()->BaiLianAPIKey);
	FString Front_M = R"({ "model": ")";
	FString Back_M = R"(", "messages": [)";
	FString Front = Front_M + CurrentModelName + Back_M;
	FString Back = R"(], "stream": true, "stream_options": { "include_usage": true } })";
	FString Body = Front + CF_GetChatHistoryString() + Back;
	LogDefault("body:%s",*Body)
	Request->SetContentAsString(Body);
	Request->SetTimeout(120);
	Request->OnProcessRequestComplete().BindUObject(this, &UBaiLian_DeepSeek::Bind_OnRequestProcessComplete);
	Request->SetResponseBodyReceiveStreamDelegate(FHttpRequestStreamDelegate::CreateUObject(this,&UBaiLian_DeepSeek::Bind_ResponseBodyReceiveStreamDelegate));
	Request->ProcessRequest();
}

FString UBaiLian_DeepSeek::GetChatMessage()
{
	return CacheMessage;
}

FString UBaiLian_DeepSeek::GetChatReason()
{
	return CacheReason;
}

void UBaiLian_DeepSeek::ResetModel()
{
	CacheMessage = TEXT("");
	CacheReason = TEXT("");
	MessageList.Empty();
}

TArray<FString> UBaiLian_DeepSeek::GetSupportModels()
{
	return {TEXT("deepseek-r1"),TEXT("deepseek-v3"),TEXT("deepseek-r1-distill-qwen-1.5b"),TEXT("deepseek-r1-distill-qwen-7b"),
		TEXT("deepseek-r1-distill-qwen-14b"),TEXT("deepseek-r1-distill-qwen-32b"),TEXT("deepseek-r1-distill-llama-8b"),TEXT("deepseek-r1-distill-llama-70b")
	};
}

void UBaiLian_DeepSeek::SetCurrentModel(FString ModelName)
{
	ResetModel();
	CurrentModelName = ModelName;
}

void UBaiLian_DeepSeek::Bind_OnRequestProcessComplete(FHttpRequestPtr Request, FHttpResponsePtr Response,bool bConnectedSuccessfully)
{
	if (!bConnectedSuccessfully)
	{
		LogWarning("request failed")
		AIManager->OnModelError.Broadcast(TEXT("Request Failed"));
		return;
	}
	// LogDefault("%s",*Response->GetContentAsString())
	CF_SaveChatMessage(CacheMessage,TEXT("assistant"));

	LogDefault("reasoning_content:%s",*CacheReason)
	LogDefault("content:%s",*CacheMessage)
}

bool UBaiLian_DeepSeek::Bind_ResponseBodyReceiveStreamDelegate(void* Ptr, int64 Length)
{
	uint8* data = reinterpret_cast<uint8*>(Ptr);
	FString ReceiveString = FString(Length,UTF8_TO_TCHAR(data));
	// data: xxxxx  | data:[DONE]
	// LogWarning("streamData:%s",*ReceiveString)
	
	ReceiveString.ReplaceInline(TEXT("\n"),TEXT(""));
	ReceiveString.ReplaceInline(TEXT("\r"),TEXT(""));
	TArray<FString> dataArray;
	ReceiveString.ParseIntoArray(dataArray,TEXT("data:"));
	
	for (FString  it : dataArray)
	{
		// LogWarning("ParseIntoArray's data:%s",*it)

		if (it == TEXT(" [DONE]"))
		{
			continue;			
		}
		// read json data
		TSharedPtr<FJsonObject> JsonRoot;
		TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(it);
		if (FJsonSerializer::Deserialize(JsonReader, JsonRoot))
		{
			FString id = FString::FromInt( JsonRoot->GetNumberField(TEXT("created")));
			auto choicesObj = JsonRoot->GetArrayField(TEXT("choices"));
			if (choicesObj.Num())
			{
				auto deltaObj = choicesObj[0]->AsObject()->GetObjectField(TEXT("delta"));
				FString content;
				FString reason;
				if (deltaObj->TryGetStringField(TEXT("content"),content))
				{
					CacheMessage += content;
					AsyncTask(ENamedThreads::Type::GameThread,[DSR1 = TWeakObjectPtr<UBaiLian_DeepSeek>(this),id]()
					{
						if (DSR1.IsValid() && DSR1->AIManager)
						{
							DSR1->AIManager->OnChatFinish.Broadcast(true,id);
						}
					});
				}
				if (deltaObj->TryGetStringField(TEXT("reasoning_content"),reason))
				{
					CacheReason += reason;
					AsyncTask(ENamedThreads::Type::GameThread,[DSR1 = TWeakObjectPtr<UBaiLian_DeepSeek>(this),id]()
					{
						if (DSR1.IsValid() && DSR1->AIManager)
						{
							DSR1->AIManager->OnChatReason.Broadcast(true,id);
						}
					});
				}
			}
		}
		else
		{
			LogDefault("%s",*it)
			LogError("Deserialize Json Failed")
		}
	}
	
	return true;
}

void UBaiLian_DeepSeek::CF_SaveChatMessage(const FString& Message, FString Role)
{
	BaiLian_DeepSeek_R1_API::FMessage Tmp;
	Tmp.content = Message;
	Tmp.role = Role;
	MessageList.Emplace(Tmp);
}

FString UBaiLian_DeepSeek::CF_GetChatHistoryString() const
{
	FString Tmp;
	// { "role": "user", "content": "你好" }
	for (const BaiLian_DeepSeek_R1_API::FMessage & it : MessageList)
	{
		Tmp += R"({ "role": ")";
		Tmp += it.role;
		Tmp += R"(", "content": ")";
		Tmp += it.content;
		Tmp += R"(" },)";
	}
	// remove the ',' on the end
	Tmp.RemoveFromEnd(TEXT(","));
	return Tmp;
}
