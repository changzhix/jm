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
 * \file block.c
 *
 * \brief
 *    Process one block
 *
 * \author
 *    Main contributors (see contributors.h for copyright, address and affiliation details)
 *    - Inge Lille-Langoy               <inge.lille-langoy@telenor.com>
 *    - Rickard Sjoberg                 <rickard.sjoberg@era.ericsson.se>
 *    - Stephan Wenger                  <stewe@cs.tu-berlin.de>
 *    - Jani Lainema                    <jani.lainema@nokia.com>
 *    - Detlev Marpe                    <marpe@hhi.de>
 *    - Thomas Wedi                     <wedi@tnt.uni-hannover.de>
 *    - Ragip Kurceren                  <ragip.kurceren@nokia.com>
 *    - Greg Conklin                    <gregc@real.com>
 *************************************************************************************
 */

#include "contributors.h"

#include <math.h>

#include "global.h"

#include "image.h"
#include "mb_access.h"
#include "block.h"
#include "vlc.h"
#include "transform.h"
#include "mc_prediction.h"
#include "q_offsets.h"
#include "q_matrix.h"
#include "quant4x4.h"

const int quant_coef[6][4][4] = {
  {{13107, 8066,13107, 8066},{ 8066, 5243, 8066, 5243},{13107, 8066,13107, 8066},{ 8066, 5243, 8066, 5243}},
  {{11916, 7490,11916, 7490},{ 7490, 4660, 7490, 4660},{11916, 7490,11916, 7490},{ 7490, 4660, 7490, 4660}},
  {{10082, 6554,10082, 6554},{ 6554, 4194, 6554, 4194},{10082, 6554,10082, 6554},{ 6554, 4194, 6554, 4194}},
  {{ 9362, 5825, 9362, 5825},{ 5825, 3647, 5825, 3647},{ 9362, 5825, 9362, 5825},{ 5825, 3647, 5825, 3647}},
  {{ 8192, 5243, 8192, 5243},{ 5243, 3355, 5243, 3355},{ 8192, 5243, 8192, 5243},{ 5243, 3355, 5243, 3355}},
  {{ 7282, 4559, 7282, 4559},{ 4559, 2893, 4559, 2893},{ 7282, 4559, 7282, 4559},{ 4559, 2893, 4559, 2893}}
};

const int dequant_coef[6][4][4] = {
  {{10, 13, 10, 13},{ 13, 16, 13, 16},{10, 13, 10, 13},{ 13, 16, 13, 16}},
  {{11, 14, 11, 14},{ 14, 18, 14, 18},{11, 14, 11, 14},{ 14, 18, 14, 18}},
  {{13, 16, 13, 16},{ 16, 20, 16, 20},{13, 16, 13, 16},{ 16, 20, 16, 20}},
  {{14, 18, 14, 18},{ 18, 23, 18, 23},{14, 18, 14, 18},{ 18, 23, 18, 23}},
  {{16, 20, 16, 20},{ 20, 25, 20, 25},{16, 20, 16, 20},{ 20, 25, 20, 25}},
  {{18, 23, 18, 23},{ 23, 29, 23, 29},{18, 23, 18, 23},{ 23, 29, 23, 29}}
};
static const int A[4][4] = {
  { 16, 20, 16, 20},
  { 20, 25, 20, 25},
  { 16, 20, 16, 20},
  { 20, 25, 20, 25}
};


const byte QP_SCALE_CR[52]=
{
  0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,
  12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,
  28,29,29,30,31,32,32,33,34,34,35,35,36,36,37,37,
  37,38,38,38,39,39,39,39
};

// Notation for comments regarding prediction and predictors.
// The pels of the 4x4 block are labelled a..p. The predictor pels above
// are labelled A..H, from the left I..P, and from above left X, as follows:
//
//  X A B C D E F G H
//  I a b c d
//  J e f g h
//  K i j k l
//  L m n o p
//

// Predictor array index definitions
#define P_X (PredPel[0])
#define P_A (PredPel[1])
#define P_B (PredPel[2])
#define P_C (PredPel[3])
#define P_D (PredPel[4])
#define P_E (PredPel[5])
#define P_F (PredPel[6])
#define P_G (PredPel[7])
#define P_H (PredPel[8])
#define P_I (PredPel[9])
#define P_J (PredPel[10])
#define P_K (PredPel[11])
#define P_L (PredPel[12])

// global pointers for 4x4 quantization parameters
extern int ****ptLevelOffset4x4;
static int **levelscale = NULL, **leveloffset = NULL;
static int **invlevelscale = NULL;
static int **fadjust4x4 = NULL;
static int **levelscaleDC = NULL, **leveloffsetDC = NULL;
static int **invlevelscaleDC = NULL;
static int M1[MB_BLOCK_SIZE][MB_BLOCK_SIZE];
static int M4[4][4];

int Inv_Residual_DPCM_4x4(int m7[16][16], int block_y, int block_x);
int Residual_DPCM_4x4_for_Intra16x16(int m7[4][4], int ipmode);
int Inv_Residual_DPCM_4x4_for_Intra16x16(int m7[4][4], int ipmode);

/*!
 ************************************************************************
 * \brief
 *    Make intra 4x4 prediction according to all 9 prediction modes.
 *    The routine uses left and upper neighbouring points from
 *    previous coded blocks to do this (if available). Notice that
 *    inaccessible neighbouring points are signalled with a negative
 *    value in the predmode array .
 *
 *  \par Input:
 *     Starting point of current 4x4 block image posision
 *
 *  \par Output:
 *      none
 ************************************************************************
 */
void intrapred_4x4(Macroblock *currMB, ColorPlane pl, int img_x,int img_y, int *left_available, int *up_available, int *all_available)
{
  int i,j;
  int s0;
  imgpel  PredPel[13];  // array of predictor pels
  imgpel  **img_enc = enc_picture->p_curr_img;
  imgpel  *img_pel;  
  imgpel (*cur_pred)[MB_BLOCK_SIZE];
  imgpel (*curr_mpr_4x4)[MB_BLOCK_SIZE][MB_BLOCK_SIZE]  = img->mpr_4x4[pl];
  unsigned int dc_pred_value = img->dc_pred_value;

  int ioff = (img_x & 15);
  int joff = (img_y & 15);

  PixelPos pix_a[4];
  PixelPos pix_b, pix_c, pix_d;

  int block_available_up;
  int block_available_left;
  int block_available_up_left;
  int block_available_up_right;
  int *mb_size = img->mb_size[IS_LUMA];

  for (i=0;i<4;i++)
  {
    getNeighbour(currMB, ioff -1 , joff +i , mb_size, &pix_a[i]);
  }

  getNeighbour(currMB, ioff    , joff -1 , mb_size, &pix_b);
  getNeighbour(currMB, ioff +4 , joff -1 , mb_size, &pix_c);
  getNeighbour(currMB, ioff -1 , joff -1 , mb_size, &pix_d);

  pix_c.available = pix_c.available && !((ioff==4) && ((joff==4)||(joff==12)));

  if (params->UseConstrainedIntraPred)
  {
    for (i=0, block_available_left=1; i<4;i++)
      block_available_left  &= pix_a[i].available ? img->intra_block[pix_a[i].mb_addr]: 0;
    block_available_up       = pix_b.available ? img->intra_block [pix_b.mb_addr] : 0;
    block_available_up_right = pix_c.available ? img->intra_block [pix_c.mb_addr] : 0;
    block_available_up_left  = pix_d.available ? img->intra_block [pix_d.mb_addr] : 0;
  }
  else
  {
    block_available_left     = pix_a[0].available;
    block_available_up       = pix_b.available;
    block_available_up_right = pix_c.available;
    block_available_up_left  = pix_d.available;
  }

  *left_available = block_available_left;
  *up_available   = block_available_up;
  *all_available  = block_available_up && block_available_left && block_available_up_left;

  i = (img_x & 15);
  j = (img_y & 15);

  // form predictor pels
  if (block_available_up)
  {
    img_pel = &img_enc[pix_b.pos_y][pix_b.pos_x];
    P_A = *(img_pel++);
    P_B = *(img_pel++);
    P_C = *(img_pel++);
    P_D = *(img_pel);

  }
  else
  {
    P_A = P_B = P_C = P_D = dc_pred_value;
  }

  if (block_available_up_right)
  {
    img_pel = &img_enc[pix_c.pos_y][pix_c.pos_x];
    P_E = *(img_pel++);
    P_F = *(img_pel++);
    P_G = *(img_pel++);
    P_H = *(img_pel);
  }
  else
  {
    P_E = P_F = P_G = P_H = P_D;
  }

  if (block_available_left)
  {
    P_I = img_enc[pix_a[0].pos_y][pix_a[0].pos_x];
    P_J = img_enc[pix_a[1].pos_y][pix_a[1].pos_x];
    P_K = img_enc[pix_a[2].pos_y][pix_a[2].pos_x];
    P_L = img_enc[pix_a[3].pos_y][pix_a[3].pos_x];
  }
  else
  {
    P_I = P_J = P_K = P_L = dc_pred_value;
  }

  if (block_available_up_left)
  {
    P_X = img_enc[pix_d.pos_y][pix_d.pos_x];
  }
  else
  {
    P_X = dc_pred_value;
  }
  for(i=0;i<9;i++)
    curr_mpr_4x4[i][0][0]=-1;

  ///////////////////////////////
  // make DC prediction
  ///////////////////////////////
  s0 = 0;
  if (block_available_up && block_available_left)
  {
    // no edge
    s0 = (P_A + P_B + P_C + P_D + P_I + P_J + P_K + P_L + 4) >> (BLOCK_SHIFT + 1);
  }
  else if (!block_available_up && block_available_left)
  {
    // upper edge
    s0 = (P_I + P_J + P_K + P_L + 2) >> BLOCK_SHIFT;;
  }
  else if (block_available_up && !block_available_left)
  {
    // left edge
    s0 = (P_A + P_B + P_C + P_D + 2) >> BLOCK_SHIFT;
  }
  else //if (!block_available_up && !block_available_left)
  {
    // top left corner, nothing to predict from
    s0 = dc_pred_value;
  }

  // store DC prediction
  cur_pred = curr_mpr_4x4[DC_PRED];
  for (j=0; j < BLOCK_SIZE; j++)
  {
    for (i=0; i < BLOCK_SIZE; i++)
      cur_pred[j][i] = (imgpel) s0;
  }

  ///////////////////////////////
  // make horiz and vert prediction
  ///////////////////////////////

  //Mode vertical
  cur_pred = curr_mpr_4x4[VERT_PRED];
  for (i=0; i < BLOCK_SIZE; i++)
  {
    cur_pred[0][i] =
    cur_pred[1][i] =
    cur_pred[2][i] =
    cur_pred[3][i] = (imgpel) (&P_A)[i];
  }
  if(!block_available_up)
    cur_pred [0][0]=-1;

  //Mode horizontal
  cur_pred = curr_mpr_4x4[HOR_PRED];
  for (i=0; i < BLOCK_SIZE; i++)
  {
    cur_pred[i][0]  =
    cur_pred[i][1]  =
    cur_pred[i][2]  =
    cur_pred[i][3]  = (imgpel) (&P_I)[i];
  }
  if(!block_available_left)
    cur_pred[0][0]=-1;

  if (block_available_up)
  {
    // Mode DIAG_DOWN_LEFT_PRED
    cur_pred = curr_mpr_4x4[DIAG_DOWN_LEFT_PRED];
    cur_pred[0][0] = (imgpel) ((P_A + P_C + ((P_B)<<1) + 2) >> 2);
    cur_pred[0][1] =
    cur_pred[1][0] = (imgpel) ((P_B + P_D + ((P_C)<<1) + 2) >> 2);
    cur_pred[0][2] =
    cur_pred[1][1] =
    cur_pred[2][0] = (imgpel) ((P_C + P_E + ((P_D)<<1) + 2) >> 2);
    cur_pred[0][3] =
    cur_pred[1][2] =
    cur_pred[2][1] =
    cur_pred[3][0] = (imgpel) ((P_D + P_F + ((P_E)<<1) + 2) >> 2);
    cur_pred[1][3] =
    cur_pred[2][2] =
    cur_pred[3][1] = (imgpel) ((P_E + P_G + ((P_F)<<1) + 2) >> 2);
    cur_pred[2][3] =
    cur_pred[3][2] = (imgpel) ((P_F + P_H + ((P_G)<<1) + 2) >> 2);
    cur_pred[3][3] = (imgpel) ((P_G + 3*(P_H) + 2) >> 2);

    // Mode VERT_LEFT_PRED
    cur_pred = curr_mpr_4x4[VERT_LEFT_PRED];
    cur_pred[0][0] = (imgpel) ((P_A + P_B + 1) >> 1);
    cur_pred[0][1] =
    cur_pred[2][0] = (imgpel) ((P_B + P_C + 1) >> 1);
    cur_pred[0][2] =
    cur_pred[2][1] = (imgpel) ((P_C + P_D + 1) >> 1);
    cur_pred[0][3] =
    cur_pred[2][2] = (imgpel) ((P_D + P_E + 1) >> 1);
    cur_pred[2][3] = (imgpel) ((P_E + P_F + 1) >> 1);
    cur_pred[1][0] = (imgpel) ((P_A + ((P_B)<<1) + P_C + 2) >> 2);
    cur_pred[1][1] =
    cur_pred[3][0] = (imgpel) ((P_B + ((P_C)<<1) + P_D + 2) >> 2);
    cur_pred[1][2] =
    cur_pred[3][1] = (imgpel) ((P_C + ((P_D)<<1) + P_E + 2) >> 2);
    cur_pred[1][3] =
    cur_pred[3][2] = (imgpel) ((P_D + ((P_E)<<1) + P_F + 2) >> 2);
    cur_pred[3][3] = (imgpel) ((P_E + ((P_F)<<1) + P_G + 2) >> 2);

  }

  /*  Prediction according to 'diagonal' modes */
  if (block_available_left)
  {
    // Mode HOR_UP_PRED
    cur_pred = curr_mpr_4x4[HOR_UP_PRED];
    cur_pred[0][0] = (imgpel) ((P_I + P_J + 1) >> 1);
    cur_pred[0][1] = (imgpel) ((P_I + 2*P_J + P_K + 2) >> 2);
    cur_pred[0][2] =
    cur_pred[1][0] = (imgpel) ((P_J + P_K + 1) >> 1);
    cur_pred[0][3] =
    cur_pred[1][1] = (imgpel) ((P_J + 2*P_K + P_L + 2) >> 2);
    cur_pred[1][2] =
    cur_pred[2][0] = (imgpel) ((P_K + P_L + 1) >> 1);
    cur_pred[1][3] =
    cur_pred[2][1] = (imgpel) ((P_K + 2*P_L + P_L + 2) >> 2);
    cur_pred[3][0] =
    cur_pred[2][2] =
    cur_pred[2][3] =
    cur_pred[3][1] =
    cur_pred[3][2] =
    cur_pred[3][3] = (imgpel) P_L;
  }

  /*  Prediction according to 'diagonal' modes */
  if (block_available_up && block_available_left && block_available_up_left)
  {
    // Mode DIAG_DOWN_RIGHT_PRED
    cur_pred = curr_mpr_4x4[DIAG_DOWN_RIGHT_PRED];
    cur_pred[3][0] = (imgpel) ((P_L + 2*P_K + P_J + 2) >> 2);
    cur_pred[2][0] =
    cur_pred[3][1] = (imgpel) ((P_K + 2*P_J + P_I + 2) >> 2);
    cur_pred[1][0] =
    cur_pred[2][1] =
    cur_pred[3][2] = (imgpel) ((P_J + 2*P_I + P_X + 2) >> 2);
    cur_pred[0][0] =
    cur_pred[1][1] =
    cur_pred[2][2] =
    cur_pred[3][3] = (imgpel) ((P_I + 2*P_X + P_A + 2) >> 2);
    cur_pred[0][1] =
    cur_pred[1][2] =
    cur_pred[2][3] = (imgpel) ((P_X + 2*P_A + P_B + 2) >> 2);
    cur_pred[0][2] =
    cur_pred[1][3] = (imgpel) ((P_A + 2*P_B + P_C + 2) >> 2);
    cur_pred[0][3] = (imgpel) ((P_B + 2*P_C + P_D + 2) >> 2);

    // Mode VERT_RIGHT_PRED
    cur_pred = curr_mpr_4x4[VERT_RIGHT_PRED];
    cur_pred[0][0] =
    cur_pred[2][1] = (imgpel) ((P_X + P_A + 1) >> 1);
    cur_pred[0][1] =
    cur_pred[2][2] = (imgpel) ((P_A + P_B + 1) >> 1);
    cur_pred[0][2] =
    cur_pred[2][3] = (imgpel) ((P_B + P_C + 1) >> 1);
    cur_pred[0][3] = (imgpel) ((P_C + P_D + 1) >> 1);
    cur_pred[1][0] =
    cur_pred[3][1] = (imgpel) ((P_I + 2*P_X + P_A + 2) >> 2);
    cur_pred[1][1] =
    cur_pred[3][2] = (imgpel) ((P_X + 2*P_A + P_B + 2) >> 2);
    cur_pred[1][2] =
    cur_pred[3][3] = (imgpel) ((P_A + 2*P_B + P_C + 2) >> 2);
    cur_pred[1][3] = (imgpel) ((P_B + 2*P_C + P_D + 2) >> 2);
    cur_pred[2][0] = (imgpel) ((P_X + 2*P_I + P_J + 2) >> 2);
    cur_pred[3][0] = (imgpel) ((P_I + 2*P_J + P_K + 2) >> 2);

    // Mode HOR_DOWN_PRED
    cur_pred = curr_mpr_4x4[HOR_DOWN_PRED];
    cur_pred[0][0] =
    cur_pred[1][2] = (imgpel) ((P_X + P_I + 1) >> 1);
    cur_pred[0][1] =
    cur_pred[1][3] = (imgpel) ((P_I + 2*P_X + P_A + 2) >> 2);
    cur_pred[0][2] = (imgpel) ((P_X + 2*P_A + P_B + 2) >> 2);
    cur_pred[0][3] = (imgpel) ((P_A + 2*P_B + P_C + 2) >> 2);
    cur_pred[1][0] =
    cur_pred[2][2] = (imgpel) ((P_I + P_J + 1) >> 1);
    cur_pred[1][1] =
    cur_pred[2][3] = (imgpel) ((P_X + 2*P_I + P_J + 2) >> 2);
    cur_pred[2][0] =
    cur_pred[3][2] = (imgpel) ((P_J + P_K + 1) >> 1);
    cur_pred[2][1] =
    cur_pred[3][3] = (imgpel) ((P_I + 2*P_J + P_K + 2) >> 2);
    cur_pred[3][0] = (imgpel) ((P_K + P_L + 1) >> 1);
    cur_pred[3][1] = (imgpel) ((P_J + 2*P_K + P_L + 2) >> 2);
  }
}


