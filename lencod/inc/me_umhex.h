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
 *
 * \file me_umhex.h
 *
 * \brief
 *   Macro definitions and global variables for UMHEX fast
 *   integer pel motion estimation and fractional pel motion estimation
 *
 * \author
 *   Main contributors: (see contributors.h for copyright, address and affiliation details)
 *    - Zhibo Chen         <chenzhibo@tsinghua.org.cn>
 *    - JianFeng Xu        <fenax@video.mdc.tsinghua.edu.cn>
 *    - Wenfang Fu         <fwf@video.mdc.tsinghua.edu.cn>
 *    - Xiaozhong Xu       <xxz@video.mdc.tsinghua.edu.cn>
 *
 * \date
 *   2006.1
 ************************************************************************
 */

#ifndef _ME_UMHEX_H_
#define _ME_UMHEX_H_

#include "mbuffer.h"

#define EARLY_TERMINATION                                                             \
  if ((min_mcost-pred_SAD)<pred_SAD*betaFourth_2)                                     \
  goto fourth_2_step;                                                                 \
  else if((min_mcost-pred_SAD)<pred_SAD*betaFourth_1)                                 \
  goto fourth_1_step;

#define SEARCH_ONE_PIXEL                                                              \
  if(iabs(cand_x - center_x) <=search_range && iabs(cand_y - center_y)<= search_range)\
  {                                                                                   \
    if(!McostState[cand_y-center_y+search_range][cand_x-center_x+search_range])       \
    {                                                                                 \
      mcost = MV_COST (lambda_factor, mvshift, cand_x, cand_y, pred_x, pred_y);       \
      if(mcost<min_mcost)                                                             \
      {                                                                               \
        mcost += computeUniPred[dist_method](orig_pic,                                \
        blocksize_y,blocksize_x, min_mcost - mcost,                                   \
        (cand_x << 2) + IMG_PAD_SIZE_TIMES4, (cand_y << 2) + IMG_PAD_SIZE_TIMES4);    \
        McostState[cand_y-center_y+search_range][cand_x-center_x+search_range] = 1;   \
        if (mcost < min_mcost)                                                        \
        {                                                                             \
          best_x = cand_x;                                                            \
          best_y = cand_y;                                                            \
          min_mcost = mcost;                                                          \
        }                                                                             \
      }                                                                               \
    }                                                                                 \
   }

#define SEARCH_ONE_PIXEL_BIPRED                                                       \
if(iabs(cand_x - center2_x) <=search_range && iabs(cand_y - center2_y)<= search_range)\
{                                                                                     \
  if(!McostState[cand_y-center2_y+search_range][cand_x-center2_x+search_range])       \
  {                                                                                   \
    mcost  = MV_COST (lambda_factor, mvshift, center1_x, center1_y, pred_x1, pred_y1);\
    mcost += MV_COST (lambda_factor, mvshift, cand_x, cand_y, pred_x2, pred_y2);      \
  if(mcost<min_mcost)                                                                 \
  {                                                                                   \
      mcost  += computeBiPred(cur_pic, blocksize_y, blocksize_x,                      \
      min_mcost - mcost,                                                              \
      (center1_x << 2) + IMG_PAD_SIZE_TIMES4,                                         \
      (center1_y << 2) + IMG_PAD_SIZE_TIMES4,                                         \
      (cand_x << 2) + IMG_PAD_SIZE_TIMES4,                                            \
      (cand_y << 2) + IMG_PAD_SIZE_TIMES4);                                           \
      McostState[cand_y-center2_y+search_range][cand_x-center2_x+search_range] = 1;   \
      if (mcost < min_mcost)                                                          \
      {                                                                               \
        best_x = cand_x;                                                              \
        best_y = cand_y;                                                              \
        min_mcost = mcost;                                                            \
      }                                                                               \
    }                                                                                   \
  }                                                                                   \
}

byte **McostState;                          //!< state for integer pel search
byte **SearchState;                         //!< state for fractional pel search

