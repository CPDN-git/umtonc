// File    : AttributeList.h
// Author  : Neil Massey
// Date    : 16/12/08
// Purpose : Class to store multiple attribute values in a list
//            e.g. all attributes for a level or variable

#include "AttributeList.h"

//*****************************************************************************

AttributeList::AttributeList(std::string name)
{
	s_name = name;
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

bool AttributeList::operator == (const AttributeList& rhs) const
{
	// just compare the names
	return s_name == rhs.s_name;
}
