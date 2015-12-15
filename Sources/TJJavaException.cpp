/*
* Copyright (c) 2015
* Bogdan Mytnyk <bogdan.mytnyk@gmail.com>
*
* This library is free software; you can redistribute it and/or modify
* it under the terms of the MIT license.
*/

#include "../TJPch.h"
#include "../Include/TJCommon.h"
#include "../Include/TJJavaException.h"
#include "../Include/TJTypes.h"
#include "../Include/TJObjectRef.h"

#include <sstream>

TJJavaException::TJJavaException(const std::string& description, const std::string& place) throw():
	TJBaseException(description),
	mPlace(place)
{
}

TJJavaException::TJJavaException(const TJJavaException& javaExc) throw():
	TJBaseException(javaExc),
	mPlace(javaExc.mPlace)
{
}

TJJavaException& TJJavaException::operator=(const TJJavaException& javaExc) throw()
{
	if (this != &javaExc)
	{
		Base::operator=(javaExc);
		mPlace = javaExc.mPlace;
	}

	return *this;
}

TJJavaOutOfMemory::TJJavaOutOfMemory(const std::string& fullDescription, const std::string& place) throw():
	TJJavaException(fullDescription, place)
{
}

TJArrayIndexOutOfBounds::TJArrayIndexOutOfBounds(const std::string& fullDescription, const std::string& place) throw():
	TJJavaException(fullDescription, place)
{
}

TJJavaInvalidArgument::TJJavaInvalidArgument(const std::string& fullDescription, const std::string& place) throw():
	TJJavaException(fullDescription, place)
{
}

TJClassInitializerError::TJClassInitializerError(const std::string& fullDescription, const std::string& place) throw():
	TJJavaException(fullDescription, place)
{
}

TJInstantiationException::TJInstantiationException(const std::string& fullDescription, const std::string& place) throw():
	TJJavaException(fullDescription, place)
{
}


TJStringIndexOutOfBoundsException::TJStringIndexOutOfBoundsException(const std::string& fullDescription, const std::string& place) throw():
	TJJavaException(fullDescription, place)
{
}

static bool GetJavaExceptionInfo(JNIEnv* jniEnv, jthrowable javaExceptionObject, std::string& exceptionClassName, std::string& fullExceptionInfo)
{
	TJ_ASSERT (jniEnv != NULL);
	TJ_ASSERT (javaExceptionObject != NULL);

	exceptionClassName.clear();
	fullExceptionInfo.clear();

	jclass javaExceptionClass = jniEnv->GetObjectClass(javaExceptionObject);
	if (javaExceptionClass == NULL)
		return false;

	// get java class name
	TJMethodID toStringId = jniEnv->GetMethodID(javaExceptionClass, "toString", "()Ljava/lang/String;");
	if (toStringId == NULL)
		return false;

	jstring javaExceptionName = static_cast<jstring>(jniEnv->CallObjectMethod(javaExceptionClass, toStringId));
		
	jboolean dumbIsCopy = JNI_FALSE;
	const char* rawChars = jniEnv->GetStringUTFChars(javaExceptionName, &dumbIsCopy);
	TJSize rawLength= jniEnv->GetStringUTFLength(javaExceptionName);
	
	exceptionClassName.assign(rawChars, rawLength);

	jstring javaExcString = static_cast<jstring>(jniEnv->CallObjectMethod(javaExceptionObject, toStringId));
	if (javaExcString == NULL)
		return false;

	rawChars = jniEnv->GetStringUTFChars(javaExcString, &dumbIsCopy);
	rawLength = jniEnv->GetStringUTFLength(javaExcString);
	fullExceptionInfo.assign(rawChars, rawLength);
	
	return true;
}

void GenerateJavaException(JNIEnv* jniEnv, jthrowable javaExceptionObject, const std::string& place)
{
	TJ_ASSERT(javaExceptionObject != NULL);
	TJ_ASSERT(jniEnv != NULL);

	std::string exceptionClass("");
	std::string fullExceptionInfo("");
	bool infoGot = GetJavaExceptionInfo(jniEnv, javaExceptionObject, exceptionClass, fullExceptionInfo);
	if (!infoGot)
	{
		// we fail to get full info
		std::stringstream exceptionInfoStream;
		exceptionInfoStream << "Unknown java exception occured in " << place;
		throw TJBaseException(exceptionInfoStream.str());
	}

	// clear exception
	jniEnv->ExceptionClear();

	// get class name
	std::string className;
	std::string::size_type pos = exceptionClass.rfind(' ');
	if (pos != std::string::npos)
		className.assign(exceptionClass.c_str() + pos + 1, exceptionClass.size() - pos - 1);
	else
		className = exceptionClass;

	// here we need to do map (TODO!!!)
	if (className == "java.lang.OutOfMemoryError")
		throw TJJavaOutOfMemory(fullExceptionInfo, place);
	else if (className == "java.lang.ArrayIndexOutOfBoundsException")
		throw TJArrayIndexOutOfBounds(fullExceptionInfo, place);
	else if (className == "java.lang.InstantiationException")
		throw TJInstantiationException(fullExceptionInfo, place);
	else if (className == "java.lang.StringIndexOutOfBoundsException")
		throw TJStringIndexOutOfBoundsException(fullExceptionInfo, place);
	else
		throw TJJavaException(fullExceptionInfo, place);
};