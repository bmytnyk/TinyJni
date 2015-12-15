/*
* Copyright (c) 2012
* Bogdan Mytnyk <bogdan.myntyk@gmail.com>
*
* This library is free software; you can redistribute it and/or modify
* it under the terms of the MIT license.
*/

#ifndef __TG_BASEOBJECTREF_H__
#define __TG_BASEOBJECTREF_H__

#include "TJRef.h"
#include "TJClass.h"
#include "TJValue.h"

/*template <typename SourceRefType, typename DestRefType>
DestRefType RefCast(const SourceRefType& refType, TJRefType refType = kLocalRef)
{
	return DestRefType(static_cast<>(refType.get()), true, refType);
}*/

//@brief
//class that represents reference to java object

class TJObjectRef : public TJRef<jobject>
{
private:
	//reference to the class of this object
	TJClassRef* mClassRef;

public:
	typedef TJRef<jobject> TJBaseObject;

	TJObjectRef(const TJClassRef& classRef, jobject sourceObj, bool doCopy = true, TJRefType refType = kLocalRef);
	explicit TJObjectRef(jobject sourceObj, bool doCopy = true, TJRefType refType = kLocalRef);
	TJObjectRef(const TJObjectRef& rht);
	TJObjectRef& operator=(const TJObjectRef& rht);
	TJObjectRef(TJObjectRef&& rht);
	TJObjectRef& operator=(TJObjectRef&& rht);

	~TJObjectRef() noexcept;

	// set or get fields : only primitive types are acceptable
	template <typename PrimitiveType>
	PrimitiveType field(const std::string& name);

	template <typename JavaPrimitiveType>
	void setField(const std::string& fieldName, JavaPrimitiveType value);

	void setField(const std::string& fieldName, const TJStringRef& string);
	
	void setField(const std::string& fieldName, jstring string);

	void setField(const std::string& fieldName, const TJObjectRef& object);

	void setField(const std::string& fieldName, jobject object);

	const std::string& descriptor() const {return mClassRef->descriptor();};

	template <typename RetType>
	RetType call(const std::string& methodName, const std::string& signature);

	//@brief:
	// call non-static methods for this objects (given by  class name)

	template <typename RetType, typename... JavaTypes>
	RetType call(const std::string& methodName, const std::string& signature, JavaTypes... args);

	//@brief:
	// create object's of the class (given by  class name)
	static TJObjectRef createObject(const std::string& className, const std::string& constrSignature);

	template <typename... JavaTypes>
	static TJObjectRef createObject(const std::string& methodName, const std::string& signature, JavaTypes... args);

	static TJObjectRef createObjectFromTJValues(const std::string& className, const std::string& constrSignature, 
									const TJValue* values, size_t count, TJRefType refType);

	// maximal args count for createObject
	static const size_t sMaxArgsCount = 128;

private:

	template <typename RetType>
	RetType callInternal(const std::string& methodName, const std::string& signature, const TJValueVector& args);

	template <typename RetType, typename ArgType, typename... JavaTypes>
	RetType callInternal(const std::string& methodName, const std::string& signature, TJValueVector& addedArgs, ArgType value, JavaTypes... futureArgs);

	static TJObjectRef createObjectInternal(const std::string& methodName, const std::string& signature, TJValueVector& addedargs);

	template <typename ArgType, typename... JavaTypes>
	static TJObjectRef createObjectInternal(const std::string& methodName, const std::string& signature, TJValueVector& addedArgs, ArgType value, JavaTypes... futureArgs);

	//@brief:
	// call object method
	//@params:
	// methodName - name of the method
	// signature - descriptor of method in form of "(Params)ReturnValue"

	template <typename RetType>
	RetType callVarArgs(const char* methodName, const char* signature, ...);

	//@brief:
	// creates object and returns global reference to it
	//@params:
	// className - name of the class
	// constrSignature - descriptor of constructor

	static TJObjectRef createObjectVarArgs(const char* className, const char* constrSignature, ...);
};

inline TJObjectRef TJObjectRef::createObject(const std::string& className, const std::string& constrSignature)
{
	return createObjectVarArgs(className.c_str(), constrSignature.c_str());
}

template <typename... JavaTypes>
TJObjectRef TJObjectRef::createObject(const std::string& methodName, const std::string& signature, JavaTypes... args)
{
	TJValueVector values;
	return createObjectInternal(methodName, signature, values, args...);
}

