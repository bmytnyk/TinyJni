#include "TJJavaVMInitializer.h"

#include <memory>

TJJavaVMInitializer::TJJavaVMInitializer(TJJNIVersion prefferedVersion, const TJStringArray& options) :
	mOptions(options),
	mVersion(prefferedVersion)
{
	std::string jreFolderPath = GetJavaHomePath();
	if (jreFolderPath.empty())
		throw std::runtime_error("Java home path not found");

	std::string jvmFullPath(jreFolderPath + "/jre/lib/amd64/server/libjvm.so");

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
		throw std::runtime_error("Failed to load JVM");
}

TJJavaVMInitializer::~TJJavaVMInitializer() noexcept
{
	TJDestroyJavaVM();
}

std::string TJJavaVMInitializer::GetJavaHomePath() const
{
	const char* javaHome = getenv("JAVA_HOME");
	if (javaHome == nullptr)
        throw std::runtime_error("Can't found JAVA_HOME environment variable");

    return std::string(javaHome);
}
