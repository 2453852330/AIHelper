// Fill out your copyright notice in the Description page of Project Settings.


#include "Models/BaiLian/BaiLian_TongYiQianWen.h"

#include "AIManager.h"
#include "HttpModule.h"
#include "LogHelper.h"

#include "Settings/AIDeveloperSettings.h"

FString UBaiLian_TongYiQianWen::GetModelType()
{
	return TEXT("Chat|Audio|Pic");
}

void UBaiLian_TongYiQianWen::UseModel()
{
	LogDefault("UTongYi_Omni_Turbo::UseModel");
}

void UBaiLian_TongYiQianWen::UnUseModel()
{
	LogDefault("UTongYi_Omni_Turbo::UnUseModel")
}

void UBaiLian_TongYiQianWen::Chat(FString Message)
{
	LogDefault("Start Chat")
	CacheMessage = TEXT("");
	CF_SaveChatMessage(Message,TEXT("user"));
	
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(TEXT("https://dashscope.aliyuncs.com/compatible-mode/v1/chat/completions"));
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetHeader(TEXT("Authorization"), TEXT(" Bearer ") + GetDefault<UAIDeveloperSettings>()->BaiLianAPIKey);
	FString Model_F = R"({ "model": ")";
	FString Model_B = R"(", "messages": [)";
	FString Front = Model_F + CurrentUseModel + Model_B;
	FString Back = R"(], "stream":true, "modalities":["text"] })";
	// FString Back = R"(" } ], "stream":true, "modalities":["text"], "audio":{"voice":"Cherry","format":"wav"} })";
	FString Body = Front + CF_GetChatHistoryString() + Back;
	// LogDefault("body:%s",*Body)
	Request->SetContentAsString(Body);
	Request->SetTimeout(120);
	Request->OnRequestProgress64().BindUObject(this,&UBaiLian_TongYiQianWen::Bind_OnRequestProgress64);
	Request->OnProcessRequestComplete().BindUObject(this, &UBaiLian_TongYiQianWen::Bind_OnRequestProcessComplete);
	Request->SetResponseBodyReceiveStreamDelegate(FHttpRequestStreamDelegate::CreateUObject(this,&UBaiLian_TongYiQianWen::Bind_ResponseBodyReceiveStreamDelegate));
	Request->ProcessRequest();
}

FString UBaiLian_TongYiQianWen::GetChatMessage()
{
	return CacheMessage;
}

void UBaiLian_TongYiQianWen::ResetModel()
{
	CacheMessage = TEXT("");
	SavedMessageList.Empty();
}

TArray<FString> UBaiLian_TongYiQianWen::GetSupportModels()
{
	return {TEXT("qwen-omni-turbo"),TEXT("qwen-omni-turbo-latest")};
}

void UBaiLian_TongYiQianWen::SetCurrentModel(FString ModelName)
{
	ResetModel();
	CurrentUseModel = ModelName;
}

void UBaiLian_TongYiQianWen::Bind_OnRequestProgress64(FHttpRequestPtr Request, uint64 BytesSent, uint64 BytesReceived)
{
	// LogDefault("BytesSent:%llu BytesReceived:%llu", BytesSent, BytesReceived);
}

void UBaiLian_TongYiQianWen::Bind_OnRequestProcessComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (!bConnectedSuccessfully)
	{
		LogWarning("request failed")
		AIManager->OnModelError.Broadcast(TEXT("Request Failed"));
		return;
	}
	// LogDefault("%s",*Response->GetContentAsString())
	CF_SaveChatMessage(CacheMessage,TEXT("assistant"));
}


bool UBaiLian_TongYiQianWen::Bind_ResponseBodyReceiveStreamDelegate(void* Ptr, int64 Length)
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
				FString content = deltaObj->GetStringField(TEXT("content"));
				CacheMessage += content;
				// LogWarning("CacheMessage:%s | id:%s | content:%s",*CacheMessage,*id,*content)
				AsyncTask(ENamedThreads::Type::GameThread,[Omni_Turbo = TWeakObjectPtr<UBaiLian_TongYiQianWen>(this),id]()
				{
					if (Omni_Turbo.IsValid() && Omni_Turbo->AIManager)
					{
						Omni_Turbo->AIManager->OnChatFinish.Broadcast(true,id);
					}
				});
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


void UBaiLian_TongYiQianWen::CF_SaveChatMessage(const FString& Message, FString Role)
{
	TongYi_Omni_Turbo_API::FMessage Tmp;
	Tmp.role = Role;
	Tmp.content.type = TEXT("text/plain");
	Tmp.content.text = Message;
	SavedMessageList.Emplace(Tmp);
}

FString UBaiLian_TongYiQianWen::CF_GetChatHistoryString() const
{
	FString Tmp;
	// 
	for (const TongYi_Omni_Turbo_API::FMessage & it : SavedMessageList)
	{
		// { "role": "user", "content": [ { "type": "text", "text": "这段音频在说什么" } ] }
		FString data = R"({ "role": ")";
		data += it.role;
		data += R"(", "content": [ { "type": "text", "text": ")";
		data += it.content.text;
		data += R"(" } ] } ,)";

		Tmp += data;
	}
	// remove the ',' on the end
	Tmp.RemoveFromEnd(TEXT(","));
	return Tmp;
}

