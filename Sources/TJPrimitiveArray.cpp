/*
* Copyright (c) 2015
* Bogdan Mytnyk <bogdan.mytnyk@gmail.com>
*
* This library is free software; you can redistribute it and/or modify
* it under the terms of the MIT license.
*/

#include "../TJPch.h"
#include "../Include/TJCommon.h"
#include "../Include/TJException.h"
#include "../Include/TJJavaException.h"
#include "../Include/TJPrimitiveArray.h"

#include "jni.h"
#include <cassert>
#include <stdexcept>

#define TJ_JAVA_ARRAY_TRAITS_DEF(type)																						\
	TJJavaArrayTraits<TJ##type>::ArrayType TJJavaArrayTraits<TJ##type>::create(JNIEnv* environment, TJSize count)			\
	{																														\
		TJ_ASSERT(environment != NULL);																						\
		TJJavaArrayTraits<TJ##type>::ArrayType resArray = environment->New##type##Array(count);								\
		if (resArray == NULL)																								\
			GenerateJavaException(environment, environment->ExceptionOccurred(), "Creating array failed");					\
		return resArray;																									\
	}																														\
																															\
	void TJJavaArrayTraits<TJ##type>::getRegion(JNIEnv* env, TJJavaArrayTraits<TJ##type>::ArrayType array,					\
		TJSize start, TJSize len, TJJavaArrayTraits<TJ##type>::NativeType *buf)												\
	{																														\
		TJ_ASSERT(buf != NULL);																								\
		TJ_ASSERT(env != NULL);																								\
																															\
		env->Get##type##ArrayRegion(array, start, len, buf);																\
		if (jthrowable error = env->ExceptionOccurred())																						\
			GenerateJavaException(env, error, "getRegion for array failed");								\
	}																														\
																															\
	void TJJavaArrayTraits<TJ##type>::setRegion(JNIEnv* env, TJJavaArrayTraits<TJ##type>::ArrayType array, TJSize start, TJSize len,		\
		const TJJavaArrayTraits<TJ##type>::NativeType* buf)																	\
	{																														\
		TJ_ASSERT(buf != NULL);																								\
		TJ_ASSERT(env != NULL);																								\
																															\
		env->Set##type##ArrayRegion(array, start, len, buf);																\
		if (jthrowable error = env->ExceptionOccurred())																						\
			GenerateJavaException(env, error, "setRegion for array failed");								\
	}																														\
	TJ##type* TJJavaArrayTraits<TJ##type>::acquireElements(JNIEnv* environment, TJJavaArrayTraits<TJ##type>::ArrayType array)					\
	{																														\
		TJ_ASSERT(array != NULL);																							\
		TJ_ASSERT(environment != NULL);																						\
		TJ##type* arrayElementsPtr = environment->Get##type##ArrayElements(array, NULL);									\
		if (arrayElementsPtr == NULL)																						\
			GenerateJavaException(environment, environment->ExceptionOccurred(), "acquireElements for array failed");								\
		return arrayElementsPtr;																										\
	}																														\
	void TJJavaArrayTraits<TJ##type>::releaseElements(JNIEnv* environment, TJJavaArrayTraits<TJ##type>::ArrayType array, NativeType* ptr)	throw()	\
	{																														\
		TJ_ASSERT(array != NULL);																							\
		TJ_ASSERT(environment != NULL);																						\
		TJ_ASSERT(ptr != NULL);																							\
																															\
		environment->Release##type##ArrayElements(array, ptr, 0);														\
	}
TJ_JAVA_ARRAY_TRAITS_DEF(Int)
TJ_JAVA_ARRAY_TRAITS_DEF(Boolean)
TJ_JAVA_ARRAY_TRAITS_DEF(Double)
TJ_JAVA_ARRAY_TRAITS_DEF(Float)
TJ_JAVA_ARRAY_TRAITS_DEF(Char)
TJ_JAVA_ARRAY_TRAITS_DEF(Long)
TJ_JAVA_ARRAY_TRAITS_DEF(Byte)
TJ_JAVA_ARRAY_TRAITS_DEF(Short)

