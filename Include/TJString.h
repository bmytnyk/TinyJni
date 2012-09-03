#ifndef __TJSTRING_H__
#define __TJSTRING_H__

#include "TJTypes.h"
#include "TJObjectRef.h"

class TJStringRef : public TJRef<jstring>
{
private:
	const char* mUtf8Chars;
	TJSize mUtf8Length;

	const TJChar* mUtf16Chars;
	TJSize mUtf16Length;

public:
	typedef TJRef<jstring> Base;

	TJStringRef(jstring str, bool copy, TJRefType ref);
	
	TJStringRef(const TJStringRef& rht);
	TJStringRef& operator=(const TJStringRef& rht);

	TJSize utf8Length() const {return mUtf8Length;}
	TJSize utf16Length() const {return mUtf16Length;};

	const char* acquireUtf8Chars();
	void releaseUtf8Chars();

	const TJChar* acquireUtf16Chars();
	void releaseUtf16Chars();

	virtual ~TJStringRef();

	static TJStringRef createString(const std::string& utf8Str, TJRefType ref);
};

#endif