/*!
 ************************************************************************
 * \brief
 *    16x16 based luma prediction
 *
 * \par Input:
 *    Image parameters
 *
 * \par Output:
 *    none
 ************************************************************************
 */
void intrapred_16x16(Macroblock *currMB, ColorPlane pl)
{
  int s0=0,s1,s2;
  imgpel s[2][16];
  int i,j;

  int ih,iv;
  int ib,ic,iaa;

  imgpel **img_enc = enc_picture->p_curr_img;
  imgpel (*curr_mpr_16x16)[MB_BLOCK_SIZE][MB_BLOCK_SIZE]  = img->mpr_16x16[pl];
  unsigned int dc_pred_value = img->dc_pred_value;

  PixelPos up;          //!< pixel position p(0,-1)
  PixelPos left[17];    //!< pixel positions p(-1, -1..15)

  int up_avail, left_avail, left_up_avail;
  int *mb_size = img->mb_size[IS_LUMA];

  for (i=0;i<17;i++)
  {
    getNeighbour(currMB, -1,  i-1, mb_size, &left[i]);
  }

  getNeighbour(currMB,    0,   -1, mb_size, &up);

  if (!(params->UseConstrainedIntraPred))
  {
    up_avail      = up.available;
    left_avail    = left[1].available;
    left_up_avail = left[0].available;
  }
  else
  {
    up_avail      = up.available ? img->intra_block[up.mb_addr] : 0;
    for (i=1, left_avail=1; i<17;i++)
      left_avail  &= left[i].available ? img->intra_block[left[i].mb_addr]: 0;
    left_up_avail = left[0].available ? img->intra_block[left[0].mb_addr]: 0;
  }

  s1=s2=0;
  // make DC prediction
  if (up_avail)
  {
    for (i=up.pos_x; i < up.pos_x + MB_BLOCK_SIZE; i++)
      s1 += img_enc[up.pos_y][i];    // sum hor pix
  }

  if (left_avail)
  {
    for (i=1; i < MB_BLOCK_SIZE + 1; i++)
      s2 += img_enc[left[i].pos_y][left[i].pos_x];    // sum vert pix
  }

  if (up_avail)
  {
    s0= left_avail
      ? rshift_rnd_sf((s1+s2),(MB_BLOCK_SHIFT + 1)) // no edge
      : rshift_rnd_sf(s1, MB_BLOCK_SHIFT);          // left edge
  }
  else
  {
    s0=left_avail
      ? rshift_rnd_sf(s2, MB_BLOCK_SHIFT)           // upper edge
      : dc_pred_value;                              // top left corner, nothing to predict from
  }

  // vertical prediction
  if (up_avail)
    memcpy(s[0], &img_enc[up.pos_y][up.pos_x], MB_BLOCK_SIZE * sizeof(imgpel));

  // horizontal prediction
  if (left_avail)
  {
    for (i=1; i < MB_BLOCK_SIZE + 1; i++)
      s[1][i - 1]=img_enc[left[i].pos_y][left[i].pos_x];
  }

  for (j=0; j < MB_BLOCK_SIZE; j++)
  {
    memcpy(curr_mpr_16x16[VERT_PRED_16][j], s[0], MB_BLOCK_SIZE * sizeof(imgpel)); // store vertical prediction
    for (i=0; i < MB_BLOCK_SIZE; i++)
    {
      curr_mpr_16x16[HOR_PRED_16 ][j][i] = s[1][j]; // store horizontal prediction
      curr_mpr_16x16[DC_PRED_16  ][j][i] = s0;      // store DC prediction
    }
  }
  
  if (!up_avail || !left_avail || !left_up_avail) // edge
    return;

  // 16 bit integer plan pred

  ih=0;
  iv=0;
  for (i=1;i<9;i++)
  {
    if (i<8)
      ih += i*(img_enc[up.pos_y][up.pos_x+7+i] - img_enc[up.pos_y][up.pos_x+7-i]);
    else
      ih += i*(img_enc[up.pos_y][up.pos_x+7+i] - img_enc[left[0].pos_y][left[0].pos_x]);

    iv += i*(img_enc[left[8+i].pos_y][left[8+i].pos_x] - img_enc[left[8-i].pos_y][left[8-i].pos_x]);
  }
  ib=(5*ih+32)>>6;
  ic=(5*iv+32)>>6;

  iaa=16*(img_enc[up.pos_y][up.pos_x+15]+img_enc[left[16].pos_y][left[16].pos_x]);

  for (j=0;j< MB_BLOCK_SIZE;j++)
  {
    for (i=0;i< MB_BLOCK_SIZE;i++)
    {
      curr_mpr_16x16[PLANE_16][j][i]= iClip1( img->max_imgpel_value, rshift_rnd_sf((iaa+(i-7)*ib +(j-7)*ic), 5));// store plane prediction
    }
  }
}


/*!
 ************************************************************************
 * \brief
 *    For new intra pred routines
 *
 * \par Input:
 *    Image par, 16x16 based intra mode
 *
 * \par Output:
 *    none
 ************************************************************************
 */
int dct_16x16(Macroblock *currMB, ColorPlane pl, int new_intra_mode)
{
  //int qp_const;
  int i,j;
  int ii,jj;
   
  int run,scan_pos,coeff_ctr,level;
  int ac_coef = 0;
  static imgpel *img_Y, *predY;
  int nonzero = FALSE;

  int   jpos, ipos;
  int   b8, b4;

  //begin the changes
  int   pl_off = pl<<2;
  int*  DCLevel = img->cofDC[pl][0];
  int*  DCRun   = img->cofDC[pl][1];
  int   ****cofAC = &img->cofAC[pl_off];
  int*  ACLevel;
  int*  ACRun;  
  int coeff_cost;
  imgpel **img_enc          = enc_picture->p_curr_img;
  int    max_imgpel_value   = img->max_imgpel_value;
  int    qp                 = currMB->qp_scaled[pl]; 
  const byte (*pos_scan)[2] = currMB->is_field_mode ? FIELD_SCAN : SNGL_SCAN;
  imgpel  (*curr_mpr_16x16)[MB_BLOCK_SIZE][MB_BLOCK_SIZE]  = img->mpr_16x16[pl];

  int qp_per = qp_per_matrix[qp];
  int qp_rem = qp_rem_matrix[qp];
  int q_bits = Q_BITS + qp_per + 1;

  // select scaling parameters
  levelscale    = LevelScale4x4Comp[pl][1][qp_rem];
  invlevelscale = InvLevelScale4x4Comp[pl][1][qp_rem];
  leveloffset   = ptLevelOffset4x4  [1][qp]; 

  fadjust4x4 = img->AdaptiveRounding ? (pl ? &img->fadjust4x4Cr[pl-1][2][0] : &img->fadjust4x4[2][0]): NULL;

  for (j = 0; j < 16; j++)
  {
    predY = curr_mpr_16x16[new_intra_mode][j];
    img_Y = &pCurImg[img->opix_y + j][img->opix_x];
    for (i = 0; i < 16; i++)
    {
      M1[j][i] = img_Y[i] - predY[i];
    }
  }

    // forward 4x4 DCT
    for (j = 0; j < 16; j+=4)
    {
      for (i = 0;i < 16; i+=4)
      {
        forward4x4(M1, M1, j, i);
      }
    }

    // pick out DC coeff
    for (j = 0; j < 4; j++)
      for (i = 0; i < 4; i++)
        M4[j][i]= M1[j << 2][i << 2];

    // hadamard of DC coefficients
    hadamard4x4(M4, M4);

    // quant
    run=-1;
    scan_pos=0;
    
    for (coeff_ctr=0;coeff_ctr<16;coeff_ctr++)
    {
      i=pos_scan[coeff_ctr][0];
      j=pos_scan[coeff_ctr][1];

      run++;
      level= (iabs(M4[j][i]) * levelscale[0][0] + (leveloffset[0][0]<<1)) >> q_bits;

      if (level != 0)
      {
        nonzero = TRUE;
        if (params->symbol_mode == CAVLC && img->qp < 10)
          level = imin(level, CAVLC_LEVEL_LIMIT);

        level = isignab(level, M4[j][i]);
        DCLevel[scan_pos  ] = level;
        DCRun  [scan_pos++] = run;
        run=-1;
        M4[j][i] = level;
      }
      else
        M4[j][i] = 0;
    }
    DCLevel[scan_pos]=0;

    // inverse DC transform
    if (nonzero)
    {
      ihadamard4x4(M4, M4);

      // Reset DC coefficients
      for (j=0;j<4;j++)
        for (i=0;i<4;i++)
          M1[j<<2][i<<2] = rshift_rnd_sf(((M4[j][i]) * invlevelscale[0][0]) << qp_per, 6);
    }
    else // All DC equal to 0.
    {
      for (j=0;j<4;j++)
        for (i=0;i<4;i++)
          M1[j<<2][i<<2] = 0;
    }

    // AC processing for MB
    for (jj=0;jj<4;jj++)
    {
      jpos = (jj << 2);
      for (ii=0;ii<4;ii++)
      {
        ipos = (ii << 2);

        b8       = 2*(jj >> 1) + (ii >> 1);
        b4       = 2*(jj & 0x01) + (ii & 0x01);
        ACLevel  = cofAC[b8][b4][0];
        ACRun    = cofAC[b8][b4][1];

        // Quantization process
        nonzero = quant_ac4x4(&M1[jpos], jpos, ipos, qp, ACLevel, ACRun, &fadjust4x4[jpos], 
          levelscale, invlevelscale, leveloffset, &coeff_cost, pos_scan, COEFF_COST4x4[params->disthres]);

        if (nonzero)
          ac_coef = 15;

        //IDCT
        if (M1[jpos][ipos]!= 0 || nonzero)
          inverse4x4(M1, M1, jpos, ipos);
      }
    }


    // Reconstruct samples
    SampleReconstruct (img_enc, curr_mpr_16x16[new_intra_mode], M1, 0, 0, img->pix_y, img->pix_x, 16, 16, max_imgpel_value, DQ_BITS);

    if(img->type == SP_SLICE)
    {
      for (j = img->pix_y; j < img->pix_y + 16;j++)
        for (i = img->pix_x; i < img->pix_x + 16;i++)
          lrec[j][i]=-16; //signals an I16 block in the SP frame
    }

  return ac_coef;
}


