//*****************************************************************************
//* File    : MinimiseDimensions.h
//* Author  : Neil Massey
//* Date    : 01/12/08
//* Purpose : Functions to return a minimal set of the dimensions
//*****************************************************************************

#ifndef MINIMISE_DIMS_H
#define MINIMISE_DIMS_H

#include <list>
#include "TimeField.h"
#include "Region.h"
#include "Series.h"
#include "Variable.h"

std::list<Series<TimeField> > MinimiseTimes(std::list<Variable>& x_var_list);
std::list<Region> MinimiseRegions(std::list<Variable>& x_var_list);
std::list<Series<Level> > MinimiseLevels(std::list<Variable>& x_var_list);

#endif
