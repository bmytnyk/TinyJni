/*
* Copyright (c) 2012
* Bogdan Mytnyk <bogdan.myntyk@gmail.com>
*
* This library is free software; you can redistribute it and/or modify
* it under the terms of the MIT license.
*/

#ifndef __TJREF_H__
#define __TJREF_H__

#include "TJTypes.h"
#include "TJCommon.h"
#include "TJException.h"

#include <stdexcept>

enum TJRefType
{
	kLocalRef = 0,
	kGlobalRef,
	kWeakGlobalRef
};

template <typename JavaType>
class TJRef
{
	static_assert(std::is_pointer<JavaType>::value, "JavaType should be pointer");

protected:
	TJRefType		mType;
	JavaType		mHandle;

public:
	explicit TJRef(JavaType value, TJRefType refType = kGlobalRef, bool makeCopy = true);
	TJRef(const TJRef<JavaType>& rht);
	TJRef<JavaType>& operator=(const TJRef<JavaType>& rht);
	TJRef(TJRef&& ref);
	TJRef& operator=(TJRef&& ref);

	~TJRef() noexcept;

	inline JavaType get() const {return mHandle;}
	inline TJRefType refType() const {return mType;} 

protected:
	// for use by derived classes : mHandle will be NULL
	// need to assign correct 
	TJRef(TJRefType refType = kGlobalRef);

private:
	static JavaType createHandle(const JavaType& sourceHandle, TJRefType refType);
	static void deleteHandle(JavaType handle, TJRefType refType) throw();
};

template <typename JavaType>
TJRef<JavaType>::TJRef(TJRefType refType):
	mHandle(NULL),
	mType(refType)
{
}

template <typename JavaType>
TJRef<JavaType>::TJRef(JavaType value, TJRefType refType, bool makeCopy):
	mType(refType),
	mHandle(value)
{
	if (makeCopy)
		mHandle = createHandle(value, refType);
}

template <typename JavaType>
TJRef<JavaType>::TJRef(const TJRef<JavaType>& rht):
	mType(rht.mType),
	mHandle(NULL)
{
	mHandle = createHandle(rht.mHandle, mType);
}

template <typename JavaType>
TJRef<JavaType>& TJRef<JavaType>::operator=(const TJRef<JavaType>& rht)
{
	if (this != &rht)
	{
		// can throw exception - to ensure in consistence do assignment later
		JavaType newHandle = createHandle(rht.mHandle, mType);

		// guaranted - no exceptions
		deleteHandle(mHandle, mType);
		mHandle = newHandle;
	}

	return *this;
}

template <typename JavaType>
TJRef<JavaType>::TJRef(TJRef<JavaType>&& ref):
	mType(ref.mType),
	mHandle(ref.mHandle)
{
	ref.mHandle = nullptr;
}

template <typename JavaType>
TJRef<JavaType>& TJRef<JavaType>::operator=(TJRef<JavaType>&& ref)
{
	deleteHandle(mHandle, mType);

	mType = ref.mType;
	mHandle = ref.mHandle;

	ref.mHandle = nullptr;

	return *this;
}

template <typename JavaType>
TJRef<JavaType>::~TJRef() noexcept
{
	// guaranteed - no exception
	if (mHandle != nullptr)
		deleteHandle(mHandle, mType);
}

template <typename JavaType>
JavaType TJRef<JavaType>::createHandle(const JavaType& sourceHandle, TJRefType refType)
{
	// check args
	if (sourceHandle == NULL)
		throw TJInvalidArgument("Invalid sourceHandle arg in TJRef<JavaType>::createHandle");

	// trying to get jni environment
	JNIEnv* environment = TJGetEnvironment();
	if (environment == NULL)
		throw TJNIException(kThreadDetached, "Failed to get jnienv in TJRef<JavaType>::createHandle");
		
	JavaType resValue = NULL;

	switch (refType)
	{
		case kGlobalRef:
			resValue = static_cast<JavaType>(environment->NewGlobalRef(sourceHandle));
			break;
		case kWeakGlobalRef:
			resValue = static_cast<JavaType>(environment->NewWeakGlobalRef(sourceHandle));
			break;
		case kLocalRef:
			resValue = static_cast<JavaType>(environment->NewLocalRef(sourceHandle));
			break;
		default:
			TJ_ASSERT(!"Unknown enum value");
			break;
	}

	// if exception occured - clear it
	if (environment->ExceptionCheck() == JNI_TRUE)
	{
		//environment->ExceptionDescribe();
		environment->ExceptionClear();
	}

	// if reference was not created and sourceHandle is not NULL - then out of memory
	if (resValue == NULL)
		throw TJNIException(kNotEnoughMemory, "Failed to create ref in TJRef<JavaType>::createHandle");

	return resValue;
};

template <typename JavaType>
void TJRef<JavaType>::deleteHandle(JavaType handle, TJRefType refType) throw()
{
	// use no exception version - delete handle has exception specification
	JNIEnv* environment = TJGetEnvironment();

	if ((environment != nullptr) && (handle != nullptr))
	{
		switch(refType)
		{
			case kGlobalRef:
				environment->DeleteGlobalRef(handle);
				break;
			case kWeakGlobalRef:
				environment->DeleteWeakGlobalRef(handle);
				break;
			case kLocalRef:
				environment->DeleteLocalRef(handle);
				break;
			default:
				TJ_ASSERT(!"Unknown enum value");
				break;
		}
	}
}

#endif  //__TJREF_H__
