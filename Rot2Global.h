//*****************************************************************************
//* File    : Rot2Global.h
//* Author  : Neil Massey, Tolu Aina, Simon Wilson
//* Date    : 11/12/08
//* Purpose : Functions to convert rotated grids to global grids
//*****************************************************************************

#ifndef ROT_2_GLOBAL
#define ROT_2_GLOBAL

void Rot2Global(float  f_rot_lat,  float f_rot_lon,
                float  f_pole_lat, float f_pole_lon,
                float& f_global_lat, float& f_global_lon);

#endif
