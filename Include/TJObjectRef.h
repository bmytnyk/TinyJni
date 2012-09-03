#ifndef __TG_BASEOBJECTREF_H__
#define __TG_BASEOBJECTREF_H__

#include "TJRef.h"
#include "TJClass.h"
#include "TJValue.h"

class TJObjectRef : public TJRef<jobject>
{
private:
	// global reference to the class of this object
	TJClassRef* mClassRef;

public:
	typedef TJRef<jobject> TJBaseObject;

	TJObjectRef(const TJClassRef& classRef, jobject sourceObj, bool doCopy = true, TJRefType refType = kLocalRef);
	TJObjectRef(jobject sourceObj, bool doCopy = true, TJRefType refType = kLocalRef);
	TJObjectRef(const TJObjectRef& rht);
	TJObjectRef& operator=(const TJObjectRef& rht);

	virtual ~TJObjectRef();

	// set or get fields : only primitive types are acceptable
	template <typename PrimitiveType>
	PrimitiveType field(const std::string& name);

	template <typename JavaType>
	void setField(const std::string& fieldName, JavaType value);

	void setField(const std::string& fieldName, const TJStringRef& string);

	void setField(const std::string& fieldName, jstring string);

	void setField(const std::string& fieldName, const TJObjectRef& object);

	void setField(const std::string& fieldName, jobject object);

	const std::string& descriptor() const {return mClassRef->descriptor();};

	// call methods for objects

	template <typename RetType>
	RetType call(const std::string& methodName, const std::string& signature);

	template <typename RetType, typename ArgType>
	RetType call(const std::string& methodName, const std::string& signature,
									ArgType arg);

	template <typename RetType, typename ArgType1, typename ArgType2>
	RetType call(const std::string& methodName, const std::string& signature,
									ArgType1 arg1, ArgType2 arg2);

	template <typename RetType, typename ArgType1, typename ArgType2, typename ArgType3>
	RetType call(const std::string& methodName, const std::string& signature,
									ArgType1 arg1, ArgType2 arg2, ArgType3 arg3);
	
	template <typename RetType>
	RetType call(const std::string& methodName, const std::string& signature, 
									const TJValue* args, size_t count);

	// creator methods for objects
	static TJObjectRef createObject(const std::string& className, const std::string& constrSignature);

	template <typename ArgType>
	static TJObjectRef createObject(const std::string& className, const std::string& constrSignature,
									ArgType arg);
	
	template <typename ArgType1, typename ArgType2>
	static TJObjectRef createObject(const std::string& className, const std::string& constrSignature,
									ArgType1 arg1, ArgType2 arg2);

	template <typename ArgType1, typename ArgType2, typename ArgType3>
	static TJObjectRef createObject(const std::string& className, const std::string& constrSignature, 
									ArgType1 arg1, ArgType2 arg2, ArgType3 arg3);

	template <typename ArgType1, typename ArgType2, typename ArgType3, typename ArgType4>
	static TJObjectRef createObject(const std::string& className, const std::string& constrSignature, 
									ArgType1 arg1, ArgType2 arg2, ArgType3 arg3, ArgType4 arg4);

	static TJObjectRef createObject(const std::string& className, const std::string& constrSignature, 
									const TJValue* values, size_t count, TJRefType refType);

	// maximal args count for createObject
	static const size_t sMaxArgsCount = 128;

private:
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

extern const char* kConstructorName;

inline TJObjectRef TJObjectRef::createObject(const std::string& className, const std::string& constrSignature)
{
	return createObjectVarArgs(className.c_str(), constrSignature.c_str());
}

template <typename ArgType>
TJObjectRef TJObjectRef::createObject(const std::string& className, const std::string& constrSignature, ArgType arg)
{
	// check types on runtime
	TJ_STATIC_ASSERT(TJTypeTraits<ArgType>::sApplicableArg, "Invalid argument type");
	return createObjectVarArgs(className.c_str(), constrSignature.c_str(), arg);
}

template <typename ArgType1, typename ArgType2>
TJObjectRef TJObjectRef::createObject(const std::string& className, const std::string& constrSignature, ArgType1 arg1, ArgType2 arg2)
{
	// check types on runtime
	TJ_STATIC_ASSERT(TJTypeTraits<ArgType1>::sApplicableArg, "Invalid argument type 1");
	TJ_STATIC_ASSERT(TJTypeTraits<ArgType2>::sApplicableArg, "Invalid argument type 2");

	return createObjectVarArgs(className.c_str(), constrSignature.c_str(), arg1, arg2);
}

template <typename ArgType1, typename ArgType2, typename ArgType3>
TJObjectRef TJObjectRef::createObject(const std::string& className, const std::string& constrSignature, ArgType1 arg1, ArgType2 arg2, ArgType3 arg3)
{
	// check types on runtime
	TJ_STATIC_ASSERT(TJTypeTraits<ArgType1>::sApplicableArg, "Invalid argument type 1");
	TJ_STATIC_ASSERT(TJTypeTraits<ArgType2>::sApplicableArg, "Invalid argument type 2");
	TJ_STATIC_ASSERT(TJTypeTraits<ArgType3>::sApplicableArg, "Invalid argument type 3");

	return createObjectVarArgs(className.c_str(), constrSignature.c_str(), arg1, arg2, arg3);
}

template <typename ArgType1, typename ArgType2, typename ArgType3, typename ArgType4>
TJObjectRef TJObjectRef::createObject(const std::string& className, const std::string& constrSignature, 
									ArgType1 arg1, ArgType2 arg2, ArgType3 arg3, ArgType4 arg4)
{
	// check types on runtime
	TJ_STATIC_ASSERT(TJTypeTraits<ArgType1>::sApplicableArg, "Invalid argument type 1");
	TJ_STATIC_ASSERT(TJTypeTraits<ArgType2>::sApplicableArg, "Invalid argument type 2");
	TJ_STATIC_ASSERT(TJTypeTraits<ArgType3>::sApplicableArg, "Invalid argument type 3");
	TJ_STATIC_ASSERT(TJTypeTraits<ArgType4>::sApplicableArg, "Invalid argument type 4");

	return createObjectVarArgs(className.c_str(), constrSignature.c_str(), arg1, arg2, arg3, arg4);
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

template <typename RetType, typename ArgType>
RetType TJObjectRef::call(const std::string& methodName, const std::string& signature, ArgType arg)
{
	TJ_STATIC_ASSERT(TJTypeTraits<ArgType>::sApplicableArg, "Invalid argument type 1");
	
	return callVarArgs<RetType>(methodName.c_str(), signature.c_str(), arg);
}

template <typename RetType, typename ArgType1, typename ArgType2>
RetType TJObjectRef::call(const std::string& methodName, const std::string& signature, ArgType1 arg1, ArgType2 arg2)
{
	TJ_STATIC_ASSERT(TJTypeTraits<ArgType1>::sApplicableArg, "Invalid argument type 1");
	TJ_STATIC_ASSERT(TJTypeTraits<ArgType2>::sApplicableArg, "Invalid argument type 2");
	
	return callVarArgs(methodName.c_str(), signature.c_str(), arg1, arg2);
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
