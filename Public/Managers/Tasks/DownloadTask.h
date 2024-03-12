// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"

#include "UObject/Object.h"
#include "DownloadTask.generated.h"

DECLARE_MULTICAST_DELEGATE_ThreeParams(FHttpRequestCompleteMulticast, FHttpRequestPtr, FHttpResponsePtr, bool)
DECLARE_MULTICAST_DELEGATE_TwoParams(FDownloadCompleteMulticast, FHttpRequestPtr, FHttpResponsePtr)
DECLARE_MULTICAST_DELEGATE_OneParam(FSimpleFloatMulticastDelegate, float)

/**
 * 
 */
UCLASS()
class WEBOPERATOR_API UDownloadTask : public UObject
{
	GENERATED_BODY()

public:
	static TObjectPtr<UDownloadTask> MakeTask(const FString& InUrl, FHttpRequestCompleteMulticast::FDelegate OnRequestComplete);
	static TObjectPtr<UDownloadTask> MakeTask(const FString& InUrl, const int32 InContentSize,
	                                          FHttpRequestCompleteMulticast::FDelegate OnRequestComplete);
	void InitializeFor(const FString& InUrl, FHttpRequestCompleteMulticast::FDelegate OnRequestComplete);
	void InitializeFor(const FString& InUrl, const int32 InContentSize, FHttpRequestCompleteMulticast::FDelegate OnRequestComplete);

	void Start() const;
	void ReinitializeRequest() const;
	void Clear() const;

	float GetProgressValue() const
	{
		return Progress;
	}

	bool IsDownloading() const
	{
		return bIsDownloading;
	}


#pragma region Delegates Related
	void SubscribeToRequestIsCompletedDelegate(const FHttpRequestCompleteMulticast::FDelegate& Delegate) const;
	void UnsubscribeFromRequestIsCompletedDelegate(const FDelegateHandle DelegateHandle) const;

	void SubscribeToDownloadingIsDoneDelegate(const FDownloadCompleteMulticast::FDelegate& Delegate) const;
	void UnsubscribeFromDownloadingIsDoneDelegate(const FDelegateHandle DelegateHandle) const;

	void SubscribeToDownloadProgressChangedDelegate(const FSimpleFloatMulticastDelegate::FDelegate& Delegate) const;
	void UnsubscribeFromDownloadProgressChangedDelegate(const FDelegateHandle DelegateHandle) const;
#pragma endregion

protected:
	virtual void BeginDestroy() override;

private:
#pragma region Feedback
	void OnProgressChanged(TSharedPtr<IHttpRequest> InHttpRequest, int32 BytesSent, int32 BytesReceived) const;
	void OnRequestCompleted(TSharedPtr<IHttpRequest> HttpRequest, TSharedPtr<IHttpResponse> HttpResponse, bool bSuccess) const;
#pragma endregion

	void UpdateRequestStatusReflection();
	
private:
	mutable TSharedPtr<IHttpRequest> RelatedHttpRequest;
	mutable float Progress;
	mutable bool bIsDownloading;
	int32 ContentSize = 0;
	FString Url;

	mutable FDownloadCompleteMulticast DownloadCompletedDelegate;
	mutable FHttpRequestCompleteMulticast RequestIsCompletedDelegate;
	mutable FSimpleFloatMulticastDelegate DownloadProgressChangedDelegate;
};
