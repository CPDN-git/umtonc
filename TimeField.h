//*****************************************************************************
//* File    : TimeField.h
//* Author  : Neil Massey
//* Date    : 27/11/08
//* Purpose : Class to store time details in
//*****************************************************************************

#ifndef TIMEFIELD_H
#define TIMEFIELD_H

#include "MTime.h"
#include <string>

class TimeField
{
	public:
		TimeField(void){};
		TimeField(int time_type, int* val_date, int* dat_date);
		bool operator == (const TimeField& rhs) const;
		bool operator != (const TimeField& rhs) const;
		void Print(void) const;
		float GetMeaningPeriod(void) const;
		const MTime& GetValDate(void) const;
		float GetValue(void) const;
		int GetType(void) const;
		std::string GetCalendarType(void) const;
	
	private:
		int i_time_type;
		MTime x_val_date;
		MTime x_dat_date;
		float f_meaning_period;
};

#endif
