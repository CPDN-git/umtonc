//*****************************************************************************
//* File    : GlobalAtts.h
//* Author  : Neil Massey
//* Date    : 15/12/08
//* Purpose : Global Attributes (ncAtts)
//*****************************************************************************

#ifndef GLOBAL_ATTS_H
#define GLOBAL_ATTS_H

#include <string>
#include "AttributeList.h"

class GlobalAtts
{
	public:
		GlobalAtts(void);
		bool Valid(void) const;
		bool LoadAttributes(std::string s_file_name);
		const AttributeList& GetAttributes(void) const;
		int GetMaxSize(void) const;

	private:
		bool b_loaded;
		AttributeList x_global_atts_list;
		int i_max_size;
		unsigned int i_max_string_length;
};

#endif
