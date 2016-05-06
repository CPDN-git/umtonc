//*****************************************************************************
//* File    : VarLevTrans.cpp 
//* Author  : Neil Massey
//* Date    : 15/12/08
//* Purpose : Translate variable and level codes to netCDF standard names
//*           and recover their attributes
//* Modified: 13/05/13 - Neil Massey - changed to use XML UM variables to
//*           CF-compliant variables translation file from PRECIS
//*****************************************************************************

#include "VarLevTrans.h"
#include "tinyxml/tinyxml.h"
#include "ToString.h"
#include <iostream>

//*****************************************************************************

// global tag names

const std::string s_stash_name("stash");
const std::string s_field_name("field");
const std::string s_model_name("submodel");
const std::string s_id_name("id");
const std::string s_section_name("section");
const std::string s_item_name("item");

const std::string s_level_name("level");
const std::string s_level_type_name("type");
const std::string s_variable_name("variable");
const std::string s_stash_section_name("stash_section");
const std::string s_stash_item_name("stash_item");

//*****************************************************************************

VarLevTrans::VarLevTrans(std::string s_xml_file_name) : x_null_list("NULL")
{
	x_xml_doc.LoadFile(s_xml_file_name.c_str());
}

//*****************************************************************************

void error(std::string text)
{
    std::cerr << text << std::endl;
}

//*****************************************************************************

void VarLevTrans::ProcessVariableAttributes(TiXmlElement* px_root_ele, int i_model_id)
{
	// get the child and loop through the elements - arranged by sections
	TiXmlElement* px_cur_section = px_root_ele->FirstChildElement();
	while (px_cur_section)
	{
	    if (px_cur_section->ValueStr() == s_section_name)
	    {
	        // get the section number
	        int i_section_number = 0;
	        int i_section_ret = px_cur_section->QueryIntAttribute(s_id_name, &i_section_number);
	        if (i_section_ret != TIXML_SUCCESS)
	            error("Error in XML file: " + px_cur_section->ValueStr());
	        // now get each "item"
	        TiXmlElement* px_cur_item = px_cur_section->FirstChildElement();
	        while (px_cur_item)
	        {
	            if (px_cur_item->ValueStr() == s_item_name)
	            {
	                // get the stash item
                    int i_item_number = 0;
                    int i_item_ret = px_cur_item->QueryIntAttribute(s_id_name, &i_item_number);
	                if (i_item_ret != TIXML_SUCCESS)
	                    error("Error in XML file: " + px_cur_item->ValueStr());
	                    
	                // create an attribute list with this stash section and item
                    std::string s_att_list_name = ToString(i_section_number * 1000 + i_item_number);
                    AttributeList x_att_list(s_att_list_name);
            		Attribute x_stash_item_att(s_stash_item_name, ToString(i_item_number));
            		Attribute x_stash_sec_att(s_stash_section_name, ToString(i_section_number));
            		
                    // add the stash section and item as attributes
            		x_att_list.Add(x_stash_item_att);
        	    	x_att_list.Add(x_stash_sec_att);
        	    	
        	    	// add the rest of the attributes
        	    	TiXmlAttribute* px_cur_attr = px_cur_item->FirstAttribute();
        	    	while (px_cur_attr)
        	    	{
        	    	    std::string s_attr_name = px_cur_attr->Name();        	    	    
        	    	    std::string s_attr_value = px_cur_attr->Value();
        	    	    // add if not "id"
        	    	    if (s_attr_name != s_id_name)
        	    	    {
            	    	    Attribute x_attr_item(s_attr_name, s_attr_value);
            	    	    x_att_list.Add(x_attr_item);
            	    	}
        	    	    px_cur_attr = px_cur_attr->Next();
        	    	}
               	    // add to the variable attributes list
	    	        x_var_att_list.push_back(x_att_list);
	            }
	            px_cur_item = px_cur_item->NextSiblingElement();
	        }
	    }
	    px_cur_section = px_cur_section->NextSiblingElement();
	}
}

//*****************************************************************************

