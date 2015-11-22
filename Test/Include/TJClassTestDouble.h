#ifndef __TJCLASSTESTDOUBLE_H__
#define __TJCLASSTESTDOUBLE_H__

#include "TJClassTest.h"

class TJClassTestDouble : public TJClassTestBase
{
public:
	TJClassTestDouble();
	virtual				~TJClassTestDouble();

private:
	virtual bool		testStaticFields();
	virtual bool		testStaticMethods();
};

#endif //__TJCLASSTESTDOUBLE_H__
