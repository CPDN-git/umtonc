//*****************************************************************************
//* File    : Packing.cpp
//* Author  : Neil Massey
//* Date    : 16/05/13
//* Purpose : Class to implement various data reduction techniques
//*           See packing.h for more details
//*****************************************************************************

#include <math.h>
#include "Packing.h"
#include <algorithm>

extern UM_WORD FLOAT_TO_RAW(float x);
extern float RAW_TO_FLOAT(unsigned int x);
extern void error(std::string);

//*****************************************************************************

const std::string s_section_name("section");
const std::string s_id_name("id");
const std::string s_pack_name("pack");
const std::string s_skip_name("skip");
const std::string s_crop_name("crop");
const std::string s_avg_name("avg");

//*****************************************************************************

Packing::Packing(std::string xml_pack_name)
{
    // Null string is allowed as default - i.e. no packing performed
    s_xml_pack_name = xml_pack_name;
}

//*****************************************************************************

bool Packing::LoadAttributes(void)
{
    // null string is allowed - no packing
	if (s_xml_pack_name == "")
		return true;

	// check whether the document was loaded correctly
	TiXmlDocument x_xml_doc;
    x_xml_doc.LoadFile(s_xml_pack_name);

	// get the root element
	TiXmlElement* px_root = x_xml_doc.RootElement();
	if (!px_root)
		error("Malformed XML in file.\n");
    TiXmlElement* px_cur = px_root->FirstChildElement();
	while (px_cur)
	{
        int i_ret;
        int i_int = -1;
        int i_stash_section = -1;
        int i_stash_id = -1;
	    PackDetails x_pack_details;
	    // get the STASH section
	    i_ret = px_cur->QueryIntAttribute(s_section_name, &i_int);
	    if (i_ret == TIXML_SUCCESS)
	        i_stash_section = i_int;
	    // get the STASH id
	    i_ret = px_cur->QueryIntAttribute(s_id_name, &i_int);
	    if (i_ret == TIXML_SUCCESS)
	        i_stash_id = i_int;
	    // fully formed STASH code?
	    if (i_stash_id != -1 && i_stash_section != -1)
	    {
	        // form the stash id
	        x_pack_details.i_stash = i_stash_section * 1000 + i_stash_id;
	        // now get the values - these can be zero
	        // packing amount
	        i_ret = px_cur->QueryIntAttribute(s_pack_name, &i_int);
	        // check value - can only be 1, 2 or 4
	        if (i_int != 0 && i_int != 1 && i_int != 2 && i_int != 4)
	        {
	            error("Unsupported byte packing value");
	            return false;
	        }
	        if (i_ret == TIXML_SUCCESS)
	            x_pack_details.i_pack = i_int;
	        // crop amount
	        i_ret = px_cur->QueryIntAttribute(s_crop_name, &i_int);
	        if (i_ret == TIXML_SUCCESS)
	            x_pack_details.i_crop = i_int;
	        // skip amount
	        i_ret = px_cur->QueryIntAttribute(s_skip_name, &i_int);
	        if (i_ret == TIXML_SUCCESS)
	            x_pack_details.i_skip = i_int;
	        // avg amount
	        i_ret = px_cur->QueryIntAttribute(s_avg_name, &i_int);
	        if (i_ret == TIXML_SUCCESS)
	            x_pack_details.i_avg = i_int;
            // add to the list if not already present
            if (find(x_pack_details_list.begin(), x_pack_details_list.end(), x_pack_details) == x_pack_details_list.end())
     	        x_pack_details_list.push_back(x_pack_details);
		}
	    else
	    {
	        error("Error in XML file: " + px_cur->ValueStr());
	        return false;
	    }
	    px_cur = px_cur->NextSiblingElement();
	}
	return true;
}

//*****************************************************************************

