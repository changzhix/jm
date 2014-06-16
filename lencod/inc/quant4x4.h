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
 * \file quant4x4.h
 *
 * \brief
 *    Quantization process header file
 *
 * \author
 *    Alexis Michael Tourapis         <alexismt@ieee.org>                
 *
 ************************************************************************
 */

#ifndef _QUANT4x4_H_
#define _QUANT4x4_H_

void init_quant_4x4(ImageParameters *img);

int quant_4x4_normal(int (*tblock)[16], int block_y, int block_x, int qp, 
                     int*  ACLevel, int*  ACRun, 
                     int **fadjust4x4, int **levelscale, int **invlevelscale, int **leveloffset,
                     int *coeff_cost, const byte (*pos_scan)[2], const byte *c_cost);

int quant_4x4_around(int (*tblock)[16], int block_y, int block_x, int qp, 
                     int*  ACLevel, int*  ACRun, 
                     int **fadjust4x4, int **levelscale, int **invlevelscale, int **leveloffset,
                     int *coeff_cost, const byte (*pos_scan)[2], const byte *c_cost);

int quant_4x4_trellis (int (*tblock)[16], int block_y, int block_x, int qp,
                       int*  ACLevel, int*  ACRun, 
                       int **fadjust4x4, int **levelscale, int **invlevelscale, int **leveloffset,
                       int *coeff_cost, const byte (*pos_scan)[2], const byte *c_cost);

int (*quant_4x4)    (int (*tblock)[16], int block_y, int block_x, int qp, 
                     int*  ACLevel, int*  ACRun, 
                     int **fadjust4x4, int **levelscale, int **invlevelscale, int **leveloffset,
                     int *coeff_cost, const byte (*pos_scan)[2], const byte *c_cost);

int (*quant_4x4cr)    (int (*tblock)[16], int block_y, int block_x, int qp, 
                     int*  ACLevel, int*  ACRun, 
                     int **fadjust4x4, int **levelscale, int **invlevelscale, int **leveloffset,
                     int *coeff_cost, const byte (*pos_scan)[2], const byte *c_cost);


int quant_ac4x4_normal(int (*tblock)[16], int block_y, int block_x, int qp,                 
                       int*  ACLevel, int*  ACRun, 
                       int **fadjust4x4, int **levelscale, int **invlevelscale, int **leveloffset,
                       int *coeff_cost, const byte (*pos_scan)[2], const byte *c_cost);

int quant_ac4x4_around(int (*tblock)[16], int block_y, int block_x, int qp,                 
                       int*  ACLevel, int*  ACRun, 
                       int **fadjust4x4, int **levelscale, int **invlevelscale, int **leveloffset,
                       int *coeff_cost, const byte (*pos_scan)[2], const byte *c_cost);

int quant_ac4x4_trellis(int (*tblock)[16], int block_y, int block_x, int qp,                
                        int*  ACLevel, int*  ACRun, 
                        int **fadjust4x4, int **levelscale, int **invlevelscale, int **leveloffset,
                        int *coeff_cost, const byte (*pos_scan)[2], const byte *c_cost);


int (*quant_ac4x4)    (int (*tblock)[16], int block_y, int block_x, int qp,                 
                       int*  ACLevel, int*  ACRun, 
                       int **fadjust4x4, int **levelscale, int **invlevelscale, int **leveloffset,
                       int *coeff_cost, const byte (*pos_scan)[2], const byte *c_cost);

int (*quant_ac4x4cr)  (int (*tblock)[16], int block_y, int block_x, int qp,                 
                       int*  ACLevel, int*  ACRun, 
                       int **fadjust4x4, int **levelscale, int **invlevelscale, int **leveloffset,
                       int *coeff_cost, const byte (*pos_scan)[2], const byte *c_cost);


#endif

