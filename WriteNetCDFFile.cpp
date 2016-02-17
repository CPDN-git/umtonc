// File    : WriteNetCDFFile.h
// Author  : Neil Massey
// Date    : 03/11/08
// Purpose : Main function for writing out the netCDF files      

#include "netcdfcpp.h"

#include "WriteNetCDFFile.h"
#include "MinimiseDimensions.h"
#include "Variable.h"
#include "ToString.h"
#include "AttributeList.h"
#include "VarLevTrans.h"
#include "GlobalAtts.h"
#include "math.h"

//*****************************************************************************
const std::string s_time_name 			  ("time");
const std::string s_latitude_name         ("latitude");
const std::string s_longitude_name        ("longitude");
const std::string s_global_latitude_name  ("global_latitude");
const std::string s_global_longitude_name ("global_longitude");
const std::string s_rot_grid_map_name     ("rotated_pole");
//*****************************************************************************

void CreateTimeDims(NcFile& x_nc_file, 
					std::list<Series<TimeField> >& x_ts,
					const MTime& x_ref_time)
{
	std::list<Series<TimeField> >::iterator it_ts;
	std::list<float>::iterator it_ts_vals;
	int px_yz[6] = {0,0,0,0,0,0};

	// create the dimensions from the region, levels and timeseries
	for (it_ts = x_ts.begin(); it_ts != x_ts.end(); it_ts++)
	{
		// set the base time for this time type in the time series
		int i_time_type = it_ts->Get().front().GetType();
		MTime x_yz(px_yz, i_time_type);
		float f_yz = x_ref_time.CalcDays();
		it_ts->SetBaseValue(f_yz);

		// get the time series as a series of floating point values
		std::list<float> x_ts_vals = it_ts->GetValues();

		// create the name of the dimension
		std::string s_time_dim_name(s_time_name);
		s_time_dim_name += ToString(it_ts->GetID());

		// and create the dimension - unlimited for time 0
		NcDim* px_dim = NULL;
		if (it_ts->GetID() == 0)
			px_dim = x_nc_file.add_dim(s_time_dim_name.c_str());
		else
			px_dim = x_nc_file.add_dim(s_time_dim_name.c_str(), 
									   x_ts_vals.size());

		// Now add the variable corresponding to the dimension
		NcVar* px_var = x_nc_file.add_var(s_time_dim_name.c_str(),
										  ncFloat, px_dim);
		// Add attributes to the dimension variable
		px_var->add_att("standard_name", "time");
		px_var->add_att("axis", "T");
		px_var->add_att("calendar", it_ts->Get().front().GetCalendarType().c_str());
		// construct the days since string
		std::string s_time_string = "days since ";
		s_time_string += x_ref_time.GetFormatted();
		px_var->add_att("units", s_time_string.c_str());
		int i_lbtim = it_ts->Get().front().GetType();
		if (i_lbtim > 100)
		{
			std::string s_mp_string = ToString(it_ts->Get().front().GetMeaningPeriod()) + " hours";	
			px_var->add_att("meaning_period", s_mp_string.c_str());
		}
		
		// Now add the values to the dimension
		int i_c = 0;
		for (it_ts_vals = x_ts_vals.begin(); it_ts_vals != x_ts_vals.end();
			 it_ts_vals++)
		{
			px_var->set_cur(i_c);
			px_var->put(&(*it_ts_vals), 1L);
			i_c++;
		}
	}
}

//*****************************************************************************

std::string GetLevelName(NcFile& x_nc_file, const Level& x_lev, 
						 const VarLevTrans& x_var_lev_trans, int i_size)
{
	// get the attribute list
	const AttributeList& x_att_list = x_var_lev_trans.GetLevelAttributes(x_lev.GetType(), i_size);
	std::string s_base_name = "";
	// create the name of the dimension - now in the attribute list
	if (x_att_list.HasAttribute("out_name"))
		s_base_name = x_att_list.GetAttribute("out_name").GetValue();
	else
		s_base_name = "Z";		

	// check whether it exists in the file already
	int i_v = 1;
	std::string s_lev_dim_name = s_base_name + "_0";
	// fail silently as we're searching for names
	NcError x_err(NcError::silent_nonfatal);		
	while (x_nc_file.get_var(s_lev_dim_name.c_str()) != NULL)
	{
		s_lev_dim_name = s_base_name + "_" + ToString(i_v);
		i_v ++;
	}
	return s_lev_dim_name;
}

