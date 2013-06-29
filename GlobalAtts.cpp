// File    : GlobalAtts.cpp
// Author  : Neil Massey
// Date    : 15/12/08
// Purpose : Global Attributes (ncAtts)                                 

#include "GlobalAtts.h"
#include <fstream>
#include <iostream>

//*****************************************************************************

GlobalAtts::GlobalAtts(void) : x_global_atts_list("global")
{
	b_loaded = false;
	i_max_size = 0;
	i_max_string_length = 1;
}

//*****************************************************************************

bool GlobalAtts::Valid(void) const
{
	return b_loaded;
}

//*****************************************************************************

bool GlobalAtts::LoadAttributes(std::string s_file_name)
{
	// load in the attributes from a plain text file
	// format is: name, type, number of values, list of values
	// open the file
	std::ifstream x_gatts_file;
	x_gatts_file.open(s_file_name.c_str(), std::ios::in);
	if (!x_gatts_file)
		return false;

	// read the file
	while (!x_gatts_file.eof())
	{
		// get the values from the file
		std::string s_name;
		std::string s_type;
		int i_vals;
		std::string s_value;
		x_gatts_file >> s_name;
		x_gatts_file >> s_type;
		x_gatts_file >> i_vals;
		std::list<std::string> x_values;
		for (int i=0; i<i_vals; i++)
		{
			x_gatts_file >> s_value;
			// get the maximum string length
			if (s_type == "string")
				// +2 for termination character \n
				if (s_value.size() + 2 > i_max_string_length)
					i_max_string_length = s_value.size() + 2;
			x_values.push_back(s_value);
		}			
		if (i_vals > i_max_size)
			i_max_size = i_vals;
		// create the attribute with the above values
		Attribute x_att(s_name, x_values, s_type);
		x_global_atts_list.Add(x_att);
	}
	b_loaded = true;
	return true;
}

//*****************************************************************************

const AttributeList& GlobalAtts::GetAttributes(void) const
{
	return x_global_atts_list;
}

//*****************************************************************************

int GlobalAtts::GetMaxSize(void) const
{
	return i_max_size * i_max_string_length;
}
