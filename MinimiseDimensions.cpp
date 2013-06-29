// File    : MinimiseDimensions.cpp
// Author  : Neil Massey
// Date    : 01/12/08
// Purpose : Functions to return a minimal set of the dimensions

#include "MinimiseDimensions.h"

#include <algorithm>
#include "VarField.h"
#include <iostream>

//*****************************************************************************

std::list<Series<TimeField> > MinimiseTimes(std::list<Variable>& x_var_list)
{
	// create output list
	std::list<Series<TimeField> > x_ts_list;
	int i_n = 0;

	// create a time series from the var fields in the var_list
    std::list<Variable>::iterator it_var_list;

    for (it_var_list = x_var_list.begin(); it_var_list != x_var_list.end();
		 it_var_list++)
    {
		// create an empty timefield series
		Series<TimeField> x_tf_series;
	   	std::list<VarField>::iterator it_var_field;
		// build the time series
	    for (it_var_field = it_var_list->GetVarFields().begin(); 
			 it_var_field != it_var_list->GetVarFields().end();
    	     it_var_field++)
    	    x_tf_series.Add(it_var_field->GetTimeField());

        // now see if the time has already been added to the time series list
		std::list<Series<TimeField> >::iterator it_ts_list;
        it_ts_list = find(x_ts_list.begin(), x_ts_list.end(), x_tf_series);

        // not found so add to the list and label variable and level accordingly
        if (it_ts_list == x_ts_list.end())
        {
            it_var_list->SetTimeID(i_n);
            x_tf_series.SetID(i_n);
            i_n++;
            x_ts_list.push_back(x_tf_series);
        }
        else
            //found so just label level in variable
            it_var_list->SetTimeID(it_ts_list->GetID());
	}
	return x_ts_list;
}

//*****************************************************************************

std::list<Region> MinimiseRegions(std::list<Variable>& x_var_list)
{
    // create output list
    std::list<Region> x_region_list;
    std::list<Region>::iterator it_region_list;
    int i_n = 0;

    // create a time series from the var fields in the var_list
    std::list<Variable>::iterator it_var_list;
    for (it_var_list = x_var_list.begin(); it_var_list != x_var_list.end();
         it_var_list++)
    {
        // get data from variable and create the Region
		// all the regions within the variable are equal
		Region x_region = it_var_list->GetVarFields().front().GetRegion();

        // check whether this is in the list or not
        it_region_list = find(x_region_list.begin(), x_region_list.end(), x_region);
        // not found
        if (it_region_list == x_region_list.end())
        {
            // append to list and label the number of time series in both
            // the dimension and the variable
            x_region.SetID(i_n);
            it_var_list->SetRegionID(i_n);
            x_region_list.push_back(x_region);
            i_n++;
        }
        else
			// found, so the region already exists in the list
			// label the region in the variable list with the number in the found region
            it_var_list->SetRegionID(it_region_list->GetID());
    }

    return x_region_list;
}

//*****************************************************************************

std::list<Series<Level> > MinimiseLevels(std::list<Variable>& x_var_list)
{
    std::list<Series<Level> > x_lev_series_list;
    std::list<Series<Level> >::iterator it_lev_series_list;	
    int i_n = 0;

    // create a time series from the var fields in the var_list
    std::list<Variable>::iterator it_var_list;
    std::list<VarField>::iterator it_var_field;
    for (it_var_list = x_var_list.begin(); it_var_list != x_var_list.end();
         it_var_list++)
    {
    	// create an empty level series
    	Series<Level> x_lev_series;
    	// loop through the varfield list of the variable adding the levels
    	for (it_var_field = it_var_list->GetVarFields().begin();
    		 it_var_field != it_var_list->GetVarFields().end();
    		 it_var_field++)
    		x_lev_series.Add(it_var_field->GetLevel());
    		
		// now see if the level has already been added to the level series list
		it_lev_series_list = find(x_lev_series_list.begin(),
		                          x_lev_series_list.end(), x_lev_series);
		// not found so add to the list and label variable and level accordingly
		if (it_lev_series_list == x_lev_series_list.end())
		{
			it_var_list->SetLevelID(i_n);
			x_lev_series.SetID(i_n);
			i_n++;
			x_lev_series_list.push_back(x_lev_series);
		}
		else
			//found so just label level in variable
			it_var_list->SetLevelID(it_lev_series_list->GetID());
    }
    return x_lev_series_list;
}

//*****************************************************************************
