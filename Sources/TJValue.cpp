/*
* Copyright (c) 2015
* Bogdan Mytnyk <bogdan.mytnyk@gmail.com>
*
* This library is free software; you can redistribute it and/or modify
* it under the terms of the MIT license.
*/

#include "../Include/TJValue.h"

#include "../Include/TJTypeTraits.h"
#include "../Include/TJObjectRef.h"
#include "../Include/TJClass.h"


TJValue::TJValue(const TJValue& rht):
	mJavaValue(rht.mJavaValue),
    mTypeId(rht.mTypeId)
{
}

TJValue& TJValue::operator=(const TJValue& rht)
{
	if (this != &rht)
	{
		mTypeId = rht.mTypeId;
		mJavaValue = rht.mJavaValue;
	}

	return *this;
}

std::string TJValue::descriptor() const
{
	if (mTypeId != kObject)
		return sFieldDescriptorsArray[mTypeId];
	else
	{
		TJObjectRef localObj(mJavaValue.l, true, kLocalRef);
		return localObj.descriptor();
	}
}
