#include "TJJavaVMInitializer.h"

#include <memory>

TJJavaVMInitializer::TJJavaVMInitializer(TJJNIVersion prefferedVersion, const TJStringArray& options) :
	mOptions(options),
	mVersion(prefferedVersion)
{
	std::string jreFolderPath = GetJavaHomePath();
	if (jreFolderPath.empty())
		throw std::runtime_error("Java home path not found");

	std::string jvmFullPath(jreFolderPath + "/../MacOS/libjli.dylib");

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
    std::shared_ptr<FILE> pipe(popen("/usr/libexec/java_home", "r"), pclose);
    if (!pipe.get())
        throw std::runtime_error("Failed to execute java_home");
    
    char buffer[1024];
    size_t actualSize = fread(buffer, 1, sizeof(buffer) - 1, pipe.get());
    if (actualSize == 0)
        throw std::runtime_error("Invalid result of java_home execution");
        
    return std::string(buffer, actualSize - 1);
}
