//*****************************************************************************
//* File    : Packing.h
//* Author  : Neil Massey
//* Date    : 16/05/13
//* Purpose : Class to implement various data reduction techniques:
//*           1) Region cropping - to remove the sponge layer
//*           2) Data skipping - to reduce the resolution without averaging
//*           3) Data averaging - to reduce the resolution with averaging
//*           4) Data packing - transform data to byte or short int, with an
//*              offset and scaling factor
//* Control of the packing is performed by an xml file supplied by -p option
//* format of this file is:
//* <pack_data>
//*     <item name='field16' pack='1' skip='2' crop='4' avg='2'>
//* </pack_data>
//* pack=1 means pack to one byte.  pack=2 means pack to a short.
//* skip=2 means write out every 2nd data point
//* crop=4 means remove 4 grid points from each side of the region
//* avg=2  means pass a 2x2 averaging window over the data and take every 2nd data point
//*****************************************************************************

#ifndef PACKING_H
#define PACKING_H

#include <string>
#include <list>
//#include "tinyxml/tinyxml.h"
#include "tinyxml.h"
#include "Variable.h"

struct PackDetails
{
    PackDetails(void) : i_stash(0), i_skip(0), i_crop(0), i_avg(0), i_pack(0){};
    bool operator==(const PackDetails& rhs)
    {
    	return (i_stash == rhs.i_stash && i_id == rhs.i_id && i_skip == rhs.i_skip &&
                i_crop == rhs.i_crop && i_avg == rhs.i_avg && i_pack == rhs.i_pack);
    }
    int i_stash;
    int i_id;
    int i_skip;
    int i_crop;
    int i_avg;
    int i_pack;
};

class Packing
{
    public:
        Packing(std::string s_xml_pack_name);
        bool LoadAttributes(void);
        void PackVariables(std::list<Variable>& x_var_list);
        
    private:
        void PackVariable(Variable& x_variable);

        std::list<PackDetails> x_pack_details_list;
        std::string s_xml_pack_name;
};

#endif
