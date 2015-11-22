#ifndef __TJCLASSTEST_H__
#define __TJCLASSTEST_H__

class TJClassTestBase
{
public:
	virtual				~TJClassTestBase() {};

	bool				performTest();

private:
	virtual bool		testMethods() = 0;
	virtual bool		testFields() = 0;

	bool				testBaseClassFields();
	bool				testBaseClassMethods();

protected:
	TJClassRef			mClassRef;
	
	TJClassTestBase(const std::string& className):
		mClassRef(TJClassRef::FindClass(className, kGlobalRef))
	{
	};
};

#endif //__TJCLASSTEST_H__
