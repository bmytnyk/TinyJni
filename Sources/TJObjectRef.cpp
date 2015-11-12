/*
* Copyright (c) 2012
* Bogdan Mytnyk <bogdan.mytnyk@gmail.com>
*
* This library is free software; you can redistribute it and/or modify
* it under the terms of the MIT license.
*/

#include "../TJPch.h"
#include "../Include/TJObjectRef.h"
#include "../Include/TJUtils.h"

namespace
{
	const char* kConstructorName = "<init>";
};

TJObjectRef::TJObjectRef(const TJClassRef& classRef, jobject sourceObj, bool doCopy, TJRefType refType):
	TJRef<jobject>(sourceObj, refType, doCopy),
	mClassRef(new TJClassRef(classRef))
{
};

TJObjectRef::TJObjectRef(jobject sourceObj, bool doCopy, TJRefType refType):
	TJRef<jobject>(sourceObj, refType, doCopy),
	mClassRef(nullptr)
{
	JNIEnv* environment = TJGetEnvironment();
	if (environment == NULL)
		throw TJNIException(kThreadDetached, "Failed to get jnienv in TJObjectRef::TJObjectRef");

	mClassRef = new TJClassRef(environment->GetObjectClass(sourceObj));
}

TJObjectRef::TJObjectRef(const TJObjectRef& rht):
	TJRef<jobject>(rht),
	mClassRef(new TJClassRef(*rht.mClassRef))
{
}

TJObjectRef& TJObjectRef::operator=(const TJObjectRef& rht)
{
	if (this != &rht)
	{
		TJBaseObject::operator=(rht);
		mClassRef = new TJClassRef(*rht.mClassRef);
	}

	return *this;
}

TJObjectRef::TJObjectRef(TJObjectRef&& rht):
	TJRef<jobject>(std::move(rht)),
	mClassRef(rht.mClassRef)
{
}

TJObjectRef& TJObjectRef::operator=(TJObjectRef&& rht)
{
	TJRef<jobject>::operator=(std::move(rht));
	mClassRef = std::move(rht.mClassRef);
	return *this;
}

TJObjectRef::~TJObjectRef() noexcept
{
	delete mClassRef;
}

TJObjectRef TJObjectRef::createObjectVarArgs(const char* className, const char* constrSignature, ...)
{
	// find corresponded class
	// check environment
	JNIEnv* environment = TJGetEnvironment();
	if (environment == NULL)
		throw TJNIException(kThreadDetached, "Failed to get jnienv in TJObjectRef::call");
	
	// find corresponded class
	TJClassRef classRef = TJClassRef::FindClass(className, kGlobalRef);

	jmethodID constructorID = environment->GetMethodID(classRef.get(), kConstructorName, constrSignature);
	if (constructorID == NULL)
		GenerateJavaException(environment, environment->ExceptionOccurred(), "GetMethodID failed");

	// get local reference to object
	va_list args;
	va_start(args, constrSignature);
	jobject objRef = environment->NewObjectV(classRef.get(), constructorID, args);
	va_end(args);

	if (objRef == NULL)
		GenerateJavaException(environment, environment->ExceptionOccurred(), "Failed to call constructor in JObjectRef::createObjectVarArgs");

	// to destroy local reference on exit
	ScopedLocalRef<jobject> cleaner(objRef);

	return TJObjectRef(classRef, objRef, true, kGlobalRef);
}

TJObjectRef TJObjectRef::createObjectFromTJValues(const std::string& className, const std::string& constrSignature,
									const TJValue* values, size_t count, TJRefType refType)
{
	// check environment
	JNIEnv* environment = TJGetEnvironment();
	if (environment == NULL)
		throw TJNIException(kThreadDetached, "Failed to get jnienv in TJObjectRef::createObject");

	// check arguments
	if ((values == NULL) || (count == 0) || (count > sMaxArgsCount))
		throw TJInvalidArgument("Invalid parameters in TJObjectRef::createObject");

	// create array of jvalue
	jvalue args[sMaxArgsCount] = {0};
	for (size_t i = 0; i < count; ++i)
		args[i] = values[i].handle();

	// find corresponded class
	TJClassRef classRef = TJClassRef::FindClass(className, kGlobalRef);

	// find corresponed constructor id
	TJMethodID constructorID = environment->GetMethodID(classRef.get(), kConstructorName, constrSignature.c_str());
	if (constructorID == NULL)
		GenerateJavaException(environment, environment->ExceptionOccurred(), "Failed to get constructor id in TJObjectRef::createObject");

	jobject objRefLocal = environment->NewObjectA(classRef.get(), constructorID, args);
	if (objRefLocal == NULL)
		GenerateJavaException(environment, environment->ExceptionOccurred(), "Failed to call constructor in TJObjectRef::createObject");
	
	if (refType == kLocalRef)
		return TJObjectRef(classRef, objRefLocal, false, kLocalRef);
	else
	{
		ScopedLocalRef<jobject> cleaner(objRefLocal);
		TJObjectRef resultRef(classRef, objRefLocal, true, refType);
		return resultRef;
	}
}

TJObjectRef TJObjectRef::createObjectInternal(const std::string& methodName, const std::string& signature, TJValueVector& args)
{
	return createObjectFromTJValues(methodName, signature, args.data(), args.size(), kGlobalRef);
}
