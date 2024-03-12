/* Copyright 2024 LeastSquares. All Rights Reserved. */

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DownloadManager.generated.h"


class UDownloadTask;
/**
 * @class UDownloadManager
 * @brief The UDownloadManager class handles downloading tasks.
 *
 * The UDownloadManager class is responsible for managing and executing download tasks. It allows users to add download tasks and get the corresponding task's information.
 */
UCLASS()
class WEBOPERATOR_API UDownloadManager : public UObject
{
	GENERATED_BODY()

public:
	void StartTask(const FString& Name);
	TObjectPtr<const UDownloadTask> AddDownloadTask(const FString& Name, const FString& InUrl);
	TObjectPtr<const UDownloadTask> AddDownloadTask(const FString& Name, int32 ContentSize, const FString& InUrl);
	TObjectPtr<const UDownloadTask> GetCorrespondingTaskChecked(const FString& Name) const;
	TOptional<TObjectPtr<const UDownloadTask>> GetCorrespondingTask(const FString& Name) const;
	void Clear();

private:
	UPROPERTY(Transient)
	TMap<FString, TObjectPtr<UDownloadTask>> ActiveTasks;
};