//*****************************************************************************

void CreateLevelDims(NcFile& x_nc_file,
					 std::list<Series<Level> >& x_ls,
					 const VarLevTrans& x_var_lev_trans)
{
    std::list<Series<Level> >::iterator it_ls;
    std::list<float>::iterator it_ls_vals;

    // create the dimensions from the region, levels and timeseries
    for (it_ls = x_ls.begin(); it_ls != x_ls.end(); it_ls++)
    {
		// get the time series as a series of floating point values
		std::list<float> x_ls_vals = it_ls->GetValues();

		// get the level
		Level& x_lev = it_ls->Get().front();
		// and create the dimension
		NcDim* px_dim = NULL;
		std::string s_lev_dim_name = GetLevelName(x_nc_file, x_lev, x_var_lev_trans, it_ls->Size());
		x_lev.SetName(s_lev_dim_name);
		px_dim = x_nc_file.add_dim(s_lev_dim_name.c_str(), x_ls_vals.size());
		// Now add the variable corresponding to the dimension
        NcVar* px_var = x_nc_file.add_var(s_lev_dim_name.c_str(), ncFloat, px_dim);

		// add attributes
		const AttributeList& x_att_list = x_var_lev_trans.GetLevelAttributes(x_lev.GetType(), it_ls->Size());
		px_var->add_att("axis", "Z");
		// check it was returned
		if (x_att_list.GetName() != "NULL")
		{
			// write the attributes out
			std::list<Attribute>::const_iterator it_att_list;
			for (it_att_list = x_att_list.Get().begin(); 
				 it_att_list != x_att_list.Get().end(); 
				 it_att_list++)
			{
				px_var->add_att(it_att_list->GetName().c_str(),
								it_att_list->GetValue().c_str());
			}
		}

		// Now add the values to the dimension
		int i_c = 0;
		for (it_ls_vals = x_ls_vals.begin(); it_ls_vals != x_ls_vals.end();
			 it_ls_vals++)
		{
			px_var->set_cur(i_c);
			px_var->put(&(*it_ls_vals), 1L);
			i_c++;
		}
	}
}

//*****************************************************************************

void CreateRegularRegionDim(NcFile& x_nc_file, const Region& x_region)
{
	// get the latitudes for the region
	std::list<float> x_lats = x_region.GetLatitudes();
	std::list<float> x_lons = x_region.GetLongitudes();
	std::list<float>::iterator it_f;

	// create the name of the dimension
	std::string s_lat_dim_name(s_latitude_name);
	s_lat_dim_name += ToString(x_region.GetID());
	std::string s_lon_dim_name(s_longitude_name);
	s_lon_dim_name += ToString(x_region.GetID());

	// and create the dimensions
	NcDim* px_lat_dim = x_nc_file.add_dim(s_lat_dim_name.c_str(),
                                          x_lats.size());
    NcDim* px_lon_dim = x_nc_file.add_dim(s_lon_dim_name.c_str(),
                                          x_lons.size());
		// then add the variables for the dims
	NcVar* px_lat_var = x_nc_file.add_var(s_lat_dim_name.c_str(),
										  ncFloat, px_lat_dim);
	NcVar* px_lon_var = x_nc_file.add_var(s_lon_dim_name.c_str(),
										  ncFloat, px_lon_dim);
	// add the attributes - just the name and degrees north / east
	px_lat_var->add_att("standard_name", "latitude");
	px_lon_var->add_att("standard_name", "longitude");
	px_lat_var->add_att("units", "degrees_north");
	px_lon_var->add_att("units", "degrees_east");
	px_lat_var->add_att("axis", "Y");
	px_lon_var->add_att("axis", "X");

	// now add the values
	int i_c = 0;
	for (it_f = x_lats.begin(); it_f != x_lats.end(); it_f++)
	{
		px_lat_var->set_cur(i_c);
		px_lat_var->put(&(*it_f), 1L);
		i_c++;
	}
	i_c = 0;
	for (it_f = x_lons.begin(); it_f != x_lons.end(); it_f++)
	{
		px_lon_var->set_cur(i_c);
		px_lon_var->put(&(*it_f), 1L);
		i_c++;
	}
}

