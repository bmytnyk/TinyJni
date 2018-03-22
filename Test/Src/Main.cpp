#include "TJJavaVMInitializer.h"

#if TJ_OS == TJ_OS_WINDOWS
#include <filesystem>
#endif

#include "../../TinyJni/Include/TJCommon.h"
#include "../../TinyJni/Include/TJPrimitiveArray.h"
#include "../../TinyJni/Include/TJClass.h"
#include "../../TinyJni/Include/TJObjectRef.h"
#include "../../TinyJni/Include/TJString.h"

#include "gtest/gtest.h"

int main(int argc, char* argv[])
{
	TJStringArray options;
	options.push_back("-verbose:jni");
	TJJavaVMInitializer sVmInitializer(kJniChooseLatest, options);

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

