/*
* Copyright (c) 2012
* Bogdan Mytnyk <bogdan.myntyk@gmail.com>
*
* This library is free software; you can redistribute it and/or modify
* it under the terms of the MIT license.
*/

#ifndef __TJSTRING_H__
#define __TJSTRING_H__

#include "TJTypes.h"
#include "TJObjectRef.h"

typedef std::basic_string<TJChar> TJCharString;
 
class TJStringRef : public TJRef<jstring>
{
private:
	const char* mUtf8Chars;
	TJSize mUtf8Length;

	const TJChar* mUtf16Chars;
	TJSize mUtf16Length;

public:
	typedef jstring NativeType;
	typedef TJRef<jstring> Base;

	TJStringRef(jstring str, bool copy, TJRefType ref);
	TJStringRef(TJStringRef&& rht);
	TJStringRef& operator=(TJStringRef&& rht);
	TJStringRef(const TJStringRef& rht);
	TJStringRef& operator=(const TJStringRef& rht);

	TJSize utf8Length() const noexcept {return mUtf8Length;}
	TJSize utf16Length() const noexcept {return mUtf16Length;};

	const char* acquireUtf8Chars();
	void releaseUtf8Chars() noexcept;

	const TJChar* acquireUtf16Chars();
	void releaseUtf16Chars() noexcept;

	~TJStringRef() noexcept;

	static TJStringRef createString(const std::string& utf8Str, TJRefType ref);

	static TJStringRef createString(const TJCharString& utf16String, TJRefType ref);
};

#endif