/*!
 ************************************************************************
 * \brief
 *    For new intra pred routines
 *
 * \par Input:
 *    Image par, 16x16 based intra mode
 *
 * \par Output:
 *    none
 ************************************************************************
 */
int dct_16x16_ls(Macroblock *currMB, ColorPlane pl, int new_intra_mode)
{
  int i,j;
  int ii,jj;

  int run,scan_pos,coeff_ctr;
  int ac_coef = 0;
  static imgpel *img_Y, *predY;

  int   b8, b4;

  //begin the changes
  int   pl_off = pl<<2;
  int*  DCLevel = img->cofDC[pl][0];
  int*  DCRun   = img->cofDC[pl][1];
  int*  ACLevel;
  int*  ACRun;  
  imgpel **img_enc = enc_picture->p_curr_img;
  int   *m7;

  const byte (*pos_scan)[2] = currMB->is_field_mode ? FIELD_SCAN : SNGL_SCAN;
  imgpel  (*curr_mpr_16x16)[MB_BLOCK_SIZE][MB_BLOCK_SIZE]  = img->mpr_16x16[pl];

  for (j = 0; j < 16; j++)
  {
    predY = curr_mpr_16x16[new_intra_mode][j];
    img_Y = &pCurImg[img->opix_y + j][img->opix_x];
    for (i = 0; i < 16; i++)
    {
      M1[j][i] = img_Y[i] - predY[i];
    }
  }
  // pick out DC coeff
  for (j = 0; j < 4;j++)
    for (i = 0; i < 4;i++)
      M4[j][i]= M1[j << 2][i << 2];

  run=-1;
  scan_pos=0;

  for (coeff_ctr=0;coeff_ctr<16;coeff_ctr++)
  {
    i=pos_scan[coeff_ctr][0];
    j=pos_scan[coeff_ctr][1];

    run++;

    m7 = &M4[j][i];

    if (*m7 != 0)
    {
      if (params->symbol_mode == CAVLC)
        *m7 = iClip3(-CAVLC_LEVEL_LIMIT, CAVLC_LEVEL_LIMIT, *m7);

      DCLevel[scan_pos  ] = *m7;
      DCRun  [scan_pos++] = run;
      run=-1;
    }
  }
  DCLevel[scan_pos]=0;

  // replace DC coeff. This is needed in case of out of limits for CAVLC. Could be done only for CAVLC
  for (j = 0; j < 4;j++)
    for (i = 0; i < 4;i++)
      M1[j << 2][i << 2] = M4[j][i];

  // AC inverse trans/quant for MB
  for (jj=0;jj<4;jj++)
  {
    for (ii=0;ii<4;ii++)
    {
      for (j=0;j<4;j++)
      {
        memcpy(M4[j],&M1[(jj<<2)+j][(ii<<2)], BLOCK_SIZE * sizeof(int));
      }

      //For residual DPCM
      if(new_intra_mode < 2)  //residual DPCM
      {
        Residual_DPCM_4x4_for_Intra16x16(M4, new_intra_mode);  
      }

      run      = -1;
      scan_pos =  0;
      b8       = 2*(jj >> 1) + (ii >> 1);
      b4       = 2*(jj & 0x01) + (ii & 0x01);
      ACLevel  = img->cofAC [b8+pl_off][b4][0];
      ACRun    = img->cofAC [b8+pl_off][b4][1];

      for (coeff_ctr=1;coeff_ctr<16;coeff_ctr++) // set in AC coeff
      {
        i=pos_scan[coeff_ctr][0];
        j=pos_scan[coeff_ctr][1];

        run++;
        *m7 = M4[j][i];

        if (*m7 != 0)
        {
          if (params->symbol_mode == CAVLC)
            *m7 = iClip3(-CAVLC_LEVEL_LIMIT, CAVLC_LEVEL_LIMIT, *m7);

          ac_coef = 15;
          ACLevel[scan_pos  ] = *m7;
          ACRun  [scan_pos++] = run;
          run=-1;
        }
        // set adaptive rounding params to 0 since process is not meaningful here.
      }
      ACLevel[scan_pos] = 0;

      ///For residual DPCM.  inv. residual DCPM
      if(new_intra_mode<2)  
      {
        Inv_Residual_DPCM_4x4_for_Intra16x16(M4, new_intra_mode);  
      }
      for (j=0;j<4;j++)
        memcpy(&M1[(jj<<2)+j][(ii<<2)],M4[j], BLOCK_SIZE * sizeof(int)); 
    }
  }

  for (j = 0; j < 16; j++)
  {
    img_Y = &img_enc[img->pix_y + j][img->pix_x];
    predY = curr_mpr_16x16[new_intra_mode][j];        
    for (i = 0; i < 16; i++)
      img_Y[i]=(imgpel)(M1[j][i] + predY[i]);
  }

  if(img->type == SP_SLICE)
  {
    for (j = img->pix_y; j < img->pix_y + 16;j++)
      for (i = img->pix_x; i < img->pix_x + 16;i++)
        lrec[j][i]=-16; //signals an I16 block in the SP frame
  }

  return ac_coef;
}

/*!
************************************************************************
* \brief
*    The routine performs transform,quantization,inverse transform, 
*    adds the diff to the prediction and writes the result to the 
*    decoded luma frame. 
*
* \par Input:
*    currMB:          Current macroblock.
*    pl:              Color plane for 4:4:4 coding.
*    block_x,block_y: Block position inside a macro block (0,4,8,12).
*    intra:           Intra block indicator.
*
* \par Output_
*    nonzero:         0 if no levels are nonzero. \n
*                     1 if there are nonzero levels.\n
*    coeff_cost:      Coeff coding cost for thresholding consideration.\n
************************************************************************
*/
int dct_4x4(Macroblock *currMB, ColorPlane pl, int block_x,int block_y, int *coeff_cost, int intra)
{
  int j;

  int nonzero = FALSE;  

  int   pos_x   = block_x >> BLOCK_SHIFT;
  int   pos_y   = block_y >> BLOCK_SHIFT;
  int   b8      = 2*(pos_y >> 1) + (pos_x >> 1) + (pl<<2);
  int   b4      = 2*(pos_y & 0x01) + (pos_x & 0x01);
  int*  ACLevel = img->cofAC[b8][b4][0];
  int*  ACRun   = img->cofAC[b8][b4][1];

  imgpel **img_enc      = enc_picture->p_curr_img;
  imgpel (*curr_mpr)[MB_BLOCK_SIZE] = img->mpr[pl];
  int    (*curr_res)[MB_BLOCK_SIZE] = img->m7[pl]; 

  int   max_imgpel_value = img->max_imgpel_value;
  int   qp = currMB->qp_scaled[pl]; 
  const byte (*pos_scan)[2] = currMB->is_field_mode ? FIELD_SCAN : SNGL_SCAN;

  int qp_rem = qp_rem_matrix[qp];

  // select scaling parameters
  levelscale    = LevelScale4x4Comp[pl][intra][qp_rem];
  invlevelscale = InvLevelScale4x4Comp[pl][intra][qp_rem];
  leveloffset   = ptLevelOffset4x4[intra][qp];

  fadjust4x4    = img->AdaptiveRounding ? (pl ? &img->fadjust4x4Cr[pl-1][intra][block_y] : &img->fadjust4x4[intra][block_y]) : NULL;

  //  Forward 4x4 transform
  forward4x4(curr_res, M1, block_y, block_x);

  // Quantization process
  nonzero = quant_4x4(&M1[block_y], block_y, block_x, qp, ACLevel, ACRun, fadjust4x4, 
    levelscale, invlevelscale, leveloffset, coeff_cost, pos_scan, COEFF_COST4x4[params->disthres]);

  if (nonzero)
  {
    // Inverse 4x4 transform
    inverse4x4(M1, curr_res, block_y, block_x);

    // generate final block
    SampleReconstruct (img_enc, curr_mpr, curr_res, block_y, block_x, img->pix_y, img->pix_x + block_x, BLOCK_SIZE, BLOCK_SIZE, max_imgpel_value, DQ_BITS);
  }
  else // if (nonzero) => No transformed residual. Just use prediction.
  {
    for (j=block_y; j < block_y + BLOCK_SIZE; j++)
    {
      memcpy(&(img_enc[img->pix_y + j][img->pix_x + block_x]),&(curr_mpr[j][block_x]), BLOCK_SIZE * sizeof(imgpel));
    }
  }

  //  Decoded block moved to frame memory
  return nonzero;
}



/*!
************************************************************************
* \brief
*    Process for lossless coding of coefficients.
*    The routine performs transform, quantization,inverse transform, 
*    adds the diff to the prediction and writes the result to the 
*    decoded luma frame. 
*
* \par Input:
*    currMB:          Current macroblock.
*    pl:              Color plane for 4:4:4 coding.
*    block_x,block_y: Block position inside a macro block (0,4,8,12).
*    intra:           Intra block indicator.
*
* \par Output_
*    nonzero:         0 if no levels are nonzero. \n
*                     1 if there are nonzero levels.\n
*    coeff_cost:      Coeff coding cost for thresholding consideration.\n
************************************************************************
*/
int dct_4x4_ls(Macroblock *currMB, ColorPlane pl, int block_x,int block_y,int *coeff_cost, int intra)
{
  int i,j, coeff_ctr;
  int run = -1;
  int nonzero = FALSE;  

  int   pos_x   = block_x >> BLOCK_SHIFT;
  int   pos_y   = block_y >> BLOCK_SHIFT;
  int   b8      = 2*(pos_y >> 1) + (pos_x >> 1) + (pl<<2);
  int   b4      = 2*(pos_y & 0x01) + (pos_x & 0x01);

  int*  ACL = &img->cofAC[b8][b4][0][0];
  int*  ACR = &img->cofAC[b8][b4][1][0];

  int   pix_y, pix_x;
  imgpel **img_enc       = enc_picture->p_curr_img;
  imgpel (*curr_mpr)[MB_BLOCK_SIZE] = img->mpr[pl];
  int    (*curr_res)[MB_BLOCK_SIZE] = img->m7[pl]; 
  int   *m7;

  const byte *p_scan = currMB->is_field_mode ? &FIELD_SCAN[0][0] : &SNGL_SCAN[0][0];

  // select scaling parameters
  fadjust4x4    = img->AdaptiveRounding ? &img->fadjust4x4[intra][block_y] : NULL;

  if((ipmode_DPCM < 2))
  {
    Residual_DPCM_4x4(ipmode_DPCM, curr_res, block_y, block_x);
  }

  for (coeff_ctr=0;coeff_ctr < 16;coeff_ctr++)
  {
    i = *p_scan++;
    j = *p_scan++;

    run++;
    
    m7 = &curr_res[j+block_y][i+block_x];    

    if (img->AdaptiveRounding)
      fadjust4x4[j][block_x+i] = 0;

    if (*m7 != 0)
    {
      if (params->symbol_mode == CAVLC)
        *m7 = iClip3(-CAVLC_LEVEL_LIMIT, CAVLC_LEVEL_LIMIT, *m7);

      nonzero=TRUE;
      *coeff_cost += MAX_VALUE;
      *ACL++ = *m7;
      *ACR++ = run;
      run=-1;                     // reset zero level counter        
    }
  }
  *ACL = 0;

  if(ipmode_DPCM<2)  //For residual DPCM
  {
    Inv_Residual_DPCM_4x4(curr_res, block_y, block_x);
  }

  for (j=0; j < BLOCK_SIZE; j++)
  {
    pix_y = img->pix_y + block_y + j;
    pix_x = img->pix_x+block_x;
    for (i=0; i < BLOCK_SIZE; i++)
    {
      img_enc[pix_y][pix_x+i] = curr_res[j+block_y][i+block_x] + curr_mpr[j+block_y][i+block_x];
    }
  }

  return nonzero;
}


