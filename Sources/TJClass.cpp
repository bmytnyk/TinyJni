/*
* Copyright (c) 2012
* Bogdan Mytnyk <bogdan.myntyk@gmail.com>
*
* This library is free software; you can redistribute it and/or modify
* it under the terms of the MIT license.
*/

#include "../TJPch.h"
#include "../Include/TJClass.h"
#include "../Include/TJObjectRef.h"
#include "../Include/TJValue.h"
#include "../Include/TJUtils.h"

#include <algorithm>

TJClassRef::TJClassRef(jclass classHandle, TJRefType refType, bool makeCopy):
	TJRef<jclass>(classHandle, refType, makeCopy),
	mClassName("")
{
	JNIEnv* jniEnv = TJGetEnvironment();
	if (jniEnv == NULL)
		throw TJNIException(kThreadDetached, "Failed to get jni environment");

	TJMethodID methodNameId = jniEnv->GetMethodID(mHandle, "toString", "()Ljava/lang/String;");
	TJ_ASSERT (methodNameId != NULL);
	
	ScopedLocalRef<jstring> name(static_cast<jstring>(jniEnv->CallObjectMethod(mHandle, methodNameId)));
	if (name != NULL)
	{
		const char* chars = jniEnv->GetStringUTFChars(name, NULL);
		if (chars == NULL)
			GenerateJavaException(jniEnv, jniEnv->ExceptionOccurred(), "GetStringUTFChars failed in TJClassRef::TJClassRef");

		size_t length = static_cast<size_t>(jniEnv->GetStringUTFLength(name));

		// chars is in form of "class a.b.c"
		std::string fullName(chars, chars + length);
		std::string::size_type pos = fullName.rfind(' ');
		if (pos != std::string::npos)
		{
			mClassName.assign(chars + pos + 1, length - pos - 1);

			// convert a.b.c form to La/b/c class name form
			std::replace(mClassName.begin(), mClassName.end(), '.', '/');
			mClassName = std::string("L") + mClassName;
		}

		jniEnv->ReleaseStringUTFChars(name, chars);
	}
}

TJClassRef::TJClassRef(const TJClassRef& rht):
	Base(rht),
	mClassName(rht.mClassName)
{
}

TJClassRef& TJClassRef::operator = (const TJClassRef& rht)
{
	if (this != &rht)
	{
		Base::operator=(rht);
		mClassName = rht.mClassName;
	}
	return *this;
}

TJClassRef TJClassRef::FindClass(const std::string& className, TJRefType refType)
{
	// get JNI environment
	JNIEnv* environment = TJGetEnvironment();
	if (environment == NULL)
		throw TJNIException(kThreadDetached, "Failed to get jnienv pointer in TJClassRef::FindClass");

	// try to find appropriate class
	jclass localHandle = environment->FindClass(className.c_str());
	if (localHandle == NULL)
		GenerateJavaException(environment, environment->ExceptionOccurred(), "Failed to find class in TJClassRef::FindClass");

	if (refType == kLocalRef)
		return TJClassRef(localHandle, kLocalRef, false);
	else
	{
		ScopedLocalRef<jclass> cleaner(localHandle);
		TJClassRef resultClassRef(localHandle, refType, true);
		return resultClassRef;
	}
}

TJClassRef TJClassRef::DefineClass(const std::string& className, const TJObjectRef& loader, 
									const TJByte* buffer, TJSize bufLength, TJRefType refType)
{
	// get JNI environment
	JNIEnv* environment = TJGetEnvironment();
	if (environment == NULL)
		throw TJNIException(kThreadDetached, "Failed to get jnienv pointer in TJClassRef::FindClass");

	if ((loader.get() == NULL) || (buffer == NULL) || (bufLength == 0))
		throw TJInvalidArgument("Invalid params in TJClassRef::DefineClass");

	jclass localClassHandle = environment->DefineClass(className.c_str(), loader.get(), buffer, bufLength);
	if (localClassHandle == NULL)
		GenerateJavaException(environment, environment->ExceptionOccurred(), "Failed to find class in TJClassRef::FindClass");

	if (refType == kLocalRef)
		return TJClassRef(localClassHandle, kLocalRef, false);
	else
	{
		ScopedLocalRef<jclass> cleaner(localClassHandle);
		return TJClassRef(localClassHandle, refType, true);
	}
}

