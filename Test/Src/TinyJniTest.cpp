#include <windows.h>

#include "../../TinyJni/Include/TJCommon.h"
#include "../../TinyJni/Include/TJPrimitiveArray.h"
#include "../../TinyJni/Include/TJClass.h"
#include "../../TinyJni/Include/TJObjectRef.h"
#include "../../TinyJni/Include/TJString.h"

#include <iostream>
#include <limits>
#include <array>

#include "gtest/gtest.h"

TEST(TestStaticFields, TestDoubleClass)
{
	const std::string doubleClassName("java/lang/Double");
	TJClassRef doubleClass = TJClassRef::FindClass(doubleClassName, kGlobalRef);

	const std::string positiveInfinityName("POSITIVE_INFINITY");
	const TJDouble infinityPositive = doubleClass.field<TJDouble>(positiveInfinityName);
	EXPECT_EQ(infinityPositive, std::numeric_limits<double>::infinity());
		
	const std::string negativeInfinityName("NEGATIVE_INFINITY");
	const TJDouble infinityNegative = doubleClass.field<TJDouble>(negativeInfinityName);
	EXPECT_EQ(infinityNegative, -std::numeric_limits<double>::infinity());

	EXPECT_EQ(doubleClass.field<TJInt>("SIZE"), 64);

	// Expect no exception
	doubleClass.field("TYPE", "Ljava/lang/Class;", kGlobalRef);

	// Expect Java Exception
	EXPECT_THROW(doubleClass.field<TJDouble>("UnknownField"), TJJavaException);

	//const TJInt res = doubleClass.call<TJInt, TJDouble, TJDouble>("compare", "(DD)I", 6.0, 5.0);
	//EXPECT_EQ(res, 1);
}

bool TestClassMethods()
{
	bool succeeded = false;

	try
	{
		const std::string doubleClassName("java/lang/Double");
		TJClassRef doubleClass = TJClassRef::FindClass(doubleClassName, kGlobalRef);

		std::cout << "Double class descriptor - " << doubleClass.descriptor() <<  std::endl;
		
		// check field method  on examples
		const std::string positiveInfinityName("POSITIVE_INFINITY");
		TJDouble infinityPositive = doubleClass.field<TJDouble>(positiveInfinityName);
		if (infinityPositive != std::numeric_limits<double>::infinity())
		{
			std::cout << "POSITIVE_INFINITY static field in Double class incorrect - " << infinityPositive << std::endl;
			return false;
		}

		const std::string negativeInfinityName("NEGATIVE_INFINITY");
		TJDouble infinityNegative = doubleClass.field<TJDouble>(negativeInfinityName);
		if (infinityNegative != - std::numeric_limits<double>::infinity())
		{
			std::cout << "NEGATIVE_INFINITY static field in Double class incorrect - " << infinityPositive << std::endl;
			return false;
		}

		TJInt bitsCount = doubleClass.field<TJInt>("SIZE");
		if (bitsCount != 64)
		{
			std::cout << "SIZE static field in Double class incorrect - " << bitsCount << std::endl;
			return false;
		}

		TJObjectRef typeField = doubleClass.field("TYPE", "Ljava/lang/Class;", kGlobalRef); 

		// absence field "UnknownField"
		try
		{
			doubleClass.field<TJDouble>("UnknownField");
			succeeded = false;
		}
		catch(const TJJavaException& exc)
		{
			std::cout << "Needed behaviour : exception occured - " << exc.what() << std::endl;
			succeeded = true;
		}

		if (!succeeded)
		{
			std::cout << "Java exception was not thrown when needed" << std::endl;
			return false;
		}

		//doubleClass.call<jobject>("clone", "()Ljava/lang/Object;");


		TJInt res = doubleClass.call<TJInt, TJDouble, TJDouble>("compare", "(DD)I", 6.0, 5.0);
		if (res != 1)
		{
			std::cout << "Invalid return value for compare method";
			return false;
		}

		succeeded = true;
	}
	catch(std::exception& exc)
	{
		std::cout << "Test failed : exception occured - " << exc.what() << std::endl;
		succeeded = false;
	}
	catch(...)
	{
		std::cout << "Test failed : unknown exception occured";
		succeeded = false;
	}

	return succeeded;
}

bool TestPrimitiveArray()
{
	const size_t kSizeToTest = 1024;
	TJJavaPrimitiveArray<TJInt> sampleArray = TJJavaPrimitiveArray<TJInt>::CreateArray(kSizeToTest, kGlobalRef);

	TJInt* rawArray = sampleArray.getPtr();
	if (rawArray == nullptr)
	{
		std::cout << "Failed to get raw array pointer\n" << std::endl;
		return false;
	}

	for (size_t i = 0; i < kSizeToTest; ++i)
		rawArray[i] = static_cast<TJInt>(i);

	TJInt regionCorrect[16] = {0};
	sampleArray.getRegion(0, 16, regionCorrect);

	if (regionCorrect[8] != 8)
	{
		std::cout << "Failed to get correct region" << std::endl;
		return false;
	}

	rawArray = sampleArray.getPtr();
	if (rawArray[0] != 0xFFFFFFFF)
		return false;
	
	return true;
}