bool VarLevTrans::LoadAttributes(void)
{
	// check whether the document was loaded correctly in the constructor
	if (x_xml_doc.Error())
		return false;

	// get the root element
	TiXmlElement* px_root = x_xml_doc.RootElement();
	if (!px_root)
		error("Malformed XML in file.");

	// loop through all the elements
	TiXmlElement* px_cur_outer = px_root->FirstChildElement();
	while (px_cur_outer)
	{
		if (px_cur_outer->ValueStr() == s_stash_name)
		{
            // model type
	        TiXmlElement* px_cur_model = px_cur_outer->FirstChildElement();
	        while (px_cur_model)
	        {
	            if (px_cur_model->ValueStr() == s_model_name)
	            {
    	            int model_id = 0;
    	            int model_id_ret = px_cur_model->QueryIntAttribute(s_id_name, &model_id);
    	            if (model_id_ret == TIXML_SUCCESS)
            			ProcessVariableAttributes(px_cur_model, model_id);
		            else
		                error("Error in XML in file: " + px_cur_model->ValueStr());
    	    	}
    			px_cur_model = px_cur_model->NextSiblingElement();
	    	}
		}
		px_cur_outer = px_cur_outer->NextSiblingElement();
	}

	return true;
}

//*****************************************************************************

const AttributeList& VarLevTrans::GetVariableAttributes(int i_stash_code) const
{
	// make a string from the stash list
	std::string s_stash_code = ToString(i_stash_code);
	// loop through and find the list
	std::list<AttributeList>::const_iterator it;
	for (it = x_var_att_list.begin(); it != x_var_att_list.end(); it++)
		if (it->GetName() == s_stash_code)
			return *it;

	// if not found, return the null attribute list
	return x_null_list;
}

//*****************************************************************************