//*****************************************************************************

void CreateRotatedRegionDim(NcFile& x_nc_file, Region& x_region)
{
	// get the rotated lats and lons
	std::list<float> x_rot_lats = x_region.GetLatitudes();
	std::list<float> x_rot_lons = x_region.GetLongitudes();
	// get the global lats and lons
	float *pf_global_lats, *pf_global_lons;
	x_region.GetGlobalCoordsForRotatedGrid(pf_global_lats, pf_global_lons);

	// create the rotated lat / lon name
	std::string s_rot_lat_name(s_latitude_name);
	s_rot_lat_name += ToString(x_region.GetID());
	std::string s_rot_lon_name(s_longitude_name);
	s_rot_lon_name += ToString(x_region.GetID());

	// create the rotated lat / lon dims and vars
	NcDim* px_rot_lat_dim = x_nc_file.add_dim(s_rot_lat_name.c_str(),
                  		                      x_rot_lats.size());
    NcDim* px_rot_lon_dim = x_nc_file.add_dim(s_rot_lon_name.c_str(),
                                          	  x_rot_lons.size());
        // then add the variables for the dims
    NcVar* px_rot_lat_var = x_nc_file.add_var(s_rot_lat_name.c_str(),
                  		                      ncFloat, px_rot_lat_dim);
    NcVar* px_rot_lon_var = x_nc_file.add_var(s_rot_lon_name.c_str(),
                                      		  ncFloat, px_rot_lon_dim);
    // add the attributes - these are different as they are for a rotated grid
    px_rot_lat_var->add_att("standard_name", "grid_latitude");
    px_rot_lon_var->add_att("standard_name", "grid_longitude");
    px_rot_lat_var->add_att("units", "degrees");
    px_rot_lon_var->add_att("units", "degrees");
    px_rot_lat_var->add_att("axis", "Y");
    px_rot_lon_var->add_att("axis", "X");

	// create the global lat / lon name
	std::string s_global_lat_name(s_global_latitude_name);
	s_global_lat_name += ToString(x_region.GetID());
	std::string s_global_lon_name(s_global_longitude_name);
	s_global_lon_name += ToString(x_region.GetID());
	
	// create the variables
	NcVar* px_glob_lat_var = x_nc_file.add_var(s_global_lat_name.c_str(), ncFloat,
											   px_rot_lat_dim, px_rot_lon_dim);
	NcVar* px_glob_lon_var = x_nc_file.add_var(s_global_lon_name.c_str(), ncFloat,
											   px_rot_lat_dim, px_rot_lon_dim);
	// add the attributes
    px_glob_lat_var->add_att("long_name", "latitude");
    px_glob_lon_var->add_att("long_name", "longitude");
    px_glob_lat_var->add_att("units", "degrees_north");
    px_glob_lon_var->add_att("units", "degrees_east");
    
    // create the grid mapping variable
    std::string s_grid_map_name(s_rot_grid_map_name);
    s_grid_map_name += ToString(x_region.GetID());
    NcVar* px_grid_map_var = x_nc_file.add_var(s_grid_map_name.c_str(),
    										   ncChar, 0L);
    px_grid_map_var->add_att("grid_mapping_name", "rotated_latitude_longitude");
    px_grid_map_var->add_att("grid_north_pole_latitude",
    						 x_region.GetPoleLatitude());
    px_grid_map_var->add_att("grid_north_pole_longitude",
    						 x_region.GetPoleLongitude());

    // now add the values for the global and the rotated grid
    // rotated grid
    std::list<float>::iterator it_f;
    int i_c = 0;
    for (it_f = x_rot_lats.begin(); it_f != x_rot_lats.end(); it_f++)
    {
        px_rot_lat_var->set_cur(i_c);
        px_rot_lat_var->put(&(*it_f), 1L);
        i_c++;
    }
    i_c = 0;
    for (it_f = x_rot_lons.begin(); it_f != x_rot_lons.end(); it_f++)
    {
        px_rot_lon_var->set_cur(i_c);
        px_rot_lon_var->put(&(*it_f), 1L);
        i_c++;
    }
	// global grid
	px_glob_lat_var->put(pf_global_lats, x_rot_lats.size(), x_rot_lons.size());
	px_glob_lon_var->put(pf_global_lons, x_rot_lats.size(), x_rot_lons.size());
}

