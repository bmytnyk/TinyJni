/*
* Copyright (c) 2012
* Bogdan Mytnyk <bogdan.myntyk@gmail.com>
*
* This library is free software; you can redistribute it and/or modify
* it under the terms of the MIT license.
*/

#ifndef __TG_TYPES_H__
#define __TG_TYPES_H__

#include "jni.h"

typedef jint TJInt;
typedef jdouble TJDouble;
typedef jfloat TJFloat;
typedef jboolean TJBoolean;
typedef jchar TJChar;
typedef jshort TJShort;
typedef jlong TJLong;
typedef jbyte TJByte;
typedef jsize TJSize;

typedef jintArray TJIntArray;
typedef jbyteArray TJByteArray;
typedef jdoubleArray TJDoubleArray;
typedef jbooleanArray TJBooleanArray;
typedef jcharArray TJCharArray;
typedef jshortArray TJShortArray;
typedef jlongArray TJLongArray;
typedef jfloatArray TJFloatArray;

typedef jmethodID TJMethodID;
typedef jfieldID TJFieldID;

typedef jthrowable TJThrowable;

enum TJPrimitiveTypeId
{
	kInt = 0,
	kLong = 1,
	kFloat = 2,
	kChar = 3,
	kDouble = 4,
	kBoolean = 5,
	kByte = 6,
	kShort = 7,
	kVoid = 8,
	kObject = 9
};

const TJSize kTypesCount = 10;

#endif //__TG_TYPES_H__

