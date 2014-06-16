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
 *
 * \file me_umhexsmp.h
 *
 * \brief
 *   Fast integer pixel and sub pixel motion estimation
 *   Improved and simplified from the original UMHexagonS algorithms
 *   See JVT-P021 for details
 *
 * \author
 *    Main contributors: (see contributors.h for copyright, address and affiliation details)
 *    - Zhibo Chen                      <chenzhibo@tsinghua.org.cn>
 *    - JianFeng Xu                     <fenax@video.mdc.tsinghua.edu.cn>
 *    - Wenfang Fu                      <fwf@video.mdc.tsinghua.edu.cn>
 *
 *    - Xiaoquan Yi                     <xyi@engr.scu.edu>
 *    - Jun Zhang                       <jzhang2@engr.scu.edu>
 *
 * \date
 *    6. Nov. 2006
 *************************************************************************************
 */

#ifndef _ME_UMHEXSMP_H_
#define _ME_UMHEXSMP_H_

#include "mbuffer.h"

unsigned short  SymmetricalCrossSearchThreshold1;
unsigned short  SymmetricalCrossSearchThreshold2;
unsigned short  ConvergeThreshold;
unsigned short  SubPelThreshold1;
unsigned short  SubPelThreshold3;

byte  **smpUMHEX_SearchState;          //state for fractional pel search
int  ***smpUMHEX_l0_cost;       //store SAD information needed for forward median and uplayer prediction
int  ***smpUMHEX_l1_cost;       //store SAD information needed for backward median and uplayer prediction
byte   *smpUMHEX_flag_intra;
int     smpUMHEX_flag_intra_SAD;

int     smpUMHEX_pred_SAD_uplayer;     // Up layer SAD prediction
short   smpUMHEX_pred_MV_uplayer_X;    // Up layer MV predictor X-component
short   smpUMHEX_pred_MV_uplayer_Y;    // Up layer MV predictor Y-component

void    smpUMHEX_init(void);
int     smpUMHEX_get_mem(void);
void    smpUMHEX_free_mem(void);
void    smpUMHEX_decide_intrabk_SAD(void);
void    smpUMHEX_skip_intrabk_SAD(int, int);
void    smpUMHEX_setup(short, int, int, int, int, short ******);

int                                     //  ==> minimum motion cost after search
smpUMHEXIntegerPelBlockMotionSearch (Macroblock *currMB,      // <--  current Macroblock
                                     imgpel   *orig_pic,      // <--  not used
                                     short     ref,           // <--  reference frame (0... or -1 (backward))
                                     int       list,          // <--  reference picture list
                                     int       list_offset,   // <--  MBAFF list offset
                                     char   ***refPic,        // <--  reference array
                                     short ****tmp_mv,        // <--  mv array
                                     int       pic_pix_x,     // <--  absolute x-coordinate of regarded AxB block
                                     int       pic_pix_y,     // <--  absolute y-coordinate of regarded AxB block
                                     int       blocktype,     // <--  block type (1-16x16 ... 7-4x4)
                                     short     pred_mv[2],    // <--  motion vector predictor (x|y) in sub-pel units
                                     short     mv[2],         //  --> motion vector (x|y) - in pel units
                                     int       search_range,  // <--  1-d search range in pel units
                                     int       min_mcost,     // <--  minimum motion cost (cost for center or huge value)
                                     int       lambda_factor, // <--  lagrangian parameter for determining motion cost
                                     int       apply_weights
                                     );

int                                     //  ==> minimum motion cost after search
smpUMHEXSubPelBlockMotionSearch  (
               imgpel* orig_pic,        // <--  original pixel values for the AxB block
               short     ref,           // <--  reference frame (0... or -1 (backward))
               int       list,          // <--  reference picture list
               int       list_offset,   // <--  MBAFF list offset
               int       pic_pix_x,     // <--  absolute x-coordinate of regarded AxB block
               int       pic_pix_y,     // <--  absolute y-coordinate of regarded AxB block
               int       blocktype,     // <--  block type (1-16x16 ... 7-4x4)
               short     pred_mv[2],    // <--  motion vector predictor (x|y) in sub-pel units
               short     mv[2],         // <--> in: search center (x|y) / out: motion vector (x|y) - in pel units
               int       search_pos2,   // <--  search positions for    half-pel search  (default: 9)
               int       search_pos4,   // <--  search positions for quarter-pel search  (default: 9)
               int       min_mcost,     // <--  minimum motion cost (cost for center or huge value)
               int       lambda_factor, // <--  lagrangian parameter for determining motion cost
               int       apply_weights
               );

int                                               //  ==> minimum motion cost after search
smpUMHEXFullSubPelBlockMotionSearch (imgpel*   orig_pic,      // <--  original pixel values for the AxB block
                         short     ref,           // <--  reference frame (0... or -1 (backward))
                         int       list,          // <--  reference picture list
                         int       list_offset,   // <--  MBAFF list offset
                         int       pic_pix_x,     // <--  absolute x-coordinate of regarded AxB block
                         int       pic_pix_y,     // <--  absolute y-coordinate of regarded AxB block
                         int       blocktype,     // <--  block type (1-16x16 ... 7-4x4)
                         short     pred_mv[2],    // <--  motion vector predictor (x|y) in sub-pel units
                         short     mv[2],         // <--> in: search center (x|y) / out: motion vector (x|y) - in pel units
                         int       search_pos2,   // <--  search positions for    half-pel search  (default: 9)
                         int       search_pos4,   // <--  search positions for quarter-pel search  (default: 9)
                         int       min_mcost,     // <--  minimum motion cost (cost for center or huge value)
                         int       lambda_factor, // <--  lagrangian parameter for determining motion cost
                         int       apply_weights
                         );
int                                                                 //  ==> minimum motion cost after search
smpUMHEXBipredIntegerPelBlockMotionSearch (Macroblock *currMB,      // <--  current Macroblock
                                           imgpel* cur_pic,         // <--  original pixel values for the AxB block
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
                                           short     mv[2],         // <--> in: search center (x|y) / out: motion vector (x|y) - in pel units
                                           short     s_mv[2],       // <--> in: search center (x|y) / out: motion vector (x|y) - in pel units
                                           int       search_range,  // <--  1-d search range in pel units
                                           int       min_mcost,     // <--  minimum motion cost (cost for center or huge value)
                                           int       lambda_factor, // <--  lagrangian parameter for determining motion cost
                                           int       apply_weights
                                           );


int                                                   //  ==> minimum motion cost after search
smpUMHEXSubPelBlockME (imgpel*   orig_pic,      // <--  original pixel values for the AxB block
                    short     ref,           // <--  reference frame (0... or -1 (backward))
                    int       list,
                    int       list_offset,   // <--  MBAFF list offset
                    int       pic_pix_x,     // <--  absolute x-coordinate of regarded AxB block
                    int       pic_pix_y,     // <--  absolute y-coordinate of regarded AxB block
                    int       blocktype,     // <--  block type (1-16x16 ... 7-4x4)
                    short     pred_mv[2],    // <--  motion vector predictor (x|y) in sub-pel units
                    short     mv[2],         // <--> in: search center (x|y) / out: motion vector (x|y) - in sub-pel units
                    int       search_pos2,   // <--  search positions for    half-pel search  (default: 9)
                    int       search_pos4,   // <--  search positions for quarter-pel search  (default: 9)
                    int       min_mcost,     // <--  minimum motion cost (cost for center or huge value)
                    int*      lambda,
                    int       apply_weights
                    );

#endif
