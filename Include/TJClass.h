/*
* Copyright (c) 2012
* Bogdan Mytnyk <bogdan.myntyk@gmail.com>
*
* This library is free software; you can redistribute it and/or modify
* it under the terms of the MIT license.
*/

#ifndef __TG_CLASSREF_H__
#define __TG_CLASSREF_H__

#include "TJRef.h"
#include "TJTypeTraits.h"
#include "TJJavaException.h"
#include "TJValue.h"

class TJObjectRef;
class TJStringRef;

class TJClassRef : public TJRef<jclass>
{
private:
	std::string mClassName;

public:
	typedef TJRef<jclass> Base;

	explicit TJClassRef(jclass classHandle, TJRefType refType = kGlobalRef, bool makeCopy = true);
	TJClassRef(const TJClassRef& rht);
	TJClassRef& operator = (const TJClassRef& rht);
	TJClassRef(TJClassRef&& rht);
	TJClassRef& operator = (TJClassRef&& rht);

	const std::string& descriptor() const {return mClassName;};

	bool operator == (const TJClassRef& rht) const;
	bool operator != (const TJClassRef& rht) const;

	template <typename RetType>
	RetType call(const std::string& methodName, const std::string& signature);

	template <typename RetType, typename... JavaTypes>
	RetType call(const std::string& methodName, const std::string& signature, JavaTypes... args);

	template <typename RetType>
	RetType callInternal(const std::string& methodName, const std::string& signature, const TJValueVector& args);

	template <typename RetType, typename ArgType, typename... JavaTypes>
	RetType callInternal(const std::string& methodName, const std::string& signature, TJValueVector& addedArgs, ArgType value, JavaTypes... futureArgs);

	template <typename RetType>
	RetType call(const std::string& methodName, const TJValue* args, size_t count);

	virtual ~TJClassRef() throw()
	{
	}

	template <typename PrimitiveType>
	PrimitiveType field(const std::string& name);

	// return reference to string field
	TJStringRef field(const std::string& name, TJRefType refType);

	TJObjectRef field(const std::string& name, const std::string& objClassDescriptor, TJRefType refType);

	template <typename JavaType>
	void setField(const std::string& fieldName, JavaType value);

	void setField(const std::string& fieldName, jobject field);

	void setField(const std::string& fieldName, jstring string);

	void setField(const std::string& fieldName, const TJObjectRef& objectRef);

	void setField(const std::string& fieldName, const TJStringRef& stringRef);	

	// can throw exceptions
	static TJClassRef FindClass(const std::string& className, TJRefType refType);

	// can throw exceptions
	static TJClassRef DefineClass(const std::string& className, const TJObjectRef& loader, const TJByte* buffer, TJSize bufLength, TJRefType refType);

private:
	template <typename RetType>
	RetType callVarArgs(const char* name, const char* signature, ...);
};

template <typename JavaType>
void TJClassRef::setField(const std::string& fieldName, JavaType value)
{
	// get JNI environment
	JNIEnv* environment = TJGetEnvironment();
	if (environment == nullptr)
		throw TJNIException(kThreadDetached, "Failed to get jnienv in TJClassRef::setField");

	jfieldID fieldDescriptor = environment->GetStaticFieldID(mHandle, fieldName.c_str(), TJTypeTraits<JavaType>::sFieldSignature.c_str());
	if (fieldDescriptor == nullptr)
		GenerateJavaException(environment, environment->ExceptionOccurred(), "GetStaticFieldID failed in TJClassRef::setField");

	// finally set value : no error checking - all values are already checked
	TJTypeTraits<JavaType>::SetStaticField(environment, mHandle, fieldDescriptor, value);
}

