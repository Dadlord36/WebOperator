/* Copyright 2024 LeastSquares. All Rights Reserved. */


#include "Managers/DownloadManager.h"
#include "WebOperatorLogCategories.h"

#include "Interfaces/IHttpResponse.h"
#include "Managers/Tasks/DownloadTask.h"


TObjectPtr<const UDownloadTask> UDownloadManager::AddDownloadTask(const FString& Name, const FString& InUrl)
{
	return ActiveTasks.FindOrAdd(Name, UDownloadTask::MakeTask(InUrl,
	                                                           FHttpRequestCompleteMulticast::FDelegate::CreateLambda([Name](
		                                                           TSharedPtr<IHttpRequest>
		                                                           HttpRequest, TSharedPtr<IHttpResponse> HttpResponse, bool Success)
		                                                           {
			                                                           UE_LOG(LogWebOperator, Log,
			                                                                  TEXT("Download task [%s] is finished with code: %i"), *Name,
			                                                                  HttpResponse->GetResponseCode())
		                                                           })));
}

TObjectPtr<const UDownloadTask> UDownloadManager::AddDownloadTask(const FString& Name, const int32 ContentSize, const FString& InUrl)
{
	ensureAlwaysMsgf(ContentSize>0, TEXT("Content size is 0"));
	return ActiveTasks.FindOrAdd(Name, UDownloadTask::MakeTask(InUrl, ContentSize,
	                                                           FHttpRequestCompleteMulticast::FDelegate::CreateLambda([Name](
		                                                           TSharedPtr<IHttpRequest>, TSharedPtr<IHttpResponse> HttpResponse,
		                                                           bool Success)
		                                                           {
			                                                           UE_LOG(LogWebOperator, Log,
			                                                                  TEXT("Download task [%s] is finished with code: %i"), *Name,
			                                                                  HttpResponse->GetResponseCode())
		                                                           })));
}

void UDownloadManager::StartTask(const FString& Name)
{
	ActiveTasks[Name]->Start();
}

TObjectPtr<const UDownloadTask> UDownloadManager::GetCorrespondingTaskChecked(const FString& Name) const
{
	checkf(ActiveTasks.Contains(Name), TEXT("There is no {%s} task registered"), *Name);
	return ActiveTasks[Name];
}

TOptional<TObjectPtr<const UDownloadTask>> UDownloadManager::GetCorrespondingTask(const FString& Name) const
{
	if (ActiveTasks.Contains(Name))
	{
		auto Task = const_cast<const UDownloadTask*>(ActiveTasks[Name].Get());
		return {Task};
	}
	return {};
}

void UDownloadManager::Clear()
{
	ActiveTasks.Empty();
}
