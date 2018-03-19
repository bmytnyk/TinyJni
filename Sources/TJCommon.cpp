/*
* Copyright (c) 2015
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

#if TJ_OS == TJ_OS_WINDOWS
#include "windows.h"
HINSTANCE sVMLibrary = NULL;
#else
#include <dlfcn.h>
void* sVMLibrary = NULL;
#endif

namespace
{
	typedef jint (JNICALL *JNI_CreateJavaVMPtr) (JavaVM **pvm, void **penv, void *args);
	typedef jint (JNICALL *JNI_GetDefaultJavaVMInitArgsPtr)(void *args);
	typedef jint (JNICALL *JNI_GetCreatedJavaVMsPtr)(JavaVM **vmBuf, jsize bufLen, jsize *nVMs);

	JavaVM* sJavaVM = nullptr;
	jint sCreatedVersion = JNI_VERSION_1_2;
	JNI_GetCreatedJavaVMsPtr sGetCreatedJavaVMsPtr = nullptr;
	JNI_CreateJavaVMPtr pCreateFunc = nullptr;
	JNI_GetDefaultJavaVMInitArgsPtr pGetDefaultArgFunc = nullptr;
};


JNIEnv* TJCreateJavaVm(const std::string& libPath, TJJNIVersion version, const TJStringArray& args)
{
	// check if vm already exists
	if (sJavaVM != nullptr)
		return TJGetEnvironment();

#if TJ_OS == TJ_OS_WINDOWS
	sVMLibrary = ::LoadLibraryA(libPath.c_str());
	if (sVMLibrary == nullptr)
		return nullptr;

	// get appropriate function pointers
	pCreateFunc = reinterpret_cast<JNI_CreateJavaVMPtr>(::GetProcAddress(sVMLibrary, "JNI_CreateJavaVM"));
	if (pCreateFunc == nullptr)
	{
		::FreeLibrary(sVMLibrary);
		sVMLibrary = nullptr;
		return nullptr;
	}
	
	pGetDefaultArgFunc = reinterpret_cast<JNI_GetDefaultJavaVMInitArgsPtr>(::GetProcAddress(sVMLibrary, "JNI_GetDefaultJavaVMInitArgs"));
	if (pGetDefaultArgFunc == nullptr)
	{
		::FreeLibrary(sVMLibrary);
		sVMLibrary = nullptr;
		return nullptr;
	}

	sGetCreatedJavaVMsPtr = reinterpret_cast<JNI_GetCreatedJavaVMsPtr>(::GetProcAddress(sVMLibrary, "JNI_GetCreatedJavaVMs"));
	if (sGetCreatedJavaVMsPtr == NULL)
	{
		::FreeLibrary(sVMLibrary);
		sVMLibrary = nullptr;
		return nullptr;
	}

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
    
    sGetCreatedJavaVMsPtr = reinterpret_cast<JNI_GetCreatedJavaVMsPtr>(dlsym(sVMLibrary, "JNI_GetCreatedJavaVMs"));
    if (sGetCreatedJavaVMsPtr == NULL)
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
	sCreatedVersion = version;

	return env;
}

void TJDestroyJavaVM() noexcept
{
	if (sVMLibrary != nullptr)
	{
		if (sJavaVM != nullptr)
			sJavaVM->DestroyJavaVM();

#if TJ_OS == TJ_OS_WINDOWS
		// unloads library
		::FreeLibrary(sVMLibrary);
#else
		// unload library on other platform
		dlclose(sVMLibrary);
#endif
		// set all to NULL
		sVMLibrary = nullptr;
		sJavaVM = nullptr;
	}
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
	TJ_UNREFERENCED_PARAMETER(reserved);
	assert(vm != NULL);
	sJavaVM = vm;
	return sCreatedVersion;

}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved)
{
	TJ_UNREFERENCED_PARAMETER(vm);
	TJ_UNREFERENCED_PARAMETER(reserved);
	
	sJavaVM = NULL;
}

JNIEnv* TJGetEnvironment(TJInt* pError) noexcept
{
	//TJ_ASSERT(sJavaVM != NULL);
	JNIEnv* env = NULL;
	if (sJavaVM == NULL)
	{
		if (pError != NULL)
			*pError = kInvalidVM;
		return NULL;
	}

	TJInt res = sJavaVM->GetEnv(reinterpret_cast<void**>(&env), sCreatedVersion);
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
	JNIEnv* env = nullptr;
	if (sJavaVM == nullptr)
	{
		if (pError != nullptr)
			*pError = kInvalidVM;
		return nullptr;
	}

#if TJ_OS != TJ_OS_ANDROID
	void** pEnvironment = reinterpret_cast<void**>(&env);
#else
	JNIEnv** pEnvironment = &env;
#endif

	TJInt res = sJavaVM->AttachCurrentThread(pEnvironment, NULL);
	if (pError != nullptr)
		*pError = res;
	return env;
}

TJInt TJDetachCurrentThreadFromJni()
{
	if (sJavaVM == nullptr)
		return kInvalidVM;

	return sJavaVM->DetachCurrentThread();
}
