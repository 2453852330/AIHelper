// Fill out your copyright notice in the Description page of Project Settings.


#include "Lib/AIUtils.h"

#include "AIManager.h"

void UAIUtils::BPL_WriteDataToFile(FString FolderName, FString Data, FString FileName, bool bUseRealTime, FString Time)
{
	FString WriteName = ( bUseRealTime ? ( FileName +  FDateTime::Now().ToString(TEXT("%Y_%m_%d_%H_%M_%S"))) : FileName + TEXT("_") +
	Time ) + TEXT(".txt");
	FString WritePath = FPaths::Combine(FPaths::ProjectSavedDir(),FolderName,WriteName);;
	FFileHelper::SaveStringToFile(Data,*WritePath,FFileHelper::EEncodingOptions::AutoDetect,&IFileManager::Get(),FILEWRITE_Append);
}


