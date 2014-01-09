                             UM model output to netCDF convertor
                                        Neil Massey

Written at University of Oxford for Climateprediction.net / Weather at Home projects

Please give credit to the original author if redistributed.

==================================================================================== 
Purpose 
==================================================================================== 
umtonc translates UK Met Office Unified Model (UM) output to CF-Compliant 
(v1.6) netCDF files umtonc converts rotated pole grids, producing the true 
latitude longitude coordinates as an additional variable.

==================================================================================== 
Building 
==================================================================================== 
Pre-requisites: umtonc requires netCDF libraries and the netCDF C++ 
development libraries for v3 of netCDF.  The last time I downloaded the 
libraries I used these links:

	http://www.unidata.ucar.edu/downloads/netcdf/ftp/netcdf-4.3.0.tar.gz
	http://www.unidata.ucar.edu/downloads/netcdf/ftp/netcdf-cxx-4.2.tar.gz

Only the "classic" model of netCDF is supported, so when installing the 
first tar file, you have to use ./configure --disable-netcdf-4

umtonc also requires TCLAP and TinyXML, but these are included in the tar 
file. To build umtonc, simply invoke make in the untarred directory.

====================================================================================
Using
====================================================================================

   ./um2nc  [-o <string>] [-p <string>] [-x <string>] [-g <string>] [--]
            [--version] [-h] <input file names> ...

Where: 

   -o <string>,  --output <string>
     Output file name

   -p <string>,  --packfile <string>
     XML file name containing information about which variables are to be
     packed

   -x <string>,  --xmlfile <string>
     XML file name containing variable definitions

   -g <string>,  --global_atts <string>
     Global attributes file name

   --,  --ignore_rest
     Ignores the rest of the labeled arguments following this flag.

   --version
     Displays version information and exits.

   -h,  --help
     Displays usage information and exits.

   <input file names>  (accepted multiple times)
     (required)  Input file names


   um to netCDF file conversion utility

umtonc can convert files in a directory with normal linux-like pattern 
matching. e.g.	umtonc -x $XML_TRANSLATION_DOC xaakm.* will convert all 
files beginning xaakm.

=====================================================================================
XML Translation Document
=====================================================================================

umtonc translates between UM stash codes and CF standard_names / units using a XML 
document.
The format of this document is as follows:

<?xml version='1.0' encoding='UTF-8'?>
<translation xmlns='http://www-hc/~hadsa/CF_convention'>
  <stash xmlns='http://www-hc/~hadsa/CF_convention/standard_name_20010305.html'>
    <submodel id='1' description='UM Atmosphere.'>
      <section id='0' description='Primary Fields.'>
        <item units='Pa' standard_name='pstar' id='001' description='PSTAR AFTER TIMESTEP'/>
        ...
      </section>
      <section id='2' description='L.W Radiation.'>
        <item units='W m-2' standard_name='net_down_surface_lw_rad_flux' id='201' description='NET DOWN SURFACE LW RAD FLUX'/>
        ...
      </section>
    <submodel>
  </stash>
</translation>

This is the same format as used by PRECIS. The conversion is arranged by 
STASH code, so a stash section with id=0, and an item with id=001 has the 
STASH code of 00001. To add a new variable, locate which STASH section the 
item belongs to and put a new <item> line in with the standard_name, units, 
description and STASH id for that variable.

The current document used for this is: cf_metadata_translations_precis.xml

=====================================================================================
XML Translation Document
=====================================================================================

umtonc has the ability to reduce data usage in 4 ways.  These are cropping a 
region, subsampling a region without averaging, subsampling a region with 
averaging and packing a (floating point) variable into a byte or short 
integer (fixed point). This is controlled by another XML file, which has the 
format:

<?xml version='1.0' encoding='UTF-8'?>
<pack_data>
    <!- Packing data example - possible attributes are:>
    <!- pack, skip, crop and avg>
    <item section='16' id='222' pack='2'/>
    <item section='3' id='236' pack='1'/>
    <item section='0' id='24' crop='4' skip='2'/>
    <item section='3' id='237' crop='4' avg='4'/>
</pack_data>

Variables to be packed are identified by their STASH section and id within 
that section.  The attributes (pack, crop, skip and avg) can be applied to 
any STASH variable, and moe than one attribute can be applied to each 
variable.  The attributes have the following syntax:

<pack=n> pack the variable into n bytes.  Current 1 (byte), 2 (short int) 
and 4 (32 bit floating point) are supported.  The new variable is written 
into the netCDF file with a scale_factor and add_offset attribute, which 
allows for translation back to the original value. (v1 = v0 * scale_factor + 
add_offset). New variables should be treated as unsigned, which is reflected 
by the variable having the _Unsigned = true attribute.

<crop=c> remove c grid boxes from around the perimeter of the region.  This 
is currently used by CPDN / WaH to remove the sponge layer prior to 
uploading.

<skip=s> only output every s grid boxes to reduce data size.  No averaging. 
Used by WaH Southern Africa to degrade the region, and reduce the data size, 
prior to upload.

<avg=a> run an a x a averaging window over the region and output the result. 
The result will be a x a times smaller than the original data.  Used by WaH 
Southern Africa to degrade the region, and reduce the data size prior to 
upload.