int ****fastme_ref_cost;                    //!< store SAD information needed for forward ref-frame prediction
int ***fastme_l0_cost;                      //!< store SAD information needed for forward median and uplayer prediction
int ***fastme_l1_cost;                      //!< store SAD information needed for backward median and uplayer prediction
int ***fastme_l0_cost_bipred;               //!< store SAD information for bipred mode
int ***fastme_l1_cost_bipred;               //!< store SAD information for bipred mode
int bipred_flag;                            //!< flag for bipred
int **fastme_best_cost;                     //!< for multi ref early termination threshold
int pred_SAD;                               //!<  SAD prediction in use.
int pred_MV_ref[2], pred_MV_uplayer[2];     //!< pred motion vector by space or temporal correlation,Median is provided

int UMHEX_blocktype;                        //!< blocktype for UMHEX SetMotionVectorPredictor
int predict_point[5][2];
int SAD_a,SAD_b,SAD_c,SAD_d;
int Threshold_DSR_MB[8];                    //!<  Threshold for usage of DSR. DSR refer to JVT-Q088
//for early termination
float  Bsize[8];
float AlphaFourth_1[8];
float AlphaFourth_2[8];
byte *flag_intra;
int  flag_intra_SAD;

void UMHEX_DefineThreshold(void);
void UMHEX_DefineThresholdMB(void);
int  UMHEX_get_mem(void);
void UMHEX_free_mem(void);

void UMHEX_decide_intrabk_SAD(void);
void UMHEX_skip_intrabk_SAD(int best_mode, int ref_max);
void UMHEX_setup(short ref, int list, int block_y, int block_x, int blocktype, short   ******all_mv);

int                                     //  ==> minimum motion cost after search
UMHEXIntegerPelBlockMotionSearch  (Macroblock *currMB,     // <--  current Macroblock
                                  imgpel   *orig_pic,      // <--  not used
                                  short     ref,           // <--  reference frame (0... or -1 (backward))
                                  int       list,          // <--  reference picture list
                                  int       list_offset,   // <--  MBAFF list offset
                                  char   ***refPic,        // <--  reference array
                                  short ****tmp_mv,        // <--  mv array
                                  int       pic_pix_x,     // <--  absolute x-coordinate of regarded AxB block
                                  int       pic_pix_y,     // <--  absolute y-coordinate of regarded AxB block
                                  int       blocktype,     // <--  block type (1-16x16 ... 7-4x4)
                                  short     pred_mv[2],    // <--  motion vector predictor (x) in sub-pel units
                                  short     mv[2],         //  --> motion vector (x) - in pel units
                                  int       search_range,  // <--  1-d search range in pel units
                                  int       min_mcost,     // <--  minimum motion cost (cost for center or huge value)
                                  int       lambda_factor, // <--  lagrangian parameter for determining motion cost
                                  int       apply_weights
                                  );

int                                                   //  ==> minimum motion cost after search
UMHEXSubPelBlockMotionSearch (
                             imgpel*   orig_pic,      // <--  original pixel values for the AxB block
                             short       ref,         // <--  reference frame (0... or -1 (backward))
                             int       list,          // <--  reference picture list
                             int       list_offset,   // <--  MBAFF list offset
                             int       pic_pix_x,     // <--  absolute x-coordinate of regarded AxB block
                             int       pic_pix_y,     // <--  absolute y-coordinate of regarded AxB block
                             int       blocktype,     // <--  block type (1-16x16 ... 7-4x4)
                             short     pred_mv[2],    // <--  motion vector predictor (x) in sub-pel units
                             short     mv[2],         // <--> in: search center (x) / out: motion vector (x) - in pel units
                             int       search_pos2,   // <--  search positions for    half-pel search  (default: 9)
                             int       search_pos4,   // <--  search positions for quarter-pel search  (default: 9)
                             int       min_mcost,     // <--  minimum motion cost (cost for center or huge value)
                             int       lambda_factor, // <--  lagrangian parameter for determining motion cost
                             int       apply_weights
                             );

