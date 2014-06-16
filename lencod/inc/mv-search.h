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
 * \file mv-search.h
 *
 * \brief
 *   array definition for motion search
 *
 * \author
 *    Inge Lille-Langoy               <inge.lille-langoy@telenor.com>   \n
 *    Alexis Michael Tourapis         <alexis.tourapis@dolby.com>       \n
 *    Copyright (C) 1999  Telenor Satellite Services, Norway
 *
 ************************************************************************
 */

#ifndef _MV_SEARCH_H_
#define _MV_SEARCH_H_

extern int (*BiPredME)      (Macroblock *, imgpel *, short, int, int, char  ***, short  ****,
                       int, int, int, short[2], short[2], short[2], short[2], int, int, int, int);

extern int (*SubPelBiPredME)(imgpel* orig_pic, short ref, int list, int pic_pix_x, int pic_pix_y,
                             int blocktype, short pred_mv1[2], short pred_mv2[2], short mv1[2], short mv2[2], 
                             int search_pos2, int search_pos4, int min_mcost, int* lambda_factor, int apply_weights);
extern int (*SubPelME)      (imgpel* orig_pic, short ref, int list, int list_offset, int pic_pix_x, int pic_pix_y, 
                             int blocktype, short pred_mv[2], short mv[2], 
                             int search_pos2, int search_pos4, int min_mcost, int* lambda_factor, int apply_weights);

extern void SetMotionVectorPredictor (Macroblock *currMB, short  pmv[2], char   **refPic, short  ***tmp_mv,
                               short  ref_frame, int list, int block_x, int block_y, int blockshape_x, int blockshape_y);

extern void PrepareMEParams(int apply_weights, int ChromaMEEnable, int list, int ref);
extern void PrepareBiPredMEParams(int apply_weights, int ChromaMEEnable, int list, int list_offset, int ref);

#endif

