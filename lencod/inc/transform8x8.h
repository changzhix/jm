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
 ***************************************************************************
 *
 * \file transform8x8.h
 *
 * \brief
*    prototypes of 8x8 transform functions
  *
 * \date
 *    9. October 2003
 *
 * \author
 *    Main contributors (see contributors.h for copyright, address and affiliation details)
 *    Yuri Vatis
 **************************************************************************/

#ifndef _TRANSFORM8X8_H_
#define _TRANSFORM8X8_H_

int    Mode_Decision_for_new_Intra8x8Macroblock (Macroblock *currMB, double lambda, double *min_cost);
int    (*Mode_Decision_for_new_8x8IntraBlocks)(Macroblock *currMB, int b8, double lambda, double *min_cost, int cr_cbp[3]);
int    Mode_Decision_for_new_8x8IntraBlocks_JM_Low(Macroblock *currMB, int b8, double lambda, double *min_cost, int cr_cbp[3]);
int    Mode_Decision_for_new_8x8IntraBlocks_JM_High(Macroblock *currMB, int b8, double lambda, double *min_cost, int cr_cbp[3]);

double RDCost_for_8x8IntraBlocks(Macroblock *currMB, int *c_nz, int b8, int ipmode, double lambda, double min_rdcost, int mostProbableMode, int c_nzCbCr[3]);
void   compute_comp_cost8x8(imgpel **cur_img, imgpel mpr8x8[16][16], int pic_opix_x, double *cost);

void   intrapred_8x8(Macroblock *currMB, ColorPlane pl, int img_x,int img_y, int *left_available, int *up_available, int *all_available);
void   LowPassForIntra8x8Pred(imgpel *PredPel, int block_up_left, int block_up, int block_left);

#endif //_TRANSFORM8X8_H_
