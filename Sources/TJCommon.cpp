/*
* Copyright (c) 2012
* Bogdan Mytnyk <bogdan.mytnyk@gmail.com>
*
* This library is free software; you can redistribute it and/or modify
* it under the terms of the MIT license.
*/

#include "../TJPch.h"
#include "../Include/TJCommon.h"
#include "../Include/TJTypes.h"
#include "../Include/TJException.h"
#include "../Include/TJPrimitiveArray.h"

#include "jni.h"
#include <cassert>
#include <stdexcept>

#ifdef _WIN32
#include "windows.h"
HINSTANCE sVMLibrary = NULL;
#elif defined (TJ_ANDROID)
#include <dlfcn.h>

void* sVMLibrary = NULL;
#else
// Another platform implementation
#endif

static JavaVM* sJavaVM = NULL;
static const jint kRequiredVersion = JNI_VERSION_1_2;

typedef jint (JNICALL *JNI_CreateJavaVMPtr) (JavaVM **pvm, void **penv, void *args);
typedef jint (JNICALL *JNI_GetDefaultJavaVMInitArgsPtr)(void *args);

//jint JNI_GetCreatedJavaVMs(JavaVM **vmBuf, jsize bufLen, jsize *nVMs);
typedef jint (JNICALL *JNI_GetCreatedJavaVMsPtr)(JavaVM **vmBuf, jsize bufLen, jsize *nVMs);

static JNI_GetCreatedJavaVMsPtr sGetCreatedJavaVMsPtr = NULL;


JNIEnv* TJCreateJavaVm(const std::string& libPath, TJJNIVersion version, const TJStringArray& args)
{
	// check if vm already exists
	if (sJavaVM != NULL)
		return TJGetEnvironment();

	JNI_CreateJavaVMPtr pCreateFunc = NULL;
	JNI_GetDefaultJavaVMInitArgsPtr pGetDefaultArgFunc = NULL;

#ifdef _WIN32
	sVMLibrary = ::LoadLibraryA(libPath.c_str());
	if (sVMLibrary == NULL)
		return NULL;

	// get appropriate function pointers
	pCreateFunc = reinterpret_cast<JNI_CreateJavaVMPtr>(::GetProcAddress(sVMLibrary, "JNI_CreateJavaVM"));
	if (pCreateFunc == NULL)
	{
		::FreeLibrary(sVMLibrary);
		sVMLibrary = NULL;
		return NULL;
	}
	
	pGetDefaultArgFunc = reinterpret_cast<JNI_GetDefaultJavaVMInitArgsPtr>(::GetProcAddress(sVMLibrary, "JNI_GetDefaultJavaVMInitArgs"));
	if (pGetDefaultArgFunc == NULL)
	{
		::FreeLibrary(sVMLibrary);
		sVMLibrary = NULL;
		return NULL;
	}

	/*sGetCreatedJavaVMsPtr = reinterpret_cast<JNI_GetCreatedJavaVMsPtr>(::GetProcAddress(sVMLibrary, "JNI_GetCreatedJavaVMsPtr"));
	if (sGetCreatedJavaVMsPtr == NULL)
	{
		::FreeLibrary(sVMLibrary);
		sVMLibrary = NULL;
		return NULL;
	}*/

#else

	sVMLibrary = dlopen(libPath.c_str(), RTLD_NOW);
	if (sVMLibrary == NULL)
		return NULL;
	
	pCreateFunc = reinterpret_cast<JNI_CreateJavaVMPtr>(dlsym(sVMLibrary, "JNI_CreateJavaVM"));
	if (pCreateFunc == NULL)
	{
		dlclose(sVMLibrary);
		sVMLibrary = NULL;
		return NULL;
	}

	pGetDefaultArgFunc = reinterpret_cast<JNI_GetDefaultJavaVMInitArgsPtr>(dlsym(sVMLibrary, "JNI_GetDefaultJavaVMInitArgs"));
	if (pGetDefaultArgFunc == NULL)
	{
		dlclose(sVMLibrary);
		sVMLibrary = NULL;
		return NULL;
	}

#endif

	JavaVMInitArgs vm_args = {0};
    vm_args.version = static_cast<TJInt>(version);
    
	std::vector<JavaVMOption> optionsArray;
	if (!args.empty())
	{
		size_t optionCount = args.size();
		vm_args.nOptions = static_cast<jint>(optionCount);
		
		optionsArray.resize(optionCount);
		for (size_t i = 0; i < optionCount; ++i)
		{
			optionsArray[i].optionString = (char*)args[i].c_str();
			optionsArray[i].extraInfo = NULL;
		}

		vm_args.options = &optionsArray[0];
		vm_args.ignoreUnrecognized = JNI_TRUE;
	}

	/* Get the default initialization arguments and set the class  path */ 
    //pGetDefaultArgFunc(&vm_args); 
 
    /* load and initialize a Java VM, return a JNI interface  pointer in env */ 
    JNIEnv* env = NULL;
	pCreateFunc(&sJavaVM, reinterpret_cast<void**>(&env), &vm_args); 
	
#ifdef WIN32
	sJniEnv = env;
#endif

	return env;
}

void TJDestroyJavaVM()
{
	if (sVMLibrary != NULL)
	{
#ifdef _WIN32
		// unloads library
		::FreeLibrary(sVMLibrary);
#else
		// unload library on other platform
		dlclose(sVMLibrary);
#endif
		// set all to NULL
		sVMLibrary = NULL;
		sJavaVM = NULL;
	}
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
	TJ_UNREFERENCED_PARAMETER(reserved);
	assert(vm != NULL);
	sJavaVM = vm;
	return kRequiredVersion;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved)
{
	TJ_UNREFERENCED_PARAMETER(vm);
	TJ_UNREFERENCED_PARAMETER(reserved);
	
	sJavaVM = NULL;
}

JNIEnv* TJGetEnvironment(TJInt* pError)
{
	TJ_ASSERT(sJavaVM != NULL);
	JNIEnv* env = NULL;
	if (sJavaVM == NULL)
	{
		if (pError != NULL)
			*pError = kInvalidVM;
		return NULL;
	}

	TJInt res = sJavaVM->GetEnv(reinterpret_cast<void**>(&env), kRequiredVersion);
	if (pError != NULL)
		*pError = res;

	return env;
}

JNIEnv* TJGetEnvironmentExc()
{
	TJInt resCode = kNoError;
	JNIEnv* environment = TJGetEnvironment(&resCode);
	if (environment == NULL)
		throw TJNIException(resCode, "Failed to get jnienv");
	return environment;
}

JNIEnv* TJAttachCurrentThreadToJNI(TJInt* pError)
{
	assert(sJavaVM != NULL);
	JNIEnv* env = NULL;
	if (sJavaVM == NULL)
	{
		if (pError != NULL)
			*pError = kInvalidVM;
		return NULL;
	}


#ifdef WIN32
	TJInt res = sJavaVM->AttachCurrentThread(reinterpret_cast<void**>(&env), NULL);
	if (pError != NULL)
		*pError = res;
	sJniEnv = env;
#else
	TJInt res = sJavaVM->AttachCurrentThread(&env, NULL);
	if (pError != NULL)
		*pError = res;
#endif

	return env;
}

TJInt TJDetachCurrentThreadFromJni()
{
	assert(sJavaVM != NULL);
	if (sJavaVM == NULL)
		return kInvalidVM;

	return sJavaVM->DetachCurrentThread();
}

#ifdef _WIN32
_declspec(thread) JNIEnv* sJniEnv = NULL;
#endif