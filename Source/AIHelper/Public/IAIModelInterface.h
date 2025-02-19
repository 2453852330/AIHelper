#pragma once

class IAIModelInterface
{
public:
	virtual ~IAIModelInterface() {}

	virtual FString GetModelType() = 0;
	virtual void UseModel() = 0;
	virtual void UnUseModel() = 0;
	virtual void Chat(FString Message) = 0;
	virtual FString GetChatMessage() = 0;
	virtual FString GetChatReason() = 0;
	virtual void ResetModel() = 0;
};
