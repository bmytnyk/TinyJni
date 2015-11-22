#include "gtest/gtest.h"

#include "../../TinyJni/Include/TJCommon.h"
#include "../../TinyJni/Include/TJException.h"

#include <thread>

class TJThreadTest : public ::testing::Test
{
private:
	// Sets up the test fixture.
	virtual void SetUp();

	// Tears down the test fixture.
	virtual void TearDown();
};

void TestAttachedThreadFunc()
{
	JNIEnv* env = TJAttachCurrentThreadToJNI(nullptr);
	EXPECT_NE(env, nullptr);
	EXPECT_EQ(env, TJGetEnvironment());

	TJDetachCurrentThreadFromJni();
	EXPECT_EQ(nullptr, TJGetEnvironment());
}

TEST(TJThreadTest, TestAttachThread)
{
	std::thread testThread(TestAttachedThreadFunc);
	testThread.join();
};

void TestNotAttachedThreadFunc()
{
	TJInt error = 0;
	JNIEnv* pEnv = TJGetEnvironment(&error);
	EXPECT_EQ(pEnv, nullptr);
	EXPECT_EQ(error, kThreadDetached);
}

TEST(TJThreadTest, TestNotAttachedThread)
{
	std::thread testThread(TestNotAttachedThreadFunc);
	testThread.join();
};
