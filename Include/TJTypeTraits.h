/*
* Copyright (c) 2012
* Bogdan Mytnyk <bogdan.myntyk@gmail.com>
*
* This library is free software; you can redistribute it and/or modify
* it under the terms of the MIT license.
*/

#ifndef __TG_TYPETRAITS_H__
#define __TG_TYPETRAITS_H__

#include "TJTypes.h"
#include "TJCommon.h"
#include "TJRef.h"
#include "TJException.h"

#include "TJRef.h"

extern const std::string sFieldDescriptorsArray[kTypesCount];

template <typename JavaType>
struct TJTypeTraits
{
	static const bool sApplicableArg = false;
	static const TJPrimitiveTypeId sTypeId;
	static JavaType CallStaticMethod(JNIEnv* env, jclass classHandle, jmethodID methodID, ...);
	static JavaType CallStaticMethod(JNIEnv* env, jclass classHandle, jmethodID methodID, va_list args);
	static JavaType CallStaticMethod(JNIEnv* env, jclass classHandle, jmethodID methodID, jvalue* args);
	static JavaType GetStaticField(JNIEnv* env, jclass classHandle, jfieldID fieldId);
	static void SetStaticField(JNIEnv* env, jclass classHandle, jfieldID fieldId, JavaType value);
	static JavaType CallMethod(JNIEnv* env, jobject objectHandle, jmethodID methodID, ...);
	static JavaType CallMethodV(JNIEnv* env, jobject objectHandle, jmethodID methodID, va_list args);
	static JavaType GetField(JNIEnv* env, jobject objectHandle, jfieldID fieldId);
	static void SetField(JNIEnv* env, jobject objectHandle, jfieldID fieldId, JavaType value);
};


#define TJTypeTraitsDecl(Type)																				\
template <>																									\
struct TJTypeTraits<Type>																					\
{																											\
	static const bool sApplicableArg = true;																\
	static const TJPrimitiveTypeId sTypeId;																	\
	static Type CallStaticMethod(JNIEnv* env, jclass classHandle, jmethodID methodID, va_list args);		\
	static Type CallStaticMethod(JNIEnv* env, jclass classHandle, jmethodID methodID, ...);					\
	static Type CallStaticMethod(JNIEnv* env, jclass classHandle, jmethodID methodID, jvalue* args);		\
	static Type GetStaticField(JNIEnv* env, jclass classHandle, jfieldID fieldId);							\
	static void SetStaticField(JNIEnv* env, jclass classHandle, jfieldID fieldId, Type value);				\
	static Type CallMethod(JNIEnv* env, jobject objectHandle, jmethodID methodID, ...);						\
	static Type CallMethodV(JNIEnv* env, jobject objectHandle, jmethodID methodID, va_list args);				\
	static Type GetField(JNIEnv* env, jobject objectHandle, jfieldID fieldId);								\
	static void SetField(JNIEnv* env, jobject objectHandle, jfieldID fieldId, Type value);					\
};

TJTypeTraitsDecl(TJInt)
TJTypeTraitsDecl(TJLong)
TJTypeTraitsDecl(TJByte)
TJTypeTraitsDecl(TJChar)
TJTypeTraitsDecl(TJFloat)
TJTypeTraitsDecl(TJDouble)
TJTypeTraitsDecl(TJBoolean)
TJTypeTraitsDecl(TJShort)
TJTypeTraitsDecl(jobject)


#endif //__TG_TYPETRAITS_H__