bool TJClassRef::operator == (const TJClassRef& rht) const
{
	JNIEnv* environment = TJGetEnvironment();
	if (environment == NULL)
		throw TJNIException(kThreadDetached, "Failed to get jnienv pointer in TJClassRef::operator ==");

	return environment->IsSameObject(mHandle, rht.mHandle) == JNI_TRUE; 
}

bool TJClassRef::operator != (const TJClassRef& rht) const
{
	return !(*this == rht);
}

void TJClassRef::setField(const std::string& fieldName, const TJObjectRef& field)
{
	JNIEnv* environment = TJGetEnvironment();
	if (environment == NULL)
		throw TJNIException(kThreadDetached, "Failed to get jnienv in TJClassRef::setObjectField");

	jfieldID fieldDescriptor = environment->GetStaticFieldID(mHandle, fieldName.c_str(), field.descriptor().c_str());
	if (fieldDescriptor == NULL)
		GenerateJavaException(environment, environment->ExceptionOccurred(), "GetStaticFieldID failed in TJClassRef::setObjectField");

	environment->SetStaticObjectField(mHandle, fieldDescriptor, field.get());
}

void TJClassRef::setField(const std::string& fieldName, jobject value)
{
	JNIEnv* environment = TJGetEnvironment();
	if (environment == NULL)
		throw TJNIException(kThreadDetached, "Failed to get jnienv in TJClassRef::setObjectField");

	TJClassRef classRef(environment->GetObjectClass(value), kLocalRef);

	jfieldID fieldDescriptor = environment->GetStaticFieldID(mHandle, fieldName.c_str(), classRef.descriptor().c_str());
	if (fieldDescriptor == NULL)
		GenerateJavaException(environment, environment->ExceptionOccurred(), "GetStaticFieldID failed in TJClassRef::setObjectField");

	environment->SetStaticObjectField(mHandle, fieldDescriptor, value);
}

void TJClassRef::setField(const std::string& fieldName, jstring string)
{
	if (string == NULL)
		throw TJInvalidArgument("Invalid string argument in setStringField");

	JNIEnv* environment = TJGetEnvironment();
	if (environment == NULL)
		throw TJNIException(kThreadDetached, "Failed to get jnienv in TJClassRef::setStringField");

	jfieldID fieldDescriptor = environment->GetStaticFieldID(mHandle, fieldName.c_str(), "Ljava/lang/String;");
	if (fieldDescriptor == NULL)
		GenerateJavaException(environment, environment->ExceptionOccurred(), "GetStaticFieldID failed in TJClassRef::setObjectField");
}

TJObjectRef TJClassRef::field(const std::string& name, const std::string& objClassDescriptor, TJRefType refType)
{
	JNIEnv* environment = TJGetEnvironment();
	if (environment == NULL)
		throw TJNIException(kThreadDetached, "Failed to get jnienv in TJClassRef::getObjectField");

	jfieldID fieldDescriptor = environment->GetStaticFieldID(mHandle, name.c_str(), objClassDescriptor.c_str());
	if (fieldDescriptor == NULL)
		GenerateJavaException(environment, environment->ExceptionOccurred(), "GetStaticFieldID failed in TJClassRef::getObjectField");

	jobject resObject = environment->GetStaticObjectField(mHandle, fieldDescriptor);
	if (refType == kLocalRef)
		return TJObjectRef(resObject, false, kLocalRef);
	else
	{
		ScopedLocalRef<jobject> cleaner(resObject);
		return TJObjectRef(resObject, true, refType);
	}
};

