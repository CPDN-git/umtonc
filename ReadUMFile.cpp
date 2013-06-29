// File    : ReadHdrs.cpp
// Author  : Neil Massey
// Date    : 20/11/08
// Purpose : Functions to read parts of the UM headers           

#include "ReadUMFile.h"
#include <iostream>

//*****************************************************************************

void ReadHdrs(std::ifstream& x_file,
              UM_WORD* &px_fixhdr, UM_WORD* &px_pphdrs,
              UM_WORD* &px_intc,   UM_WORD* &px_realc,
              UM_WORD* &px_levc,   UM_WORD* &px_rowc,
              UM_WORD* &px_colc,   UM_WORD* &px_fieldc)
{
	// set all to NULL to start with
	px_fixhdr = px_pphdrs = px_intc = px_realc = NULL;
	px_levc = px_rowc = px_colc = px_fieldc = NULL;

	// read the fixed header
	px_fixhdr = ReadFixHdr(x_file);
	// read the number of fields and fix the fixed header accordingly
	UM_WORD i_n_fields = ReadNumberOfFields(x_file, px_fixhdr);
	px_fixhdr[PPHDR_IDX+2] = i_n_fields;

	// check for integer constants and read if present
	if (px_fixhdr[INTC_IDX] != 0)
		px_intc = Read1DConstants(x_file, px_fixhdr, INTC_IDX);

	// check for real constants and read if present
	if (px_fixhdr[REALC_IDX] != 0)
		px_realc = Read1DConstants(x_file, px_fixhdr, REALC_IDX);

	// check for level constants and read if present
	if (px_fixhdr[LEVC_IDX] != 0)
		px_levc = Read2DConstants(x_file, px_fixhdr, LEVC_IDX);

	// check for row constants and read if present
	if (px_fixhdr[ROWC_IDX] != 0)
		px_rowc = Read2DConstants(x_file, px_fixhdr, ROWC_IDX);

	// check for column constants and read if present
	if (px_fixhdr[COLC_IDX] != 0)
		px_colc = Read2DConstants(x_file, px_fixhdr, COLC_IDX);

	// check for field constants and read if present
	if (px_fixhdr[FIELDC_IDX] != 0)
		px_fieldc = Read2DConstants(x_file, px_fixhdr, FIELDC_IDX);

	// check for pp headers and read if present
	if (px_fixhdr[PPHDR_IDX] != 0)
		px_pphdrs = Read2DConstants(x_file, px_fixhdr, PPHDR_IDX);
}

//*****************************************************************************

UM_WORD ReadNumberOfFields(std::ifstream& x_file, UM_WORD* px_fixhdr)
{
	// if we're reading the PP hdrs then the above value of i_dims2 will
	// be incorrect.  Calculate the correct length
	UM_WORD i_off = px_fixhdr[PPHDR_IDX]-1;
	UM_WORD i_dim1 = px_fixhdr[PPHDR_IDX+1];
	UM_WORD i_dim2 = -1;
    UM_WORD i_fc = 1;
    
	while (!x_file.eof() && i_fc > 0)
    {
    	// this reads in the field code
	    i_dim2 ++;
        x_file.seekg(sizeof(UM_WORD) * (i_off + i_dim1 * i_dim2 + 22));
        if (!x_file.eof())
	        x_file.read((char*)(&i_fc), sizeof(UM_WORD));
    }
    return i_dim2;
}

//*****************************************************************************

UM_WORD* Read1DConstants(std::ifstream& x_file, UM_WORD* px_fixhdr, int i_idx)
{
	// get the start and size of the integer constants from the fixed header
	UM_WORD i_off = px_fixhdr[i_idx]-1;
	UM_WORD i_len = px_fixhdr[i_idx+1];

	// create the storage
	UM_WORD* px_array = new UM_WORD[i_len];

	// read the constants
	x_file.seekg(sizeof(UM_WORD) * i_off);
	x_file.read((char*)px_array, sizeof(UM_WORD) * i_len);
	return px_array;
}

