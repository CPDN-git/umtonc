//*****************************************************************************
//* File    : ToString.h
//* Author  : Neil Massey
//* Date    : 08/12/08
//* Purpose : Convert any type to std::string and vice versa
//*****************************************************************************

#ifndef TO_STRING_H
#define TO_STRING_H

#include <string>
#include <sstream>
#include <iostream>

template <typename T>
std::string ToString (T t, int i_field_width=0, 
					  char c_fill_char=' ')
{
	std::stringstream ss;
	if (i_field_width > 0)
	{
		ss.width(i_field_width);
		ss.fill(c_fill_char);
	}
	ss << t;
	return ss.str();
}

//*****************************************************************************

template <typename T>
T FromString(const std::string& s)
{
	std::istringstream is(s);
	T t;
	is >> t;
	return t;
}

#endif