/*!
************************************************************************
* \brief
*    Residual DPCM for Intra lossless coding
*
* \par Input:
*    block_x,block_y: Block position inside a macro block (0,4,8,12).
************************************************************************
*/
int Residual_DPCM_4x4(int ipmode, int m7[16][16], int block_y, int block_x)
{
  int i,j;
  int temp[4][4];

  if(ipmode==VERT_PRED)
  {

    for (i=1; i<4; i++) 
    {
      temp[i][0] = m7[block_y + i][block_x    ] - m7[block_y + i - 1][block_x    ];
      temp[i][1] = m7[block_y + i][block_x + 1] - m7[block_y + i - 1][block_x + 1];
      temp[i][2] = m7[block_y + i][block_x + 2] - m7[block_y + i - 1][block_x + 2];
      temp[i][3] = m7[block_y + i][block_x + 3] - m7[block_y + i - 1][block_x + 3];
    }

    for (i=1; i<4; i++)
    {
      m7[block_y+i][block_x    ] = temp[i][0];
      m7[block_y+i][block_x + 1] = temp[i][1];
      m7[block_y+i][block_x + 2] = temp[i][2];
      m7[block_y+i][block_x + 3] = temp[i][3];
    }
  }
  else  //HOR_PRED
  {
    for (i=0; i<4; i++)
      for (j=1; j<4; j++)
      {
        temp[i][1] = m7[block_y + i][block_x + 1] - m7[block_y + i][block_x    ];
        temp[i][2] = m7[block_y + i][block_x + 2] - m7[block_y + i][block_x + 1];
        temp[i][3] = m7[block_y + i][block_x + 3] - m7[block_y + i][block_x + 2];
      }

    for (i=0; i<4; i++)
    {
      m7[block_y+i][block_x + 1] = temp[i][1];
      m7[block_y+i][block_x + 2] = temp[i][2];
      m7[block_y+i][block_x + 3] = temp[i][3];
    }
  }
  return 0;
}

/*!
************************************************************************
* \brief
*    Inverse residual DPCM for Intra lossless coding
*
* \par Input:
*    block_x,block_y: Block position inside a macro block (0,4,8,12).
************************************************************************
*/
//For residual DPCM
int Inv_Residual_DPCM_4x4(int m7[16][16], int block_y, int block_x)  
{
  int i;
  int temp[4][4];

  if(ipmode_DPCM==VERT_PRED)
  {
    for(i=0; i<4; i++)
    {
      temp[0][i] = m7[block_y + 0][block_x + i];
      temp[1][i] = temp[0][i] + m7[block_y + 1][block_x + i];
      temp[2][i] = temp[1][i] + m7[block_y + 2][block_x + i];
      temp[3][i] = temp[2][i] + m7[block_y + 3][block_x + i];
    }
    for(i=0; i<4; i++)
    {      
      m7[block_y + i][block_x    ] = temp[i][0];
      m7[block_y + i][block_x + 1] = temp[i][1];
      m7[block_y + i][block_x + 2] = temp[i][2];
      m7[block_y + i][block_x + 3] = temp[i][3];
    }
  }
  else //HOR_PRED
  {
    for(i=0; i<4; i++)
    {
      temp[i][0] = m7[block_y+i][block_x+0];
      temp[i][1] = temp[i][0] + m7[block_y + i][block_x + 1];
      temp[i][2] = temp[i][1] + m7[block_y + i][block_x + 2];
      temp[i][3] = temp[i][2] + m7[block_y + i][block_x + 3];    
    }
    for(i=0; i<4; i++)
    {
      m7[block_y+i][block_x  ] = temp[i][0];
      m7[block_y+i][block_x+1] = temp[i][1];
      m7[block_y+i][block_x+2] = temp[i][2];
      m7[block_y+i][block_x+3] = temp[i][3];
    }
  }
  return 0;
}

/*!
************************************************************************
* \brief
*    Residual DPCM for Intra lossless coding
************************************************************************
*/
//For residual DPCM
int Residual_DPCM_4x4_for_Intra16x16(int m7[4][4], int ipmode)  
{
  int i,j;
  int temp[4][4];

  if(ipmode==VERT_PRED)
  {   
    for (i=1; i<4; i++) 
      for (j=0; j<4; j++)
        temp[i][j] = m7[i][j] - m7[i-1][j];

    for (i=1; i<4; i++)
      for (j=0; j<4; j++)
        m7[i][j] = temp[i][j];
  }
  else  //HOR_PRED
  {
    for (i=0; i<4; i++)
      for (j=1; j<4; j++)
        temp[i][j] = m7[i][j] - m7[i][j-1];
    
      for (i=0; i<4; i++)
        for (j=1; j<4; j++)
        m7[i][j] = temp[i][j];
  }
  return 0;
}
/*!
************************************************************************
* \brief
*    Inverse residual DPCM for Intra lossless coding
************************************************************************
*/
//For residual DPCM
int Inv_Residual_DPCM_4x4_for_Intra16x16(int m7[4][4], int ipmode)  
{
  int i;
  int temp[4][4];

  if(ipmode==VERT_PRED)
  {
    for (i=0; i<4; i++) 
    {
      temp[0][i] = m7[0][i];
      temp[1][i] = m7[1][i] + temp[0][i];
      temp[2][i] = m7[2][i] + temp[1][i];
      temp[3][i] = m7[3][i] + temp[2][i];
    }
    // These could now just use a memcpy
    for (i=0; i<4; i++)
    {
      m7[1][i] = temp[1][i];
      m7[2][i] = temp[2][i];
      m7[3][i] = temp[3][i];
    }
  }
  else  //HOR_PRED
  {
    for(i=0; i<4; i++)
    {
      temp[i][0] = m7[i][0];
      temp[i][1] = m7[i][1] + temp[i][0];
      temp[i][2] = m7[i][2] + temp[i][1];
      temp[i][3] = m7[i][3] + temp[i][2];
    }
    for (i=0; i<4; i++)
    {
      m7[i][1] = temp[i][1];
      m7[i][2] = temp[i][2];
      m7[i][3] = temp[i][3];
    }    
  }
  return 0;
}

/*!
 ************************************************************************
 * \brief
 *    Transform,quantization,inverse transform for chroma.
 *    The main reason why this is done in a separate routine is the
 *    additional 2x2 transform of DC-coeffs. This routine is called
 *    once for each of the chroma components.
 *
 * \par Input:
 *    uv    : Make difference between the U and V chroma component  \n
 *    cr_cbp: chroma coded block pattern
 *
 * \par Output:
 *    cr_cbp: Updated chroma coded block pattern.
 ************************************************************************
 */
int dct_chroma(Macroblock *currMB, int uv, int cr_cbp)
{
  int i,j,n2,n1,coeff_ctr,level ,scan_pos,run;
  static int m1[BLOCK_SIZE],m5[BLOCK_SIZE];
  int coeff_cost;
  int cr_cbp_tmp;
  int DCcoded=0 ;

  const byte *c_cost = COEFF_COST4x4[params->disthres];

  int   b4;
  int*  DCLevel = img->cofDC[uv+1][0];
  int*  DCRun   = img->cofDC[uv+1][1];
  int*  ACLevel;
  int*  ACRun;
  int   intra = IS_INTRA (currMB);
  int   uv_scale = uv * (img->num_blk8x8_uv >> 1);

  //FRExt
  static const int64 cbpblk_pattern[4]={0, 0xf0000, 0xff0000, 0xffff0000};
  int yuv = img->yuv_format;
  int b8;
  static int m3[4][4];
  int qp_per_dc = 0;
  int qp_rem_dc = 0;
  int q_bits_422 = 0;
  const byte (*pos_scan)[2] = currMB->is_field_mode ? FIELD_SCAN : SNGL_SCAN;
  int cur_qp = currMB->qpc[uv] + img->bitdepth_chroma_qp_scale;
  int cur_qp_dc = currMB->qpc[uv] + 3 + img->bitdepth_chroma_qp_scale;

  int qp_per = qp_per_matrix[cur_qp];
  int qp_rem = qp_rem_matrix[cur_qp];
  int q_bits = Q_BITS + qp_per + 1;
  int max_imgpel_value_uv = img->max_imgpel_value_comp[1];
  int    (*curr_res)[MB_BLOCK_SIZE] = img->m7[uv + 1]; 
  imgpel (*curr_mpr)[MB_BLOCK_SIZE] = img->mpr[uv + 1]; 
  int nonzero[4][4] = {{FALSE}};
  int nonezero = FALSE;

  levelscale    = LevelScale4x4Comp   [uv + 1][intra][qp_rem];
  leveloffset   = LevelOffset4x4Comp  [uv + 1][intra][cur_qp];
  invlevelscale = InvLevelScale4x4Comp[uv + 1][intra][qp_rem];
  fadjust4x4    = img->AdaptiveRounding ? img->fadjust4x4Cr[intra][uv] : NULL;

  //============= dct transform ===============
  for (n2=0; n2 < img->mb_cr_size_y; n2 += BLOCK_SIZE)
  {
    for (n1=0; n1 < img->mb_cr_size_x; n1 += BLOCK_SIZE)
    {
      forward4x4(curr_res, curr_res, n2, n1);
    }
  }

  if (yuv == YUV420)
  {
    //================== CHROMA DC YUV420 ===================
    //     2X2 transform of DC coeffs.
    run=-1;
    scan_pos=0;
    
    // forward 2x2 hadamard
    hadamard2x2(curr_res, m1);

    //     Quant of chroma 2X2 coeffs.
    for (coeff_ctr=0; coeff_ctr < 4; coeff_ctr++)
    {
      run++;

      level =(iabs(m1[coeff_ctr]) * levelscale[0][0] + (leveloffset[0][0]<<1)) >> q_bits;

      if (level  != 0)
      {
        if (params->symbol_mode == CAVLC && img->qp < 4)
          level = imin(level, CAVLC_LEVEL_LIMIT);

        currMB->cbp_blk |= 0xf0000 << (uv << 2) ;    // if one of the 2x2-DC levels is != 0 set the
        cr_cbp=imax(1,cr_cbp);                     // coded-bit all 4 4x4 blocks (bit 16-19 or 20-23)
        DCcoded = 1;
        level = isignab(level, m1[coeff_ctr]);
        DCLevel[scan_pos  ] = level;
        DCRun  [scan_pos++] = run;
        run=-1;          
        m1[coeff_ctr] = level;
      }
      else
        m1[coeff_ctr] = 0;
    }

    DCLevel[scan_pos] = 0;
    //  Inverse transform of 2x2 DC levels
    ihadamard2x2(m1, m5);

    curr_res[0][0] = ((m5[0] * invlevelscale[0][0]) << qp_per) >> 5;
    curr_res[0][4] = ((m5[1] * invlevelscale[0][0]) << qp_per) >> 5;
    curr_res[4][0] = ((m5[2] * invlevelscale[0][0]) << qp_per) >> 5;
    curr_res[4][4] = ((m5[3] * invlevelscale[0][0]) << qp_per) >> 5;
  }
  else if(yuv == YUV422)
  {
    //for YUV422 only
    qp_per_dc = qp_per_matrix[cur_qp_dc];
    qp_rem_dc = qp_rem_matrix[cur_qp_dc];

    invlevelscaleDC = InvLevelScale4x4Comp[uv + 1][intra][qp_rem_dc];
    levelscaleDC    = LevelScale4x4Comp   [uv + 1][intra][qp_rem_dc];
    leveloffsetDC   = LevelOffset4x4Comp  [uv + 1][intra][cur_qp_dc];

    q_bits_422 = Q_BITS + qp_per_dc + 1;

    //================== CHROMA DC YUV422 ===================
    //transform DC coeff
    //horizontal

    //pick out DC coeff    
    for (j=0; j < img->mb_cr_size_y; j+=BLOCK_SIZE)
    {
      for (i=0; i < img->mb_cr_size_x; i+=BLOCK_SIZE)
        m3[i>>2][j>>2]= curr_res[j][i];
    }
    // forward transform. Note that coeffs have been transposed (4x2 instead of 2x4) which makes transform a bit faster
    hadamard4x2(m3, M4);

    run=-1;
    scan_pos=0;

    //quant of chroma DC-coeffs
    for (coeff_ctr=0;coeff_ctr<8;coeff_ctr++)
    {
      i = SCAN_YUV422[coeff_ctr][0];
      j = SCAN_YUV422[coeff_ctr][1];

      run++;

      level =(iabs(M4[i][j]) * levelscaleDC[0][0] + (leveloffsetDC[0][0]<<1)) >> q_bits_422;

      if (level != 0)
      {
        //YUV422
        currMB->cbp_blk |= 0xff0000 << (uv << 3) ;   // if one of the DC levels is != 0 set the
        cr_cbp=imax(1,cr_cbp);                       // coded-bit all 4 4x4 blocks (bit 16-31 or 32-47) //YUV444
        DCcoded = 1 ;
        
        level = isignab(level,M4[i][j]);
        DCLevel[scan_pos  ] = level;
        DCRun  [scan_pos++] = run;
        run=-1;
        m3[i][j] = level;
      }
      else
        m3[i][j] = 0;     
    }
    DCLevel[scan_pos]=0;


    //inverse DC transform. Note that now M4 is transposed back
    ihadamard4x2(m3, M4);    

    // This code assumes sizeof(int) > 16. Therefore, no need to have conditional
    for (j = 0; j < 4; j++)
    {
      curr_res[j << 2 ][0] = rshift_rnd_sf(((M4[j][0]) * invlevelscaleDC[0][0]) << qp_per_dc, 6);
      curr_res[j << 2 ][4] = rshift_rnd_sf(((M4[j][1]) * invlevelscaleDC[0][0]) << qp_per_dc, 6);
    }
  }

  //     Quant of chroma AC-coeffs.
  coeff_cost=0;
  cr_cbp_tmp=0;

  for (b8=0; b8 < (img->num_blk8x8_uv >> 1); b8++)
  {
    for (b4=0; b4 < 4; b4++)
    {
      int64 uv_cbpblk = ((int64)1) << cbp_blk_chroma[b8 + uv_scale][b4];      
      n1 = hor_offset[yuv][b8][b4];
      n2 = ver_offset[yuv][b8][b4];
      ACLevel = img->cofAC[4 + b8 + uv_scale][b4][0];
      ACRun   = img->cofAC[4 + b8 + uv_scale][b4][1];
      run=-1;
      scan_pos=0;

      // Quantization process
      nonzero[n2>>2][n1>>2] = quant_ac4x4cr(&curr_res[n2], n2, n1, cur_qp, ACLevel, ACRun, &fadjust4x4[n2], 
        levelscale, invlevelscale, leveloffset, &coeff_cost, pos_scan, c_cost);

      if (nonzero[n2>>2][n1>>2])
      {
        currMB->cbp_blk |= uv_cbpblk;
        cr_cbp_tmp=2;
        nonezero = TRUE;
      }
    }
  }

  // Perform thresholding
  // * reset chroma coeffs
  if(nonezero && coeff_cost < _CHROMA_COEFF_COST_)
  {
    int64 uv_cbpblk = ((int64)cbpblk_pattern[yuv] << (uv << (1+yuv)));
    cr_cbp_tmp = 0;

    for (b8 = 0; b8 < (img->num_blk8x8_uv >> 1); b8++)
    {
      for (b4 = 0; b4 < 4; b4++)
      {
        n1 = hor_offset[yuv][b8][b4];
        n2 = ver_offset[yuv][b8][b4];
        if (nonzero[n2>>2][n1>>2] == TRUE)
        {
          nonzero[n2>>2][n1>>2] = FALSE;
          ACLevel = img->cofAC[4 + b8 + uv_scale][b4][0];
          ACRun   = img->cofAC[4 + b8 + uv_scale][b4][1];

          if (DCcoded == 0)
            currMB->cbp_blk &= ~(uv_cbpblk);  // if no chroma DC's: then reset coded-bits of this chroma subblock

          ACLevel[0] = 0;

          for (coeff_ctr=1; coeff_ctr < 16; coeff_ctr++)// ac coeff
          {
            curr_res[n2 + pos_scan[coeff_ctr][1]][n1 + pos_scan[coeff_ctr][0]] = 0;
            ACLevel[coeff_ctr]  = 0;
          }
        }
      }
    }
  }

  //     IDCT.
  //     Horizontal.
  if(cr_cbp_tmp == 2)
    cr_cbp = 2;

  nonezero = FALSE;
  for (n2=0; n2 < img->mb_cr_size_y; n2 += BLOCK_SIZE)
  {
    for (n1=0; n1 < img->mb_cr_size_x; n1 += BLOCK_SIZE)
    {
      if (curr_res[n2][n1] != 0 || nonzero[n2>>2][n1>>2] == TRUE)
      {
        inverse4x4(curr_res, curr_res, n2, n1);
        nonezero = TRUE;
      }
    }
  }

  //  Decoded block moved to memory
  if (nonezero == TRUE)
  {
    SampleReconstruct (enc_picture->imgUV[uv], curr_mpr, curr_res, 0, 0, img->pix_c_y, img->pix_c_x, img->mb_cr_size_x, img->mb_cr_size_y, max_imgpel_value_uv, DQ_BITS);
  }
  else
  {
    for (j=0; j < img->mb_cr_size_y; j++)
    {
      memcpy(&enc_picture->imgUV[uv][img->pix_c_y + j][img->pix_c_x], curr_mpr[j], img->mb_cr_size_x * sizeof(imgpel));
    }
  }

  return cr_cbp;
}


