/*
* Copyright (c) 2012
* Bogdan Mytnyk <bogdan.myntyk@gmail.com>
*
* This library is free software; you can redistribute it and/or modify
* it under the terms of the MIT license.
*/

#ifndef __TJUTILS_H__
#define __TJUTILS_H__

template <typename TJniType>
class ScopedLocalRef
{
private:
	TJniType mValue;

public:
	ScopedLocalRef(TJniType value):
		mValue(value)
	{
	}

	operator TJniType ()
	{
		return mValue;
	}

	~ScopedLocalRef()
	{
		JNIEnv* jniEnv = TJGetEnvironment();
		if ((jniEnv != NULL) && (mValue != NULL))
			jniEnv->DeleteLocalRef(mValue);
	}
};

#endif //__TJUTILS_H__
