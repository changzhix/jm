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
 * \file
 *    ratectl.h
 *
 * \author
 *    Zhengguo LI
 *
 * \date
 *    14 Jan 2003
 *
 * \brief
 *    Headerfile for rate control
 **************************************************************************
 */

#ifndef _RATE_CTL_H_
#define _RATE_CTL_H_

#include "rc_quadratic.h"

/* generic rate control variables */
typedef struct {
  // RC flags
  int   TopFieldFlag;
  int   FieldControl;
  int   FieldFrame;
  int   NoGranularFieldRC;
  // bits stats
  int   NumberofHeaderBits;
  int   NumberofTextureBits;
  int   NumberofBasicUnitHeaderBits;
  int   NumberofBasicUnitTextureBits;
  // frame stats
  int   NumberofGOP;
  int   NumberofCodedBFrame;  
  // MAD stats
  int64 TotalMADBasicUnit;
  int   *MADofMB;
  // buffer and budget
  int64 CurrentBufferFullness; //LIZG 25/10/2002
  int64 RemainingBits;
  // bit allocations for RC_MODE_3
  int   RCPSliceBits;
  int   RCISliceBits;
  int   RCBSliceBits[RC_MAX_TEMPORAL_LEVELS];
  int   temporal_levels;
  int   hierNb[RC_MAX_TEMPORAL_LEVELS];
  int   NPSlice;
  int   NISlice;
} rc_generic;

// macroblock activity
int    diffy[16][16];
int    qp_mbaff[2][2],       qp_mbaff[2][2];
int    delta_qp_mbaff[2][2], delta_qp_mbaff[2][2];

// generic functions
int    Qstep2QP          ( double Qstep );
double QP2Qstep          ( int QP );
int    ComputeMBMAD      ( void );
double ComputeFrameMAD   ( void );
void   rc_store_mad      (Macroblock *currMB);
void   update_qp_cbp     (Macroblock *currMB, short best_mode);
void   update_qp_cbp_tmp (Macroblock *currMB, int cbp, int best_mode);

// rate control functions
// init/copy
void  rc_alloc_generic           ( rc_generic **prc );
void  rc_free_generic            ( rc_generic **prc );
void  rc_copy_generic            ( rc_generic *dst, rc_generic *src );
void  rc_init_gop_params         (void);
void  rc_init_frame              (int FrameNumberInFile);
void  rc_init_sequence           (void);
void  rc_store_slice_header_bits (int len);


// rate control CURRENT pointers
rc_generic   *generic_RC;
// rate control object pointers for RDPictureDecision buffering...
rc_generic   *generic_RC_init, *generic_RC_best;


#endif

