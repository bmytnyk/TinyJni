/*
* Copyright (c) 2015
* Bogdan Mytnyk <bogdan.mytnyk@gmail.com>
*
* This library is free software; you can redistribute it and/or modify
* it under the terms of the MIT license.
*/

#include "../Include/TJCommon.h"
#include "../Include/TJException.h"
#include "../Include/TJJavaException.h"

TJNIException::TJNIException(TJInt code, const std::string& explanation) throw() :
	TJBaseException(""),
	mCode(code),
	mDescription(explanation)
{
}
	
TJNIException::TJNIException(const TJNIException& exc) throw() :
	TJBaseException(exc),
	mCode(exc.mCode),
	mDescription(exc.mDescription)
{
}

TJNIException& TJNIException::operator=(const TJNIException& exc) throw()
{
	if (this != &exc)
	{
		mCode = exc.mCode;
		mDescription = exc.mDescription;
	}
	return *this;
}


TJNIException::~TJNIException() throw()
{
}