template <typename ArgType, typename... JavaTypes>
TJObjectRef TJObjectRef::createObjectInternal(const std::string& methodName, const std::string& signature, TJValueVector& addedArgs, ArgType value, JavaTypes... futureArgs)
{
	addedArgs.push_back(TJValue(value));
	return createObjectInternal(methodName, signature, addedArgs, futureArgs...);
}

template <typename RetType>
RetType TJObjectRef::callVarArgs(const char* methodName, const char* signature, ...)
{
	JNIEnv* environment = TJGetEnvironment();
	if (environment == NULL)
		throw TJNIException(kThreadDetached, "Failed to get jnienv in TJClassRef::call");

	TJMethodID methodID = environment->GetMethodID(mClassRef->get(), methodName, signature);
	if (methodID == NULL)
		GenerateJavaException(environment, environment->ExceptionOccurred(), "GetMethodID failed");

	va_list args;
	va_start(args, signature);
	RetType retValue = TJTypeTraits<RetType>::CallMethodV(environment, mHandle, methodID, args);
	va_end(args);

	if (environment->ExceptionCheck() == JNI_TRUE)
		GenerateJavaException(environment, environment->ExceptionOccurred(), "CallMethodV failed");

	return retValue;
}

template <typename RetType>
RetType TJObjectRef::call(const std::string& methodName, const std::string& signature)
{
	return callVarArgs<RetType>(methodName.c_str(), signature.c_str());
}

template <typename RetType, typename... JavaTypes>
RetType TJObjectRef::call(const std::string& methodName, const std::string& signature, JavaTypes... args)
{
	return call(methodName, signature, args...);
}
/*
template <typename RetType>
RetType TJObjectRef::callInternal(const std::string& methodName, const std::string& signature, const TJValueVector& args)
{
	return call(methodName, args.data(), args.size());
}
*/
template <typename RetType, typename ArgType, typename... JavaTypes>
RetType TJObjectRef::callInternal(const std::string& methodName, const std::string& signature, TJValueVector& addedArgs, ArgType singleArgument, JavaTypes... args)
{
	TJ_STATIC_ASSERT(TJTypeTraits<ArgType>::sApplicableArg, "Invalid argument");
	addedArgs.push_back(TJValue(singleArgument));
	return callInternal(methodName, signature, addedArgs, args...);
}

template <typename PrimitiveType>
PrimitiveType TJObjectRef::field(const std::string& name)
{
	JNIEnv* environment = TJGetEnvironment();
	if (environment == NULL)
		throw TJNIException(kThreadDetached, "Failed to get jnienv in TJObjectRef::field()");

	static const std::string& sFieldSignature = sFieldDescriptorsArray[TJTypeTraits<PrimitiveType>::sTypeId];
	TJFieldID fieldID = environment->GetFieldID(mClassRef->get(), name.c_str(), sFieldSignature.c_str());
	if (fieldID == NULL)
		GenerateJavaException(environment, environment->ExceptionOccurred(), "GetFieldID failed in TJObjectRef::field()");

	PrimitiveType resultFieldValue = TJTypeTraits<PrimitiveType>::GetField(environment, mHandle, fieldID);
	if (environment->ExceptionCheck() == JNI_TRUE)
		GenerateJavaException(environment, environment->ExceptionOccurred(), "GetField failed in TJObjectRef::field()");

	return resultFieldValue;
}

template <typename JavaType>
void TJObjectRef::setField(const std::string& fieldName, JavaType value)
{
	JNIEnv* environment = TJGetEnvironment();
	if (environment == NULL)
		throw TJNIException(kThreadDetached, "Failed to get jnienv in TJObjectRef::setField()");

	static const std::string& sFieldSignature = sFieldDescriptorsArray[TJTypeTraits<JavaType>::sTypeId];
	TJFieldID fieldID = environment->GetFieldID(mClassRef->get(), fieldName.c_str(), sFieldSignature.c_str());
	if (fieldID == NULL)
		GenerateJavaException(environment, environment->ExceptionOccurred(), "GetFieldID failed in TJObjectRef::setField()");

	JavaType resultFieldValue = TJTypeTraits<JavaType>::SetField(environment, mHandle, fieldID, value);
	if (environment->ExceptionCheck() == JNI_TRUE)
		GenerateJavaException(environment, environment->ExceptionOccurred(), "SetField failed in TJObjectRef::setField()");
}

#endif //__TG_BASEOBJECTREF_H__
