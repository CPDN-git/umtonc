// File    : Level.cpp
// Author  : Neil Massey
// Date    : 25/11/08
// Purpose : Class to store level details in

#include "Level.h"
#include "ToString.h"
#include <iostream>

//*****************************************************************************

Level::Level(void)
{
	i_levtype = 0;
	f_value = 0.0f;
	f_acoeff = 0.0f;
	f_bcoeff = 0.0f;
	f_p0 = 0.0f;
}

//*****************************************************************************

void Level::Set(int levtype, float value)
{
	// regular level constructor
	i_levtype = levtype;
	f_value = value;
	f_acoeff = 0.0f;
	f_bcoeff = 0.0f;
	f_p0 = 0.0f;
}

//*****************************************************************************

void Level::Set(int levtype, float acoeff, float bcoeff, float p0)
{
	// hybrid level constructor
	i_levtype = levtype;
	f_acoeff = acoeff;
	f_bcoeff = bcoeff;
	f_p0 = p0;
	f_value = f_bcoeff + f_acoeff / p0;
}

//*****************************************************************************

bool Level::operator == (const Level &rhs) const
{
	return (i_levtype == rhs.i_levtype) &&
		   (f_value   == rhs.f_value)   &&
		   (f_acoeff  == rhs.f_acoeff)  &&
		   (f_bcoeff  == rhs.f_bcoeff)  &&
		   (f_p0      == rhs.f_p0);
}

//*****************************************************************************

bool Level::operator != (const Level &rhs) const
{
	return !(*this == rhs);
}

//*****************************************************************************

int Level::GetType(void) const
{
	return i_levtype;
}

//*****************************************************************************

float Level::GetValue(void) const
{
	return f_value;
}

//*****************************************************************************

void Level::Print(void) const
{
	std::cout << i_levtype << "  " << f_value << std::endl;
	if (i_levtype == 9)
		std::cout << "\t" << f_acoeff << " " << f_bcoeff << " " << f_p0 << std::endl;
}

//*****************************************************************************
