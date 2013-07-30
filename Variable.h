//*****************************************************************************
//* File    : Variable.h
//* Author  : Neil Massey
//* Date    : 01/12/08
//* Purpose : Class to store consolidated variables
//*****************************************************************************

#ifndef VARIABLE_H
#define VARIABLE_H

#include <list>
#include "MTime.h"
#include "VarField.h"

class Variable
{
	public:
		Variable(VarField x_varfield);
		// checks whether a varfield belongs to this variable
		bool BelongsVF(const VarField& x_varfield) const;
		void AddVF(VarField x_varfield);
		std::list<VarField>& GetVarFields(void);
		
		int GetType(void) const;
		int GetFieldCode(void) const;
		int GetStashCode(void) const;
		int GetGridType(void);
		int GetProcCode(void) const;
		float GetMissingValue(void) const;
		float GetMKSValue(void) const;
		float GetSamplingFrequency(void) const;
		float GetMaximumValue(void);  // get the max / min values in the data
		float GetMinimumValue(void);  // required for the byte-packing algorithm

		// functions to set the number of time, level and region dimensions
		// to use
		void SetLevelID(int level_id);
		void SetRegionID(int region_id);
		void SetTimeID(int time_id);
		
		// functions to set the offset and scaling factor and packing bytes
		// used by the packing code
		void SetScalingFactor(float f_sf);
		void SetOffset(float f_off);
		void SetPackingBytes(int i_nbytes);

		float GetScalingFactor(void) const;
		float GetOffset(void) const;
		int GetPackingBytes(void) const;
		
		// function to get the id of the time, level and region
		int GetLevelID(void) const;
		int GetRegionID(void) const;
		int GetTimeID(void) const;
		
		// functions to set and get the netCDF variable associated with this
		void SetNcVar(class NcVar* px_ncvar);
		class NcVar* GetNcVar(void);
		void Print(void);
		int GetNumberOfFields(void) const
		{
		    return x_varfields.size();
		}

	private:
		std::list<VarField> x_varfields;
		class NcVar* px_ncvar;
		int   i_level_id;
		int   i_region_id;
		int	  i_time_id;
		float f_sf;
		float f_off;
		int   i_npack_bytes;
		std::string s_level_name;
};

#endif
