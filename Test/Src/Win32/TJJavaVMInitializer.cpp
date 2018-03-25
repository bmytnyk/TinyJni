#include "windows.h"
#include "shlwapi.h"

#include <array>

#include "TJJavaVMInitializer.h"

#pragma comment(lib, "shlwapi.lib")

TJJavaVMInitializer::TJJavaVMInitializer(TJJNIVersion prefferedVersion, const TJStringArray& options) :
	mOptions(options),
	mVersion(prefferedVersion)
{
	char buffer[1024];
	DWORD charsCount = ::GetEnvironmentVariableA("JAVA_HOME", buffer, sizeof(buffer));
	if (charsCount == 0)
		throw std::runtime_error("Failed to get JAVA_HOME environment variable");
	
	std::string jreFolderPath(buffer, charsCount);

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

std::string TJJavaVMInitializer::GetJavaHomePathFromRegistry() const
{
	std::string javaHomePath;
	static std::array<const char*, 2> sPossibleJVMLocationRegPathes =
	{
		"SOFTWARE\\Wow6432Node\\JavaSoft\\Java Development Kit",
		"SOFTWARE\\JavaSoft\\Java Development Kit"
	};

	bool pathFound = false;
	for (auto it = sPossibleJVMLocationRegPathes.begin(); (it != sPossibleJVMLocationRegPathes.end()) && (!pathFound); ++it)
	{
		const char* regPath = *it;
		HKEY regKey = NULL;
		long error = ::RegOpenKeyA(HKEY_LOCAL_MACHINE, regPath, &regKey);
		if (error != NO_ERROR)
			continue;

		// enumerate all subkeys
		char name[64];
		DWORD index = 0;

		while (!pathFound && (error == NO_ERROR))
		{
			DWORD nameBytesCount = sizeof(name);
			error = ::RegEnumKeyExA(regKey, index, name, &nameBytesCount, NULL, NULL, NULL, NULL);
			if (error != NO_ERROR)
				break;

			++index;
			HKEY subKey = NULL;

			error = ::RegOpenKeyA(regKey, name, &subKey);
			if (error != NO_ERROR)
				continue;

			char name[256];
			char jniLibraryPath[1024];

			bool keyFound = false;
			while (!keyFound)
			{
				DWORD nameByteCount = sizeof(name);
				DWORD jniLibraryPathCount = sizeof(jniLibraryPath);
				LSTATUS result = ::RegEnumValueA(subKey, 0, name, &nameByteCount, NULL, NULL, reinterpret_cast<unsigned char*>(jniLibraryPath), &jniLibraryPathCount);
				if (result == NO_ERROR)
				{
					static const char* sJavaPathKeyName = "JavaHome";
					if (strcmp(name, sJavaPathKeyName) == 0)
					{
						javaHomePath.assign(jniLibraryPath);
						keyFound = true;
					}
				}
				else
				{
					break;
				}
			}

			::RegCloseKey(subKey);
		};

		::RegCloseKey(regKey);
	}

	return javaHomePath;
}

