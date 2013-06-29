// File    : TimeField.cpp
// Author  : Neil Massey
// Date    : 27/11/08
// Purpose : Class to store time details in

#include "TimeField.h"
#include <iostream>

//*****************************************************************************

TimeField::TimeField(int time_type, int* val_date, int* dat_date):
                     x_val_date(val_date, time_type), 
					 x_dat_date(dat_date, time_type)
{
	i_time_type = time_type;
	// set meaning period to the difference between the val date and dat date
	f_meaning_period = (x_dat_date.CalcDays() - x_val_date.CalcDays()) * 24.0;
}

//*****************************************************************************

bool TimeField::operator == (const TimeField &rhs) const
{
	bool b_equal = true;
	b_equal &= (f_meaning_period == rhs.f_meaning_period) &&
	           ((i_time_type % 100) % 10 == (rhs.i_time_type % 100) % 10);
	// check dates are equal
	b_equal &= (x_val_date == rhs.x_val_date);
	if (i_time_type / 100 > 0)
		b_equal &= (x_dat_date == rhs.x_dat_date);
	return b_equal;
}

//*****************************************************************************

bool TimeField::operator != (const TimeField &rhs) const
{
	return !(*this == rhs);
}

//*****************************************************************************

float TimeField::GetMeaningPeriod(void) const
{
	return f_meaning_period;
}

//*****************************************************************************

const MTime& TimeField::GetValDate(void) const
{
	return x_val_date;
}

//*****************************************************************************

void TimeField::Print(void) const
{
	std::cout << i_time_type << " " << f_meaning_period << std::endl << "\t";
	x_val_date.Print();

	std::cout << "\t";	
	if (i_time_type / 100 > 0)
		x_dat_date.Print();
}

//*****************************************************************************

float TimeField::GetValue(void) const
{
	// return number of days since 0AD
	int px_yzi[6] = {0,0,0,0,0,0};
	MTime x_yz(px_yzi, i_time_type);
	float v = 0.0;
	if (f_meaning_period == 0.0)
	    v = x_val_date.CalcDays();
	else
	    v = x_val_date.CalcDays() + 0.5 * f_meaning_period / 24;
	return v;
}

//*****************************************************************************

int TimeField::GetType(void) const
{
	return i_time_type;
}

//*****************************************************************************

std::string TimeField::GetCalendarType(void) const
{
	std::string s_caltype;
	switch ((i_time_type % 100) % 10)
    {
		case 0:
	        s_caltype = "none";
            break;
        case 1:
            s_caltype = "gregorian";
            break;
        case 2:
            s_caltype = "360_day";
            break;
    }
	return s_caltype;
}

//*****************************************************************************
