#include "../Include/TJPrimitiveArray.h"

#include <cassert>
#include <iostream>
#include <algorithm>

#include "gtest/gtest.h"

template <typename PrimitiveType>
class TJPrimitiveArrayTest : public ::testing::Test
{
private:
	// Sets up the test fixture.
	virtual void SetUp();

	// Tears down the test fixture.
	virtual void TearDown();
};

template <typename PrimitiveType>
void TJPrimitiveArrayTest<PrimitiveType>::SetUp()
{
}

template <typename PrimitiveType>
void TJPrimitiveArrayTest<PrimitiveType>::TearDown()
{
}

typedef ::testing::Types<TJChar, TJInt, TJFloat, TJDouble, TJSize, TJShort, TJLong, TJByte> AllPrimitiveTypes;
TYPED_TEST_CASE(TJPrimitiveArrayTest, AllPrimitiveTypes);

TYPED_TEST(TJPrimitiveArrayTest, Initialization)
{
	const TJSize arrayLength = 256 + (std::rand() * 256) / RAND_MAX;
	TJJavaPrimitiveArray<TypeParam> testedArray = TJJavaPrimitiveArray<TypeParam>::CreateArray(arrayLength, kLocalRef);
	EXPECT_NE(testedArray.get(), nullptr);

	TypeParam* pRawInfo = testedArray.getPtr();
	EXPECT_NE(pRawInfo, nullptr);
	pRawInfo[0] = TypeParam();

	EXPECT_EQ(testedArray.count(), arrayLength);
}

TYPED_TEST(TJPrimitiveArrayTest, IncorrectInitialization)
{
	EXPECT_THROW(TJJavaPrimitiveArray<TypeParam>::CreateArray(0, kLocalRef), TJInvalidArgument);
	EXPECT_THROW(TJJavaPrimitiveArray<TypeParam>::CreateArray(-1, kLocalRef), TJInvalidArgument);
}

TYPED_TEST(TJPrimitiveArrayTest, TooBigArraySize)
{
	EXPECT_ANY_THROW(TJJavaPrimitiveArray<TypeParam>::CreateArray(std::numeric_limits<TJSize>::max(), kLocalRef));
}

TYPED_TEST(TJPrimitiveArrayTest, RegionAccess)
{
	TJJavaPrimitiveArray<TypeParam> testedArray = TJJavaPrimitiveArray<TypeParam>::CreateArray(1024, kLocalRef);

	const size_t regionSize = 64;
	TypeParam regionCorrect[regionSize] = {0};
	for (size_t i = 0; i < regionSize; ++i)
	{
		regionCorrect[i] = static_cast<TypeParam>(i);
	}
	testedArray.setRegion(0, regionSize, regionCorrect);

	testedArray.getRegion(0, regionSize, regionCorrect);
	for (size_t i = 0; i < regionSize; ++i)
	{
		EXPECT_EQ(regionCorrect[i], static_cast<TypeParam>(i));
	}
}

TYPED_TEST(TJPrimitiveArrayTest, IncorrectRegionAccess)
{
	TJJavaPrimitiveArray<TypeParam> testedArray = TJJavaPrimitiveArray<TypeParam>::CreateArray(1024, kLocalRef);

	const size_t regionSize = 2048;
	TypeParam regionCorrect[regionSize] = { 0 };
	EXPECT_THROW(testedArray.getRegion(0, regionSize, regionCorrect), TJInvalidArgument);
}

TYPED_TEST(TJPrimitiveArrayTest, TestMoveConstructor)
{
	TJJavaPrimitiveArray<TypeParam>  testedArray = TJJavaPrimitiveArray<TypeParam>::CreateArray(1024, kLocalRef);
	auto handle = testedArray.get();
	TJJavaPrimitiveArray<TypeParam> movedArray(std::move(testedArray));

	EXPECT_EQ(nullptr, testedArray.get()) << "After move handle must be zero";
	EXPECT_EQ(handle, movedArray.get());
	EXPECT_THROW(testedArray.getPtr(), TJNIException);
}

TYPED_TEST(TJPrimitiveArrayTest, TestMoveAssignment)
{
	TJJavaPrimitiveArray<TypeParam>  testedArray = TJJavaPrimitiveArray<TypeParam>::CreateArray(1024, kLocalRef);
	auto handle = testedArray.get();
	TJJavaPrimitiveArray<TypeParam> movedArray = TJJavaPrimitiveArray<TypeParam>::CreateArray(1024, kLocalRef);
	TypeParam* ptr = movedArray.getPtr();
	movedArray = std::move(testedArray);

	EXPECT_EQ(nullptr, testedArray.get()) << "After move handle must be zero";
	EXPECT_EQ(handle, movedArray.get());
	EXPECT_THROW(testedArray.getPtr(), TJNIException);
}