void BytePackVariable(Variable& x_variable, int i_nbytes)
{
    // Byte packing converts data to 1 or 2 byte words
    // the minimum value is the offset
    // (maximum-minimum) / 256^i_nbytes is the scaling factor
    
    float min = x_variable.GetMinimumValue();
    float max = x_variable.GetMaximumValue();
    float sf = (max-min) / (pow(256, i_nbytes)-2);
	float pmv = pow(256, i_nbytes)-1;				// packed missing value
	float offset = pow(256, i_nbytes)*0.5;
    
    // loop through the var_fields in the x_variable
    std::list<VarField>& x_var_fields = x_variable.GetVarFields();
    for (std::list<VarField>::iterator it_var_fields = x_var_fields.begin();
         it_var_fields != x_var_fields.end();
         it_var_fields++)
    {
        // get the region size
        int i_reg_size = it_var_fields->GetRegion().GetNCols() * it_var_fields->GetRegion().GetNRows();
        // loop through the data - this is easy, just every data point
        UM_WORD* px_data = it_var_fields->GetData();        
        for (int w=0; w<i_reg_size; w++)
        {            
            // get the value as a float
            float v = RAW_TO_FLOAT(px_data[w]);
            // if it is / is not the missing value
            if (v != x_variable.GetMissingValue())
            {
                // apply the offset and scaling - round to nearest integer
                float v2 = (v - min) * 1.0 / sf - offset;
                v2 = (float)((int)(v2));
                // transform back to a RAW
                int r = FLOAT_TO_RAW(v2);
                // put this back in at the same position
                px_data[w] = r;
            }
            else
            {
                px_data[w] = FLOAT_TO_RAW(pmv);
            }
        }
    }
    // set the packing bytes, offset and scaling factor
	if (i_nbytes != 0 && i_nbytes != 4)
	{
	    x_variable.SetPackingBytes(i_nbytes);
    	x_variable.SetScalingFactor(sf);
    	x_variable.SetOffset(min);
	}
}

//*****************************************************************************

void CropVariable(Variable& x_variable, int i_ncrop)
{
    // loop through the var_fields in the x_variable
    std::list<VarField>& x_var_fields = x_variable.GetVarFields();
    for (std::list<VarField>::iterator it_var_fields = x_var_fields.begin();
         it_var_fields != x_var_fields.end();
         it_var_fields++)
    {
        // get the original region size
        int i_oncols = it_var_fields->GetRegion().GetNCols();
        int i_onrows = it_var_fields->GetRegion().GetNRows();
        // crop the region
        it_var_fields->GetRegion().Crop(i_ncrop);
        // crop the data
        UM_WORD* px_data = it_var_fields->GetData();
        // calculate the starting offset
        int y_off = i_oncols-2*i_ncrop;
        int i_posn = i_ncrop * i_oncols + i_ncrop;
        // loop through and move the data
        for (int y=0; y<i_onrows-2*i_ncrop; y++)
        {
            for (int x=0; x<y_off; x++)
            {
                px_data[y*y_off+x] = px_data[i_posn];
                i_posn+=1;
            }
            i_posn += 2 * i_ncrop;
        }
    }
}

//*****************************************************************************

void SkipVariable(Variable& x_variable, int i_nskip)
{
    // loop through the var_fields in the x_variable
    std::list<VarField>& x_var_fields = x_variable.GetVarFields();
    for (std::list<VarField>::iterator it_var_fields = x_var_fields.begin();
         it_var_fields != x_var_fields.end();
         it_var_fields++)
    {
        // get the original region size
        int i_oncols = it_var_fields->GetRegion().GetNCols();
        int i_onrows = it_var_fields->GetRegion().GetNRows();
        it_var_fields->GetRegion().SubSample(i_nskip);
        // loop through the data - this is easy, just reposition data at pos/i_nskip
        UM_WORD* px_data = it_var_fields->GetData();
        int y_off = i_oncols / i_nskip;
        for (int y=0; y<i_onrows / i_nskip; y++)
        {
            int i_posn = y * i_oncols * i_nskip + (i_nskip/2)*i_oncols + i_nskip/2;  // start in middle of skip rather than left hand edge
            for (int x=0; x<y_off; x++)
            {
                px_data[y*y_off+x] = px_data[i_posn];
                i_posn += i_nskip;
            }
        }
    }
}