//*****************************************************************************

void CreateRegionDims(NcFile& x_nc_file, std::list<Region>& x_rs)
{
	// create the latitude / longitude grids for the various regions
	std::list<Region>::iterator it_rs;

	for (it_rs = x_rs.begin(); it_rs != x_rs.end(); it_rs++)
	{
		if (it_rs->GetGridType() / 100 > 0)	// rotated grid
			CreateRotatedRegionDim(x_nc_file, *it_rs);
		else
			CreateRegularRegionDim(x_nc_file, *it_rs);
	}
}

//*****************************************************************************

void GetDimensions(NcFile& x_nc_file, Variable* px_var, 
				   std::list<Series<Level> >* px_levs,
				   NcDim*& px_lond, NcDim*& px_latd, NcDim*& px_zd, NcDim*& px_td)
{
	// get the (already defined) dimensions for a variable
	std::string s_lat_name(s_latitude_name);
	s_lat_name += ToString(px_var->GetRegionID());
	std::string s_lon_name(s_longitude_name);
	s_lon_name += ToString(px_var->GetRegionID());
	std::string s_t_name(s_time_name);
	s_t_name += ToString(px_var->GetTimeID());
	int i_lev_idx = px_var->GetLevelID();
	std::string s_l_name = "";
	for (std::list<Series<Level> >::iterator it_lev = px_levs->begin();
		 it_lev != px_levs->end(); it_lev++)
		if (it_lev->GetID() == i_lev_idx)
			s_l_name = it_lev->Get().front().GetName();
	px_lond = x_nc_file.get_dim(s_lon_name.c_str());
	px_latd = x_nc_file.get_dim(s_lat_name.c_str());
	px_td   = x_nc_file.get_dim(s_t_name.c_str());
	px_zd   = x_nc_file.get_dim(s_l_name.c_str());
}

//*****************************************************************************

NcType UMtoNCtype(int i_um_type)
{
	// convert the um type to the netCDF type
	NcType type_val;
	switch(i_um_type)
	{
		case 1: case -1:
			type_val = ncFloat;
			break;
		case 2: case -2:
			type_val = ncInt;
			break;
		case 3: case -3:
			type_val = ncByte;
			break;
		default:
			type_val = ncFloat;
	}
	return type_val;
}

//*****************************************************************************

std::string GetVarName(NcFile& x_nc_file, const Variable& x_var, const VarLevTrans& x_var_lev_trans)
{
	// fail silently as we're searching for names
	NcError x_err(NcError::silent_nonfatal);
	std::string s_base_name;
	const AttributeList& x_att_list =  x_var_lev_trans.GetVariableAttributes(x_var.GetStashCode());
	// create the var name - this is a CMOR name if the attribute "out_name" exists
	if (x_att_list.HasAttribute("out_name"))
		s_base_name = x_att_list.GetAttribute("out_name").GetValue();
	else
		s_base_name = "field" + ToString(x_var.GetFieldCode());
	// check on cell method whether to add min or max - see GetCellMethod function
	int i_pcode = x_var.GetProcCode();
	if ((i_pcode & 4096) != 0)
		s_base_name += "min";
	if ((i_pcode & 8192) != 0)
		s_base_name += "max";
	
	// check whether it exists in the file already
	int i_c = 1;
	std::string s_var_name = s_base_name + "_0";
	while (x_nc_file.get_var(s_var_name.c_str()) != NULL)
	{
		s_var_name = s_base_name + "_" + ToString(i_c);
		i_c ++;
	}
	return s_var_name;
}

