/*
 * Disclaimer of Warranty
 *
 * Copyright 2004-2008, International Telecommunication Union, Geneva
 *
 * These software programs are available to the user without any
 * license fee or royalty on an "as is" basis. The ITU disclaims
 * any and all warranties, whether express, implied, or statutory,
 * including any implied warranties of merchantability or of fitness
 * for a particular purpose.  In no event shall the ITU be liable for
 * any incidental, punitive, or consequential damages of any kind
 * whatsoever arising from the use of these programs.
 *
 * This disclaimer of warranty extends to the user of these programs
 * and the user's customers, employees, agents, transferees,
 * successors, and assigns.
 *
 * The ITU does not represent or warrant that the programs furnished
 * hereunder are free of infringement of any third-party patents.
 * Commercial implementations of ITU-T Recommendations, including
 * shareware, may be subject to royalty fees to patent holders.
 * Information regarding the ITU-T patent policy is available from the
 * ITU web site at http://www.itu.int.
 *
 * THIS IS NOT A GRANT OF PATENT RIGHTS - SEE THE ITU-T PATENT POLICY.
 *
 */


/*!
 ************************************************************************
 * \file  memalloc.h
 *
 * \brief
 *    Memory allocation and free helper funtions
 *
 * \author
 *    Main contributors (see contributors.h for copyright, address and affiliation details)
 ************************************************************************
 */

#ifndef _MEMALLOC_H_
#define _MEMALLOC_H_

#include "global.h"

int  get_mem2D(byte ***array2D, int rows, int columns);
int  get_mem3D(byte ****array2D, int frames, int rows, int columns);

int  get_mem2Dint(int ***array2D, int rows, int columns);
int  get_mem3Dint(int ****array3D, int frames, int rows, int columns);
int  get_mem4Dint(int *****array4D, int idx, int frames, int rows, int columns );

int  get_mem3Dint64(int64 ****array3D, int frames, int rows, int columns);
int  get_mem2Dint64(int64 ***array2D, int rows, int columns);

int  get_mem2Dshort(short ***array2D, int rows, int columns);
int  get_mem3Dshort(short ****array3D, int frames, int rows, int columns);
int  get_mem4Dshort(short *****array4D, int idx, int frames, int rows, int columns );

int get_mem2Dpel(imgpel  ***array2D, int rows, int columns);
int get_mem3Dpel(imgpel ****array3D, int frames, int rows, int columns);

void free_mem2D(byte **array2D);
void free_mem3D(byte ***array2D);

void free_mem2Dint(int   **array2D);
void free_mem3Dint(int  ***array3D);
void free_mem4Dint(int ****array4D);

void free_mem2Dint64(int64  **array2D);
void free_mem3Dint64(int64 ***array3D);

void free_mem2Dshort(short  **array2D);
void free_mem3Dshort(short  ***array3D);
void free_mem4Dshort(short ****array4D);

void free_mem2Dpel(imgpel  **array2D);
void free_mem3Dpel(imgpel ***array3D);

void no_mem_exit(char *where);

#endif
