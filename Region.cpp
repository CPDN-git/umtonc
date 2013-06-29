// File    : Region.h
// Author  : Neil Massey
// Date    : 24/11/08
// Purpose : Class to store region details in

#include "Region.h"
#include "Rot2Global.h"
#include <math.h>
#include <iostream>

//*****************************************************************************

Region::Region(int gridc, float slat, float slon, float dlat, float dlon, 
               int nrows, int ncols)
{
	// just load the details into the class
	f_slat = slat;
	f_slon = slon;
	f_dlat = dlat;
	f_dlon = dlon;
	i_nrows = nrows;
	i_ncols = ncols;
	i_id = -1;
	i_gridc = gridc;
	f_plon = -1e20;	// sentinels - should be replaced
	f_plat = -1e20;
	pf_global_lon_coords = NULL;
	pf_global_lat_coords = NULL;
}

//*****************************************************************************

Region::Region(void)
{
	f_slat = 0.0;
	f_slon = 0.0;
	f_dlat = 0.0;
	f_dlon = 0.0;
	i_nrows = 0;
	i_ncols = 0;
	i_id = -1;
	i_gridc = 0;

	f_plon = -1e20;
	f_plat = -1e20;
	pf_global_lon_coords = NULL;
	pf_global_lat_coords = NULL;
}

//*****************************************************************************

Region::Region(const Region& rhs)
{
	f_slat = rhs.f_slat;
	f_slon = rhs.f_slon;
	f_dlat = rhs.f_dlat;
	f_dlon = rhs.f_dlon;
	i_nrows = rhs.i_nrows;
	i_ncols = rhs.i_ncols;
	i_id = rhs.i_id;
	i_gridc = rhs.i_gridc;
	f_plon = rhs.f_plon;
	f_plat = rhs.f_plat;
	pf_global_lon_coords = rhs.pf_global_lon_coords;
	pf_global_lat_coords = rhs.pf_global_lat_coords;
}

//*****************************************************************************

Region::~Region(void)
{
	if (pf_global_lon_coords)
	{
		delete [] pf_global_lon_coords;
		pf_global_lon_coords = NULL;
	}

	if (pf_global_lat_coords)
	{
		delete [] pf_global_lat_coords;
		pf_global_lat_coords = NULL;
	}
}

//*****************************************************************************

bool Region::operator == (const Region& rhs) const
{
	return (f_slat  == rhs.f_slat)  &&
		   (f_slon  == rhs.f_slon)  &&
		   (f_dlat  == rhs.f_dlat)  &&
		   (f_dlon  == rhs.f_dlon)  &&
		   (i_nrows == rhs.i_nrows) &&
		   (i_ncols == rhs.i_ncols) &&
		   (f_plon  == rhs.f_plon)  &&
		   (f_plat  == rhs.f_plat)  &&
		   (i_gridc == rhs.i_gridc);
}

//*****************************************************************************

bool Region::operator != (const Region& rhs) const
{
	return !(*this == rhs);
}

//*****************************************************************************

void Region::Print(void) const
{
	std::cout << f_slat  << " " << f_slon  << " ";
	std::cout << f_dlat  << " " << f_dlon  << " ";
	std::cout << i_nrows << " " << i_ncols << " " << i_gridc << " ";
	std::cout << f_plon  << " " << f_plat  << std::endl;
}

//*****************************************************************************

int Region::GetID(void) const
{
	return i_id;
}

//*****************************************************************************

void Region::SetID(int id)
{
	i_id = id;
}

//*****************************************************************************

int Region::GetNRows(void) const
{
    return i_nrows;
}

//*****************************************************************************

int Region::GetNCols(void) const
{
    return i_ncols;
}

//*****************************************************************************

std::list<float> Region::GetLatitudes(void) const
{
	std::list<float> x_lats;
	for (int y=0; y<i_nrows; y++)
	{
		float f_lat = f_slat + (y+1) * f_dlat;
		x_lats.push_back(f_lat);
	}
	return x_lats;
}

//*****************************************************************************

