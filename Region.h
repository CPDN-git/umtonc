//*****************************************************************************
//* File    : Region.h
//* Author  : Neil Massey
//* Date    : 24/11/08
//* Purpose : Class to store region details in
//*****************************************************************************

#ifndef REGION_H
#define REGION_H

#include <list>
#include <vector>

class Region
{
	public:
		Region(void);
		~Region(void);
		Region(const Region& rhs);
		Region(int gridc, float slat, float slon, float dlat, float dlon, 
               int nrows, int ncols);
		bool operator == (const Region &rhs) const;
		bool operator != (const Region &rhs) const;
		void SetPoleLocation(float plat, float plon);
		float GetPoleLatitude(void) const;
		float GetPoleLongitude(void) const;
		void Print(void) const;
		int GetGridType(void) const;
		int GetID(void) const;
		void SetID(int id);
		int GetNRows(void) const;
		int GetNCols(void) const;
		// return a list of the latitudes and longitudes
		std::list<float> GetLatitudes(void) const;
		std::list<float> GetLongitudes(void) const;
		// reduce size of region
        void SubSample(int i_skip);
        void Crop(int i_crop);
		
		// return a matrix of the rotated latitudes and longitudes
		void GetGlobalCoordsForRotatedGrid(float*& pf_global_lat, 
										   float*& pf_global_lon);
	
	private:
		float f_slat;		// starting latitude
		float f_slon;		// starting longitude

		float f_dlat;		// delta latitude
		float f_dlon;		// delta longitude

		float f_plat;		// location of pole, only needed for rotated
		float f_plon;		// grids

		int   i_nrows;		// number of rows
		int   i_ncols;		// number of columns;

		int   i_id;		    // number / label of region in netCDF file
		int	  i_gridc;		// grid type
		
		float* pf_global_lon_coords;	// translated rotated coords
		float* pf_global_lat_coords;	// translated rotated coords
};

#endif
