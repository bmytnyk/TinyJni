#include "../Include/TJString.h"

#include <cassert>
#include <iostream>
#include <algorithm>

#include "gtest/gtest.h"

TEST(TestTJString, Utf8Init)
{
	// generate some utf8string
	const uint32_t stringLength = 16 + (std::rand() * 256) / RAND_MAX;
	std::string content(stringLength, 0);
	std::generate(content.begin(), content.end(), []() {return 'a' + (std::rand() * ('z' - 'a')) / RAND_MAX;});

	// check string content
	TJStringRef testString = TJStringRef::createString(content.c_str(), kLocalRef);
	const char* utf8Chars = testString.acquireUtf8Chars();
	EXPECT_NE(utf8Chars, nullptr);
	EXPECT_STREQ(utf8Chars, content.c_str());

	EXPECT_EQ(testString.utf8Length(), stringLength);
}

TEST(TestTJString, Utf16Init)
{
	TJCharString content {'H', 'e', 'l', 'l', 'o'};
	TJStringRef testString = TJStringRef::createString(content, kLocalRef);
	const TJChar* utf16Chars = testString.acquireUtf16Chars();
	EXPECT_NE(utf16Chars, nullptr);

	EXPECT_EQ(testString.utf16Length(), content.length());

	TJCharString tjStringContenUtf16(utf16Chars);
	EXPECT_EQ(tjStringContenUtf16, content);
}

TEST(TestTJString, CopyConstructor)
{
	TJStringRef testString = TJStringRef::createString("TestString", kLocalRef);
	TJStringRef copyString(testString);
	EXPECT_STREQ(copyString.acquireUtf8Chars(), testString.acquireUtf8Chars());
}

TEST(TestTJString, CopyAssignment)
{
	TJStringRef testString = TJStringRef::createString("TestString", kLocalRef);
	TJStringRef assignedStringRef = testString;
	EXPECT_STREQ(assignedStringRef.acquireUtf8Chars(), testString.acquireUtf8Chars());
}

TEST(TestTJString, ConstructorNullHandle)
{
	EXPECT_THROW(TJStringRef(nullptr, false, kLocalRef), TJInvalidArgument);
}

TEST(TestTJString, MoveConstructor)
{
	TJStringRef testString = TJStringRef::createString("TestString", kLocalRef);
	jstring handle = testString.get();
	TJStringRef movedString(std::move(testString));
	EXPECT_EQ(testString.get(), nullptr);
	EXPECT_EQ(handle, movedString.get());

	EXPECT_ANY_THROW(testString.acquireUtf16Chars());
	EXPECT_ANY_THROW(testString.acquireUtf8Chars());
}

TEST(TestTJString, MoveAssignment)
{
	TJStringRef testString = TJStringRef::createString("TestString", kLocalRef);
	jstring handle = testString.get();
	TJStringRef movedString = TJStringRef::createString("OldString", kLocalRef);
	movedString = std::move(testString);
	EXPECT_EQ(testString.get(), nullptr);
	EXPECT_EQ(handle, movedString.get());
	EXPECT_STREQ(movedString.acquireUtf8Chars(), "TestString");

	EXPECT_ANY_THROW(testString.acquireUtf16Chars());
	EXPECT_ANY_THROW(testString.acquireUtf8Chars());
}