//*****************************************************************************
//* File    : AttributeList.h
//* Author  : Neil Massey
//* Date    : 16/12/08
//* Purpose : Class to store multiple attribute values in a list
//			  e.g. all attributes for a level or variable
//* Modified: NRM 02/07/13 - added "HasAttribute" and GetAttribute function
//*****************************************************************************

#ifndef ATT_LIST_H
#define ATT_LIST_H

#include "Attribute.h"
#include <list>

class AttributeList
{
	public:
		AttributeList(std::string name, int i_model_id);
		void Add(Attribute& x_att);
		const std::list<Attribute>& Get(void) const;
		const std::string& GetName(void) const;
		const int GetModel(void) const;
		bool operator == (const AttributeList& rhs) const;
		bool HasAttribute(std::string name) const;
		const Attribute& GetAttribute(std::string name) const;
	
	private:
		std::list<Attribute> x_atts;
		std::string s_name;
		int i_model_id;
};

#endif