std::list<float> Region::GetLongitudes(void) const
{
	std::list<float> x_lons;
	
	float fmax = f_slon + i_ncols * f_dlon;
	 
	for (int x = 0; x < i_ncols; x++)
	{
		float f_lon = f_slon + (x+1)*f_dlon;
		f_lon = f_lon - floor(fmax/360.0) * 360.0;
		x_lons.push_back(f_lon);
	}
	return x_lons;
}

//*****************************************************************************

void Region::SetPoleLocation(float plat, float plon)
{
	f_plat = plat;
	f_plon = plon;
}

//*****************************************************************************

float Region::GetPoleLatitude(void) const
{
	return f_plat;
}

//*****************************************************************************

float Region::GetPoleLongitude(void) const
{
	return f_plon;
}

//*****************************************************************************

int Region::GetGridType(void) const
{
	return i_gridc;
}

//*****************************************************************************

void Region::Crop(int i_crop_size)
{
    // crop n number of grid boxes off all sides of the region.
    // this is useful to remove the sponge layer
    // check first whether the pf_global coords have been created - delete if they have
	if (pf_global_lon_coords)
	{
		delete [] pf_global_lon_coords;
		pf_global_lon_coords = NULL;
	}

	if (pf_global_lat_coords)
	{
	    delete [] pf_global_lat_coords;
		pf_global_lat_coords = NULL;
	}
	
	// just have to reassign the grid size variables
	// reduce the width and height
	i_nrows = i_nrows - 2 * i_crop_size;
	i_ncols = i_ncols - 2 * i_crop_size;
	// reassign the start position
	f_slon = f_slon + i_crop_size * f_dlon;
	f_slat = f_slat + i_crop_size * f_dlat;
}

//*****************************************************************************

void Region::SubSample(int i_skip)
{
    // reduce the size of the region by i_skip amount
    // useful to reduce size of data output for fields where high spatial
    // resolution is not required
    // check first whether the pf_global coords have been created - delete if they have
	if (pf_global_lon_coords)
	{
		delete [] pf_global_lon_coords;
		pf_global_lon_coords = NULL;
	}

	if (pf_global_lat_coords)
	{
		delete [] pf_global_lat_coords;
		pf_global_lat_coords = NULL;
	}
	
	// have 1) reassign the start
	//      2) reassign the spacing 
	//      3) reassign the number of rows and columns
	f_slon = f_slon + ((i_skip-1)*f_dlon)/2.0;
	f_slat = f_slat + ((i_skip-1)*f_dlat)/2.0;
	f_dlon = f_dlon * i_skip;
	f_dlat = f_dlat * i_skip;
	i_nrows = i_nrows / i_skip;
	i_ncols = i_ncols / i_skip;
}

//*****************************************************************************

void Region::GetGlobalCoordsForRotatedGrid(float*& pf_global_lat, 
										   float*& pf_global_lon)
{
	// if these coords have already been made then just return them
	if (pf_global_lon_coords && pf_global_lat_coords)
	{
		pf_global_lat = pf_global_lat_coords;
		pf_global_lon = pf_global_lon_coords;
		return;
	}
	
	pf_global_lon_coords = new float[i_nrows * i_ncols];
	pf_global_lat_coords = new float[i_nrows * i_ncols];

	// convert the rotated coords to the global coords
	// co-ordinates are returned in Y, X order
	float f_global_lon, f_global_lat;

	for (int y=0; y<i_nrows; y++)
	{
		// calculate latitude value for the rotated grid
		float f_rot_lat = f_slat + (y+1) * f_dlat;
		for (int x=0; x<i_ncols; x++)
		{
			// calculate the longitude value for the rotated grid
			float f_rot_lon = f_slon + (x+1) * f_dlon;
			// calculate global grid values
			Rot2Global(f_rot_lat, f_rot_lon, f_plat, f_plon, 
					   f_global_lat, f_global_lon);

			// calculate the offset for the array
			int i_offset = y * i_ncols + x;
			pf_global_lat_coords[i_offset] = f_global_lat;
			pf_global_lon_coords[i_offset] = f_global_lon;
		}
	}
	// assign return values
	pf_global_lat = pf_global_lat_coords;
	pf_global_lon = pf_global_lon_coords;
}
