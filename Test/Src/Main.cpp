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
	int res = 0;

	try
	{
		TJStringArray options;
		options.push_back("-verbose:jni");
		TJJavaVMInitializer sVmInitializer(kJniChooseLatest, options);


		::testing::InitGoogleTest(&argc, argv);
		
		res = RUN_ALL_TESTS();

	}
	catch (const TJJavaException& exc)
	{
		std::cout << exc.what() << std::endl;
		res = -1;
	}
	catch (const std::exception& exc)
	{
		std::cout << exc.what() << std::endl;
		res = -1;
	}
	catch (...)
	{
		std::cout << "Unknown exception occured" << std::endl;
		res = -1;
	}


	return res;
}

