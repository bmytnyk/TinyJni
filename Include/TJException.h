#ifndef __TG_EXCEPTION_H__
#define __TG_EXCEPTION_H__

#include "TJTypes.h"

#include <stdexcept>

#ifdef TJ_ANDROID
#define JNI_ENOMEM       (-4)              /* not enough memory */
#define JNI_EEXIST       (-5)              /* VM already created */
#define JNI_EINVAL       (-6)              /* invalid arguments */
#endif

enum
{
	kClassNotFound = 3,
	kInvalidFunctionArg = 2,
	kInvalidVM = 1,
	kNoError = 0,
	kThreadDetached = JNI_EDETACHED,
	kInvalidVersion = JNI_EVERSION,
	kNotEnoughMemory = JNI_ENOMEM
};

class TJJavaException;

class TJBaseException : public std::runtime_error
{
public:
	explicit TJBaseException(const std::string& description = std::string("")):std::runtime_error(description)  {};
	TJBaseException(const TJBaseException& srcException) :std::runtime_error(srcException) {};
	TJBaseException& operator=(const TJBaseException& srcException) throw() 
	{
		std::runtime_error::operator = (srcException);
		return *this;
	};
	
	virtual ~TJBaseException() throw() {};
};

class TJInvalidArgument : public TJBaseException
{ 
public:
	explicit TJInvalidArgument(const std::string& description) throw():
		TJBaseException(description)
	{
	}

	TJInvalidArgument(const TJInvalidArgument& exc) throw():
		TJBaseException(exc)
	{
	}
	
	TJInvalidArgument& operator=(const TJInvalidArgument& srcException) throw()
	{
		TJBaseException::operator=(srcException);
		return *this;
	}

};

class TJNIException : public TJBaseException
{
public:
	explicit TJNIException(TJInt code, const std::string& expl = std::string("")) throw();
	TJNIException(const TJNIException& exc) throw();
	TJNIException& operator=(const TJNIException& exc) throw();

	virtual const char* what() const throw() {return mDescription.c_str();};

	inline TJInt code() const {return mCode;}

	virtual ~TJNIException() throw();

private:
	TJInt		mCode;
	std::string mDescription;
};

#endif //__TG_COMMON_H_
