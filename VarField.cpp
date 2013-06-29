// File    : VarField.cpp
// Author  : Neil Massey
// Date    : 28/11/08
// Purpose : Class to store variable field details

#include "VarField.h"
#include <iostream>
extern float RAW_TO_FLOAT(unsigned int x);

//*****************************************************************************

VarField::VarField(int field_code, int stash_code, int field_type)
         : px_data_as_byte(NULL), px_data_as_short(NULL)
{
	i_field_code = field_code;
	i_stash_code = stash_code;
	i_field_type = field_type;
	i_pack_code  = 0;    		// data packing code
	i_proc_code  = 0;
	f_bmdi		 = 0.0f;       	// missing value 
	f_mks		 = 0.0f;        // scaling value
}

//*****************************************************************************

VarField::~VarField(void)
{
    if (px_data_as_byte != NULL)
        delete [] px_data_as_byte;
    if (px_data_as_short != NULL)
        delete [] px_data_as_short;
}

//*****************************************************************************

void VarField::SetBMDI_MKS(float bmdi, float mks)
{
	f_bmdi = bmdi;
	f_mks = mks;
}

//*****************************************************************************

void VarField::SetDims(Region region, Level level, TimeField time)
{
	x_region = region;
	x_time   = time;
	x_level  = level;
}

//*****************************************************************************

void VarField::SetPackCode(int pack_code)
{
	i_pack_code = pack_code;
}

//*****************************************************************************

void VarField::Print(void) const
{
	std::cout << i_field_code << "  " << i_stash_code << " " << i_field_type
              << std::endl;
	x_region.Print();
	x_level.Print();
	x_time.Print();
	std::cout << std::endl;
}

//*****************************************************************************

bool VarField::InGroup(const VarField& rhs) const
{
	// group defined as having same: time mean period, field code, level code
	//                               and region
	bool b_mean_period = (x_time.GetMeaningPeriod() == rhs.x_time.GetMeaningPeriod());
	bool b_region = (x_region == rhs.x_region);
	bool b_fc = (i_field_code == rhs.i_field_code);
	bool b_stash = (i_stash_code == rhs.i_stash_code);
	bool b_levtype = (x_level.GetType() == rhs.x_level.GetType());
    bool b_proc_code = (i_proc_code == rhs.i_proc_code);
	bool b_ret = b_mean_period & b_region & b_fc & b_stash & b_levtype & b_proc_code;
	return b_ret;
}

//*****************************************************************************

TimeField& VarField::GetTimeField(void)
{
	return x_time;
}

//*****************************************************************************

Level& VarField::GetLevel(void)
{
	return x_level;
}

//*****************************************************************************

Region& VarField::GetRegion(void)
{
	return x_region;
}

//*****************************************************************************

int VarField::GetType(void) const
{
	return i_field_type;
}

//*****************************************************************************

int VarField::GetFieldCode(void) const
{
	return i_field_code;
}

//*****************************************************************************

void VarField::SetData(UM_WORD* data)
{
	px_data = data;
}

//*****************************************************************************

UM_WORD* VarField::GetData(void) const
{
	return px_data;
}

//*****************************************************************************

ncbyte* VarField::GetDataAsByte(void)
{
    // transform the data to a byte
    // have to loop through, get the data and cast it
    int i_size = x_region.GetNRows() * x_region.GetNCols();
    if (px_data_as_byte == NULL)
    {    
        // create data array to put data in
        px_data_as_byte = new ncbyte[i_size];
        for (int i=0; i<i_size; i++)
            // do the casting
            px_data_as_byte[i] = (ncbyte)(RAW_TO_FLOAT(px_data[i]));
    }
    return px_data_as_byte;
}

//*****************************************************************************

short* VarField::GetDataAsShort(void)
{
    // transform the data to a short integer
    // have to loop through, get the data and cast it
    int i_size = x_region.GetNRows() * x_region.GetNCols();
    if (px_data_as_short == NULL)
    {    
        // create data array to put data in
        px_data_as_short = new short[i_size];
        for (int i=0; i<i_size; i++)
            // do the casting
            px_data_as_short[i] = (short)(RAW_TO_FLOAT(px_data[i]));
    }
    return px_data_as_short;
}

//*****************************************************************************

float VarField::GetMissingValue(void) const
{
	return f_bmdi;
}

//*****************************************************************************

int VarField::GetStashCode(void) const
{
	return i_stash_code;
}

//*****************************************************************************

float VarField::GetMKSValue(void) const
{
	return f_mks;
}

//*****************************************************************************

void VarField::SetProcCode(int proc_code)
{
	i_proc_code = proc_code;
}

//*****************************************************************************

int VarField::GetProcCode(void) const
{
	return i_proc_code;
}

//*****************************************************************************

void VarField::SetSamplingFrequency(float sampling_frequency)
{
    f_sampling_freq = sampling_frequency;
}

//*****************************************************************************

float VarField::GetSamplingFrequency(void) const
{
    return f_sampling_freq;
}
