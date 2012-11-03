/*
* Copyright (c) 2012
* Bogdan Mytnyk <bogdan.myntyk@gmail.com>
*
* This library is free software; you can redistribute it and/or modify
* it under the terms of the MIT license.
*/

#ifndef __TG_COMMON_H__
#define __TG_COMMON_H__

#include "TJTypes.h"

#include <cassert>
#include <string>

// defines for different operating systems

#define TJ_OS_WINDOWS 0
#define TJ_OS_LINUX 1
#define TJ_OS_ANDROID 2
#define TJ_OS_MACOSX 3

#ifdef _WIN32
#define TJ_THREAD_SPECIFIC _declspec(thread)
#else
// do for other platforms
#endif

#define TJ_USE_CPP11

enum TJJNIVersion
{
	kJNIVersion_1_1 = JNI_VERSION_1_1,
	kJNIVersion_1_2 = JNI_VERSION_1_2,
	kJNIVersion_1_4 = JNI_VERSION_1_4,
	kJNIVersion_1_6 = JNI_VERSION_1_6
};

// Create JavaVM if it was not created
// params:
// libPath - path to *.so or *.dll which contains jvm implementation
// version - jni version needed
// returns:
// JNIEnv pointer for current thread or NULL in case of error
// never throws
JNIEnv* TJCreateJavaVm(const std::string& libPath, TJJNIVersion version);

// Destroys previously created java vm and unloads jvm library
// params:
// none
// return:
// none
// never throws
void TJDestroyJavaVM();

// Get JNIEnv* corresponed to current thread
// params:
// pointer to int which holds error code : can be NULL
// returns:
// JNIEnv8 for current thread or NULL if error happen
// never throws
JNIEnv* TJGetEnvironment(TJInt* pError = NULL);


// Get JNIEnv* corresponed to current thread
// params:
// none
// returns:
// JNIEnv8 for current thread or NULL if error happen
// throws TJNIException on case of failure
JNIEnv* TJGetEnvironmentExc();

// Attaches current thread to a Java VM
// If thread is attached - no - operation
// If pError is not a NULL then return error code
// If succeeded return pointer to current jni environment
// If failed return NULL
JNIEnv* TJAttachCurrentThreadToJNI(TJInt* pError = NULL);

// Detaches current thread from a Java VM
// returns:
// error code
TJInt TJDetachCurrentThreadFromJni();

#define TJ_ASSERT(arg) assert(arg)

#define TJ_UNREFERENCED_PARAMETER(arg) arg

#ifdef WIN32
extern TJ_THREAD_SPECIFIC JNIEnv* sJniEnv;
#endif

#ifndef TJ_STATIC_ASSERT
#define TJ_STATIC_ASSERT(condition, message) static_assert(condition, message)
#endif

#endif //__TG_COMMON_H__
