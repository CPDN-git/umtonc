//*****************************************************************************
//* File    : WriteNetCDFFile.h
//* Author  : Neil Massey
//* Date    : 03/11/08
//* Purpose : Main function for writing out the netCDF files
//*****************************************************************************

#ifndef WRITE_NETCDF_H
#define WRITE_NETCDF_H

#include <string>
#include <list>

int WriteNetCDFFile(std::string s_output_file_name,
					 std::list<class Variable>& x_var_list,
					 const class MTime& x_ref_time,
					 const class VarLevTrans& x_var_lev_trans,
					 const class GlobalAtts& x_global_atts);

#endif