//*****************************************************************************

std::string GetCellMethod(int i_proc_code)
{
	std::string s_ret_string = "";
	if ((i_proc_code & 1) != 0)
		s_ret_string += "lon: lat: time: difference from another experiment ";
	if ((i_proc_code & 2) != 0)
		s_ret_string += "lon: lat: difference from mean ";
	if ((i_proc_code & 4) != 0)
		s_ret_string += "time: difference from mean ";
	if ((i_proc_code & 8) != 0)
		s_ret_string += "lon: derivative ";
	if ((i_proc_code & 16) != 0)
		s_ret_string += "lat: derivative ";
	if ((i_proc_code & 32) != 0)
		s_ret_string += "time: derivative ";
	if ((i_proc_code & 64) != 0)
		s_ret_string += "lon: lat: mean ";
	if ((i_proc_code & 128) != 0)
		s_ret_string += "time: mean ";
	if ((i_proc_code & 256) != 0)
		s_ret_string += "lon: lat: product of two fields ";
	if ((i_proc_code & 512) != 0)
		s_ret_string += "lon: lat: square root ";
	if ((i_proc_code & 1024) != 0)
		s_ret_string += "level: difference between fields at BLEV and BRLEV ";
	if ((i_proc_code & 2048) != 0)
		s_ret_string += "level: mean over layer between BLEV and BRLEV ";
	if ((i_proc_code & 4096) != 0)
		s_ret_string += "time: minimum ";
	if ((i_proc_code & 8192) != 0)
		s_ret_string += "time: maximum ";
	if ((i_proc_code & 16384) != 0)
		s_ret_string += "lon: lat: magnitude of vector ";
	if ((i_proc_code & 32768) != 0)
		s_ret_string += "lon: lat: log10 ";
	if ((i_proc_code & 65536) != 0)
		s_ret_string += "lon: lat: variance ";
	if ((i_proc_code & 131072) != 0)
		s_ret_string += "lon: lat: mean over ensemble ";
	if ((i_proc_code & 262144) != 0)
		s_ret_string += "lon: lat: band pass filter ";
	if ((i_proc_code & 524288) != 0)
		s_ret_string += "lon: lat: low pass filter ";

	return s_ret_string;
}

//*****************************************************************************

