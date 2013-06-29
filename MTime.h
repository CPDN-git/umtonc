//*****************************************************************************
//* File    : MTime.h
//* Author  : Neil Massey
//* Date    : 01/12/08
//* Purpose : Class to store time details in
//*****************************************************************************

#ifndef MTIME_H
#define MTIME_H

#include <string>

class MTime
{
	public:
		// constructor for using 6 dimension time array
		// dimensions are year, month, day, hour, min, day number
		// time type is 1 = 365 day year, 2 = 360 day year
		MTime(void){};
		MTime(int* px_time_array, int i_time_type);
		bool operator == (const MTime& rhs) const;
		bool operator != (const MTime& rhs) const;
		bool operator >  (const MTime& rhs) const;
		bool operator <  (const MTime& rhs) const;
		bool operator >= (const MTime& rhs) const;
		bool operator <= (const MTime& rhs) const;
		void operator = (const MTime& rhs);	
		void Print(void) const;
		// calculate the number of days since the year zero
		double CalcDays(void) const;
		const int* GetTimeArray(void) const;
		std::string GetFormatted(void) const;

	private:
		// calculate the number of hours in the time : since the year 0
		double CalcHours(void) const;
		int px_time_array[6];
		int i_time_type;
};

#endif
