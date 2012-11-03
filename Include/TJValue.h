/*
* Copyright (c) 2012
* Bogdan Mytnyk <bogdan.myntyk@gmail.com>
*
* This library is free software; you can redistribute it and/or modify
* it under the terms of the MIT license.
*/

#ifndef __TJVALUE_H__
#define __TJVALUE_H__

#include "TJTypes.h"
#include "TJCommon.h"
#include "TJTypeTraits.h"

#include <vector>

class TJValue
{
public:
	explicit TJValue(TJInt intValue);
	explicit TJValue(TJFloat floatValue);
	explicit TJValue(TJDouble doubleValue);
	explicit TJValue(TJBoolean booleanValue);
	explicit TJValue(TJChar charValue);
	explicit TJValue(TJShort shortValue);
	explicit TJValue(TJByte byteValue);
	explicit TJValue(TJLong longValue);
	explicit TJValue(jobject objValue);

	TJValue(const TJValue& rht);
	TJValue& operator=(const TJValue& rht);

	std::string descriptor() const;

	inline const jvalue& handle() const {return mJavaValue;}
	inline TJPrimitiveTypeId id() const {return mTypeId;}

private:
	jvalue				mJavaValue;
	TJPrimitiveTypeId	mTypeId;
};

inline TJValue::TJValue(TJInt intValue):
	mTypeId(kInt)
{
	mJavaValue.i = intValue;
}

inline TJValue::TJValue(TJFloat floatValue):
	mTypeId(kFloat)
{
	mJavaValue.f = floatValue;
}

inline TJValue::TJValue(TJDouble doubleValue):
	mTypeId(kDouble)
{
	mJavaValue.d = doubleValue;
}

inline TJValue::TJValue(TJBoolean booleanValue):
	mTypeId(kBoolean)
{
	mJavaValue.z = booleanValue;
}

inline TJValue::TJValue(TJChar charValue):
	mTypeId(kChar)
{
	mJavaValue.c = charValue;
};

inline TJValue::TJValue(TJShort shortValue):
	mTypeId(kShort)
{
	mJavaValue.s = shortValue;
}

inline TJValue::TJValue(TJByte byteValue):
	mTypeId(kByte)
{
	mJavaValue.b = byteValue;
}

inline TJValue::TJValue(TJLong longValue):
	mTypeId(kLong)
{
	mJavaValue.j = longValue;
}

inline TJValue::TJValue(jobject objValue):
	mTypeId(kObject)
{
	mJavaValue.l = objValue;
}

typedef std::vector<TJValue> TJValueVector;

#endif

