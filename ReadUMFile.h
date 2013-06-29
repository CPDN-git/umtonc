//*****************************************************************************
//* File    : ReadHdrs.h  
//* Author  : Neil Massey
//* Date    : 20/11/08
//* Purpose : Functions to read parts of the UM headers
//*****************************************************************************

#ifndef READHDRS_H
#define READHDRS_H

#include <fstream>

#define UM_WORD int
#define UM_UNDEFINED -32768

#define FIXHDR_LEN 256
#define INTC_IDX   99
#define REALC_IDX  104
#define LEVC_IDX   109
#define ROWC_IDX   114
#define COLC_IDX   119
#define FIELDC_IDX 124
#define PPHDR_IDX  149

// Parent function
void ReadHdrs(std::ifstream& x_file,
              UM_WORD* &px_fixhdr, UM_WORD* &px_pphdrs,
              UM_WORD* &px_intc,   UM_WORD* &px_realc,
              UM_WORD* &px_levc,   UM_WORD* &px_rowc,
              UM_WORD* &px_colc,   UM_WORD* &px_fieldc);

// constituent functions
UM_WORD ReadNumberOfFields(std::ifstream& x_file, UM_WORD* px_fixhdr);

UM_WORD* ReadFixHdr(std::ifstream& x_file);
UM_WORD* Read1DConstants(std::ifstream& x_file, UM_WORD* px_fixhdr, int idx);
UM_WORD* Read2DConstants(std::ifstream& x_file, UM_WORD* px_fixhdr, int idx);
UM_WORD  ReadValueFrom2D(UM_WORD* px_fixhdr, UM_WORD* px_pphdr, int i_idx,
                         int i_wnum, int i_fnum);
UM_WORD* ReadData(std::ifstream& x_file, UM_WORD* px_fixhdr, UM_WORD* px_pphdr);

// Clean up of headers function
void CleanHdrs(UM_WORD* &px_fixhdr, UM_WORD* &px_pphdrs,
               UM_WORD* &px_intc,   UM_WORD* &px_realc,
               UM_WORD* &px_levc,   UM_WORD* &px_rowc,
			   UM_WORD* &px_colc,   UM_WORD* &px_fieldc);
void CleanData(UM_WORD* &px_data);

#endif
