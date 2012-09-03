#ifndef __TJ_JAVAEXCEPTION_H__
#define __TJ_JAVAEXCEPTION_H__

#include <stdexcept>
#include "jni.h"

#include "TJException.h"
#include "TJTypes.h"

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
	std::string mPlace;
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
