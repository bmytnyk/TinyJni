/*
* Copyright (c) 2012
* Bogdan Mytnyk <bogdan.myntyk@gmail.com>
*
* This library is free software; you can redistribute it and/or modify
* it under the terms of the MIT license.
*/

#ifndef __TJPRIMITIVEARRAY_H__
#define __TJPRIMITIVEARRAY_H__

#include "TJTypes.h"
#include "TJCommon.h"
#include "TJRef.h"
#include "TJException.h"

template <typename T>
struct TJJavaArrayTraits
{
	typedef TJIntArray ArrayType;
	typedef TJInt NativeType;
	static ArrayType create(JNIEnv* env, TJSize count);
	static void getRegion(JNIEnv* env, ArrayType array, TJSize start, TJSize len, NativeType *buf);
	static void setRegion(JNIEnv* env, ArrayType array, TJSize start, TJSize len, const NativeType *buf);
	static T* acquireElements(JNIEnv* env, ArrayType array);
	static void releaseElements(JNIEnv* env, ArrayType array, NativeType* ptr) throw();
};

#define TJ_JAVA_ARRAY_TRAITS_DECL(type) \
template <> \
struct TJJavaArrayTraits<TJ##type> \
{ \
	typedef TJ##type##Array ArrayType;\
	typedef TJ##type NativeType;\
	static ArrayType create(JNIEnv* env, TJSize count);\
	static void getRegion(JNIEnv* env, ArrayType array, TJSize start, TJSize len, NativeType *buf);\
	static void setRegion(JNIEnv* env, ArrayType array, TJSize start, TJSize len, const NativeType *buf);\
	static TJ##type* acquireElements(JNIEnv* env, ArrayType array);\
	static void releaseElements(JNIEnv* env, ArrayType array, NativeType* ptr) throw(); \
};

TJ_JAVA_ARRAY_TRAITS_DECL(Int)
TJ_JAVA_ARRAY_TRAITS_DECL(Boolean)
TJ_JAVA_ARRAY_TRAITS_DECL(Double)
TJ_JAVA_ARRAY_TRAITS_DECL(Float)
TJ_JAVA_ARRAY_TRAITS_DECL(Char)
TJ_JAVA_ARRAY_TRAITS_DECL(Long)
TJ_JAVA_ARRAY_TRAITS_DECL(Byte)
TJ_JAVA_ARRAY_TRAITS_DECL(Short)

template <typename T>
class TJJavaPrimitiveArray : public TJRef<typename TJJavaArrayTraits<T>::ArrayType>
{
public:
	typedef typename TJJavaArrayTraits<T>::ArrayType TJJavaArrayType;
	typedef TJRef<TJJavaArrayType> TJBase;

private:
	// quantity of elements in array
	// always > 0
	size_t												mCount;
	
	// cached pointer to array contents (or copy) 
	mutable T*											mAcquiredDataPtr;
	
	TJJavaPrimitiveArray(TJJavaArrayType javaArrayHandle, size_t size, TJRefType refType = kGlobalRef, bool makeCopy = true):
		TJBase(javaArrayHandle, refType, makeCopy),
		mCount(size),
		mAcquiredDataPtr(NULL)
	{
	}

public:

	~TJJavaPrimitiveArray() throw ()
	{
		releasePtr();
	}
	
	inline size_t count() const
	{
		return mCount;
	}

	// return pointer to entire array itself (ot copy)
	const T* getPtr() const;
	T* getPtr();

	// never throws
	void releasePtr() throw();

	void getRegion(TJSize startIdx, TJSize length, T* resultBuffer);
	void setRegion(TJSize startIdx, TJSize length, const T* sourceBuffer);

	inline const	TJJavaArrayType get() const {return this->mHandle;}
	inline			TJJavaArrayType get() {return this->mHandle;}
	
	static TJJavaPrimitiveArray CreateArray(size_t size, TJRefType refType);

private:
	void* _getCriticalRow() const
	{
		JNIEnv* environment = TJGetEnvironment();
		if (environment != NULL)
			return environment->GetPrimitiveArrayCritical(this->mHandle, NULL);
		return NULL;
	}
	
	void _releaseCriticalRow(T* ptr) const
	{
		TJ_ASSERT(ptr != NULL);
		JNIEnv* environment = TJGetEnvironment();
        if (environment != NULL)
			environment->ReleasePrimitiveArrayCritical(this->mHandle, ptr, NULL);
	}
};