//*****************************************************************************

UM_WORD* Read2DConstants(std::ifstream& x_file, UM_WORD* px_fixhdr, int i_idx)
{
	// start and dimensions of 2D data
	UM_WORD i_off = px_fixhdr[i_idx]-1;
	UM_WORD i_dim1 = px_fixhdr[i_idx+1];
	UM_WORD i_dim2 = px_fixhdr[i_idx+2];

	// create the storage and read the constants
	UM_WORD* px_array = NULL;
	if (i_dim2 != 0)
	{
		px_array = new UM_WORD[i_dim1 * i_dim2];
		x_file.seekg(sizeof(UM_WORD) * i_off);
		x_file.read((char*)px_array, sizeof(UM_WORD) * i_dim1 * i_dim2);
	}
	return px_array;
}

//*****************************************************************************

UM_WORD* ReadFixHdr(std::ifstream& x_file)
{
	// create the fixed header
	UM_WORD* px_fixhdr = new UM_WORD[FIXHDR_LEN];

	// note, only the first 162 ints are valid
	x_file.seekg(0);
	x_file.read((char*)px_fixhdr, sizeof(UM_WORD)*FIXHDR_LEN);
	return px_fixhdr;
}

//*****************************************************************************

UM_WORD ReadValueFrom2D(UM_WORD* px_fixhdr, UM_WORD* px_pphdr, int i_idx,
						int i_wnum, int i_fnum)
{
    // read the value from the pp hdr for the field number: f_num
    // and word number: w_num
    // fix_hdr is needed for the width of the pp field
    UM_WORD i_ppw = px_fixhdr[i_idx+1];
    UM_WORD i_off = i_fnum * i_ppw + i_wnum;
    return px_pphdr[i_off];
}

//*****************************************************************************

UM_WORD* ReadData(std::ifstream& x_file, UM_WORD* px_fixhdr, UM_WORD* px_pphdr)
{
	// read the data section from the file
	UM_WORD i_data_start = px_fixhdr[159]-1;

	// the fixed hdr has now been fixed so that PP_IDX+2 contains the correct
	// number of fields
	UM_WORD i_n_fields = px_fixhdr[PPHDR_IDX+2];

	// check that at least one variable exists
	UM_WORD* px_data = NULL;
	if (i_n_fields != 0)
	{
		// get the data size from the last pphdr entry as the fixhdr lies!
		UM_WORD i_last_data_start = ReadValueFrom2D(px_fixhdr, px_pphdr, PPHDR_IDX,
								    				28, i_n_fields-1);
		UM_WORD i_last_data_len = ReadValueFrom2D(px_fixhdr, px_pphdr, PPHDR_IDX,
												  14, i_n_fields-1);
		UM_WORD i_data_len = i_last_data_start + i_last_data_len - i_data_start;

		// create the storage
		px_data = new UM_WORD[i_data_len];
		// read from the file
		x_file.seekg(i_data_start * sizeof(UM_WORD));
		x_file.read((char*)px_data, sizeof(UM_WORD) * i_data_len);
	}
	return px_data;
}

//*****************************************************************************

void CleanHdrs(UM_WORD* &px_fixhdr, UM_WORD* &px_pphdrs,
               UM_WORD* &px_intc,   UM_WORD* &px_realc,
               UM_WORD* &px_levc,   UM_WORD* &px_rowc,
               UM_WORD* &px_colc,   UM_WORD* &px_fieldc)
{
	delete[] px_fixhdr;
	delete[] px_pphdrs;
	delete[] px_intc;
	delete[] px_realc;
	delete[] px_levc;
	delete[] px_rowc;
	delete[] px_colc;
	delete[] px_fieldc;
}

//*****************************************************************************

void CleanData(UM_WORD* &px_data)
{
	delete [] px_data;
}