/*!
 ************************************************************************
 * \brief
 *    Transform,quantization,inverse transform for chroma.
 *    The main reason why this is done in a separate routine is the
 *    additional 2x2 transform of DC-coeffs. This routine is called
 *    once for each of the chroma components.
 *
 * \par Input:
 *    uv    : Make difference between the U and V chroma component  \n
 *    cr_cbp: chroma coded block pattern
 *
 * \par Output:
 *    cr_cbp: Updated chroma coded block pattern.
 ************************************************************************
 */
int dct_chroma_ls(Macroblock *currMB, int uv, int cr_cbp)
{
  int i,j,n2,n1,coeff_ctr,level ,scan_pos,run;
  static int m1[BLOCK_SIZE];
  int coeff_cost;
  int cr_cbp_tmp;
  int DCcoded=0 ;
  static imgpel *orig_img, *pred_img;

  int   b4;
  int*  DCLevel = img->cofDC[uv+1][0];
  int*  DCRun   = img->cofDC[uv+1][1];
  int*  ACLevel;
  int*  ACRun;
  int   intra = IS_INTRA (currMB);
  int   uv_scale = uv * (img->num_blk8x8_uv >> 1);

  //FRExt
  int yuv = img->yuv_format;
  int b8;
  static int *m7;
  static int m3[4][4];

  const byte (*pos_scan)[2] = currMB->is_field_mode ? FIELD_SCAN : SNGL_SCAN;

  int    (*curr_res)[MB_BLOCK_SIZE] = img->m7[uv + 1]; 
  imgpel (*curr_mpr)[MB_BLOCK_SIZE] = img->mpr[uv + 1]; 

  fadjust4x4    = img->AdaptiveRounding ? img->fadjust4x4Cr[intra][uv] : NULL;


  if (yuv == YUV420)
  {
    //================== CHROMA DC YUV420 ===================
    //     2X2 transform of DC coeffs.
    run=-1;
    scan_pos=0;    
    m1[0]=curr_res[0][0];
    m1[1]=curr_res[0][4];
    m1[2]=curr_res[4][0];
    m1[3]=curr_res[4][4];

    for (coeff_ctr=0; coeff_ctr < 4; coeff_ctr++)
    {
      run++;

      level =iabs(m1[coeff_ctr]);

      if (level  != 0)
      {
        if (params->symbol_mode == CAVLC && img->qp < 4)
          level = imin(level, CAVLC_LEVEL_LIMIT);

        currMB->cbp_blk |= 0xf0000 << (uv << 2) ;    // if one of the 2x2-DC levels is != 0 set the
        cr_cbp=imax(1, cr_cbp);                     // coded-bit all 4 4x4 blocks (bit 16-19 or 20-23)
        DCcoded = 1;
        level = isignab(level, m1[coeff_ctr]);
        DCLevel[scan_pos  ] = level;
        DCRun  [scan_pos++] = run;
        run=-1;
      }
    }
    DCLevel[scan_pos] = 0;    
  }
  else if(yuv == YUV422)
  {
    //================== CHROMA DC YUV422 ===================
    //transform DC coeff
    //horizontal

    //pick out DC coeff
    for (j=0; j < img->mb_cr_size_y; j+=BLOCK_SIZE)
    {
      for (i=0; i < img->mb_cr_size_x; i+=BLOCK_SIZE)
        m3[i>>2][j>>2]= curr_res[j][i];
    }


    run=-1;
    scan_pos=0;

    //quant of chroma DC-coeffs
    for (coeff_ctr=0;coeff_ctr<8;coeff_ctr++)
    {
      i=SCAN_YUV422[coeff_ctr][0];
      j=SCAN_YUV422[coeff_ctr][1];

      run++;

      level = iabs(m3[i][j]);
      M4[i][j]=m3[i][j];

      if (level != 0)
      {
        //YUV422
        currMB->cbp_blk |= 0xff0000 << (uv << 3) ;   // if one of the DC levels is != 0 set the
        cr_cbp=imax(1,cr_cbp);                       // coded-bit all 4 4x4 blocks (bit 16-31 or 32-47) //YUV444
        DCcoded = 1 ;

        DCLevel[scan_pos  ] = isignab(level,M4[i][j]);
        DCRun  [scan_pos++] = run;
        run=-1;
      }
    }
    DCLevel[scan_pos]=0;

    //inverse DC transform
    //horizontal    
  }

  //     Quant of chroma AC-coeffs.
  coeff_cost=0;
  cr_cbp_tmp=0;

  for (b8=0; b8 < (img->num_blk8x8_uv >> 1); b8++)
  {
    for (b4=0; b4 < 4; b4++)
    {
      int64 uv_cbpblk = ((int64)1) << cbp_blk_chroma[b8 + uv_scale][b4];
      n1 = hor_offset[yuv][b8][b4];
      n2 = ver_offset[yuv][b8][b4];
      ACLevel = img->cofAC[4 + b8 + uv_scale][b4][0];
      ACRun   = img->cofAC[4 + b8 + uv_scale][b4][1];
      run=-1;
      scan_pos=0;

      for (coeff_ctr=1; coeff_ctr < 16; coeff_ctr++)// start change rd_quant
      {
        i=pos_scan[coeff_ctr][0];
        j=pos_scan[coeff_ctr][1];

        ++run;

        level = iabs(curr_res[n2+j][n1+i]);

        if (img->AdaptiveRounding)
        {
          fadjust4x4[n2+j][n1+i] = 0;
        }

        if (level  != 0)
        {
          currMB->cbp_blk |= uv_cbpblk;
          coeff_cost += MAX_VALUE;                // set high cost, shall not be discarded

          cr_cbp_tmp=2;
          ACLevel[scan_pos  ] = isignab(level,curr_res[n2+j][n1+i]);
          ACRun  [scan_pos++] = run;
          run=-1;

          level=isignab(level, curr_res[n2+j][n1+i]);
        }
      }
      ACLevel[scan_pos] = 0;
    }
  }

  for (j=0; j < img->mb_cr_size_y; j++)
  {      
    orig_img = &enc_picture->imgUV[uv][img->pix_c_y + j][img->pix_c_x];
    m7 = curr_res[j];
    pred_img = curr_mpr[j];
    for (i=0; i < img->mb_cr_size_x; i++)
    {        
      orig_img[i] = (imgpel) m7[i] + pred_img[i];
    }
  }  

  return cr_cbp;
}

/*!
 ************************************************************************
 * \brief
 *    The routine performs transform,quantization,inverse transform, adds the diff.
 *    to the prediction and writes the result to the decoded luma frame. Includes the
 *    RD constrained quantization also.
 *
 * \par Input:
 *    block_x,block_y: Block position inside a macro block (0,4,8,12).
 *
 * \par Output:
 *    nonzero: 0 if no levels are nonzero.  1 if there are nonzero levels.              \n
 *    coeff_cost: Counter for nonzero coefficients, used to discard expensive levels.
 *
 *
 ************************************************************************
 */
