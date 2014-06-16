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
 * \file block.h
 *
 * \brief
 *    definitions for block decoding functions
 *
 * \author
 *  Inge Lille-Langoy               <inge.lille-langoy@telenor.com>    \n
 *  Telenor Satellite Services                                         \n
 *  P.O.Box 6914 St.Olavs plass                                        \n
 *  N-0130 Oslo, Norway
 *
 ************************************************************************
 */

#ifndef _BLOCK_H_
#define _BLOCK_H_

#include "global.h"

#define DQ_BITS         6

extern const byte QP_SCALE_CR[52] ;
extern const int  dequant_coef[6][4][4];
extern const unsigned char subblk_offset_x[3][8][4];
extern const unsigned char subblk_offset_y[3][8][4];
extern void iMBtrans4x4(ColorPlane pl, ImageParameters *img, int smb);
extern void iMBtrans8x8(ColorPlane pl, ImageParameters *img);

extern void itrans_sp_cr(ImageParameters *img, int uv);
void (*itrans_4x4)(ColorPlane pl, ImageParameters *img, int ioff, int joff);
void (*itrans_8x8)(ColorPlane pl, ImageParameters *img, int ioff, int joff);

extern void Inv_Residual_trans_4x4(ColorPlane pl, ImageParameters *img, int ioff, int joff);
extern void Inv_Residual_trans_8x8(ColorPlane pl, ImageParameters *img, int ioff,int joff);

extern void itrans8x8(ColorPlane pl, ImageParameters *img, int ioff, int joff);
extern void itrans4x4(ColorPlane pl, ImageParameters *img, int ioff, int joff);
extern void itrans4x4_ls(ColorPlane pl, ImageParameters *img, int ioff, int joff);
extern void itrans_sp(ColorPlane pl, ImageParameters *img, int ioff, int joff);
extern int  intrapred(Macroblock *currMB, ColorPlane pl, ImageParameters *img,int ioff,int joff,int i4,int j4);
extern void itrans_2 (ColorPlane pl, ImageParameters *img);
extern void iTransform(Macroblock *currMB, ColorPlane pl, ImageParameters *img, int need_4x4_transform, int smb);
#endif

