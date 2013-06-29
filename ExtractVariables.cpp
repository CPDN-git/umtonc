// File    : ExtractVariables.cpp
// Author  : Neil Massey
// Date    : 03/12/08
// Purpose : Extract variables from the UM field file

#include "ExtractVariables.h"
#include "Region.h"
#include "Level.h"
#include "TimeField.h"
#include "VarField.h"
#include "RawConvert.h"
#include "Variable.h"

#include <iostream>

//*****************************************************************************

Region ExtractRegion(UM_WORD* px_fixhdr, UM_WORD* px_pphdrs, int idx)
{
    UM_WORD i_raw;
    // get the details needed
    i_raw = ReadValueFrom2D(px_fixhdr, px_pphdrs, PPHDR_IDX, 58, idx);  // 58 = BZY
    float f_slat = RAW_TO_FLOAT(i_raw);
    i_raw = ReadValueFrom2D(px_fixhdr, px_pphdrs, PPHDR_IDX, 59, idx);  // 59 = BDY
    float f_dlat = RAW_TO_FLOAT(i_raw);
    i_raw = ReadValueFrom2D(px_fixhdr, px_pphdrs, PPHDR_IDX, 60, idx);  // 60 = BZX
    float f_slon = RAW_TO_FLOAT(i_raw);
    i_raw = ReadValueFrom2D(px_fixhdr, px_pphdrs, PPHDR_IDX, 61, idx);  // 61 = BDX
    float f_dlon = RAW_TO_FLOAT(i_raw);
	// get the grid code
	UM_WORD i_gridc = ReadValueFrom2D(px_fixhdr, px_pphdrs, PPHDR_IDX, 15, idx);

	// get the size of the region
    UM_WORD i_nrows = ReadValueFrom2D(px_fixhdr, px_pphdrs, PPHDR_IDX, 17, idx);
    UM_WORD i_ncols = ReadValueFrom2D(px_fixhdr, px_pphdrs, PPHDR_IDX, 18, idx);

    // create the region
    Region x_region(i_gridc, f_slat, f_slon, f_dlat, f_dlon, i_nrows, i_ncols);
	// extract the pole location
	i_raw = ReadValueFrom2D(px_fixhdr, px_pphdrs, PPHDR_IDX, 55, idx);
	float f_plat = RAW_TO_FLOAT(i_raw);
	i_raw = ReadValueFrom2D(px_fixhdr, px_pphdrs, PPHDR_IDX, 56, idx);
	float f_plon = RAW_TO_FLOAT(i_raw);
	x_region.SetPoleLocation(f_plat, f_plon);
	
    return x_region;
}

//*****************************************************************************

Level ExtractLevel(UM_WORD* px_fixhdr, UM_WORD* px_pphdrs, int idx)
{
    UM_WORD i_raw;
    UM_WORD i_levtype = ReadValueFrom2D(px_fixhdr, px_pphdrs, PPHDR_IDX, 25, idx);
    i_raw = ReadValueFrom2D(px_fixhdr, px_pphdrs, PPHDR_IDX, 51, idx);
    float f_blev = RAW_TO_FLOAT(i_raw);  // B value or absolute value

    Level x_level;
    // convert hybrid coordinates
    if (i_levtype == 9)
    {
        i_raw = ReadValueFrom2D(px_fixhdr, px_pphdrs, PPHDR_IDX, 53, idx);
        float f_bhlev = RAW_TO_FLOAT(i_raw); // A value of level        
        x_level.Set(i_levtype, f_bhlev, f_blev, 1e5);
    }
	else if (i_levtype == 0)
	{
		// get the pseudo level number
		i_raw = ReadValueFrom2D(px_fixhdr, px_pphdrs, PPHDR_IDX, 42, idx);
		x_level.Set(i_levtype, float(i_raw));
	}
    else
        x_level.Set(i_levtype, f_blev);
    return x_level;
}

//*****************************************************************************

TimeField ExtractTime(UM_WORD* px_fixhdr, UM_WORD* px_pphdrs, int idx)
{
    UM_WORD i_lbtim = ReadValueFrom2D(px_fixhdr, px_pphdrs, PPHDR_IDX, 12, idx);
    UM_WORD px_val_time[6];
    UM_WORD px_dat_time[6];

    // read the validity time
    for (int i=0; i< 6; ++i)
    {
        px_val_time[i] = ReadValueFrom2D(px_fixhdr, px_pphdrs, PPHDR_IDX, 0+i, idx);
        px_dat_time[i] = ReadValueFrom2D(px_fixhdr, px_pphdrs, PPHDR_IDX, 6+i, idx);
    }
    TimeField x_time(i_lbtim, px_val_time, px_dat_time);
    return x_time;
}

