// File    : Attribute.cpp
// Author  : Neil Massey
// Date    : 16/12/08
// Purpose : Class to store attribute values                            

#include "Attribute.h"

Attribute::Attribute(std::string name, std::string value, std::string type)
{
	s_name = name;
	s_value.push_back(value);
	s_type = type;
}

//*****************************************************************************

Attribute::Attribute(std::string name, std::list<std::string> value,
                     std::string type)
{
	s_name = name;
	s_value = value;
	s_type = type;
}

//*****************************************************************************

const std::string& Attribute::GetName(void) const
{
	return s_name;
}

//*****************************************************************************

const std::string& Attribute::GetValue(void) const
{
	return s_value.front();
}

//*****************************************************************************

const std::list<std::string>& Attribute::GetValues(void) const
{
	return s_value;
}

//*****************************************************************************

const std::string& Attribute::GetType(void) const
{
	return s_type;
}

//*****************************************************************************

int Attribute::GetNumber(void) const
{
	return s_value.size();
}
