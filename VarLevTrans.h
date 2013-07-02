//*****************************************************************************
//* File    : VarLevTrans.h
//* Author  : Neil Massey
//* Date    : 15/12/08
//* Purpose : Translate variable and level codes to netCDF standard names
//*           and recover their attributes
//* Modified: 13/05/13 - Neil Massey - changed to use XML UM variables to
//*           CF-compliant variables translation file from PRECIS
//*           02/07/13 - NRM - Allow for CMOR compatible names with the
//*           <out_name> tag
//*****************************************************************************

#ifndef VARLEV_TRANS_H
#define VARLEV_TRANS_H

#include <string>
#include "AttributeList.h"
#include "tinyxml/tinyxml.h"

class VarLevTrans
{
	public:
		VarLevTrans(std::string s_xml_file_name);
		bool LoadAttributes(void);
		const AttributeList& GetVariableAttributes(int i_stash_code) const;
		AttributeList GetLevelAttributes(int i_level_type) const;

	private:
		void ProcessVariableAttributes(TiXmlElement* px_root_ele, int i_model_id);
		bool ProcessLevelAttributes(TiXmlElement* px_root_ele);
		std::list<AttributeList> x_lev_att_list;
		std::list<AttributeList> x_var_att_list;
		AttributeList x_null_list;
		TiXmlDocument x_xml_doc;
};

#endif
