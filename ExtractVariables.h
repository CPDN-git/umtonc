//*****************************************************************************
//* File    : ExtractVariables.h
//* Author  : Neil Massey
//* Date    : 03/12/08
//* Purpose : Extract variables from the UM field file headers
//*****************************************************************************

#ifndef EXTRACT_VARS_H
#define EXTRACT_VARS_H

#include "ReadUMFile.h"
#include <list>

void ExtractVarFields(UM_WORD* px_fixhdr, UM_WORD* px_pphdrs, UM_WORD* px_data,
                      std::list<class Variable>& x_var_list);

#endif
