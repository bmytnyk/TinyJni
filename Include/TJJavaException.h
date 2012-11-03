/*
* Copyright (c) 2012
* Bogdan Mytnyk <bogdan.myntyk@gmail.com>
*
* This library is free software; you can redistribute it and/or modify
* it under the terms of the MIT license.
*/

#ifndef __TJ_JAVAEXCEPTION_H__
#define __TJ_JAVAEXCEPTION_H__

#include <stdexcept>
#include "jni.h"

#include "TJException.h"
#include "TJTypes.h"

// @brief
// class that is base for all of exceptions
// which are generated as result of throwing specific java exception
// during java code execution

class TJJavaException : public TJBaseException
{
public:
	typedef TJBaseException Base;

	explicit TJJavaException(const std::string& fullDescription, const std::string& description = std::string("")) throw();
	TJJavaException(const TJJavaException& javaExc) throw();
	TJJavaException& operator=(const TJJavaException& javaExc) throw();

	virtual const char* what() const throw() {return TJBaseException::what();} 

	virtual ~TJJavaException() throw() {};

private:
	// name of jni method which failed due to java exception
	std::string			mPlace;
};

class TJJavaOutOfMemory : public TJJavaException
{
public:
	TJJavaOutOfMemory(const std::string& fullDescription, const std::string& place) throw();
};

class TJJavaInvalidArgument : public TJJavaException
{
public:
	TJJavaInvalidArgument(const std::string& fullDescription, const std::string& place) throw();
};

class TJArrayIndexOutOfBounds : public TJJavaException
{
public:
	TJArrayIndexOutOfBounds(const std::string& fullDescription, const std::string& place) throw();
};

class TJClassInitializerError : public TJJavaException
{
public:
	TJClassInitializerError(const std::string& fullDescription, const std::string& place) throw();
};

class TJInstantiationException : public TJJavaException
{
public:
	TJInstantiationException(const std::string& fullDescription, const std::string& place) throw();
};

class TJStringIndexOutOfBoundsException : public TJJavaException
{
public:
	TJStringIndexOutOfBoundsException(const std::string& fullDescription, const std::string& place) throw();
};

void GenerateJavaException(JNIEnv* jniEnv, jthrowable javaExceptionHandle, const std::string& place);


#endif //__TJ_JAVAEXCEPTION_H__