extern int UMHEXSubPelBlockME ( //  ==> minimum motion cost after search
                             imgpel*   orig_pic,      // <--  original pixel values for the AxB block
                             short       ref,         // <--  reference frame (0... or -1 (backward))
                             int       list,          // <--  reference picture list
                             int       list_offset,   // <--  MBAFF list offset
                             int       pic_pix_x,     // <--  absolute x-coordinate of regarded AxB block
                             int       pic_pix_y,     // <--  absolute y-coordinate of regarded AxB block
                             int       blocktype,     // <--  block type (1-16x16 ... 7-4x4)
                             short     pred_mv[2],    // <--  motion vector predictor (x) in sub-pel units
                             short     mv[2],         // <--> in: search center (x) / out: motion vector (x) - in pel units
                             int       search_pos2,   // <--  search positions for    half-pel search  (default: 9)
                             int       search_pos4,   // <--  search positions for quarter-pel search  (default: 9)
                             int       min_mcost,     // <--  minimum motion cost (cost for center or huge value)
                             int*      lambda, // <--  lagrangian parameter for determining motion cost
                             int       apply_weights
                             );


extern int                                         //  ==> minimum motion cost after search
SubPelBlockMotionSearch (imgpel*   orig_pic,       // <--  original pixel values for the AxB block
                         short     ref,            // <--  reference frame (0... or -1 (backward))
                         int       list,
                         int       list_offset,   // <--  MBAFF list offset
                         int       pic_pix_x,      // <--  absolute x-coordinate of regarded AxB block
                         int       pic_pix_y,      // <--  absolute y-coordinate of regarded AxB block
                         int       blocktype,      // <--  block type (1-16x16 ... 7-4x4)
                         short     pred_mv[2],     // <--  motion vector predictor in sub-pel units
                         short     mv[2],          // <--> in: search center  / out: motion vector - in pel units
                         int       search_pos2,    // <--  search positions for    half-pel search  (default: 9)
                         int       search_pos4,    // <--  search positions for quarter-pel search  (default: 9)
                         int       min_mcost,      // <--  minimum motion cost (cost for center or huge value)
                         int       *lambda_factor, // <--  lagrangian parameter for determining motion cost
                         int       apply_weights);

int                                                //  ==> minimum motion cost after search
UMHEXBipredIntegerPelBlockMotionSearch (Macroblock *currMB, // <--  current Macroblock
                                       imgpel*   orig_pic,      // <--  original pixel values for the AxB block
                                       short     ref,           // <--  reference frame (0... or -1 (backward))
                                       int       list,          // <--  Current reference list
                                       int       list_offset,   // <--  MBAFF list offset
                                       char   ***refPic,        // <--  reference array
                                       short ****tmp_mv,        // <--  mv array
                                       int       pic_pix_x,     // <--  absolute x-coordinate of regarded AxB block
                                       int       pic_pix_y,     // <--  absolute y-coordinate of regarded AxB block
                                       int       blocktype,     // <--  block type (1-16x16 ... 7-4x4)
                                       short     pred_mv1[2],   // <--  motion vector predictor (x|y) in sub-pel units
                                       short     pred_mv2[2],   // <--  motion vector predictor (x|y) in sub-pel units
                                       short     mv[2],         // <--> in: search center (x) / out: motion vector (x) - in pel units
                                       short     s_mv[2],       // <--> in: search center (x) / out: motion vector (x) - in pel units
                                       int       search_range,  // <--  1-d search range in pel units
                                       int       min_mcost,     // <--  minimum motion cost (cost for center or huge value)
                                       int       lambda_factor, // <--  lagrangian parameter for determining motion cost
                                       int       apply_weights
                                       );

void UMHEXSetMotionVectorPredictor (Macroblock *currMB, short pmv[2], char **refPic, short ***tmp_mv,
                                    short  ref_frame, int list, int mb_x, int mb_y,
                                    int blockshape_x, int blockshape_y, int *search_range);

#endif
