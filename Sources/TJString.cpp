/*
* Copyright (c) 2015
* Bogdan Mytnyk <bogdan.mytnyk@gmail.com>
*
* This library is free software; you can redistribute it and/or modify
* it under the terms of the MIT license.
*/

#include "../TJPch.h"
#include "../Include/TJCommon.h"
#include "../Include/TJString.h"
#include "../Include/TJUtils.h"

TJStringRef TJStringRef::createString(const TJCharString& utf16String, TJRefType refType)
{
	JNIEnv* jniEnv = TJGetEnvironment();
	if (jniEnv == NULL)
		throw TJNIException(kThreadDetached, "Failed to get jni environment");

	jstring rawString = jniEnv->NewString(utf16String.c_str(), static_cast<TJInt>(utf16String.length()));
	if (rawString == NULL)
		GenerateJavaException(jniEnv, jniEnv->ExceptionOccurred(), "TJStringRef::createString failed");

	if (refType == kLocalRef)
		return TJStringRef(rawString, false, kLocalRef);
	else
	{
		ScopedLocalRef<jstring> cleaner(rawString);
		return TJStringRef(rawString, true, refType);
	}
}

TJStringRef TJStringRef::createString(const std::string& utf8Str, TJRefType refType)
{
	JNIEnv* jniEnv = TJGetEnvironment();
	if (jniEnv == NULL)
		throw TJNIException(kThreadDetached, "Failed to get jni environment");

	jstring rawString = jniEnv->NewStringUTF(utf8Str.c_str());
	if (rawString == NULL)
		GenerateJavaException(jniEnv, jniEnv->ExceptionOccurred(), "TJStringRef::createString failed");

	if (refType == kLocalRef)
		return TJStringRef(rawString, false, kLocalRef);
	else
	{
		ScopedLocalRef<jstring> cleaner(rawString);
		return TJStringRef(rawString, true, refType);
	}
}

TJStringRef::TJStringRef(jstring str, bool copy, TJRefType ref):
	TJStringRef::Base(str, ref, copy),
	mUtf8Chars(NULL),
	mUtf8Length(0),
	mUtf16Chars(NULL),
	mUtf16Length(0)
{
	JNIEnv* jniEnv = TJGetEnvironment();
	if (jniEnv == NULL)
		throw TJNIException(kThreadDetached, "Failed to get jni environment");

	if (str == nullptr)
		throw TJInvalidArgument("Null pointer jstring in TJStringRef ctor");
	
	mUtf8Length = jniEnv->GetStringUTFLength(mHandle);
	mUtf16Length = jniEnv->GetStringLength(mHandle);
}

TJStringRef::TJStringRef(const TJStringRef& rht):
	TJStringRef::Base(rht),
	mUtf8Chars(nullptr),
    mUtf8Length(rht.mUtf8Length),
	mUtf16Chars(nullptr),
    mUtf16Length(rht.mUtf16Length)
{
}

TJStringRef::TJStringRef(TJStringRef&& rht):
	Base(std::move(rht)),
	mUtf8Chars(rht.mUtf8Chars),
    mUtf8Length(rht.mUtf8Length),
	mUtf16Chars(rht.mUtf16Chars),
    mUtf16Length(rht.mUtf16Length)
{
	rht.mUtf8Length = 0;
	rht.mUtf16Length = 0;
	rht.mUtf8Chars = nullptr;
	rht.mUtf16Chars = nullptr;
}

TJStringRef& TJStringRef::operator=(TJStringRef&& rht)
{
	Base::operator=(std::move(rht));
	mUtf8Length = rht.mUtf8Length;
	mUtf16Length = rht.mUtf16Length;
	mUtf8Chars = rht.mUtf8Chars;
	mUtf16Chars = rht.mUtf16Chars;

	rht.mUtf8Length = 0;
	rht.mUtf16Length = 0;
	rht.mUtf8Chars = nullptr;
	rht.mUtf16Chars = nullptr;

	return *this;
}

TJStringRef& TJStringRef::operator=(const TJStringRef& rht)
{
	if (this != &rht)
	{
		Base::operator=(rht);
		mUtf8Length = rht.mUtf8Length;
		mUtf16Length = rht.mUtf16Length;
		mUtf8Chars = NULL;
		mUtf16Chars = NULL;
	}

	return *this;
}

TJStringRef::~TJStringRef() noexcept
{
	releaseUtf8Chars();
	releaseUtf16Chars();
}

const char* TJStringRef::acquireUtf8Chars()
{
	JNIEnv* jniEnv = TJGetEnvironment();
	if (jniEnv == NULL)
		throw TJNIException(kThreadDetached, "Failed to get jni environment");

	if (mHandle == nullptr)
		throw TJNIException(kInvalidHandle, "Null handle in TJStringRef");

	if (mUtf8Chars == NULL)
	{
		TJBoolean isCopy = JNI_FALSE;
		mUtf8Chars = jniEnv->GetStringUTFChars(mHandle, &isCopy);
		if (mUtf8Chars == NULL)
			GenerateJavaException(jniEnv, jniEnv->ExceptionOccurred(), "TJStringRef::acquireUtf8Chars failed");
	}

	return mUtf8Chars;
}

void TJStringRef::releaseUtf8Chars() noexcept
{
	if ((mUtf8Chars != nullptr) && (mHandle != nullptr))
	{
		JNIEnv* jniEnv = TJGetEnvironment();
		if (jniEnv != nullptr)
		{
			jniEnv->ReleaseStringUTFChars(mHandle, mUtf8Chars);
			mUtf8Chars = nullptr;
		}
	}
}

const TJChar* TJStringRef::acquireUtf16Chars()
{
	JNIEnv* jniEnv = TJGetEnvironment();
	if (jniEnv == nullptr)
		throw TJNIException(kThreadDetached, "Failed to get jni environment");

	if (mHandle == nullptr)
		throw TJNIException(kInvalidHandle, "Null handle in TJStringRef");

	if (mUtf16Chars == nullptr)
	{
		TJBoolean isCopy = JNI_FALSE;
		mUtf16Chars = jniEnv->GetStringChars(mHandle, &isCopy);
		if (mUtf16Chars == nullptr)
			GenerateJavaException(jniEnv, jniEnv->ExceptionOccurred(), "TJStringRef::acquireUtf16Chars failed");
	}

	return mUtf16Chars;
}
 
void TJStringRef::releaseUtf16Chars() noexcept
{
	if ((mUtf16Chars != nullptr) && (mHandle != nullptr))
	{
		JNIEnv* jniEnv = TJGetEnvironment();
		if (jniEnv != nullptr)
		{
			jniEnv->ReleaseStringChars(mHandle, mUtf16Chars);
			mUtf16Chars = nullptr;
		}
	}
}
