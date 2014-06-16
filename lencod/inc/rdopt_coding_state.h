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
 *    rdopt_coding_state.h
 *
 * \author
 *    Heiko Schwarz
 *
 * \date
 *    17. April 2001
 *
 * \brief
 *    Headerfile for storing/restoring coding state
 *    (for rd-optimized mode decision)
 **************************************************************************
 */

#ifndef _RD_OPT_CS_H_
#define _RD_OPT_CS_H_

typedef struct {

  // important variables of data partition array
  int                   no_part;
  EncodingEnvironment  *encenv;
  Bitstream            *bitstream;

  // contexts for binary arithmetic coding
  int                   symbol_mode;
  MotionInfoContexts   *mot_ctx;
  TextureInfoContexts  *tex_ctx;

  // bit counter
  int                   bitcounter[MAX_BITCOUNTER_MB];

  // elements of current macroblock
  short                 mvd[2][BLOCK_MULTIPLE][BLOCK_MULTIPLE][2];
  int64                 cbp_bits[3];
  int64                 cbp_bits_8x8[3];
} CSobj;
typedef CSobj* CSptr;


void  delete_coding_state  (CSptr);  //!< delete structure
CSptr create_coding_state  (void);       //!< create structure

void  store_coding_state   (Macroblock *currMB, CSptr);  //!< store parameters
void  reset_coding_state   (Macroblock *currMB, CSptr);  //!< restore parameters


#endif

