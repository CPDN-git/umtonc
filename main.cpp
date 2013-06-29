//*****************************************************************************
// File    : main.cpp
// Author  : Neil Massey
// Date    : 26/03/08
// Purpose : Main entry point for pp to netcdf conversion program
//           See "Unified Model Documentation Paper F3" for details of the
//           file format: pf003.ps
//*****************************************************************************

#include <iostream>
#include <tclap/CmdLine.h>
#include <string>
#include <vector>
#include "ConvertFile.h"

const std::string s_default_output = "um2nc_output";

int main(int argc, char** argv)
{
	std::vector<std::string> x_input_file_names;
	std::vector<std::string>::iterator it_input_file_names;
	std::string s_output_file_name;
	std::string s_xml_file_name;
	std::string s_pack_file_name;
	std::string s_gatts_file_name;

	try
	{
		// define tclap command interpreter
		TCLAP::CmdLine cmd("um to netCDF file conversion utility", ' ', "0.1");
		// define the arguments - backwards!
		// global attributes file (ncatts)
		TCLAP::ValueArg<std::string> x_gatts_file_arg("g", "global_atts", "Global attributes file name", false, "", "string");
		cmd.add(x_gatts_file_arg);
		// xml file containing variable definitions 
		TCLAP::ValueArg<std::string> x_xml_file_arg("x", "xmlfile", "XML file name containing variable definitions", false, "cf_metadata_translations_precis.xml", "string");
		cmd.add(x_xml_file_arg);
		// xml file containing packing information
		TCLAP::ValueArg<std::string> x_pack_file_arg("p", "packfile", "XML file name containing information about which variables are to be packed", false, "", "string");
		cmd.add(x_pack_file_arg);
		// output file
		TCLAP::ValueArg<std::string> x_output_file_arg("o", "output", "Output file name", false, s_default_output, "string");
		cmd.add(x_output_file_arg);
		// input file
		TCLAP::UnlabeledMultiArg<std::string> x_input_file_arg("input", "Input file names", true, "input file names");
		cmd.add(x_input_file_arg);
		cmd.parse(argc, argv);

		// load the values
		x_input_file_names = x_input_file_arg.getValue();
		s_output_file_name = x_output_file_arg.getValue();
		s_xml_file_name = x_xml_file_arg.getValue();
		s_pack_file_name = x_pack_file_arg.getValue();
		s_gatts_file_name = x_gatts_file_arg.getValue();
	}
	catch (TCLAP::ArgException &e)  // catch exceptions
	{
		std::cerr << "Error: " << e.error() << " for arg " << e.argId() << std::endl;
		return 1;
	}
	catch (char const* m)           // general error message exception
	{
		std::cerr << "Error: " << m << std::endl;
		return 1;
	}
	catch (std::string &s)          // string error message
	{
		std::cerr << "Error: " << s << std::endl;
		return 1;
	}

	int i_ret = 1;

	for (it_input_file_names = x_input_file_names.begin();
         it_input_file_names != x_input_file_names.end();
		 it_input_file_names++)
	{
		// check whether the s_output_file_name has been passed or not
		if (s_output_file_name == s_default_output || 
            x_input_file_names.size() > 1)
		{
			s_output_file_name = *it_input_file_names + ".nc";
		}
		i_ret &= ConvertFile(*it_input_file_names, s_output_file_name,
			                 s_xml_file_name, s_pack_file_name, s_gatts_file_name);
	}
	return i_ret;
}

