/*
* Copyright (c) 2015
* Bogdan Mytnyk <bogdan.mytnyk@gmail.com>
*
* This library is free software; you can redistribute it and/or modify
* it under the terms of the MIT license.
*/

#ifndef __TG_COMMON_H__
#define __TG_COMMON_H__

#include "TJTypes.h"

#include <cassert>
#include <string>
#include <vector>

// defines for different operating systems

#define TJ_OS_WINDOWS   0
#define TJ_OS_LINUX     1
#define TJ_OS_ANDROID   2
#define TJ_OS_MACOSX    3

#ifdef _WIN32
#define TJ_OS TJ_OS_WINDOWS
#define TJ_THREAD_SPECIFIC _declspec(thread)
#elif defined(__APPLE__)
#define TJ_OS TJ_OS_MACOSX
#elif deinfed(__ANDROID__)
#define TJ_OS TJ_OS_ANDROID
#else
// do for other platforms
#endif

enum TJJNIVersion
{
	kJNIVersion_1_1 = JNI_VERSION_1_1,
	kJNIVersion_1_2 = JNI_VERSION_1_2,
	kJNIVersion_1_4 = JNI_VERSION_1_4,
	kJNIVersion_1_6 = JNI_VERSION_1_6,

	// Additional constants
	kJniVersionLast = JNI_VERSION_1_6,
	kJniVersionFirst = JNI_VERSION_1_1,
	kJniChooseLatest = static_cast<uint32_t>(-1)
};

// Create JavaVM if it was not created
// params:
// libPath - path to *.so or *.dll which contains jvm implementation
// version - jni version needed
// returns:
// JNIEnv pointer for current thread or NULL in case of error
// never throws

typedef std::vector<std::string> TJStringArray;

JNIEnv* TJCreateJavaVm(const std::string& libPath, TJJNIVersion version, const TJStringArray& args = TJStringArray());

// Destroys previously created java vm and unloads jvm library
// params:
// none
// return:
// none
// never throws
void TJDestroyJavaVM() noexcept;

// Get JNIEnv* corresponed to current thread
// params:
// pointer to int which holds error code : can be NULL
// returns:
// JNIEnv* for current thread or nullptr if error happen
// never throws
JNIEnv* TJGetEnvironment(TJInt* pError = nullptr) noexcept;

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
JNIEnv* TJAttachCurrentThreadToJNI(TJInt* pError = nullptr);

// Detaches current thread from a Java VM
// returns:
// error code
TJInt TJDetachCurrentThreadFromJni();

#define TJ_ASSERT(arg) assert(arg)

#define TJ_UNREFERENCED_PARAMETER(arg) (void)arg

#ifndef TJ_STATIC_ASSERT
#define TJ_STATIC_ASSERT(condition, message) static_assert(condition, message)
#endif

#endif //__TG_COMMON_H__
