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
 * \file
 *    mc_prediction.h
 *
 * \brief
 *    motion compensation header
 *
 * \author
 *    Alexis Michael Tourapis         <alexismt@ieee.org>                 \n
 *
 ************************************************************************/

#ifndef _MC_PREDICTION_H_
#define _MC_PREDICTION_H_
#include "mbuffer.h"

void LumaPrediction      ( Macroblock* currMB, int, int, int, int, int, int, int, short, short );
void LumaPredictionBi    ( Macroblock* currMB, int, int, int, int, int, int, short, short, int );
void ChromaPrediction    ( Macroblock* currMB, int, int, int, int, int, int, int, int, short, short );
void ChromaPrediction4x4 ( Macroblock* currMB, int, int, int, int, int, int, short, short);   

// function pointer for different ways of obtaining chroma interpolation
void (*OneComponentChromaPrediction4x4)         (imgpel* , int , int , short*** , StorablePicture *listX, int );
void OneComponentChromaPrediction4x4_regenerate (imgpel* , int , int , short*** , StorablePicture *listX, int );
void OneComponentChromaPrediction4x4_retrieve   (imgpel* , int , int , short*** , StorablePicture *listX, int );

void IntraChromaPrediction ( Macroblock *currMB, int*, int*, int*);
void IntraChromaRDDecision ( Macroblock *currMB, RD_PARAMS);

void ComputeResidue (imgpel **curImg, imgpel mpr[16][16], int img_m7[16][16], int mb_y, int mb_x, int opix_y, int opix_x, int width, int height);
void SampleReconstruct (imgpel **curImg, imgpel mpr[16][16], int img_m7[16][16], int mb_y, int mb_x, int opix_y, int opix_x, int width, int height, int max_imgpel_value, int dq_bits);
#endif

