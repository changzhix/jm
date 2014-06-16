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
 *************************************************************************************
 * \file mc_prediction.h
 *
 * \brief
 *    definitions for motion compensated prediction
 *
 * \author
 *      Main contributors (see contributors.h for copyright, 
 *                         address and affiliation details)
 *      - Alexis Michael Tourapis  <alexismt@ieee.org>
 *
 *************************************************************************************
 */

#ifndef _MC_PREDICTION_H_
#define _MC_PREDICTION_H_

#include "global.h"
#include "mbuffer.h"

extern StorablePicture *dec_picture;

extern void get_block_luma(ColorPlane pl, StorablePicture *list, int x_pos, int y_pos, int ver_block_size, int hor_block_size, ImageParameters *img, imgpel block[MB_BLOCK_SIZE][MB_BLOCK_SIZE]);
extern void get_block_chroma(int uv, StorablePicture *list, int x_pos, int y_pos, int hor_block_size, int ver_block_size, ImageParameters *img, imgpel block[MB_BLOCK_SIZE][MB_BLOCK_SIZE]);

extern void intra_cr_decoding(Macroblock *currMB, int yuv, ImageParameters *img, int smb);
extern void prepare_direct_params(Macroblock *currMB, StorablePicture *dec_picture, ImageParameters *img, short pmvl0[2], short pmvl1[2],char *l0_rFrame, char *l1_rFrame);
extern void perform_mc(ColorPlane pl, StorablePicture *dec_picture, ImageParameters *img, int pred_dir, int i, int j, int list_offset,   int block_size_x, int block_size_y, int curr_mb_field);
#endif

