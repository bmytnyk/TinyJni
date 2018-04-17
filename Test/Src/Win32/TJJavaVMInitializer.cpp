#include "windows.h"
#include "shlwapi.h"

#include <array>

#include "TJJavaVMInitializer.h"

#pragma comment(lib, "shlwapi.lib")

TJJavaVMInitializer::TJJavaVMInitializer(TJJNIVersion prefferedVersion, const TJStringArray& options) :
	mOptions(options),
	mVersion(prefferedVersion)
{
	std::string jreFolderPath = GetJavaHomePath();

	const char* const sClientPath = "\\jre\\bin\\client\\jvm.dll";
	const char* const sServerPath = "\\jre\\bin\\server\\jvm.dll";

	std::string jvmFullPath(jreFolderPath);
	jvmFullPath += sClientPath;
	if (!PathFileExists(jvmFullPath.c_str()))
	{
		jvmFullPath = jreFolderPath + sServerPath;
	}

	bool libraryLoaded = false;
	if (prefferedVersion == kJniChooseLatest)
	{
		int versionToCreate = kJniVersionLast;
		for (; (versionToCreate >= kJniVersionFirst) && !libraryLoaded; --versionToCreate)
		{
			JNIEnv* jniEnv = TJCreateJavaVm(jvmFullPath, static_cast<TJJNIVersion>(versionToCreate), options);
			libraryLoaded = (jniEnv != nullptr);
		}

		mVersion = static_cast<TJJNIVersion>(versionToCreate);
	}
	else
	{
		// by default create jni version 1_2
		JNIEnv* jniEnv = TJCreateJavaVm(jvmFullPath, prefferedVersion, options);
		libraryLoaded = (jniEnv != nullptr);
	}

	if (!libraryLoaded)
		throw std::runtime_error("Failed to load jvm.dll");
}

TJJavaVMInitializer::~TJJavaVMInitializer() noexcept
{
	TJDestroyJavaVM();
}

std::string TJJavaVMInitializer::GetJavaHomePath() const
{
	char buffer[1024];
	DWORD charsCount = ::GetEnvironmentVariableA("JAVA_HOME", buffer, sizeof(buffer));
	if (charsCount == 0)
		throw std::runtime_error("Failed to get JAVA_HOME environment variable");

	return std::string(buffer, charsCount);
}