//*****************************************************************************

VarField ExtractVarField(UM_WORD* px_fixhdr, UM_WORD* px_pphdrs, 
						 UM_WORD* px_data, int idx)
{
    // get var info
    UM_WORD i_fc = ReadValueFrom2D(px_fixhdr, px_pphdrs, PPHDR_IDX, 22, idx);
//    UM_WORD i_ffc = ReadValueFrom2D(px_fixhdr, px_pphdrs, PPHDR_IDX, 23, idx);
    UM_WORD i_ft = ReadValueFrom2D(px_fixhdr, px_pphdrs, PPHDR_IDX, 38, idx);
    UM_WORD i_sc = ReadValueFrom2D(px_fixhdr, px_pphdrs, PPHDR_IDX, 41, idx);
    UM_WORD i_packc = ReadValueFrom2D(px_fixhdr, px_pphdrs, PPHDR_IDX, 20, idx);
	UM_WORD i_procc = ReadValueFrom2D(px_fixhdr, px_pphdrs, PPHDR_IDX, 24, idx);
    UM_WORD i_raw;
    i_raw = ReadValueFrom2D(px_fixhdr, px_pphdrs, PPHDR_IDX, 62, idx);
    float f_bmdi = RAW_TO_FLOAT(i_raw);
    i_raw = ReadValueFrom2D(px_fixhdr, px_pphdrs, PPHDR_IDX, 63, idx);
    float f_bmks = RAW_TO_FLOAT(i_raw);

    // get the time information - interested in sampling frequency from this
    UM_WORD i_lbtim = ReadValueFrom2D(px_fixhdr, px_pphdrs, PPHDR_IDX, 12, idx);
    float sampling_frequency = -1.0;
    if (i_lbtim > 100)
        sampling_frequency = i_lbtim / 100;

    // create var and set pack code etc.
    VarField x_varfield(i_fc, i_sc, i_ft);
    x_varfield.SetPackCode(i_packc);
    x_varfield.SetBMDI_MKS(f_bmdi, f_bmks);
	x_varfield.SetProcCode(i_procc);
	x_varfield.SetSamplingFrequency(sampling_frequency);

    // read data - get the offset first
	UM_WORD i_data_start = px_fixhdr[159]-1;
    UM_WORD i_offset = ReadValueFrom2D(px_fixhdr, px_pphdrs, PPHDR_IDX, 28, idx);
	// set to the idx value for now
	x_varfield.SetData(px_data + i_offset - i_data_start);
    return x_varfield;
}

//*****************************************************************************

void ExtractVarFields(UM_WORD* px_fixhdr, UM_WORD* px_pphdrs, UM_WORD* px_data,
                      // these are the return variables
                      std::list<Variable>& x_var_list)
{
    // loop through the pp headers and extract the variable information
    // including the region, time and levels used
    std::list<Variable>::iterator ix_var_list_it;

    // 22 is field number
    for (int i_c=0; i_c<px_fixhdr[PPHDR_IDX+2]; i_c++)
    {
        // Extract the region, level and time
        Region  x_region = ExtractRegion(px_fixhdr, px_pphdrs, i_c);
        Level    x_level = ExtractLevel(px_fixhdr, px_pphdrs, i_c);
        TimeField x_time = ExtractTime(px_fixhdr, px_pphdrs, i_c);
        
        // Extract the variable field
        VarField x_varfield = ExtractVarField(px_fixhdr, px_pphdrs, px_data,
											  i_c);
        // set the dimensions to point to those created above
        x_varfield.SetDims(x_region, x_level, x_time);

        // search in existing added variables to see if this VarField belongs
        // to the variable
        bool b_found = false;
        for (ix_var_list_it = x_var_list.begin();
             ix_var_list_it != x_var_list.end();
             ix_var_list_it++)
            if (ix_var_list_it->BelongsVF(x_varfield))
            {
                ix_var_list_it->AddVF(x_varfield);
                b_found = true;
                break;
            }
        // not found so append to end
        if (!b_found)
        {
            Variable x_var(x_varfield);
            x_var_list.push_back(x_var);
        }
    }
    // x_var_list now contains all the variables with their associated regions
    // times, levels and data sorted by time and level
}
