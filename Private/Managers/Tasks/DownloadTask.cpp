// Fill out your copyright notice in the Description page of Project Settings.


#include "..\..\..\Public\Managers\Tasks\DownloadTask.h"

#include "HttpModule.h"


void UDownloadTask::OnProgressChanged(TSharedPtr<IHttpRequest> InHttpRequest, int32 BytesSent, const int32 BytesReceived) const
{
	if (ContentSize > 0)
	{
		Progress = static_cast<float>(BytesReceived) / static_cast<float>(ContentSize);
	}
	DownloadProgressChangedDelegate.Broadcast(Progress);
}

void UDownloadTask::BeginDestroy()
{
	Clear();
	Super::BeginDestroy();
}

void UDownloadTask::InitializeFor(const FString& InUrl, FHttpRequestCompleteMulticast::FDelegate OnRequestComplete)
{
	Url = InUrl;
	ReinitializeRequest();
	SubscribeToRequestIsCompletedDelegate(OnRequestComplete);
}

void UDownloadTask::InitializeFor(const FString& InUrl, const int32 InContentSize, FHttpRequestCompleteMulticast::FDelegate OnRequestComplete)
{
	Url = InUrl;
	ContentSize = InContentSize;
	ReinitializeRequest();
	SubscribeToRequestIsCompletedDelegate(OnRequestComplete);
}

TObjectPtr<UDownloadTask> UDownloadTask::MakeTask(const FString& InUrl, FHttpRequestCompleteMulticast::FDelegate OnRequestComplete)
{
	auto Task = NewObject<UDownloadTask>();
	Task->InitializeFor(InUrl, OnRequestComplete);
	return Task;
}

TObjectPtr<UDownloadTask> UDownloadTask::MakeTask(const FString& InUrl, const int32 InContentSize,
                                                  FHttpRequestCompleteMulticast::FDelegate OnRequestComplete)
{
	auto Task = NewObject<UDownloadTask>();
	Task->InitializeFor(InUrl, InContentSize, OnRequestComplete);
	return Task;
}


void UDownloadTask::OnRequestCompleted(TSharedPtr<IHttpRequest> HttpRequest, TSharedPtr<IHttpResponse> HttpResponse, bool bSuccess) const
{
	if (RequestIsCompletedDelegate.IsBound())
	{
		RequestIsCompletedDelegate.Broadcast(HttpRequest, HttpResponse, bSuccess);
	}
	if (bSuccess)
	{
		if (DownloadCompletedDelegate.IsBound())
		{
			DownloadCompletedDelegate.Broadcast(HttpRequest, HttpResponse);
		}
	}
	Clear();
}

void UDownloadTask::UpdateRequestStatusReflection()
{
	bIsDownloading = RelatedHttpRequest ? RelatedHttpRequest->GetStatus() == EHttpRequestStatus::Processing : false;
}

bool UDownloadTask::IsDownloading() const
{
	return bIsDownloading;
}

void UDownloadTask::SubscribeToRequestIsCompletedDelegate(const FHttpRequestCompleteMulticast::FDelegate& Delegate) const
{
	if (RequestIsCompletedDelegate.IsBoundToObject(Delegate.GetUObject()))
	{
		return;
	}
	RequestIsCompletedDelegate.Add(Delegate);
}

void UDownloadTask::UnsubscribeFromRequestIsCompletedDelegate(const FDelegateHandle DelegateHandle) const
{
	RequestIsCompletedDelegate.Remove(DelegateHandle);
}

void UDownloadTask::SubscribeToDownloadingIsDoneDelegate(const FDownloadCompleteMulticast::FDelegate& Delegate) const
{
	if (DownloadCompletedDelegate.IsBoundToObject(Delegate.GetUObject()))
	{
		return;
	}
	DownloadCompletedDelegate.Add(Delegate);
}

void UDownloadTask::UnsubscribeFromDownloadingIsDoneDelegate(const FDelegateHandle DelegateHandle) const
{
	DownloadCompletedDelegate.Remove(DelegateHandle);
}

void UDownloadTask::SubscribeToDownloadProgressChangedDelegate(const FSimpleFloatMulticastDelegate::FDelegate& Delegate) const
{
	if (DownloadProgressChangedDelegate.IsBoundToObject(Delegate.GetUObject()))
	{
		return;
	}
	DownloadProgressChangedDelegate.Add(Delegate);
}

void UDownloadTask::UnsubscribeFromDownloadProgressChangedDelegate(const FDelegateHandle DelegateHandle) const
{
	DownloadProgressChangedDelegate.Remove(DelegateHandle);
}

void UDownloadTask::ReinitializeRequest() const
{
	RelatedHttpRequest = FHttpModule::Get().CreateRequest();
	RelatedHttpRequest->SetVerb("GET");
	RelatedHttpRequest->SetURL(Url);
	RelatedHttpRequest->OnProcessRequestComplete().BindUObject(this, &UDownloadTask::OnRequestCompleted);
	RelatedHttpRequest->OnRequestProgress().BindUObject(this, &UDownloadTask::OnProgressChanged);
}

void UDownloadTask::Clear() const
{
	Progress = 0.0;
	bIsDownloading = false;
	if (RelatedHttpRequest.IsValid())
	{
		if (RelatedHttpRequest->GetStatus() == EHttpRequestStatus::Processing)
		{
			RelatedHttpRequest->CancelRequest();
		}
	}

	RelatedHttpRequest.Reset();
}

float UDownloadTask::GetProgressValue() const
{
	return Progress;
}

/*bool UDownloadTask::IsDownloading() const
{
	return RelatedHttpRequest ? RelatedHttpRequest->GetStatus() == EHttpRequestStatus::Processing : false;
}*/

void UDownloadTask::Start() const
{
	if (!RelatedHttpRequest.IsValid() || RelatedHttpRequest->GetStatus() != EHttpRequestStatus::NotStarted)
	{
		ReinitializeRequest();
	}
	bIsDownloading = true;
	RelatedHttpRequest->ProcessRequest();
}
