#include "../Include/TJClass.h"

#include <cassert>
#include <iostream>

#include "gtest/gtest.h"

namespace
{
	const std::string sTestedValidClass = "java/lang/Integer";
	const std::string sTestedValidClass2 = "java/lang/Double";
	const std::string sTestedInvalidClass = "java/lang/Integer2";
};

TEST(TestTJClass, TestConstructionCorrect)
{
	TJClassRef validClassRef = TJClassRef::FindClass(sTestedValidClass, kGlobalRef);
	EXPECT_NE(validClassRef.get(), nullptr) << "FindClass return nullptr for correct valid class - " << sTestedValidClass;
}

TEST(TestTJClass, TestInvalidConstruction)
{
	EXPECT_THROW(TJClassRef::FindClass(sTestedInvalidClass, kGlobalRef), TJJavaException);

	EXPECT_ANY_THROW(TJClassRef invalidClass(nullptr)) << "Should throw exception on invalid construction";
}

TEST(TestTJClass, TestEquality)
{
	TJClassRef validClass = TJClassRef::FindClass("java/lang/Integer", kGlobalRef);
	TJClassRef validClassCopy(validClass);
	EXPECT_EQ(validClassCopy, validClass) << "Not equal classes (after copy c-tor)";

	validClassCopy = validClass;
	EXPECT_EQ(validClassCopy, validClass) << "Not equal classes(after operator =)";

	TJClassRef anotherValidClass = TJClassRef::FindClass(sTestedValidClass2, kGlobalRef);
	EXPECT_NE(anotherValidClass, validClass) << "Different classes equal";
}

TEST(TestTJClass, TestMove)
{
	TJClassRef testedClass = TJClassRef::FindClass(sTestedValidClass, kGlobalRef);
	auto testedClassId = testedClass.get();
	TJClassRef movedClass(std::move(testedClass));

	EXPECT_EQ(testedClass.get(), nullptr) << "After move handle must be zero";

	EXPECT_EQ(movedClass.get(), testedClassId);
}
