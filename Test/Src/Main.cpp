#include <windows.h>

#include "../../TinyJni/Include/TJCommon.h"
#include "../../TinyJni/Include/TJPrimitiveArray.h"
#include "../../TinyJni/Include/TJClass.h"
#include "../../TinyJni/Include/TJObjectRef.h"
#include "../../TinyJni/Include/TJString.h"

#include "../Include/TJClassTest.h"
#include "../Include/TJStringTest.h"

#include "gtest/gtest.h"
#include "shlwapi.h"

#include <array>
#include <filesystem>

#pragma comment(lib, "jvm.lib")
#pragma comment(lib, "shlwapi.lib")

class JavaVMInitializer
{
public:
	JavaVMInitializer(TJJNIVersion prefferedVersion, const TJStringArray& options = TJStringArray());
	~JavaVMInitializer() noexcept;

private:
	TJStringArray mOptions;
	TJJNIVersion mVersion;

private:
	std::string GetJavaHomePathFromRegistry() const;

	// no implementation
	JavaVMInitializer(const JavaVMInitializer&) = delete;
	JavaVMInitializer& operator=(const JavaVMInitializer&) = delete;
};

JavaVMInitializer::JavaVMInitializer(TJJNIVersion prefferedVersion, const TJStringArray& options):
	mOptions(options),
	mVersion(prefferedVersion)
{
	std::string jreFolderPath = GetJavaHomePathFromRegistry();
	if (jreFolderPath.empty())
		throw std::runtime_error("Java home path not found");

	const char* sClientPath = "\\jre\\bin\\client\\jvm.dll";
	const char* sServerPath = "\\jre\\bin\\server\\jvm.dll";

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

JavaVMInitializer::~JavaVMInitializer()
{
	TJDestroyJavaVM();
}

std::string JavaVMInitializer::GetJavaHomePathFromRegistry() const
{
	std::string javaHomePath;
	static std::array<const char*, 2> sPossibleJVMLocationRegPathes =
	{
		"SOFTWARE\\Wow6432Node\\JavaSoft\\Java Development Kit",
		"SOFTWARE\\JavaSoft\\Java Development Kit"
	};

	bool pathFound = 0;
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

int main(int argc, char* argv[])
{
	TJStringArray options;
	options.push_back("-verbose:jni");
	JavaVMInitializer sVmInitializer(kJniChooseLatest, options);

	try
	{
		::testing::InitGoogleTest(&argc, argv);
		int res = RUN_ALL_TESTS();
		/*TJStringRef str = TJStringRef::createString("Hello world", kGlobalRef);

		TJInt minInt = intClass.field<TJInt>(std::string("MIN_VALUE"));

		TJObjectRef inObject = TJObjectRef::createObject("java/lang/Integer", std::string("(I)V"), (TJInt)5);

		TJInt capacity = 10;
		TJObjectRef vectorObject = TJObjectRef::createObject("java/util/Vector", std::string("(I)V"), capacity);

		TJClassRef vectorClass = TJClassRef::FindClass("java/util/Vector", kGlobalRef);
		TJObjectRef typeClassRef = intClass.field("TYPE", "Ljava/lang/Class;", kGlobalRef);

		TJInt elementCount = vectorObject.field<TJInt>("elementCount");
		TJInt capacityRes = vectorObject.call<TJInt>(std::string("capacity"), std::string("()I"));

		jobjectRefType refType = tjEnv->GetObjectRefType(inObject.get());
		TJInt resValue = inObject.call<TJByte>(std::string("byteValue"), std::string("()B"));

		TJInt maxInt = intClass.field<TJInt>(std::string("MAX_VALUE"));
		intClass.call<TJByte>("byteValue", "Z()");

		TJClassRef strClass = TJClassRef::FindClass("java/lang/String", kGlobalRef);
		strClass.call<TJInt>(std::string("methodInt"), std::string("()I"));
		TJClassRef newClassRef = strClass;

		TJJavaByteArray byteArray = TJJavaByteArray::CreateArray(1024 * 16, kGlobalRef);
		TJByte* elements = byteArray.getPtr();
		elements[0] = 54;

		byteArray.releasePtr();
		elements[0] = 54;*/
	}
	catch (const TJJavaException& exc)
	{
		std::cout << exc.what() << std::endl;
	}
	catch (const std::exception& exc)
	{
		std::cout << exc.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "Unknown exception occured" << std::endl;
	}


	return 0;
}

