#include "../TJPch.h"
#include "../Include/TJTypeTraits.h"

const TJPrimitiveTypeId TJTypeTraits<TJInt>::sTypeId = kInt;
const TJPrimitiveTypeId TJTypeTraits<TJLong>::sTypeId = kLong;
const TJPrimitiveTypeId TJTypeTraits<TJFloat>::sTypeId = kFloat;
const TJPrimitiveTypeId TJTypeTraits<TJChar>::sTypeId = kChar;
const TJPrimitiveTypeId TJTypeTraits<TJDouble>::sTypeId = kDouble;
const TJPrimitiveTypeId TJTypeTraits<TJBoolean>::sTypeId = kBoolean;
const TJPrimitiveTypeId TJTypeTraits<TJByte>::sTypeId = kByte;
const TJPrimitiveTypeId TJTypeTraits<TJShort>::sTypeId = kShort;

const std::string sFieldDescriptorsArray[kTypesCount] = 
{
	std::string("I"), // kInt
	std::string("J"), // kLong
	std::string("F"), // kFloat
	std::string("C"), // kChar
	std::string("D"), // kDouble
	std::string("Z"), // kBoolean
	std::string("B"), // kByte
	std::string("S"), // kShort
	std::string("V"), // kVoid
	std::string("")   // kObject
};

#define TJTypeTraitsImpl(TYPE)																					\
	TJ##TYPE TJTypeTraits<TJ##TYPE>::CallStaticMethod(JNIEnv* env, jclass classHandle, jmethodID methodID, ...)	\
	{																											\
		TJ_ASSERT(env != NULL);																					\
		TJ_ASSERT(classHandle != NULL);																			\
		TJ_ASSERT(methodID != NULL);																			\
																												\
		va_list args;																							\
		va_start(args, methodID);																				\
		TJ##TYPE resultValue = env->CallStatic##TYPE##MethodV(classHandle, methodID, args);						\
		va_end(args);																							\
																												\
		return resultValue;																						\
	}																											\
																												\
	TJ##TYPE TJTypeTraits<TJ##TYPE>::GetStaticField(JNIEnv* env, jclass classHandle, jfieldID fieldId)			\
	{																											\
		TJ_ASSERT(env != NULL);																					\
		TJ_ASSERT(classHandle != NULL);																			\
		TJ_ASSERT(fieldId != NULL);																				\
																												\
		return env->GetStatic##TYPE##Field(classHandle, fieldId);												\
	}																											\
																												\
	void TJTypeTraits<TJ##TYPE>::SetStaticField(JNIEnv* env, jclass classHandle, jfieldID fieldId, TJ##TYPE value)			\
	{																											\
		TJ_ASSERT(env != NULL);																					\
		TJ_ASSERT(classHandle != NULL);																			\
		TJ_ASSERT(fieldId != NULL);																				\
																												\
		env->SetStatic##TYPE##Field(classHandle, fieldId, value);												\
	}																											\
	TJ##TYPE TJTypeTraits<TJ##TYPE>::GetField(JNIEnv* env, jobject objectHandle, jfieldID fieldId)				\
	{																											\
		TJ_ASSERT(env != NULL);																					\
		TJ_ASSERT(objectHandle != NULL);																		\
		TJ_ASSERT(fieldId != NULL);																				\
																												\
		return env->Get##TYPE##Field(objectHandle, fieldId);													\
	}																											\
	void TJTypeTraits<TJ##TYPE>::SetField(JNIEnv* env, jobject objectHandle, jfieldID fieldId, TJ##TYPE value)	\
	{																											\
		TJ_ASSERT(env != NULL);																					\
		TJ_ASSERT(objectHandle != NULL);																		\
		TJ_ASSERT(fieldId != NULL);																				\
																												\
		return env->Set##TYPE##Field(objectHandle, fieldId, value);												\
	}																											\
	TJ##TYPE TJTypeTraits<TJ##TYPE>::CallMethod(JNIEnv* env, jobject objHandle, jmethodID methodID, ...)		\
	{																											\
		TJ_ASSERT(env != NULL);																					\
		TJ_ASSERT(objHandle != NULL);																			\
		TJ_ASSERT(methodID != NULL);																			\
																												\
		va_list args;																							\
		va_start(args, methodID);																				\
		TJ##TYPE resultValue = env->Call##TYPE##MethodV(objHandle, methodID, args);								\
		va_end(args);																							\
																												\
		return resultValue;																						\
	}																											\
	TJ##TYPE TJTypeTraits<TJ##TYPE>::CallMethodV(JNIEnv* env, jobject objHandle, jmethodID methodID, va_list args)\
	{																											\
		TJ_ASSERT(env != NULL);																					\
		TJ_ASSERT(objHandle != NULL);																			\
		TJ_ASSERT(methodID != NULL);																			\
		TJ##TYPE resValue = env->Call##TYPE##MethodV(objHandle, methodID, args);								\
		return resValue;																						\
	}																											\
	TJ##TYPE TJTypeTraits<TJ##TYPE>::CallStaticMethod(JNIEnv* env, jclass classHandle, jmethodID methodID, jvalue* args)  \
	{																														\
		TJ_ASSERT(env != NULL);																					\
		TJ_ASSERT(classHandle != NULL);																			\
		TJ_ASSERT(methodID != NULL);																			\
		TJ_ASSERT(args != NULL);																				\
		return env->CallStatic##TYPE##MethodA(classHandle, methodID, args);										\
	}