AttributeList VarLevTrans::GetLevelAttributes(int i_level_type, int i_size) const
{
    // this now switches on level type rather than relying on xml
    AttributeList x_att_list("Level");
    switch (i_level_type)
    {
        case 0:
        {
            Attribute x_std_name_att("standard_name", "unspecified");
            Attribute x_out_name_att("out_name", "unspecified");
            x_att_list.Add(x_std_name_att);
            x_att_list.Add(x_out_name_att);
        }
        break;
		case 1:
		{
		    Attribute x_std_name_att("standard_name", "height");
			Attribute x_dir_att("direction", "up");
			Attribute x_units_att("units", "m");
            Attribute x_out_name_att("out_name", "height");
            x_att_list.Add(x_out_name_att);
			x_att_list.Add(x_std_name_att);
			x_att_list.Add(x_units_att);
			x_att_list.Add(x_dir_att);
		}
		break;
		case 5:
		{
			Attribute x_std_name_att("standard_name", "boundary_layer_height");
			x_att_list.Add(x_std_name_att);
            Attribute x_out_name_att("out_name", "boundary_layer");
            x_att_list.Add(x_out_name_att);
		}
		break;
		case 6:
		{
			Attribute x_std_name_att("standard_name", "non_dimensional_soil_level");
			x_att_list.Add(x_std_name_att);
            Attribute x_out_name_att("out_name", "soil_level");
            x_att_list.Add(x_out_name_att);
		}
		break;
    	case 8:
    	{
        	Attribute x_std_name_att("standard_name", "air_pressure");
	        Attribute x_units_att("units", "hPa");
    	    Attribute x_dir_att("direction", "down");
			x_att_list.Add(x_std_name_att);
			x_att_list.Add(x_units_att);
			x_att_list.Add(x_dir_att);
            Attribute x_out_name_att("out_name", "pressure");
            x_att_list.Add(x_out_name_att);
        }
	    break;
        case 9:
        {
            Attribute x_std_name_att("standard_name", "atmosphere_hybrid_sigma_pressure_coordinate");
			Attribute x_dir_att("direction", "down");
			x_att_list.Add(x_std_name_att);
			x_att_list.Add(x_dir_att);
            Attribute x_out_name_att("out_name", "hybrid_pressure_x1000");
            x_att_list.Add(x_out_name_att);
		}
		break;
		case 10:
		{
			Attribute x_std_name_att("standard_name", "atmosphere_sigma_coordinate");
			Attribute x_dir_att("direction", "down");
			x_att_list.Add(x_std_name_att);
			x_att_list.Add(x_dir_att);
            Attribute x_out_name_att("out_name", "sigma");
            x_att_list.Add(x_out_name_att);
		}
		break;
        case 65:
        {
            Attribute x_std_name_att("standard_name", "atmosphere_hybrid_height_coordinate");
			Attribute x_dir_att("direction",  "down");
			x_att_list.Add(x_std_name_att);
			x_att_list.Add(x_dir_att);
            Attribute x_out_name_att("out_name", "hybrid_height");
            x_att_list.Add(x_out_name_att);
		}
		break;		
		case 128:
		{
			Attribute x_std_name_att("standard_name", "height");
			Attribute x_units_att("units", "m");
			Attribute x_dir_att("direction", "up");
			x_att_list.Add(x_std_name_att);
			x_att_list.Add(x_units_att);
			x_att_list.Add(x_dir_att);
            Attribute x_out_name_att("out_name", "mean_sea_level");
            x_att_list.Add(x_out_name_att);
		}
		break;
		case 129:
		{
			Attribute x_std_name_att("standard_name", "height");
			Attribute x_units_att("units", "m");
			Attribute x_dir_att("direction", "up");
			x_att_list.Add(x_std_name_att);
			x_att_list.Add(x_units_att);
			x_att_list.Add(x_dir_att);
			if (i_size == 1)
			{
	            Attribute x_out_name_att("out_name", "surface");
	            x_att_list.Add(x_out_name_att);
	        }
	        else
	        {
	            Attribute x_out_name_att("out_name", "pseudo");
	            x_att_list.Add(x_out_name_att);
	        }
		}
		break;
		case 130:
		{
			Attribute x_std_name_att("standard_name", "tropopause_altitude");
			Attribute x_units_att("units", "m");
			Attribute x_dir_att("direction", "up");
			x_att_list.Add(x_std_name_att);
			x_att_list.Add(x_units_att);
			x_att_list.Add(x_dir_att);
            Attribute x_out_name_att("out_name", "tropopause");
            x_att_list.Add(x_out_name_att);
		}
		break;
		case 131:
		{
			Attribute x_std_name_att("standard_name", "maximum_wind_level");
			x_att_list.Add(x_std_name_att);
            Attribute x_out_name_att("out_name", "max_wind");
            x_att_list.Add(x_out_name_att);
		}
		break;
		case 132:
		{
			Attribute x_std_name_att("standard_name", "freezing_level");
			x_att_list.Add(x_std_name_att);
            Attribute x_out_name_att("out_name", "freezing_level");
            x_att_list.Add(x_out_name_att);
		}
		break;
		case 133:
		{
			Attribute x_std_name_att("standard_name", "top_of_atmosphere");
			x_att_list.Add(x_std_name_att);		
            Attribute x_out_name_att("out_name", "toa");
            x_att_list.Add(x_out_name_att);
		}
		break;
		case 142:
		{
			Attribute x_std_name_att("standard_name", "upper_hybrid_level");
			x_att_list.Add(x_std_name_att);
            Attribute x_out_name_att("out_name", "upper_hybrid_level");
            x_att_list.Add(x_out_name_att);
        }
		break;
		case 143:
		{
			Attribute x_std_name_att("standard_name", "lower_hybrid_level");
			x_att_list.Add(x_std_name_att);
            Attribute x_out_name_att("out_name", "lower_hybrid_level");
            x_att_list.Add(x_out_name_att);
		}
		break;
		case 176:
		{
			Attribute x_std_name_att("standard_name", "latitude");
			Attribute x_units_att("direction", "north");
			x_att_list.Add(x_std_name_att);
			x_att_list.Add(x_units_att);
            Attribute x_out_name_att("out_name", "latitude");
            x_att_list.Add(x_out_name_att);
		}
		break;
		case 177:
		{
			Attribute x_std_name_att("standard_name", "longitude");
			Attribute x_units_att("direction", "east");
			x_att_list.Add(x_std_name_att);
			x_att_list.Add(x_units_att);
            Attribute x_out_name_att("out_name", "longitude");
            x_att_list.Add(x_out_name_att);
		}
		break;
		case 275:
		{
			Attribute x_std_name_att("standard_name", "canopy_height");
			x_att_list.Add(x_std_name_att);		
            Attribute x_out_name_att("out_name", "canopy_height");
            x_att_list.Add(x_out_name_att);
		}
		break;
		default:
		    x_att_list = x_null_list;
		    std::cout << "Error: " << i_level_type << " not supported" << std::endl;
	}
	return x_att_list;
}