int dct_4x4_sp(Macroblock *currMB, ColorPlane pl, int block_x,int block_y,int *coeff_cost, int intra)
{
  int i,j,coeff_ctr;
  int qp_const,ilev, level,scan_pos,run;
  int nonzero;

  imgpel **img_enc = enc_picture->p_curr_img;
  imgpel (*curr_mpr)[MB_BLOCK_SIZE] = img->mpr[pl];
  int    (*curr_res)[MB_BLOCK_SIZE] = img->m7[pl]; 
  int c_err,qp_const2;
  int qp_per,qp_rem, q_bits;
  int qp_per_sp,qp_rem_sp,q_bits_sp;
  const byte *c_cost = COEFF_COST4x4[params->disthres];
  const byte (*pos_scan)[2] = currMB->is_field_mode ? FIELD_SCAN : SNGL_SCAN;

  int   pos_x   = block_x >> BLOCK_SHIFT;
  int   pos_y   = block_y >> BLOCK_SHIFT;
  int   b8      = 2*(pos_y >> 1) + (pos_x >> 1);
  int   b4      = 2*(pos_y & 0x01) + (pos_x & 0x01);
  int*  ACLevel = img->cofAC[b8][b4][0];
  int*  ACRun   = img->cofAC[b8][b4][1];

  // For encoding optimization
  int c_err1, c_err2, level1, level2;
  double D_dis1, D_dis2;
  int len, info;
  double lambda_mode   = 0.85 * pow (2, (currMB->qp - SHIFT_QP)/3.0) * 4;

  qp_per    = qp_per_matrix[(currMB->qp - MIN_QP)];
  qp_rem    = qp_rem_matrix[(currMB->qp - MIN_QP)];
  q_bits    = Q_BITS + qp_per;
  qp_per_sp = qp_per_matrix[(currMB->qpsp - MIN_QP)];
  qp_rem_sp = qp_rem_matrix[(currMB->qpsp - MIN_QP)];
  q_bits_sp = Q_BITS + qp_per_sp;

  qp_const  = (1<<q_bits)/6;    // inter
  qp_const2 = (1<<q_bits_sp)/2;  //sp_pred

  //  Horizontal transform
  for (j=0; j< BLOCK_SIZE; j++)
    for (i=0; i< BLOCK_SIZE; i++)
    {      
      curr_res[j][i]+=curr_mpr[j+block_y][i+block_x];
      M1[j][i]=curr_mpr[j+block_y][i+block_x];
    }

    
  forward4x4(curr_res, curr_res, 0, 0);
  forward4x4(M1, M1, 0, 0);

  // Quant
  nonzero=FALSE;

  run=-1;
  scan_pos=0;

  for (coeff_ctr=0;coeff_ctr < 16;coeff_ctr++)     // 8 times if double scan, 16 normal scan
  {

    i = pos_scan[coeff_ctr][0];
    j = pos_scan[coeff_ctr][1];

    run++;
    ilev=0;

    // decide prediction

    // case 1
    level1 = (iabs (M1[j][i]) * quant_coef[qp_rem_sp][j][i] + qp_const2) >> q_bits_sp;
    level1 = (level1 << q_bits_sp) / quant_coef[qp_rem_sp][j][i];
    c_err1 = curr_res[j][i]-isignab(level1, M1[j][i]);
    level1 = (iabs (c_err1) * quant_coef[qp_rem][j][i] + qp_const) >> q_bits;

    // case 2
    c_err2=curr_res[j][i]-M1[j][i];
    level2 = (iabs (c_err2) * quant_coef[qp_rem][j][i] + qp_const) >> q_bits;

    // select prediction
    if ((level1 != level2) && (level1 != 0) && (level2 != 0))
    {
      D_dis1 = curr_res[j][i] - ((isignab(level1,c_err1)*dequant_coef[qp_rem][j][i]*A[j][i]<< qp_per) >>6) - M1[j][i];
      levrun_linfo_inter(level1, run, &len, &info);
      D_dis1 = D_dis1*D_dis1 + lambda_mode * len;

      D_dis2 = curr_res[j][i] - ((isignab(level2,c_err2)*dequant_coef[qp_rem][j][i]*A[j][i]<< qp_per) >>6) - M1[j][i];
      levrun_linfo_inter(level2, run, &len, &info);
      D_dis2 = D_dis2 * D_dis2 + lambda_mode * len;

      if (D_dis1 == D_dis2)
        level = (iabs(level1) < iabs(level2)) ? level1 : level2;
      else
      {
        if (D_dis1 < D_dis2)
          level = level1;
        else
          level = level2;
      }
      c_err = (level == level1) ? c_err1 : c_err2;
    }
    else if (level1 == level2)
    {
      level = level1;
      c_err = c_err1;
    }
    else
    {
      level = (level1 == 0) ? level1 : level2;
      c_err = (level1 == 0) ? c_err1 : c_err2;
    }

    if (level != 0)
    {
      nonzero = TRUE;
            
      *coeff_cost += (level > 1) ? MAX_VALUE : c_cost[run];

      level = isignab(level,c_err);
      ACLevel[scan_pos] = level;
      ACRun  [scan_pos] = run;
      ++scan_pos;
      run=-1;                     // reset zero level counter
      ilev=((level * dequant_coef[qp_rem][j][i] * A[j][i] << qp_per) >>6);
    }
    ilev+=M1[j][i];
    
    if(!si_frame_indicator && !sp2_frame_indicator)//stores the SP frame coefficients in lrec, will be useful to encode these and create SI or SP switching frame
    {
      lrec[img->pix_y+block_y+j][img->pix_x+block_x+i]=
        isignab((iabs(ilev) * quant_coef[qp_rem_sp][j][i] + qp_const2) >> q_bits_sp, ilev);
    }    
    curr_res[j][i] = isignab((iabs(ilev) * quant_coef[qp_rem_sp][j][i] + qp_const2)>> q_bits_sp, ilev) * dequant_coef[qp_rem_sp][j][i] << qp_per_sp;
  }
  ACLevel[scan_pos] = 0;

  // inverse transform
  inverse4x4(curr_res, curr_res, 0, 0);

  for (j=0; j < BLOCK_SIZE; j++)
    for (i=0; i < BLOCK_SIZE; i++)
      curr_res[j][i] = iClip1 (img->max_imgpel_value,rshift_rnd_sf(curr_res[j][i], DQ_BITS));

  //  Decoded block moved to frame memory

  for (j=0; j < BLOCK_SIZE; j++)
  for (i=0; i < BLOCK_SIZE; i++)
    img_enc[img->pix_y+block_y+j][img->pix_x+block_x+i]= (imgpel) curr_res[j][i];

  return nonzero;
}

/*!
 ************************************************************************
 * \brief
 *    Transform,quantization,inverse transform for chroma.
 *    The main reason why this is done in a separate routine is the
 *    additional 2x2 transform of DC-coeffs. This routine is called
 *    once for each of the chroma components.
 *
 * \par Input:
 *    uv    : Make difference between the U and V chroma component               \n
 *    cr_cbp: chroma coded block pattern
 *
 * \par Output:
 *    cr_cbp: Updated chroma coded block pattern.
 ************************************************************************
 */
int dct_chroma_sp(Macroblock *currMB, int uv,int cr_cbp)
{
  int i,j,ilev,n2,n1,coeff_ctr,qp_const,c_err,level ,scan_pos,run;
  int m1[BLOCK_SIZE];
  int coeff_cost;
  int cr_cbp_tmp;
  int qp_const2,mp1[BLOCK_SIZE];
  const byte *c_cost = COEFF_COST4x4[params->disthres];
  const byte (*pos_scan)[2] = currMB->is_field_mode ? FIELD_SCAN : SNGL_SCAN;

  int qp_per, qp_rem, q_bits;
  int qp_per_sp, qp_rem_sp, q_bits_sp;

  int   b4;
  int*  DCLevel = img->cofDC[uv+1][0];
  int*  DCRun   = img->cofDC[uv+1][1];
  int*  ACLevel;
  int*  ACRun;

  int c_err1, c_err2, level1, level2;
  int len, info;
  double D_dis1, D_dis2;
  double lambda_mode   = 0.85 * pow (2, (currMB->qp -SHIFT_QP)/3.0) * 4;
  int max_imgpel_value_uv = img->max_imgpel_value_comp[1];


  int qpChroma = iClip3(-img->bitdepth_chroma_qp_scale, 51, currMB->qp + active_pps->chroma_qp_index_offset);
  int qpChromaSP=iClip3(-img->bitdepth_chroma_qp_scale, 51, currMB->qpsp + active_pps->chroma_qp_index_offset);
  int    (*curr_res)[MB_BLOCK_SIZE] = img->m7[uv + 1]; 
  imgpel (*curr_mpr)[MB_BLOCK_SIZE] = img->mpr[uv + 1]; 

  qp_per    = ((qpChroma<0?qpChroma:QP_SCALE_CR[qpChroma])-MIN_QP)/6;
  qp_rem    = ((qpChroma<0?qpChroma:QP_SCALE_CR[qpChroma])-MIN_QP)%6;
  q_bits    = Q_BITS+qp_per;
  qp_const=(1<<q_bits)/6;    // inter
  qp_per_sp    = ((qpChromaSP<0?currMB->qpsp:QP_SCALE_CR[qpChromaSP])-MIN_QP)/6;
  qp_rem_sp    = ((qpChromaSP<0?currMB->qpsp:QP_SCALE_CR[qpChromaSP])-MIN_QP)%6;
  q_bits_sp    = Q_BITS+qp_per_sp;
  qp_const2 = (1<<q_bits_sp)/2;  //sp_pred


  for (j=0; j < img->mb_cr_size_y; j++)
  {
    for (i=0; i < img->mb_cr_size_x; i++)
    {
      curr_res[j][i] += curr_mpr[j][i];
      M1[j][i] = curr_mpr[j][i];
    }
  }
  
  for (n2=0; n2 < img->mb_cr_size_y; n2 += BLOCK_SIZE)
  {
    for (n1=0; n1 < img->mb_cr_size_x; n1 += BLOCK_SIZE)
    {
      forward4x4(curr_res, curr_res, n2, n1);      
      forward4x4(M1, M1, n2, n1);
    }
  }

  //     2X2 transform of DC coeffs.
  hadamard2x2(curr_res, m1);
  hadamard2x2(M1, mp1);

  run=-1;
  scan_pos=0;

  for (coeff_ctr=0; coeff_ctr < 4; coeff_ctr++)
  {
    run++;
    ilev=0;

    // case 1
    c_err1 = (iabs (mp1[coeff_ctr]) * quant_coef[qp_rem_sp][0][0] + 2 * qp_const2) >> (q_bits_sp + 1);
    c_err1 = (c_err1 << (q_bits_sp + 1)) / quant_coef[qp_rem_sp][0][0];
    c_err1 = m1[coeff_ctr] - isignab(c_err1, mp1[coeff_ctr]);
    level1 = (iabs(c_err1) * quant_coef[qp_rem][0][0] + 2 * qp_const) >> (q_bits+1);

    // case 2
    c_err2 = m1[coeff_ctr] - mp1[coeff_ctr];
    level2 = (iabs(c_err2) * quant_coef[qp_rem][0][0] + 2 * qp_const) >> (q_bits+1);

    if (level1 != level2 && level1 != 0 && level2 != 0)
    {
      D_dis1 = m1[coeff_ctr] - ((isignab(level1,c_err1)*dequant_coef[qp_rem][0][0]*A[0][0]<< qp_per) >>5)- mp1[coeff_ctr];
      levrun_linfo_c2x2(level1, run, &len, &info);
      D_dis1 = D_dis1 * D_dis1 + lambda_mode * len;

      D_dis2 = m1[coeff_ctr] - ((isignab(level2,c_err2)*dequant_coef[qp_rem][0][0]*A[0][0]<< qp_per) >>5)- mp1[coeff_ctr];
      levrun_linfo_c2x2(level2, run, &len, &info);
      D_dis2 = D_dis2 * D_dis2 + lambda_mode * len;

      if (D_dis1 == D_dis2)
        level = (iabs(level1) < iabs(level2)) ? level1 : level2;
      else
      {
        if (D_dis1 < D_dis2)
          level = level1;
        else
          level = level2;
      }
      c_err = (level == level1) ? c_err1 : c_err2;
    }
    else if (level1 == level2)
    {
      level = level1;
      c_err = c_err1;
    }
    else
    {
      level = (level1 == 0) ? level1 : level2;
      c_err = (level1 == 0) ? c_err1 : c_err2;
    }

    if (level  != 0)
    {
      if (params->symbol_mode == CAVLC && img->qp < 4)
        level = imin(level, CAVLC_LEVEL_LIMIT);
      
      currMB->cbp_blk |= 0xf0000 << (uv << 2) ;  // if one of the 2x2-DC levels is != 0 the coded-bit
      cr_cbp=imax(1,cr_cbp);
      DCLevel[scan_pos  ] = isignab(level ,c_err);
      DCRun  [scan_pos++] = run;
      run=-1;
      ilev=((isignab(level,c_err)*dequant_coef[qp_rem][0][0]*A[0][0]<< qp_per) >>5);
    }
    ilev+= mp1[coeff_ctr];
    m1[coeff_ctr]=isignab((iabs(ilev)  * quant_coef[qp_rem_sp][0][0] + 2 * qp_const2) >> (q_bits_sp+1), ilev) * dequant_coef[qp_rem_sp][0][0] << qp_per_sp;
    if(!si_frame_indicator && !sp2_frame_indicator)
      lrec_uv[uv][img->pix_c_y+4*(coeff_ctr%2)][img->pix_c_x+4*(coeff_ctr/2)]=isignab((iabs(ilev)  * quant_coef[qp_rem_sp][0][0] + 2 * qp_const2) >> (q_bits_sp+1), ilev);// stores the SP frames coefficients, will be useful to encode SI or switching SP frame
  }
  DCLevel[scan_pos] = 0;

  //  Inverse transform of 2x2 DC levels
  ihadamard2x2(m1, m1);

  curr_res[0][0]=(m1[0])>>1;
  curr_res[0][4]=(m1[1])>>1;
  curr_res[4][0]=(m1[2])>>1;
  curr_res[4][4]=(m1[3])>>1;

  //     Quant of chroma AC-coeffs.
  coeff_cost=0;
  cr_cbp_tmp=0;

  for (n2=0; n2 <= BLOCK_SIZE; n2 += BLOCK_SIZE)
  {
    for (n1=0; n1 <= BLOCK_SIZE; n1 += BLOCK_SIZE)
    {
      b4      = 2*(n2 >> 2) + (n1 >> 2);
      ACLevel = img->cofAC[uv+4][b4][0];
      ACRun   = img->cofAC[uv+4][b4][1];

      run      = -1;
      scan_pos =  0;

      for (coeff_ctr=1; coeff_ctr < 16; coeff_ctr++)// start change rd_quant
      {
        i=pos_scan[coeff_ctr][0];
        j=pos_scan[coeff_ctr][1];

        ++run;
        ilev=0;

        // quantization on prediction
        c_err1 = (iabs(M1[n2+j][n1+i]) * quant_coef[qp_rem_sp][j][i] + qp_const2) >> q_bits_sp;
        c_err1 = (c_err1 << q_bits_sp) / quant_coef[qp_rem_sp][j][i];
        c_err1 = curr_res[n2+j][n1+i] - isignab(c_err1, M1[n2+j][n1+i]);
        level1 = (iabs(c_err1) * quant_coef[qp_rem][j][i] + qp_const) >> q_bits;

        // no quantization on prediction
        c_err2 = curr_res[n2+j][n1+i] - M1[n2+j][n1+i];
        level2 = (iabs(c_err2) * quant_coef[qp_rem][j][i] + qp_const) >> q_bits;

        if (level1 != level2 && level1 != 0 && level2 != 0)
        {
          D_dis1 = curr_res[n2+j][n1+i] - ((isignab(level1,c_err1)*dequant_coef[qp_rem][j][i]*A[j][i]<< qp_per) >>6) - M1[n2+j][n1+i];

          levrun_linfo_inter(level1, run, &len, &info);
          D_dis1 = D_dis1 * D_dis1 + lambda_mode * len;

          D_dis2 = curr_res[n2+j][n1+i] - ((isignab(level2,c_err2)*dequant_coef[qp_rem][j][i]*A[j][i]<< qp_per) >>6) - M1[n2+j][n1+i];
          levrun_linfo_inter(level2, run, &len, &info);
          D_dis2 = D_dis2 * D_dis2 + lambda_mode * len;

          if (D_dis1 == D_dis2)
            level = (iabs(level1) < iabs(level2)) ? level1 : level2;
          else
          {
            if (D_dis1 < D_dis2)
              level = level1;
            else
              level = level2;
          }
          c_err = (level == level1) ? c_err1 : c_err2;
        }
        else if (level1 == level2)
        {
          level = level1;
          c_err = c_err1;
        }
        else
        {
          level = (level1 == 0) ? level1 : level2;
          c_err = (level1 == 0) ? c_err1 : c_err2;
        }

        if (level  != 0)
        {
          currMB->cbp_blk |=  (int64)1 << (16 + (uv << 2) + ((n2 >> 1) + (n1 >> 2))) ;

          coeff_cost += (level > 1) ? MAX_VALUE : c_cost[run];  // set high cost, shall not be discarded

          cr_cbp_tmp=2;
          level = isignab(level,c_err);
          ACLevel[scan_pos] = level;
          ACRun  [scan_pos] = run;
          ++scan_pos;
          run=-1;
          ilev=((level * dequant_coef[qp_rem][j][i]*A[j][i]<< qp_per) >>6);
        }
        ilev+=M1[n2+j][n1+i];
        if(!si_frame_indicator && !sp2_frame_indicator)
          if(!( (n2+j) % 4==0 && (n1+i)%4 ==0 ))
            lrec_uv[uv][img->pix_c_y+n1+j][img->pix_c_x+n2+i]=isignab((iabs(ilev) * quant_coef[qp_rem_sp][j][i] + qp_const2) >> q_bits_sp,ilev);//stores the SP frames coefficients, will be useful to encode SI or switching SP frame
        curr_res[n2+j][n1+i] = isignab((iabs(ilev) * quant_coef[qp_rem_sp][j][i] + qp_const2) >> q_bits_sp,ilev) * dequant_coef[qp_rem_sp][j][i] << qp_per_sp;
      }
      ACLevel[scan_pos] = 0;
    }
  }

  // * reset chroma coeffs

  if(cr_cbp_tmp==2)
    cr_cbp=2;
  //     IDCT.

      //     Horizontal.
  for (n2=0; n2 <= BLOCK_SIZE; n2 += BLOCK_SIZE)
  {
    for (n1=0; n1 <= BLOCK_SIZE; n1 += BLOCK_SIZE)
    {
      inverse4x4(curr_res, curr_res, n2, n1);

      for (j=0; j < BLOCK_SIZE; j++)
      for (i=0; i < BLOCK_SIZE; i++)
      {
        curr_res[n2+j][n1+i] = iClip1 (max_imgpel_value_uv,rshift_rnd_sf(curr_res[n2+j][n1+i], DQ_BITS));
      }
    }
  }

  //  Decoded block moved to memory
  for (j=0; j < BLOCK_SIZE*2; j++)
    for (i=0; i < BLOCK_SIZE*2; i++)
    {
      enc_picture->imgUV[uv][img->pix_c_y+j][img->pix_c_x+i]= (imgpel) curr_res[j][i];
    }

  return cr_cbp;
}

