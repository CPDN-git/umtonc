//*****************************************************************************
//* File    : Series.h
//* Author  : Neil Massey
//* Date    : 04/12/08
//* Purpose : Class to store consolidated levels
//*****************************************************************************

#ifndef SERIES_H
#define SERIES_H

#include <list>
#include <algorithm>

template <typename T> class Series
{
	public:
		Series(void)
		{
			i_id = -1;
			f_base_value = 0.0f;
		}
		
		//*********************************************************************

		Series(const T& x_t)
		{
			i_id = -1;
			Add(x_t);
		}

		//*********************************************************************
		
		void Add(const T& x_t)
		{
			// see if the level already exists in the list
			typename std::list<T>::const_iterator it_found;	
			it_found = find(x_series.begin(), x_series.end(), x_t);
			if (it_found == x_series.end())
				// not found so append to the list
				x_series.push_back(x_t);
		}
		
		//*********************************************************************

		bool operator == (const Series& rhs)
		{
			typename std::list<T>::iterator it_x;
			typename std::list<T>::const_iterator it_rhs_x = rhs.x_series.begin();
			// quick size check
			if (x_series.size() != rhs.x_series.size())
				return false;
			bool b_equal = true;
			for (it_x = x_series.begin(); it_x != x_series.end(); it_x++)
			{
				// check if levels are equal and in same order
				b_equal &= (*it_x == *it_rhs_x);
				it_rhs_x ++;
			}
			return b_equal;
		}
		
		//*********************************************************************

		bool operator != (const Series& rhs)
		{
			return !(*this == rhs);
		}
		
		//*********************************************************************

		const std::list<T>& Get(void) const
		{
			return x_series;
		}

		//*********************************************************************

		std::list<T>& Get(void)
		{
			return x_series;
		}
		
		//*********************************************************************

		void SetID(int id)
		{
			i_id = id;
		}
		
		//*********************************************************************

		int GetID(void) const
		{
			return i_id;
		}

		//*********************************************************************

		std::list<float> GetValues(void) const
		{
			std::list<float> x_values;
			// return a list of the values of the time series
			// relies on each template type having a GetValue method
			typename std::list<T>::const_iterator it_series;
			for (it_series = x_series.begin(); it_series != x_series.end();
				 it_series++)
				x_values.push_back(it_series->GetValue() - f_base_value);
			return x_values;
		}

		//*********************************************************************

		void SetBaseValue(float base_value)
		{
			f_base_value = base_value;
		}

		//*********************************************************************

		int Size(void)
		{
			return x_series.size();
		}

		//*********************************************************************

	private:
		std::list<T> x_series;
		int i_id;
		float f_base_value;
};

//*****************************************************************************

#endif

