//*****************************************************************************
//* File    : Level.h
//* Author  : Neil Massey
//* Date    : 25/11/08
//* Purpose : Class to store level details in
//*****************************************************************************

#ifndef LEVEL_H
#define LEVEL_H

#include <string>

class Level
{
	public:
		Level(void);
		void Set(int levtype, float value);
		void Set(int levtype, float acoeff, float bcoeff, float p0);
		bool operator == (const Level &rhs) const;
		bool operator != (const Level &rhs) const;
		void Print(void) const;
		int GetType(void) const;
		float GetValue(void) const;
	
	private:
		int i_levtype;
		float f_value;
		float f_acoeff;
		float f_bcoeff;
		float f_p0;
};

#endif
