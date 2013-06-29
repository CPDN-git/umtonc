//*****************************************************************************
//* File    : Attribute.h
//* Author  : Neil Massey
//* Date    : 16/12/08
//* Purpose : Class to store attribute values
//*****************************************************************************

#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include <string>
#include <list>

class Attribute
{
	public:
		Attribute(std::string s_name, std::string s_value, 
				  std::string s_type = "string");
		Attribute(std::string s_name, std::list<std::string> s_value,
				  std::string s_type = "string");

		const std::string& GetName(void) const;
		const std::string& GetValue(void) const;
		const std::list<std::string>& GetValues(void) const;
		const std::string& GetType(void) const;
		int GetNumber(void) const;

	private:
		std::string s_type;
		std::string s_name;
		std::list<std::string> s_value;
};

#endif
