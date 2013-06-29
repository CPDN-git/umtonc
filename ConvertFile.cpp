// File    : ConvertFile.cpp
// Author  : Neil Massey
// Date    : 20/11/08
// Purpose : Main function for um to netCDF conversion

#include <iostream>
#include <fstream>
#include <list>

#include "ConvertFile.h"
#include "ReadUMFile.h"
#include "ExtractVariables.h"
#include "Variable.h"
#include "WriteNetCDFFile.h"
#include "VarLevTrans.h"
#include "GlobalAtts.h"
#include "Packing.h"

//*****************************************************************************

bool IsUMFile(std::ifstream& x_infile)
{
	// check that the first 6 words of the file make some sense
	const int ci_hdrchk_words = 6;
	x_infile.seekg(0);
	UM_WORD px_hdrchk[ci_hdrchk_words];
	x_infile.read((char*)(px_hdrchk), sizeof(UM_WORD) * ci_hdrchk_words);

	bool b_valid = true;

/*	for (int i=0; i<6; i++)
		std::cout << px_hdrchk[i] << " ";
	std::cout << std::endl;*/

	b_valid &= (px_hdrchk[0] == -32768);
	b_valid &= (px_hdrchk[1] >= 1 && px_hdrchk[1] <= 4    );
	b_valid &= (px_hdrchk[2] >= 1 && px_hdrchk[2] <= 6    );
	b_valid &= (px_hdrchk[3] >= 0 && px_hdrchk[3] <= 105  );
	b_valid &= (px_hdrchk[4] >= 1 && px_hdrchk[4] <= 10   );
//	b_valid &= (px_hdrchk[5] >= 0 && px_hdrchk[5] <= 10000);

	return b_valid;
}

//*****************************************************************************

int ConvertFile(std::string s_input_file_name, std::string s_output_file_name,
				std::string s_xml_file_name,   std::string s_pack_file_name,
				std::string s_gatts_file_name)
{
    // try to open the file for reading
	int i_ret = 0;
	
    std::ifstream x_infile;
    x_infile.open(s_input_file_name.c_str(), std::ios::in|std::ios_base::binary);
    if (!x_infile)
    {
        std::cerr << "Error: Input file: " << s_input_file_name << " could not be opened." << std::endl;
        i_ret = 1;
    }

	// check that the file is a UM field file by checking the headers
	if (i_ret != 1 && !IsUMFile(x_infile))
	{
		std::cerr << "Error: Input file: " << s_input_file_name << " is not a UM file." << std::endl;
		i_ret = 1;
	}

if (i_ret != 1)
{
	// read headers
	UM_WORD *px_fixhdr, *px_pphdrs, *px_intc, *px_realc;
	UM_WORD *px_levc,   *px_rowc,   *px_colc, *px_fieldc;

	ReadHdrs(x_infile,
			 px_fixhdr, px_pphdrs, px_intc, px_realc,
       	     px_levc,   px_rowc,   px_colc, px_fieldc);

	// get the reference time
	MTime x_ref_time(&(px_fixhdr[20]), px_fixhdr[7]);

	// read the data - note that this will have to be converted to it's
	// actual type by checking the pphdr
	UM_WORD* px_data = ReadData(x_infile, px_fixhdr, px_pphdrs);
	// check that some data was actually read.
	if (px_data == NULL)
	{
        std::cerr << "Error: Input file: " << s_input_file_name << " contains no data." << std::endl;
		i_ret = 1;
	}

	// create the lists for storing the info about the file
	std::list<Variable> x_var_list;

	if (i_ret != 1)
		ExtractVarFields(px_fixhdr, px_pphdrs, px_data, x_var_list);

	// read the xml file definition of level and variable names and attributes
	VarLevTrans x_var_lev_trans(s_xml_file_name);
	if (i_ret != 1 && !x_var_lev_trans.LoadAttributes())
    {
        std::cerr << "Error: XML file: " << s_xml_file_name << " error in XML or file could not be opened." << std::endl;
    	i_ret = 1;
    }
    
    // read the xml file for the packing details
    Packing x_packing(s_pack_file_name);
    if (i_ret != 1 && !x_packing.LoadAttributes())
    {
        std::cerr << "Error: XML file: " << s_pack_file_name << " error in XML or file could not be opened." << std::endl;
        i_ret = 1;
    }

    // Pack the variables
    x_packing.PackVariables(x_var_list);

	GlobalAtts x_gatts;
	if (i_ret != 1)
	{
		if (s_gatts_file_name != "")		
			if (!x_gatts.LoadAttributes(s_gatts_file_name))
			{
				std::cerr << "Error: Failed to load global attributes file: " << s_gatts_file_name << std::endl;
				i_ret = 1;
			}
	}

	// write out the netCDF file - we have all the necessary info now
	if (i_ret != 1)
		i_ret = WriteNetCDFFile(s_output_file_name, x_var_list, x_ref_time,
								x_var_lev_trans, x_gatts);
								
	if (i_ret == 1)
		std::cerr << "Error: Output file could not be written." << std::endl;

	// clean up headers
	CleanHdrs(px_fixhdr, px_pphdrs, px_intc, px_realc,         
              px_levc,   px_rowc,   px_colc, px_fieldc);
	CleanData(px_data);

	// close input file
    x_infile.close();
    if (i_ret != 1)
    	std::cout << "Converted file: " << s_input_file_name << " to: " 
	    		  << s_output_file_name << std::endl;
}

    return i_ret;
}