void CreateVariables(NcFile& x_nc_file, std::list<Variable>& x_var_list,
                     std::list<Series<Level> >& x_lev_list,
                     const VarLevTrans& x_var_lev_trans)
{
	// create the variables from the var list
	std::list<Variable>::iterator it_var_list;
	for (it_var_list = x_var_list.begin(); it_var_list != x_var_list.end();
		 it_var_list++)
	{
		// get the dimensions
		NcDim *px_lond, *px_latd, *px_td, *px_zd;
		GetDimensions(x_nc_file, &*(it_var_list), &x_lev_list,
					  px_lond, px_latd, px_zd, px_td);
		// get the variable type
		NcType i_type = ncFloat;
		// check whether this is a packed variable or not and change the type
		// if it is
		switch(it_var_list->GetPackingBytes())
		{
			case 0:
				i_type = UMtoNCtype(it_var_list->GetType());	// not packed
				break;
			case 1:
				i_type = ncByte;
				break;
			case 2:
				i_type = ncShort;
				break;
			case 4:
				i_type = ncFloat;
				break;
		}
		// create the variable name
		std::string s_var_name = GetVarName(x_nc_file, *it_var_list, x_var_lev_trans);
		// create the actual variable
		NcVar* px_var = x_nc_file.add_var(s_var_name.c_str(), i_type,
										  px_td, px_zd, px_latd, px_lond);

		// add missing value attribute - need to switch on packing type
		// also add _Unsigned attribute to ncbyte and short
		float mv = it_var_list->GetMissingValue();
		switch(it_var_list->GetPackingBytes())
		{
			case 0:
				px_var->add_att("_FillValue", mv);
			break;
			case 1:
                mv = (pow(256, 1)-1) * 0.5;
				px_var->add_att("_FillValue", (ncbyte)(mv));
			break;
			case 2:
                mv = (pow(256, 2)-1) * 0.5;
				px_var->add_att("_FillValue", (short)(mv));
			break;
			case 4:
                mv = it_var_list->GetMissingValue();
				px_var->add_att("_FillValue", (float)(mv));
			break;
		}
		// calculate and add scaling value and offset value
		float f_mks = it_var_list->GetMKSValue();
		float f_scl = it_var_list->GetScalingFactor();
		float f_off = it_var_list->GetOffset();
        int   i_pack = it_var_list->GetPackingBytes();
		// ipack = 4 will just cast to a float
        if (i_pack != 0 && i_pack != 4)
		{
    		if (f_mks != 0.0)
	    		f_scl = f_scl * f_mks;
			px_var->add_att("scale_factor", f_scl);
			px_var->add_att("add_offset", f_off + pow(256, i_pack)*0.5*f_scl);
		}

		// if the grid is rotated, then it has an extra attribute
		if (it_var_list->GetGridType() > 100)
		{
			std::string s_grid_mapping_name(s_rot_grid_map_name);
			s_grid_mapping_name += ToString(it_var_list->GetRegionID());
			px_var->add_att("grid_mapping", s_grid_mapping_name.c_str());
			// add the coordinates for the rotated grid
			std::string s_global_lat("global_latitude");
			s_global_lat += ToString(it_var_list->GetRegionID());
			std::string s_global_lon("global_longitude");
			s_global_lon += ToString(it_var_list->GetRegionID());
			std::string s_grid_coordinates = s_global_lon + " " + s_global_lat;
			px_var->add_att("coordinates", s_grid_coordinates.c_str());
		}

		// add the other attributes
		int i_stash_code = it_var_list->GetStashCode();
        const AttributeList& x_att_list = x_var_lev_trans.GetVariableAttributes(i_stash_code);
		
        // check it was returned
        if (x_att_list.GetName() != "NULL")
        {
            // write the attributes out
            std::list<Attribute>::const_iterator it_att_list;
            for (it_att_list = x_att_list.Get().begin();
                 it_att_list != x_att_list.Get().end();
                 it_att_list++)
            {
                px_var->add_att(it_att_list->GetName().c_str(),
                                it_att_list->GetValue().c_str());
            }
        }
		else
		{
			// add the stash number and item as attributes
			int i_stash_code = it_var_list->GetStashCode();
			px_var->add_att("stash_item", i_stash_code % 1000);
			px_var->add_att("stash_section", i_stash_code / 1000);
		}

		// add the cell_method attribute
		std::string s_cell_method = GetCellMethod(it_var_list->GetProcCode());
		if (s_cell_method != "")
			px_var->add_att("cell_method", s_cell_method.c_str());

		// set the sampling frequency - if it exists
		float f_samp_freq = it_var_list->GetSamplingFrequency();
		if (f_samp_freq > -1.0)
		{
			std::string s_samp_freq = ToString(f_samp_freq) + " hours";
			px_var->add_att("sampling_frequency", s_samp_freq.c_str());
		}
		
		// set the variable here and then write the data later
		// this prevents the netCDF file flipping in and out of define
		// mode which slows down the write a lot.
		it_var_list->SetNcVar(px_var);
	}
}

//*****************************************************************************