//*****************************************************************************

UM_WORD average(UM_WORD* px_data, int i_posn, int i_oncols, int i_navg)
{
    // calculate the weight
    float w = 1.0 / (i_navg * i_navg);
    float sum = 0.0;
    int z0 = int(i_navg/2);
    int z1 = int(float(i_navg)/2+0.5);
    for (int y=-z0; y<z1; y++)
        for (int x=-z0; x<z1; x++)
            sum += w * RAW_TO_FLOAT(px_data[i_posn + y*i_oncols+x]);
    return FLOAT_TO_RAW(sum);
}

//*****************************************************************************

void AvgVariable(Variable& x_variable, int i_navg)
{
    // this is same function as above, but with an averaging function
    // loop through the var_fields in the x_variable
    std::list<VarField>& x_var_fields = x_variable.GetVarFields();
    for (std::list<VarField>::iterator it_var_fields = x_var_fields.begin();
         it_var_fields != x_var_fields.end();
         it_var_fields++)
    {
        // get the original region size
        int i_oncols = it_var_fields->GetRegion().GetNCols();
        int i_onrows = it_var_fields->GetRegion().GetNRows();
        it_var_fields->GetRegion().SubSample(i_navg);
        // loop through the data - this is easy, just reposition data at pos/i_nskip
        UM_WORD* px_data = it_var_fields->GetData();
        int y_off = i_oncols/i_navg;
        for (int y=0; y<i_onrows/i_navg; y++)
        {
            int i_posn = y * i_oncols * i_navg + (i_navg/2)*i_oncols + i_navg/2;  // start in middle of skip rather than left hand edge
            for (int x=0; x<y_off; x++)
            {
                px_data[y*y_off+x] = average(px_data, i_posn, i_oncols, i_navg);
                i_posn += i_navg;
            }
        }
    }
}

//*****************************************************************************

void Packing::PackVariable(Variable& x_variable)
{
    // check in the pack list whether this variable should be packed or not
    int i_var_stash_code = x_variable.GetStashCode();
    int n_rows = x_variable.GetVarFields().front().GetRegion().GetNRows();
    int n_cols  = x_variable.GetVarFields().front().GetRegion().GetNCols();
    for (std::list<PackDetails>::iterator it_pack_list = x_pack_details_list.begin();
         it_pack_list != x_pack_details_list.end();
         it_pack_list++)
    {
        if (it_pack_list->i_stash == i_var_stash_code)
        {
            // this stash code is in the list of codes to pack, so figure out
            // what to do!
            // SNS Only do the Crop, Skip or Avg if the latitude or longitude dimension is less than the crop size.
            if (it_pack_list->i_pack != 0)
                BytePackVariable(x_variable, it_pack_list->i_pack);
            if (it_pack_list->i_crop != 0 && n_rows > it_pack_list->i_crop && n_cols > it_pack_list->i_crop)
                CropVariable(x_variable, it_pack_list->i_crop);
            if (it_pack_list->i_skip != 0 && n_rows > it_pack_list->i_crop && n_cols > it_pack_list->i_crop)
                SkipVariable(x_variable, it_pack_list->i_skip);
            if (it_pack_list->i_avg != 0 && n_rows > it_pack_list->i_crop && n_cols > it_pack_list->i_crop)
                AvgVariable(x_variable, it_pack_list->i_avg);
        }
    }
}

//*****************************************************************************

void Packing::PackVariables(std::list<Variable>& x_var_list)
{
    // loop through all variables and pack them
    for (std::list<Variable>::iterator it_var_list = x_var_list.begin();
         it_var_list != x_var_list.end();
         it_var_list ++)
    {
        PackVariable(*it_var_list);
    }
}