/*!
 ************************************************************************
 * \brief
 *    The routine performs transform,quantization,inverse transform, adds the diff.
 *    to the prediction and writes the result to the decoded luma frame. Includes the
 *    RD constrained quantization also.
 *
 * \par Input:
 *    block_x,block_y: Block position inside a macro block (0,4,8,12).
 *
 * \par Output:
 *    nonzero: 0 if no levels are nonzero.  1 if there are nonzero levels.            \n
 *    coeff_cost: Counter for nonzero coefficients, used to discard expensive levels.
 ************************************************************************
 */
void copyblock_sp(Macroblock *currMB, ColorPlane pl, int block_x,int block_y)
{
  int i,j,i1,j1,m5[4],m6[4];

  int qp_per = (currMB->qpsp-MIN_QP)/6;
  int qp_rem = (currMB->qpsp-MIN_QP)%6;
  int q_bits    = Q_BITS+qp_per;
  int qp_const2=(1<<q_bits)/2;  //sp_pred
  imgpel **img_enc       = enc_picture->p_curr_img;
  imgpel (*curr_mpr)[MB_BLOCK_SIZE] = img->mpr[pl]; 
  int    (*curr_res)[MB_BLOCK_SIZE] = img->m7[pl]; 
  

  //  Horizontal transform
  for (j=0; j< BLOCK_SIZE; j++)
    for (i=0; i< BLOCK_SIZE; i++)
    {
      M1[i][j]=curr_mpr[j+block_y][i+block_x];
    }

  for (j=0; j < BLOCK_SIZE; j++)
  {
    for (i=0; i < 2; i++)
    {
      i1=3-i;
      m5[i]=M1[i][j]+M1[i1][j];
      m5[i1]=M1[i][j]-M1[i1][j];
    }
    M1[0][j]=(m5[0]+m5[1]);
    M1[2][j]=(m5[0]-m5[1]);
    M1[1][j]=m5[3]*2+m5[2];
    M1[3][j]=m5[3]-m5[2]*2;
  }

  //  Vertical transform

  for (i=0; i < BLOCK_SIZE; i++)
  {
    for (j=0; j < 2; j++)
    {
      j1=3-j;
      m5[j]=M1[i][j]+M1[i][j1];
      m5[j1]=M1[i][j]-M1[i][j1];
    }
    M1[i][0]=(m5[0]+m5[1]);
    M1[i][2]=(m5[0]-m5[1]);
    M1[i][1]=m5[3]*2+m5[2];
    M1[i][3]=m5[3]-m5[2]*2;
  }

  // Quant
  for (j=0;j < BLOCK_SIZE; j++)
  {
    for (i=0; i < BLOCK_SIZE; i++)
    {
      curr_res[j][i]=isignab((iabs(M1[i][j])* quant_coef[qp_rem][i][j]+qp_const2)>> q_bits,M1[i][j])*dequant_coef[qp_rem][i][j]<<qp_per;
      if(!si_frame_indicator && !sp2_frame_indicator)
      {
        lrec[img->pix_y+block_y+j][img->pix_x+block_x+i] =
        isignab((iabs(M1[i][j]) * quant_coef[qp_rem][i][j] + qp_const2) >> q_bits, M1[i][j]);// stores the SP frames coefficients, will be useful to encode SI or switching SP frame
      }
    }
  }

  //     IDCT.
  //     horizontal

  for (j=0;j<BLOCK_SIZE;j++)
  {
    for (i=0;i<BLOCK_SIZE;i++)
    {
      m5[i]=curr_res[j][i];
    }
    m6[0]=(m5[0]+m5[2]);
    m6[1]=(m5[0]-m5[2]);
    m6[2]=(m5[1]>>1)-m5[3];
    m6[3]=m5[1]+(m5[3]>>1);

    curr_res[j][0]=m6[0]+m6[3];
    curr_res[j][1]=m6[1]+m6[2];
    curr_res[j][2]=m6[1]-m6[2];
    curr_res[j][3]=m6[0]-m6[3];
  }
  // vertical
  for (i=0;i<BLOCK_SIZE;i++)
  {
    for (j=0;j<BLOCK_SIZE;j++)
      m5[j]=curr_res[j][i];

    m6[0]=(m5[0]+m5[2]);
    m6[1]=(m5[0]-m5[2]);
    m6[2]=(m5[1]>>1)-m5[3];
    m6[3]=m5[1]+(m5[3]>>1);

    curr_res[0][i] = iClip1 (img->max_imgpel_value,rshift_rnd_sf(m6[0]+m6[3], DQ_BITS));
    curr_res[1][i] = iClip1 (img->max_imgpel_value,rshift_rnd_sf(m6[1]+m6[2], DQ_BITS));
    curr_res[2][i] = iClip1 (img->max_imgpel_value,rshift_rnd_sf(m6[1]-m6[2], DQ_BITS));    
    curr_res[3][i] = iClip1 (img->max_imgpel_value,rshift_rnd_sf(m6[0]-m6[3], DQ_BITS));
  }

  //  Decoded block moved to frame memory

  for (j=0; j < BLOCK_SIZE; j++)
    for (i=0; i < BLOCK_SIZE; i++)
      img_enc[img->pix_y+block_y+j][img->pix_x+block_x+i]=(imgpel) curr_res[j][i];
}



int writeIPCMBytes(Bitstream *currStream)
{
  int i,j, jj;
  int len = 0, uv;
  SyntaxElement   se;

  for (j=0;j<16;j++)
  {
    jj = img->pix_y+j;
    for (i=0;i<16;i++)
    {
      se.len = img->bitdepth_luma;
      len   += se.len;
      se.bitpattern = enc_picture->imgY[jj][img->pix_x+i];
      writeSyntaxElement2Buf_Fixed(&se, currStream);
    }
  }

  for (uv = 0; uv < 2; uv ++)
  {
    for (j=0;j<img->mb_cr_size_y;j++)
    {
      jj = img->pix_c_y+j;
      for (i=0;i<img->mb_cr_size_x;i++)
      {
        se.len = img->bitdepth_chroma;
        len += se.len;
        se.bitpattern = enc_picture->imgUV[uv][jj][img->pix_c_x+i];
        writeSyntaxElement2Buf_Fixed(&se, currStream);
      }
    }
  }
  return len;
}

int writePCMByteAlign(Bitstream *currStream)
{
  int len = 0;
  if (currStream->bits_to_go < 8)
  { // trailing bits to process
    len = 8 - currStream->bits_to_go;
    currStream->byte_buf = (currStream->byte_buf <<currStream->bits_to_go) | (0xff >> (8 - currStream->bits_to_go));
    stats->bit_use_stuffingBits[img->type]+=currStream->bits_to_go;
    currStream->streamBuffer[currStream->byte_pos++]=currStream->byte_buf;
    currStream->bits_to_go = 8;
  }
  return len;
}

/*!
 ************************************************************************
 * \brief Eric Setton
 * Encoding of a secondary SP / SI frame.
 * For an SI frame the predicted block should only come from spatial pred.
 * The original image signal is the error coefficients of a primary SP in the raw data stream
 * the difference with the primary SP are :
 *  - the prediction signal is transformed and quantized (qpsp) but not dequantized
 *  - only one kind of prediction is considered and not two
 *  - the resulting error coefficients are not quantized before being sent to the VLC
 *
 * \par Input:
 *    block_x,block_y: Block position inside a macro block (0,4,8,12).
 *
 * \par Output:
 *    nonzero: 0 if no levels are nonzero.  1 if there are nonzero levels.
 *    coeff_cost: Counter for nonzero coefficients, used to discard expensive levels.
 *
 *
 ************************************************************************
 */

