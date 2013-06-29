//*****************************************************************************
//* File    : ConvertFile.h
//* Author  : Neil Massey
//* Date    : 20/11/08
//* Purpose : Main function for um to netCDF conversion           
//*****************************************************************************

#include <string>

#ifndef CONVERTFILE_H
#define CONVERTFILE_H

// functions
int ConvertFile(std::string s_input_file_name, std::string s_output_file_name,
				std::string s_xml_file_name,   std::string s_pack_file_name,
				std::string s_gatts_file_name);

#endif