void WriteVariables(NcFile& x_nc_file, std::list<Variable>& x_var_list, 
					std::list<Series<Level> >& x_lev_list)
{
    // write to the variables in the var list
    std::list<Variable>::iterator it_var_list;
    for (it_var_list = x_var_list.begin(); it_var_list != x_var_list.end();
         it_var_list++)
    {
        // get the dimensions
        NcDim *px_lond, *px_latd, *px_td, *px_zd;
        GetDimensions(x_nc_file, &*(it_var_list), &x_lev_list,
                      px_lond, px_latd, px_zd, px_td);

        // now write the data varfield by varfield
        std::list<VarField> x_vf = it_var_list->GetVarFields();
        std::list<VarField>::iterator it_vf;
        long i_lon_len = px_lond->size();
        long i_lat_len = px_latd->size();
        long i_z_size  = px_zd->size();
        long i_t = 0;
        long i_z = 0;

		// get the previously created variable
		NcVar* px_var = it_var_list->GetNcVar();
		// write the data
        for (it_vf = x_vf.begin(); it_vf != x_vf.end(); it_vf++)
        {
        	// set the current cursor position
            px_var->set_cur(i_t, i_z, 0L, 0L);
            // get the data from the variable
            float *px_f = (float*)(it_vf->GetData());
            // Check whether this is packed or not
        	switch(it_var_list->GetPackingBytes())
        	{
        		case 0: 	// no packing
					px_var->put(px_f, 1L, 1L, i_lat_len, i_lon_len);
        		break;
	        	case 1:		// byte
	        	{
	        		ncbyte* px_b = it_vf->GetDataAsByte();
		            px_var->put(px_b, 1L, 1L, i_lat_len, i_lon_len);
		        	break;
		        }
	        	case 2:		// short
	        	{
	        		short* px_s = it_vf->GetDataAsShort();
	        		px_var->put(px_s, 1L, 1L, i_lat_len, i_lon_len);
		        	break;
		        }
	        	case 4:		// float - just write out.  future compatibility to compress doubles to floats
	        	{
		            px_var->put(px_f, 1L, 1L, i_lat_len, i_lon_len);
			        break;
			    }
	        }
            
            // go to next z level and then next timestep
            i_z++;
            if (i_z == i_z_size)
            {
                i_t++;
                i_z=0;
            }
        }
    }
}

//*****************************************************************************

