// File    : Variable.cpp
// Author  : Neil Massey
// Date    : 01/12/08
// Purpose : Class to store consolidated variables

#include "Variable.h"

#include <algorithm>
#include <iostream>

extern UM_WORD FLOAT_TO_RAW(float x);
extern float RAW_TO_FLOAT(unsigned int x);

Variable::Variable(VarField x_varfield)
{
	AddVF(x_varfield);

	i_level_id  = -1;
	i_time_id   = -1;
	i_region_id = -1;
	
	f_sf = 2e20;
	f_off = 2e20;
	i_npack_bytes = 0;
}

//*****************************************************************************

bool Variable::BelongsVF(VarField& x_varfield)
{
	bool b_belongs = false;
	std::list<VarField>::iterator it = x_varfields.begin();
	if (it->InGroup(x_varfield))
		b_belongs = true;
	// check whether the time matches any in the varfields already, excluding
	// the same time on different levels.  If it does then create a new group of varfields
    bool b_match = false;
    bool b_level = false;
    bool b_time  = false;
	for (it = x_varfields.begin(); it != x_varfields.end(); it++)
	{
	    b_level = it->GetLevel().GetValue() == x_varfield.GetLevel().GetValue();
	    b_time  = it->GetTimeField().GetValue() == x_varfield.GetTimeField().GetValue();
	    b_match |= b_level && b_time;
	}
	    
	return b_belongs && (!b_match);
}

//*****************************************************************************

bool Compare(VarField& lhs, VarField& rhs)
{
	// sort in ascending order of time and level
	bool b_cmp = lhs.GetTimeField().GetValDate() < rhs.GetTimeField().GetValDate();
	switch (lhs.GetLevel().GetType())
	{
		// switch on level type so that heights can be sorted ascending
		// and pressures descending
		case 8:
		case 9:
		case 10:		// pressures
			b_cmp &= (lhs.GetLevel().GetValue() >= rhs.GetLevel().GetValue());
			break;
		default:
			b_cmp &= (lhs.GetLevel().GetValue() <= rhs.GetLevel().GetValue());
			break;

	}
	return b_cmp;
}

//*****************************************************************************

void Variable::AddVF(VarField x_varfield)
{
	x_varfields.push_back(x_varfield);
	
	// sort the var field list on time then level
	x_varfields.sort(Compare);
}

//*****************************************************************************

std::list<VarField>& Variable::GetVarFields(void)
{
	return x_varfields;
}

//*****************************************************************************

void Variable::Print(void)
{
	// start date now given in the list of varfields
	std::cout << "Var start" << std::endl;
	x_varfields.front().GetTimeField().Print();
	x_varfields.back().GetTimeField().Print();
}

//*****************************************************************************

void Variable::SetLevelID(int level_id)
{
	i_level_id = level_id;
}

//*****************************************************************************

void Variable::SetRegionID(int region_id)
{
	i_region_id = region_id;
}

//*****************************************************************************

void Variable::SetTimeID(int time_id)
{
	i_time_id = time_id;
}

//*****************************************************************************

int Variable::GetLevelID(void) const
{
	return i_level_id;
}

//*****************************************************************************

int Variable::GetTimeID(void) const
{
	return i_time_id;
}

//*****************************************************************************

int Variable::GetRegionID(void) const
{
	return i_region_id;
}

//*****************************************************************************

int Variable::GetType(void) const
{
	return x_varfields.front().GetType();
}

//*****************************************************************************

int Variable::GetFieldCode(void) const
{
	return x_varfields.front().GetFieldCode();
}

//*****************************************************************************

int Variable::GetStashCode(void) const
{
	return x_varfields.front().GetStashCode();
}

//*****************************************************************************

float Variable::GetMissingValue(void) const
{
	return x_varfields.front().GetMissingValue();
}

//*****************************************************************************

float Variable::GetMKSValue(void) const
{
	return x_varfields.front().GetMKSValue();
}

//*****************************************************************************

float Variable::GetSamplingFrequency(void) const
{
    return x_varfields.front().GetSamplingFrequency();
}

//*****************************************************************************

float Variable::GetMaximumValue(void)
{
    // loop through all the varfields for this variable and get the max and min
    // values - not including the missing value
    float mv = GetMissingValue();
    float max = -2e20;
    for (std::list<VarField>::iterator it_var_fields = x_varfields.begin();
         it_var_fields != x_varfields.end();
         it_var_fields++)
    {
        int i_reg_size = it_var_fields->GetRegion().GetNCols() * it_var_fields->GetRegion().GetNRows();
        UM_WORD* px_data = it_var_fields->GetData();
        for (int i=0; i<i_reg_size; i++)
        {
            float v = RAW_TO_FLOAT(px_data[i]);
            if (v != mv && v > max)
                max = v;
        }
    }
    return max;
}

//*****************************************************************************

float Variable::GetMinimumValue(void)
{
    // loop through all the varfields for this variable and get the max and min
    // values - not including the missing value
    float mv = GetMissingValue();
    float min = 2e20;
    for (std::list<VarField>::iterator it_var_fields = x_varfields.begin();
         it_var_fields != x_varfields.end();
         it_var_fields++)
    {
        int i_reg_size = it_var_fields->GetRegion().GetNCols() * it_var_fields->GetRegion().GetNRows();
        UM_WORD* px_data = it_var_fields->GetData();
        for (int i=0; i<i_reg_size; i++)
        {
            float v = RAW_TO_FLOAT(px_data[i]);
            if (v != mv && v < min)
                min = v;
        }
    }
    return min;
}

//*****************************************************************************

void Variable::SetNcVar(class NcVar* ncvar)
{
	px_ncvar = ncvar;
}

//*****************************************************************************

class NcVar* Variable::GetNcVar(void)
{
	return px_ncvar;
}

//*****************************************************************************

int Variable::GetGridType(void)
{
	return x_varfields.front().GetRegion().GetGridType();
}

//*****************************************************************************

int Variable::GetProcCode(void) const
{
	return x_varfields.front().GetProcCode();
}

//*****************************************************************************

void Variable::SetScalingFactor(float sf)
{
    f_sf = sf;
}

//*****************************************************************************

void Variable::SetOffset(float off)
{
    f_off = off;
}

//*****************************************************************************

void Variable::SetPackingBytes(int nbytes)
{
    i_npack_bytes = nbytes;
}

//*****************************************************************************

float Variable::GetScalingFactor(void) const
{
    return f_sf;
}

//*****************************************************************************

float Variable::GetOffset(void) const
{
    return f_off;
}

//*****************************************************************************
		
int Variable::GetPackingBytes(void) const
{
	return i_npack_bytes;
}
