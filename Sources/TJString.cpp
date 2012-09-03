#include "../TJPch.h"
#include "../Include/TJCommon.h"
#include "../Include/TJString.h"
#include "../Include/TJUtils.h"

TJStringRef TJStringRef::createString(const std::string& utf8Str, TJRefType refType)
{
	JNIEnv* jniEnv = TJGetEnvironment();
	if (jniEnv == NULL)
		throw TJNIException(kThreadDetached, "Failed to get jni environment");

	jstring rawString = jniEnv->NewStringUTF(utf8Str.c_str());
	if (rawString == NULL)
		GenerateJavaException(jniEnv, jniEnv->ExceptionOccurred(), "TJStringRef::createString failed");

	if (refType == kLocalRef)
		return TJStringRef(rawString, false, kLocalRef);
	else
	{
		ScopedLocalRef<jstring> cleaner(rawString);
		return TJStringRef(rawString, true, refType);
	}
}

TJStringRef::TJStringRef(jstring str, bool copy, TJRefType ref):
	TJStringRef::Base(str, ref, copy),
	mUtf8Chars(NULL),
	mUtf8Length(0),
	mUtf16Chars(NULL),
	mUtf16Length(0)
{
	JNIEnv* jniEnv = TJGetEnvironment();
	if (jniEnv == NULL)
		throw TJNIException(kThreadDetached, "Failed to get jni environment");
	
	mUtf8Length = jniEnv->GetStringUTFLength(mHandle);
	mUtf16Length = jniEnv->GetStringLength(mHandle);
}

TJStringRef::TJStringRef(const TJStringRef& rht):
	TJStringRef::Base(rht),
	mUtf8Length(rht.mUtf8Length),
	mUtf16Length(rht.mUtf16Length),
	mUtf8Chars(NULL),
	mUtf16Chars(NULL)
{
}

TJStringRef& TJStringRef::operator=(const TJStringRef& rht)
{
	if (this != &rht)
	{
		Base::operator=(rht);
		mUtf8Length = rht.mUtf8Length;
		mUtf16Length = rht.mUtf16Length;
		mUtf8Chars = NULL;
		mUtf16Chars = NULL;
	}

	return *this;
}

TJStringRef::~TJStringRef()
{
}

const char* TJStringRef::acquireUtf8Chars()
{
	JNIEnv* jniEnv = TJGetEnvironment();
	if (jniEnv == NULL)
		throw TJNIException(kThreadDetached, "Failed to get jni environment");

	if (mUtf8Chars == NULL)
	{
		TJBoolean isCopy = JNI_FALSE;
		mUtf8Chars = jniEnv->GetStringUTFChars(mHandle, &isCopy);
		if (mUtf8Chars == NULL)
			GenerateJavaException(jniEnv, jniEnv->ExceptionOccurred(), "TJStringRef::acquireUtf8Chars failed");
	}

	return mUtf8Chars;
}

void TJStringRef::releaseUtf8Chars()
{
	if (mUtf8Chars != NULL)
	{
		JNIEnv* jniEnv = TJGetEnvironment();
		if (jniEnv == NULL)
			throw TJNIException(kThreadDetached, "Failed to get jni environment");

		jniEnv->ReleaseStringUTFChars(mHandle, mUtf8Chars);
	}
}

const TJChar* TJStringRef::acquireUtf16Chars()
{
	JNIEnv* jniEnv = TJGetEnvironment();
	if (jniEnv == NULL)
		throw TJNIException(kThreadDetached, "Failed to get jni environment");

	if (mUtf16Chars == NULL)
	{
		TJBoolean isCopy = JNI_FALSE;
		mUtf16Chars = jniEnv->GetStringChars(mHandle, &isCopy);
		if (mUtf16Chars == NULL)
			GenerateJavaException(jniEnv, jniEnv->ExceptionOccurred(), "TJStringRef::acquireUtf16Chars failed");
	}

	return mUtf16Chars;
}

void TJStringRef::releaseUtf16Chars()
{
	if (mUtf16Chars != NULL)
	{
		JNIEnv* jniEnv = TJGetEnvironment();
		if (jniEnv == NULL)
			throw TJNIException(kThreadDetached, "Failed to get jni environment");

		jniEnv->ReleaseStringChars(mHandle, mUtf16Chars);
	}
}