int dct_4x4_sp2(Macroblock *currMB, ColorPlane pl, int block_x,int block_y,int *coeff_cost, int intra)
{
  int i,j,ilev,coeff_ctr;
  int qp_const,level,scan_pos,run;
  int nonzero;

  imgpel **img_enc = enc_picture->p_curr_img;
  imgpel (*curr_mpr)[MB_BLOCK_SIZE] = img->mpr[pl];   
  int    (*curr_res)[MB_BLOCK_SIZE] = img->m7[pl];   
  int c_err,qp_const2;
  int qp_per,qp_rem,q_bits;
  int qp_per_sp,qp_rem_sp,q_bits_sp;

  int   pos_x   = block_x >> BLOCK_SHIFT;
  int   pos_y   = block_y >> BLOCK_SHIFT;
  int   b8      = 2*(pos_y >> 1) + (pos_x >> 1);
  int   b4      = 2*(pos_y & 0x01) + (pos_x & 0x01);
  int*  ACLevel = img->cofAC[b8][b4][0];
  int*  ACRun   = img->cofAC[b8][b4][1];
  const byte *c_cost = COEFF_COST4x4[params->disthres];
  const byte (*pos_scan)[2] = currMB->is_field_mode ? FIELD_SCAN : SNGL_SCAN;

  int level1;

  qp_per    = (img->qpsp-MIN_QP)/6 ;
  qp_rem    = (img->qpsp-MIN_QP)%6;
  q_bits    = Q_BITS+qp_per;
  qp_per_sp    = (img->qpsp-MIN_QP)/6;
  qp_rem_sp    = (img->qpsp-MIN_QP)%6;
  q_bits_sp    = Q_BITS+qp_per_sp;

  qp_const=(1<<q_bits)/6;    // inter
  qp_const2=(1<<q_bits_sp)/2;  //sp_pred

  for (j=0; j< BLOCK_SIZE; j++)
    for (i=0; i< BLOCK_SIZE; i++)
    {
      //Coefficients obtained from the prior encoding of the SP frame
      curr_res[j][i]=lrec[img->pix_y+block_y+j][img->pix_x+block_x+i];
      //Predicted block
      M1[j][i]=curr_mpr[j+block_y][i+block_x];
    }

  // forward transform
  forward4x4(M1, M1, 0, 0);

  // Quant
  nonzero=FALSE;

  run=-1;
  scan_pos=0;

  for (coeff_ctr=0;coeff_ctr < 16;coeff_ctr++)     // 8 times if double scan, 16 normal scan
  {

    i=pos_scan[coeff_ctr][0];
    j=pos_scan[coeff_ctr][1];

    run++;
    ilev=0;

    //quantization of the predicted block
    level1 = (iabs (M1[j][i]) * quant_coef[qp_rem_sp][j][i] + qp_const2) >> q_bits_sp;
    //substracted from lrec
    c_err = curr_res[j][i]-isignab(level1, M1[j][i]);   //substracting the predicted block

    level = iabs(c_err);
    if (level != 0)
    {
      nonzero=TRUE;

      *coeff_cost += (level > 1) ? MAX_VALUE : c_cost[run];

      ACLevel[scan_pos] = isignab(level,c_err);
      ACRun  [scan_pos] = run;
      ++scan_pos;
      run=-1;                     // reset zero level counter
    }
    //from now on we are in decoder land
    ilev=c_err + isignab(level1,M1[j][i]) ;  // adding the quantized predicted block
    curr_res[j][i] = ilev * dequant_coef[qp_rem_sp][j][i] << qp_per_sp;

  }
  ACLevel[scan_pos] = 0;

  //  Inverse transform
  inverse4x4(curr_res, curr_res, 0, 0);

  for (j=0; j < BLOCK_SIZE; j++)
    for (i=0; i < BLOCK_SIZE; i++)
      img_enc[img->pix_y+block_y+j][img->pix_x+block_x+i] =iClip3 (0, img->max_imgpel_value,rshift_rnd_sf(curr_res[j][i], DQ_BITS));

  return nonzero;
}


/*!
 ************************************************************************
 * \brief Eric Setton
 * Encoding of the chroma of a  secondary SP / SI frame.
 * For an SI frame the predicted block should only come from spatial pred.
 * The original image signal is the error coefficients of a primary SP in the raw data stream
 * the difference with the primary SP are :
 *  - the prediction signal is transformed and quantized (qpsp) but not dequantized
 *  - the resulting error coefficients are not quantized before being sent to the VLC
 *
 * \par Input:
 *    uv    : Make difference between the U and V chroma component
 *    cr_cbp: chroma coded block pattern
 *
 * \par Output:
 *    cr_cbp: Updated chroma coded block pattern.
 *
 ************************************************************************
 */
int dct_chroma_sp2(Macroblock *currMB, int uv,int cr_cbp)
{
  int i,j,ilev,n2,n1,coeff_ctr,qp_const,c_err,level ,scan_pos,run;
  int m1[BLOCK_SIZE];
  int coeff_cost;
  int cr_cbp_tmp;
  int qp_const2,mp1[BLOCK_SIZE];
  const byte *c_cost = COEFF_COST4x4[params->disthres];
  const byte (*pos_scan)[2] = currMB->is_field_mode ? FIELD_SCAN : SNGL_SCAN;

  int qp_per,qp_rem,q_bits;
  int qp_per_sp,qp_rem_sp,q_bits_sp;

  int   b4;
  int*  DCLevel = img->cofDC[uv+1][0];
  int*  DCRun   = img->cofDC[uv+1][1];
  int*  ACLevel;
  int*  ACRun;
  int  level1;
  int    (*curr_res)[MB_BLOCK_SIZE] = img->m7[uv + 1]; 
  imgpel (*curr_mpr)[MB_BLOCK_SIZE] = img->mpr[uv + 1]; 

  qp_per    = ((img->qp<0?img->qp:QP_SCALE_CR[img->qp])-MIN_QP)/6;
  qp_rem    = ((img->qp<0?img->qp:QP_SCALE_CR[img->qp])-MIN_QP)%6;
  q_bits    = Q_BITS+qp_per;
  qp_const=(1<<q_bits)/6;    // inter

  qp_per_sp    = ((img->qpsp<0?img->qpsp:QP_SCALE_CR[img->qpsp])-MIN_QP)/6;
  qp_rem_sp    = ((img->qpsp<0?img->qpsp:QP_SCALE_CR[img->qpsp])-MIN_QP)%6;
  q_bits_sp    = Q_BITS+qp_per_sp;
  qp_const2    = (1<<q_bits_sp)/2;  //sp_pred

  for (j=0; j < MB_BLOCK_SIZE>>1; j++)
    for (i=0; i < MB_BLOCK_SIZE>>1; i++)
    {
      M1[j][i]=curr_mpr[j][i];
      curr_res[j][i]=lrec_uv[uv][img->pix_c_y+j][img->pix_c_x+i];
    }

    
  for (n2=0; n2 <= BLOCK_SIZE; n2 += BLOCK_SIZE)
  {
    for (n1=0; n1 <= BLOCK_SIZE; n1 += BLOCK_SIZE)
    {
      forward4x4(M1, M1, n2, n1);
    }
  }

  //   DC coefficients already transformed and quantized
  m1[0]= curr_res[0][0];
  m1[1]= curr_res[0][4];
  m1[2]= curr_res[4][0];
  m1[3]= curr_res[4][4];

  //     2X2 transform of predicted DC coeffs.
  hadamard2x2(M1, mp1);

  run=-1;
  scan_pos=0;

  for (coeff_ctr=0; coeff_ctr < 4; coeff_ctr++)
  {
    run++;
    ilev=0;

    //quantization of predicted DC coeff
    level1 = (iabs (mp1[coeff_ctr]) * quant_coef[qp_rem_sp][0][0] + 2 * qp_const2) >> (q_bits_sp + 1);
    //substratcted from lrecUV
    c_err = m1[coeff_ctr] - isignab(level1, mp1[coeff_ctr]);
    level = iabs(c_err);

    if (level  != 0)
    {
      currMB->cbp_blk |= 0xf0000 << (uv << 2) ;  // if one of the 2x2-DC levels is != 0 the coded-bit
      cr_cbp=imax(1,cr_cbp);
      DCLevel[scan_pos] = isignab(level ,c_err);
      DCRun  [scan_pos] = run;
      scan_pos++;
      run=-1;
    }

    //from now on decoder world
    ilev = c_err + isignab(level1,mp1[coeff_ctr]) ; // we have perfect reconstruction here

    m1[coeff_ctr]= ilev  * dequant_coef[qp_rem_sp][0][0] << qp_per_sp;

  }
  DCLevel[scan_pos] = 0;

  //  Invers transform of 2x2 DC levels
  ihadamard2x2(m1, m1);

  curr_res[0][0]=m1[0]/2;
  curr_res[0][4]=m1[1]/2;
  curr_res[4][0]=m1[2]/2;
  curr_res[4][4]=m1[3]/2;

  //     Quant of chroma AC-coeffs.
  coeff_cost=0;
  cr_cbp_tmp=0;

  for (n2=0; n2 <= BLOCK_SIZE; n2 += BLOCK_SIZE)
  {
    for (n1=0; n1 <= BLOCK_SIZE; n1 += BLOCK_SIZE)
    {
      b4      = 2*(n2/4) + (n1/4);
      ACLevel = img->cofAC[uv+4][b4][0];
      ACRun   = img->cofAC[uv+4][b4][1];

      run      = -1;
      scan_pos =  0;

      for (coeff_ctr=1; coeff_ctr < 16; coeff_ctr++)// start change rd_quant
      {

        i=pos_scan[coeff_ctr][0];
        j=pos_scan[coeff_ctr][1];

        ++run;
        ilev=0;
        // quantization on prediction
        level1 = (iabs(M1[n2+j][n1+i]) * quant_coef[qp_rem_sp][j][i] + qp_const2) >> q_bits_sp;
        //substracted from lrec
        c_err  = curr_res[n2+j][n1+i] - isignab(level1, M1[n2+j][n1+i]);
        level  = iabs(c_err) ;

        if (level  != 0)
        {
          currMB->cbp_blk |=  (int64)1 << (16 + (uv << 2) + ((n2 >> 1) + (n1 >> 2))) ;

          coeff_cost += (level > 1) ? MAX_VALUE : c_cost[run];

          cr_cbp_tmp=2;
          ACLevel[scan_pos] = isignab(level,c_err);
          ACRun  [scan_pos] = run;
          ++scan_pos;
          run=-1;
        }

        //from now on decoder land
        ilev=c_err + isignab(level1,M1[n2+j][n1+i]);
        curr_res[n2+j][n1+i] = ilev * dequant_coef[qp_rem_sp][j][i] << qp_per_sp;
      }
      ACLevel[scan_pos] = 0;
    }
  }
  // * reset chroma coeffs

  if(cr_cbp_tmp==2)
      cr_cbp=2;
  //     IDCT.

      //     Horizontal.
  for (n2=0; n2 <= BLOCK_SIZE; n2 += BLOCK_SIZE)
  {
    for (n1=0; n1 <= BLOCK_SIZE; n1 += BLOCK_SIZE)
    {
      inverse4x4(curr_res, curr_res, n2, n1);

      //     Vertical.
      for (j=0; j < BLOCK_SIZE; j++)
      {
        for (i=0; i < BLOCK_SIZE; i++)
        {
          enc_picture->imgUV[uv][img->pix_c_y+j+n2][img->pix_c_x+i +n1 ] = iClip3 (0, img->max_imgpel_value,rshift_rnd_sf(curr_res[n2+j][n1+i], DQ_BITS));
        }
      }
    }
  }

  return cr_cbp;
}


void select_dct(Macroblock *currMB)
{
  if (img->type!=SP_SLICE)
  {
    if (img->lossless_qpprime_flag == 1)
    {
      if (currMB->qp_scaled[img->colour_plane_id] == 0)
      {
        pDCT_4x4   = dct_4x4_ls;
        pDCT_16x16 = dct_16x16_ls;
        pDCT_8x8   = dct_8x8_ls;
      }
      else
      {
        pDCT_4x4   = dct_4x4;
        pDCT_16x16 = dct_16x16;
        if (params->symbol_mode == CAVLC)
          pDCT_8x8   = dct_8x8_cavlc;
        else
          pDCT_8x8   = dct_8x8;
        dct_cr_4x4[0] = dct_chroma;
        dct_cr_4x4[1] = dct_chroma;
      }
      
      if (currMB->qp_scaled[1] == 0)
      {
         dct_cr_4x4[0] = dct_chroma_ls;
      }
      if (currMB->qp_scaled[2] == 0)
      {
         dct_cr_4x4[1] = dct_chroma_ls;
      }
    }
    else
    {
      pDCT_4x4   = dct_4x4;
      pDCT_16x16 = dct_16x16;
      if (params->symbol_mode == CAVLC)
        pDCT_8x8   = dct_8x8_cavlc;
      else
        pDCT_8x8   = dct_8x8;

      dct_cr_4x4[0] = dct_chroma;
      dct_cr_4x4[1] = dct_chroma;
    }
  }
  else if(!si_frame_indicator && !sp2_frame_indicator)
  {
    pDCT_4x4 = dct_4x4_sp;
    pDCT_16x16 = dct_16x16;
    if (params->symbol_mode == CAVLC)
      pDCT_8x8   = dct_8x8_cavlc;
    else
      pDCT_8x8   = dct_8x8;

    dct_cr_4x4[0]  = dct_chroma_sp;
    dct_cr_4x4[1]  = dct_chroma_sp;
  }
  else
  {
    pDCT_4x4 = dct_4x4_sp2;
    pDCT_16x16 = dct_16x16;
    if (params->symbol_mode == CAVLC)
      pDCT_8x8   = dct_8x8_cavlc;
    else
      pDCT_8x8   = dct_8x8;

    dct_cr_4x4[0]  = dct_chroma_sp2;
    dct_cr_4x4[1]  = dct_chroma_sp2;
  }
}