template <typename T>
T* TJJavaPrimitiveArray<T>::getPtr()
{
	JNIEnv* environment = TJGetEnvironment();
	if (environment == NULL)
		throw TJNIException(kThreadDetached, "Failed to get jnienv in TJJavaPrimitiveArray::getPtr");

	if (mAcquiredDataPtr != NULL)
		return mAcquiredDataPtr;
	mAcquiredDataPtr = TJJavaArrayTraits<T>::acquireElements(environment, this->mHandle);
	return mAcquiredDataPtr;
}

template <typename T>
const T* TJJavaPrimitiveArray<T>::getPtr() const
{
	JNIEnv* environment = TJGetEnvironment();
	if (environment == NULL)
		throw TJNIException(kThreadDetached, "Failed to get jnienv in TJJavaPrimitiveArray::getPtr");

	if (mAcquiredDataPtr != NULL)
		return mAcquiredDataPtr;
	mAcquiredDataPtr = TJJavaArrayTraits<T>::acquireElements(environment, this->mHandle);
	return mAcquiredDataPtr;
}

template <typename T>
void TJJavaPrimitiveArray<T>::releasePtr() throw()
{
	JNIEnv* environment = TJGetEnvironment();
	if ((environment != NULL) && (mAcquiredDataPtr != NULL))
	{
		TJJavaArrayTraits<T>::releaseElements(environment, this->mHandle, mAcquiredDataPtr);
		mAcquiredDataPtr = NULL;
	}
}

template <typename T>
TJJavaPrimitiveArray<T> TJJavaPrimitiveArray<T>::CreateArray(size_t size, TJRefType refType)
{
	TJ_ASSERT(size != 0);
	if (size == 0)
		throw TJInvalidArgument("Invalid size in TJJavaPrimitiveArray<T>::CreateArray");

	JNIEnv* jniEnv = TJGetEnvironment();
	if (jniEnv == NULL)
		throw TJNIException(kThreadDetached, "Failed to get jni environment in CreateArray");
	
	typename TJJavaArrayTraits<T>::ArrayType arrayObj = TJJavaArrayTraits<T>::create(jniEnv, size);
	return TJJavaPrimitiveArray(arrayObj, size, refType, true);
}

template <typename T>
void TJJavaPrimitiveArray<T>::getRegion(TJSize startIdx, TJSize length, T* resultBuffer)
{
	TJ_ASSERT(resultBuffer != NULL);

	if ((startIdx + length > mCount) || (startIdx < 0) || (resultBuffer == NULL) || (length < 0))
		throw TJInvalidArgument("Invalid argument in TJJavaPrimitiveArray::getRegion");

	JNIEnv* jniEnv = TJGetEnvironment();
	if (jniEnv == NULL)
		throw TJNIException(kThreadDetached, "Failed to get jni environment in TJJavaPrimitiveArray::getRegion");

	if (mAcquiredDataPtr != NULL)
	{
		memcpy(resultBuffer, mAcquiredDataPtr + startIdx, length * sizeof(T));
	}
	else
	{
		TJJavaArrayTraits<T>::getRegion(jniEnv, this->mHandle, startIdx, length, resultBuffer);
	}
}

template <typename T>
void TJJavaPrimitiveArray<T>::setRegion(TJSize startIdx, TJSize length, const T* sourceBuffer)
{
	TJ_ASSERT(sourceBuffer != NULL);

	if ((startIdx + length > mCount) || (startIdx < 0) || (sourceBuffer == NULL) || (length < 0))
		throw TJInvalidArgument("Invalid argument in TJJavaPrimitiveArray::setRegion");

	JNIEnv* jniEnv = TJGetEnvironment();
	if (jniEnv == NULL)
		throw TJNIException(kThreadDetached, "Failed to get jni environment in TJJavaPrimitiveArray::getRegion");

	if (mAcquiredDataPtr != NULL)
	{
		memcpy(mAcquiredDataPtr + startIdx, sourceBuffer, length * sizeof(T));
	}
	else
	{
		TJJavaArrayTraits<T>::setRegion(jniEnv, this->mHandle, startIdx, length, sourceBuffer);
	}
}

typedef TJJavaPrimitiveArray<TJByte> TJJavaByteArray;
typedef TJJavaPrimitiveArray<TJShort> TJJavaShortArray;
typedef TJJavaPrimitiveArray<TJFloat> TJJavaFloatArray;
typedef TJJavaPrimitiveArray<TJDouble> TJJavaDoubleArray;
typedef TJJavaPrimitiveArray<TJInt> TJJavaIntArray;
typedef TJJavaPrimitiveArray<TJChar> TJJavaCharArray;
typedef TJJavaPrimitiveArray<TJLong> TJJavaLongArray;
typedef TJJavaPrimitiveArray<TJBoolean> TJJavaBooleanArray;

template class TJJavaPrimitiveArray<TJShort>;


#endif //__TJPRIMITIVEARRAY_H__
