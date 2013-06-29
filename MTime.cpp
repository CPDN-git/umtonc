// File    : MTime.h
// Author  : Neil Massey
// Date    : 01/12/08
// Purpose : Class to store time details in

#include "MTime.h"
#include "ToString.h"
#include <iostream>
#include <string.h>

MTime::MTime(int* px_date, int time_type)
{
	memcpy(px_time_array, px_date, 6 * sizeof(int));
	i_time_type = time_type;
}

//*****************************************************************************

bool MTime::operator == (const MTime& rhs) const
{
	bool b_equal = true;
	for (int i=0; i<6; i++)
		b_equal &= px_time_array[i] == rhs.px_time_array[i];
	return b_equal;
}

//*****************************************************************************

bool MTime::operator != (const MTime& rhs) const
{
	return !(*this == rhs);
}

//*****************************************************************************

bool MTime::operator >  (const MTime& rhs) const
{
	return CalcHours() > rhs.CalcHours();
}

//*****************************************************************************

bool MTime::operator <  (const MTime& rhs) const
{
	return CalcHours() < rhs.CalcHours();
}

//*****************************************************************************

bool MTime::operator >= (const MTime& rhs) const
{
	return CalcHours() >= rhs.CalcHours();
}

//*****************************************************************************

bool MTime::operator <= (const MTime& rhs) const
{
	return CalcHours() <= rhs.CalcHours();
}

//*****************************************************************************

void MTime::operator = (const MTime& rhs)
{
	// copy constructor: needed to copy time details over, not just
	// the pointer location
    memcpy(px_time_array, rhs.px_time_array, 6 * sizeof(int));
	i_time_type = rhs.i_time_type;
}

//*****************************************************************************

void MTime::Print(void) const
{
	for (int i=0; i<6; ++i)
		std::cout << px_time_array[i] << " ";
	std::cout << std::endl;
}

//*****************************************************************************

double MTime::CalcDays(void) const
{
	int i_days_elapsed[13] = {0,31,59,90,120,151,181,212,243,273,304,334,365};
	//			                31 28 31  30  31  30  31  31  30  31  30  31
	//			                J  F  M   A   M   J   J   A   S   O   N   D
	// get the number of days from the calendar type
	float f_days_per_year = 0.0;
	switch ((i_time_type % 100) % 10)
    {
	    case 1:
	        f_days_per_year = 365.25;
	    break;
	    default:
	        f_days_per_year = 360.0;
	}

    // get the time details from the array
	int i_years = px_time_array[0];
	int i_months = px_time_array[1];
	int i_days   = px_time_array[2];
	int i_hours  = px_time_array[3];
	int i_mins   = px_time_array[4];
	
	// get number of days upto this year
	float c_time = float(int((i_years-1) * f_days_per_year));
	// add the month days
	if (f_days_per_year == 365.25)
	{
	    c_time += i_days_elapsed[i_months-1];
	    // check for leap year
	    if ((i_years % 4 == 0) && (i_months == 2))
	        c_time += 1.0;
	}
	else
	    c_time += (i_months-1) * f_days_per_year/12;
	// add the days, hours, minutes and seconds
	c_time += (i_days-1);
	c_time += float(i_hours) / 24.0;
	c_time += float(i_mins) / (24.0 * 60.0);
	return c_time;
}

//*****************************************************************************

double MTime::CalcHours(void) const
{
	// calculate the number of hours since year 0 represented by the MTime
	return CalcDays() * 24.0;
}

//*****************************************************************************

const int* MTime::GetTimeArray(void) const
{
	return px_time_array;
}

//*****************************************************************************

std::string MTime::GetFormatted(void) const
{
	std::string s_tf = ToString(px_time_array[0]) + "-" +
		               ToString(px_time_array[1],2,'0') + "-" +
      			       ToString(px_time_array[2],2,'0') + " " +
					   ToString(px_time_array[3],2,'0') + ":" +
					   ToString(px_time_array[4],2,'0') + ":" +
					   ToString(px_time_array[5],2,'0');
	return s_tf;
}