template <typename PrimitiveType>
PrimitiveType TJClassRef::field(const std::string& fieldName)
{
	// get JNI environment
	JNIEnv* environment = TJGetEnvironment();
	if (environment == NULL)
		throw TJNIException(kThreadDetached, "Failed to get jnienv in TJClassRef::field");

	// can use static beacuse for each instantiation we have different signatures
	static const std::string& sFieldSignature = sFieldDescriptorsArray[TJTypeTraits<PrimitiveType>::sTypeId];
	
	jfieldID fieldDescriptor = environment->GetStaticFieldID(mHandle, fieldName.c_str(), sFieldSignature.c_str());
	if (fieldDescriptor == NULL)
		GenerateJavaException(environment, environment->ExceptionOccurred(), "GetStaticFieldID failed in field");

	return TJTypeTraits<PrimitiveType>::GetStaticField(environment, mHandle, fieldDescriptor);
}

template <typename RetType>
RetType TJClassRef::call(const std::string& methodName, const std::string& signature)
{
	return callVarArgs<RetType>(methodName.c_str(), signature.c_str());
}

template <typename RetType, typename... JavaTypes>
RetType TJClassRef::call(const std::string& methodName, const std::string& signature, JavaTypes... args)
{
	TJValueVector valuesVector;
	return callInternal<RetType>(methodName, signature, valuesVector, args...);
}

template <typename RetType>
RetType TJClassRef::callInternal(const std::string& methodName, const std::string& signature, const TJValueVector& args)
{
	return call<RetType>(methodName, args.data(), args.size());
}

template <typename RetType, typename ArgType, typename... JavaTypes>
RetType TJClassRef::callInternal(const std::string& methodName, const std::string& signature, TJValueVector& addedArgs, ArgType singleArgument, JavaTypes... args)
{
	TJ_STATIC_ASSERT(TJTypeTraits<ArgType>::sApplicableArg, "Invalid argument");
	addedArgs.emplace_back(TJValue(singleArgument));
	return callInternal<RetType>(methodName, signature, addedArgs, args...);
}

template <typename RetType>
RetType TJClassRef::call(const std::string& methodName, const TJValue* args, size_t count)
{
	static const uint32_t sMaxArgs = 1024;

	if ((args == NULL) || (count == 0))
		throw TJInvalidArgument("Invalid argument in TJClassRef::call");

	if (count > sMaxArgs)
		throw TJInvalidArgument("Exceeded maximum count of arguments ");

	JNIEnv* environment = TJGetEnvironment();
	if (environment == NULL)
		throw TJNIException(kThreadDetached, "Failed to get jnienv in TJClassRef::call");

	std::string signature = "";
	jvalue rawArgs[sMaxArgs] = {0};

	for (size_t i = 0; i < count; ++i)
	{
		signature += args[i].descriptor();
		rawArgs[i] = args[i].handle();
	}

	jmethodID methodID = environment->GetStaticMethodID(mHandle, methodName.c_str(), signature.c_str());
	if (methodID == NULL)
		GenerateJavaException(environment, environment->ExceptionOccurred(), "GetStaticMethodID failed");

	RetType res = TJTypeTraits<RetType>::CallStaticMethod(environment, mHandle, methodID, rawArgs);

	if (environment->ExceptionCheck() == JNI_TRUE)
		GenerateJavaException(environment, environment->ExceptionOccurred(), std::string("Method ") + methodName + std::string(" failed"));

	return res;
}

template <typename RetType>
RetType TJClassRef::callVarArgs(const char* name, const char* signature, ...)
{
	TJ_ASSERT(name != NULL);
	TJ_ASSERT(signature != NULL);

	JNIEnv* environment = TJGetEnvironment();
	if (environment == NULL)
		throw TJNIException(kThreadDetached, "Failed to get jnienv in TJClassRef::callVarArgs");

	jmethodID methodID = environment->GetStaticMethodID(mHandle, name, signature);
	if (methodID == NULL)
		GenerateJavaException(environment, environment->ExceptionOccurred(), "GetStaticMethodID failed");

	va_list args;
	va_start(args, signature);
	RetType resultValue = TJTypeTraits<RetType>::CallMethodV(environment, mHandle, methodID, args);
	va_end(args);

	if (environment->ExceptionCheck() == JNI_TRUE)
		GenerateJavaException(environment, environment->ExceptionOccurred(), std::string("Method failed"));

	return resultValue;
}

#endif //__TG_CLASSREF_H__
