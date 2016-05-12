// File    : AttributeList.h
// Author  : Neil Massey
// Date    : 16/12/08
// Purpose : Class to store multiple attribute values in a list
//            e.g. all attributes for a level or variable

#include "AttributeList.h"

//*****************************************************************************

AttributeList::AttributeList(std::string name, int model_id)
{
	s_name = name;
	i_model_id = model_id;
}

//*****************************************************************************

void AttributeList::Add(Attribute& x_att)
{
	x_atts.push_back(x_att);
}

//*****************************************************************************

const std::list<Attribute>& AttributeList::Get(void) const
{
	return x_atts;
}

//*****************************************************************************

const std::string& AttributeList::GetName(void) const
{
	return s_name;
}

//*****************************************************************************

const int AttributeList::GetModel(void) const
{
    return i_model_id;
}

//*****************************************************************************

bool AttributeList::operator == (const AttributeList& rhs) const
{
	// just compare the names
	return s_name == rhs.s_name;
}

//*****************************************************************************

bool AttributeList::HasAttribute(std::string name) const
{
	// return whether the attribute with name <name> exists in the attribute
	// list for the variable
	std::list<Attribute>::const_iterator it;
	for (it = x_atts.begin(); it != x_atts.end(); it++)
		if (it->GetName() == name)
			return true;
	return false;
}

//*****************************************************************************

const Attribute& AttributeList::GetAttribute(std::string name) const
{
	// return a reference to the attribute with the name name
	std::list<Attribute>::const_iterator it;
	for (it = x_atts.begin(); it != x_atts.end(); it++)
		if (it->GetName() == name)
			return *it;
	// not found - should use HasAttribute first to check
	throw ("Attribute with name: " + name + " not found.  Use HasAttribute to check presence before calling GetAttribute");
}