TJTypeTraitsImpl(Int)
TJTypeTraitsImpl(Double)
TJTypeTraitsImpl(Float)
TJTypeTraitsImpl(Long)
TJTypeTraitsImpl(Short)
TJTypeTraitsImpl(Byte)
TJTypeTraitsImpl(Char)
TJTypeTraitsImpl(Boolean)

jobject TJTypeTraits<jobject>::CallStaticMethod(JNIEnv* env, jclass classHandle, jmethodID methodID, ...)
{																										
	TJ_ASSERT(env != NULL);																				
	TJ_ASSERT(classHandle != NULL);																		
	TJ_ASSERT(methodID != NULL);																		
																										
	va_list args;																							
	va_start(args, methodID);																			
	jobject resultValue = env->CallStaticObjectMethodV(classHandle, methodID, args);					
	va_end(args);																						
																										
	return resultValue;																					
}

jobject TJTypeTraits<jobject>::GetStaticField(JNIEnv* env, jclass classHandle, jfieldID fieldId)
{
	TJ_ASSERT(env != NULL);
	TJ_ASSERT(classHandle != NULL);
	TJ_ASSERT(fieldId != NULL);

	return env->GetStaticObjectField(classHandle, fieldId);
}

void TJTypeTraits<jobject>::SetStaticField(JNIEnv* env, jclass classHandle, jfieldID fieldId, jobject value)
{
	TJ_ASSERT(env != NULL);
	TJ_ASSERT(classHandle != NULL);
	TJ_ASSERT(fieldId != NULL);

	env->SetStaticObjectField(classHandle, fieldId, value);
}

jobject TJTypeTraits<jobject>::GetField(JNIEnv* env, jobject objectHandle, jfieldID fieldId)
{
	TJ_ASSERT(env != NULL);
	TJ_ASSERT(objectHandle != NULL);
	TJ_ASSERT(fieldId != NULL);

	return env->GetObjectField(objectHandle, fieldId);
}

void TJTypeTraits<jobject>::SetField(JNIEnv* env, jobject objectHandle, jfieldID fieldId, jobject value)
{
	TJ_ASSERT(env != NULL);
	TJ_ASSERT(objectHandle != NULL);
	TJ_ASSERT(fieldId != NULL);

	return env->SetObjectField(objectHandle, fieldId, value);
}

jobject TJTypeTraits<jobject>::CallMethod(JNIEnv* env, jobject objHandle, jmethodID methodID, ...)
{
	TJ_ASSERT(env != NULL);
	TJ_ASSERT(objHandle != NULL);
	TJ_ASSERT(methodID != NULL);

	va_list args;
	va_start(args, methodID);
	jobject resultValue = env->CallObjectMethodV(objHandle, methodID, args);
	va_end(args);

	return resultValue;
}

jobject TJTypeTraits<jobject>::CallMethodV(JNIEnv* env, jobject objHandle, jmethodID methodID, va_list args)
{
	TJ_ASSERT(env != NULL);
	TJ_ASSERT(objHandle != NULL);
	TJ_ASSERT(methodID != NULL);
	return env->CallObjectMethodV(objHandle, methodID, args);																					\
}

// TODO : specializations for void