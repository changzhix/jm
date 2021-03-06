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
 *    macroblock.h
 *
 * \brief
 *    Arrays for macroblock processing
 *
 * \author
 *    Inge Lille-Langoy               <inge.lille-langoy@telenor.com>     \n
 *    Telenor Satellite Services                                          \n
 *    P.O.Box 6914 St.Olavs plass                                         \n
 *    N-0130 Oslo, Norway
 *
 ************************************************************************/

#ifndef _MACROBLOCK_H_
#define _MACROBLOCK_H_

void  proceed2nextMacroblock(Macroblock* currMB);
void  start_macroblock(Macroblock** currMB, int mb_addr, int mb_field);
void  reset_macroblock(Macroblock *currMB, int prev_mb);
void  terminate_macroblock(Macroblock* currMB, Boolean *end_of_slice, Boolean *recode_macroblock);

void  write_one_macroblock(Macroblock* currMB, int eos_bit, Boolean prev_recode_mb);

int  LumaResidualCoding8x8 (Macroblock* currMB, int*, int64*, int, short, int, int, short, short);
void LumaResidualCoding (Macroblock *currMB);

void ChromaResidualCoding (Macroblock *currMB);

void IntraChromaPrediction (Macroblock *currMB, int*, int*, int*);
void IntraChromaRDDecision (Macroblock *currMB, RD_PARAMS);

int  TransformDecision(Macroblock *currMB, int, int*);

int  B8Mode2Value (short b8mode, short b8pdir);

int  writeMBLayer (Macroblock *currMB, int rdopt, int *coeff_rate);
void write_terminating_bit (short bit);

int  writeReferenceFrame  (Macroblock *currMB, int mode, int i, int j, int fwd_flag, int  ref);
int  writeMotionVector8x8 (Macroblock *currMB, int  i0, int  j0, int  i1, int  j1, int  refframe, int  list_idx, int  mv_mode);

int  writeCoeff4x4_CABAC (Macroblock *currMB, ColorPlane, int, int, int);
int  writeCoeff8x8_CABAC (Macroblock* currMB, ColorPlane, int, int);
int  writeCoeff8x8       (Macroblock* currMB, ColorPlane, int, int, int);
int  writeCoeff16x16     (Macroblock* currMB, ColorPlane, int);

int  writeCoeff4x4_CAVLC (Macroblock* currMB, int block_type, int b8, int b4, int param);

int distortion_sad(imgpel **img_org, imgpel pred_img[16][16]);
int distortion_sse(imgpel **img_org, imgpel pred_img[16][16]);
int distortion_hadamard(imgpel **img_org, imgpel pred_img[16][16]);

double (*find_sad_16x16) (Macroblock *currMB, int *intra_mode);
double find_sad_16x16_JM (Macroblock *currMB, int *intra_mode);

void SetLagrangianMultipliers();
void  init_slice(int start_mb_addr);

#endif

