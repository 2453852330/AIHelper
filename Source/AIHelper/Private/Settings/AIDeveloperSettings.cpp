// Fill out your copyright notice in the Description page of Project Settings.


#include "Settings/AIDeveloperSettings.h"

FName UAIDeveloperSettings::GetCategoryName() const
{
	return TEXT("AI Model");
}

FName UAIDeveloperSettings::GetContainerName() const
{
	return TEXT("Project");
}

#if WITH_EDITOR

FText UAIDeveloperSettings::GetSectionDescription() const
{
	return FText::FromString(TEXT("config the ai model data"));
}

FText UAIDeveloperSettings::GetSectionText() const
{
	return FText::FromString(TEXT("AI Model Settings"));
}

#endif