int WriteGlobalAtts(NcFile& x_nc_file, const GlobalAtts& x_global_atts)
{
	// add the cf compliance global attribute
	x_nc_file.add_att("Conventions", "CF-1.3");
	if (!x_global_atts.Valid())
		return 0;
	
	std::list<Attribute>::const_iterator it_atts;
	// get maximum size and create the storage - we're going to cast a 
	// pointer to what we want the storage to be though
	int* pi_buffer = new int[x_global_atts.GetMaxSize()];
	// loop through the attributes
	for (it_atts  = x_global_atts.GetAttributes().Get().begin();
		 it_atts != x_global_atts.GetAttributes().Get().end();
		 it_atts++)
	{
		if (it_atts->GetType() == "string")
		{
			int i_count = 0;
			char* pc_buffer = (char*)(pi_buffer);
            std::list<std::string>::const_iterator it_values;
            for (it_values  = it_atts->GetValues().begin();
                 it_values != it_atts->GetValues().end();
                 it_values ++)
            {
				memcpy(pc_buffer+i_count, it_values->c_str(),
					   it_values->size());
				i_count += it_values->size();
				pc_buffer[i_count++] = ',';
            }
			pc_buffer[--i_count] = '\0';
			// this is the only way I could determine how to write an
			// array of string attributes to the file
			x_nc_file.add_att(it_atts->GetName().c_str(),
							  pc_buffer);
		}
		else if (it_atts->GetType() == "integer")
		{
			int i_count = 0;
			std::list<std::string>::const_iterator it_values;
            for (it_values  = it_atts->GetValues().begin();
                 it_values != it_atts->GetValues().end();
                 it_values ++)
            {
				int i_val = FromString<int>(*it_values);
				pi_buffer[i_count++] = i_val;
			}
			x_nc_file.add_att(it_atts->GetName().c_str(),
							  it_atts->GetNumber(),
							  pi_buffer);
		}
		else if (it_atts->GetType() == "float")
		{
			int i_count = 0;
			float* pf_buffer = (float*)(pi_buffer);
            std::list<std::string>::const_iterator it_values;
            for (it_values  = it_atts->GetValues().begin();
                 it_values != it_atts->GetValues().end();
                 it_values ++)
            {
                float i_val = FromString<float>(*it_values);
                pf_buffer[i_count++] = i_val;
            }
            x_nc_file.add_att(it_atts->GetName().c_str(),
                              it_atts->GetNumber(),
                              pf_buffer);			
		}
     	else if (it_atts->GetType() == "double")
        {
            int i_count = 0;
            double* pf_buffer = (double*)(pi_buffer);
            std::list<std::string>::const_iterator it_values;
            for (it_values  = it_atts->GetValues().begin();
                 it_values != it_atts->GetValues().end();
                 it_values ++)
            {
                double i_val = FromString<double>(*it_values);
                pf_buffer[i_count++] = i_val;
            }
            x_nc_file.add_att(it_atts->GetName().c_str(),
                              it_atts->GetNumber(),
                              pf_buffer);
        }
        else if (it_atts->GetType() == "short")
        {
            int i_count = 0;
            short* ps_buffer = (short*)(pi_buffer);
            std::list<std::string>::const_iterator it_values;
            for (it_values  = it_atts->GetValues().begin();
                 it_values != it_atts->GetValues().end();
                 it_values ++)
            {
                short int i_val = FromString<short int>(*it_values);
                ps_buffer[i_count++] = i_val;
            }
            x_nc_file.add_att(it_atts->GetName().c_str(),
                              it_atts->GetNumber(),
                              ps_buffer);
        }
        else if (it_atts->GetType() == "byte")
        {
            int i_count = 0;
            char* pc_buffer = (char*)(pi_buffer);
            std::list<std::string>::const_iterator it_values;
            for (it_values  = it_atts->GetValues().begin();
                 it_values != it_atts->GetValues().end();
                 it_values ++)
            {
                char i_val = FromString<char>(*it_values);
                pc_buffer[i_count++] = i_val;
            }
            x_nc_file.add_att(it_atts->GetName().c_str(),
                              it_atts->GetNumber(),
                              pc_buffer);
        }
	}
	delete [] pi_buffer;
	return 0;
}

//*****************************************************************************

int WriteNetCDFFile(std::string s_output_file_name, 
					std::list<Variable>& x_var_list,
					const MTime& x_ref_time,
					const VarLevTrans& x_var_lev_trans,
					const GlobalAtts& x_global_atts)
{
	// open the netCDF file
	NcFile x_nc_file(s_output_file_name.c_str(), NcFile::New);
	if (!x_nc_file.is_valid())
	{
		std::cerr << "Error: Output file: " << s_output_file_name <<
                     " already exists or could not be opened" << std::endl;
		return 1;
	}

	// get the minimal set of dimensions
	std::list<Series<TimeField> > x_min_ts = MinimiseTimes(x_var_list);
	std::list<Region> x_min_reg = MinimiseRegions(x_var_list);
	std::list<Series<Level> > x_min_lev = MinimiseLevels(x_var_list);

	// write the global attributes
	WriteGlobalAtts(x_nc_file, x_global_atts);
	// write the dimensions to the file
	CreateTimeDims(x_nc_file, x_min_ts, x_ref_time);
	CreateLevelDims(x_nc_file, x_min_lev, x_var_lev_trans);
	CreateRegionDims(x_nc_file, x_min_reg);
	CreateVariables(x_nc_file, x_var_list, x_min_lev, x_var_lev_trans);
	WriteVariables(x_nc_file, x_var_list, x_min_lev);

	x_nc_file.close();
	return 0;
}
