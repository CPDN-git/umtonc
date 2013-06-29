//*****************************************************************************
//* File    : RawConvert.h
//* Author  : Neil Massey
//* Date    : 22/07/08
//* Purpose : Convert floating point to raw data format and back
//*****************************************************************************

// Note : 17/09/08 - could templatize this!

#ifndef RAWCONVERT_H
#define RAWCONVERT_H

int FLOAT_TO_RAW(float x)
{	
	return *((int*)(&x));
}

float RAW_TO_FLOAT(unsigned int x)
{
	return *((float*)(&x));
}

#endif
