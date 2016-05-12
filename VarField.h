//*****************************************************************************
//* File    : VarField.h
//* Author  : Neil Massey
//* Date    : 28/11/08
//* Purpose : Class to store variable field details
//*****************************************************************************

#ifndef VARFIELD_H
#define VARFIELD_H

#include "Level.h"
#include "Region.h"
#include "TimeField.h"
#include "ReadUMFile.h"
#include "netcdfcpp.h"

class VarField
{
	public:
		VarField(int model_code, int field_code, int stash_code, int field_type);
		~VarField(void);

		int GetType(void) const;
		int GetFieldCode(void) const;
		int GetStashCode(void) const;
		int GetModelCode(void) const;
		int GetProcCode(void) const;
		float GetMissingValue(void) const;
		float GetMKSValue(void) const;
		UM_WORD* GetData(void) const;
        ncbyte* GetDataAsByte(void);
		short* GetDataAsShort(void);
		float GetSamplingFrequency(void) const;
		
		void SetBMDI_MKS(float bmdi, float mks);
		void SetPackCode(int pack_code);
		void SetProcCode(int proc_code);
		void SetDims(Region region, Level level, TimeField time);
		void SetData(UM_WORD* data);
		void SetSamplingFrequency(float sampling_frequency);
		
		void Print(void) const;
		bool InGroup(const VarField& rhs) const;
		
		TimeField& GetTimeField(void);
		Level& GetLevel(void);
		Region& GetRegion(void);

	private:
		// associated dimensions with the variable field
		Region    x_region;
		TimeField x_time;
		Level     x_level;
		UM_WORD*  px_data;
		ncbyte*   px_data_as_byte;
		short*    px_data_as_short;

		int i_field_code;
		int i_field_type;
		int i_stash_code;
		int i_pack_code;	// data packing code
		int i_proc_code;	// processing code, mean, max, min, variance etc.
		int i_model_code;   // code for the model 1=atmos, 2=ocean, 4=wave
		float f_bmdi;		// missing value
		float f_mks;		// scaling value
		float f_sampling_freq;  // sampling frequency
};

#endif
