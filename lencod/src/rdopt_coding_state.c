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
 * \file rdopt_coding_state.c
 *
 * \brief
 *    Storing/restoring coding state for
 *    Rate-Distortion optimized mode decision
 *
 * \author
 *    Heiko Schwarz
 *
 * \date
 *    17. April 2001
 **************************************************************************/

#include "global.h"

#include "rdopt_coding_state.h"
#include "cabac.h"

/*!
 ************************************************************************
 * \brief
 *    delete structure for storing coding state
 ************************************************************************
 */
void delete_coding_state (CSptr cs)
{
  if (cs != NULL)
  {
    //=== structures of data partition array ===
    if (cs->encenv    != NULL)   free (cs->encenv);
    if (cs->bitstream != NULL)   free (cs->bitstream);

    //=== contexts for binary arithmetic coding ===
    delete_contexts_MotionInfo  (cs->mot_ctx);
    delete_contexts_TextureInfo (cs->tex_ctx);

    //=== coding state structure ===
    free (cs);
    cs=NULL;
  }
}


/*!
 ************************************************************************
 * \brief
 *    create structure for storing coding state
 ************************************************************************
 */
CSptr create_coding_state ()
{
  CSptr cs;

  //=== coding state structure ===
  if ((cs = (CSptr) calloc (1, sizeof(CSobj))) == NULL)
    no_mem_exit("init_coding_state: cs");

  //=== important variables of data partition array ===
  cs->no_part = params->partition_mode==0?1:3;
  if (params->symbol_mode == CABAC)
  {
    if ((cs->encenv = (EncodingEnvironment*) calloc (cs->no_part, sizeof(EncodingEnvironment))) == NULL)
      no_mem_exit("init_coding_state: cs->encenv");
  }
  else
  {
    cs->encenv = NULL;
  }
  if ((cs->bitstream = (Bitstream*) calloc (cs->no_part, sizeof(Bitstream))) == NULL)
    no_mem_exit("init_coding_state: cs->bitstream");

  //=== context for binary arithmetic coding ===
  cs->symbol_mode = params->symbol_mode;
  if (cs->symbol_mode == CABAC)
  {
    cs->mot_ctx = create_contexts_MotionInfo ();
    cs->tex_ctx = create_contexts_TextureInfo();
  }
  else
  {
    cs->mot_ctx = NULL;
    cs->tex_ctx = NULL;
  }

  return cs;
}


/*!
 ************************************************************************
 * \brief
 *    store coding state (for rd-optimized mode decision)
 ************************************************************************
 */
void store_coding_state (Macroblock *currMB, CSptr cs)
{
  int  i;
  int  i_last = img->currentPicture->idr_flag? 1:cs->no_part;
  Slice *currSlice = img->currentSlice;

  if (!params->rdopt)  return;

  if (cs->symbol_mode==CABAC)
  {
    //=== important variables of data partition array ===
    //only one partition for IDR img
    for (i = 0; i < i_last; i++)
    {
      cs->encenv[i] = currSlice->partArr[i].ee_cabac;
      cs->bitstream[i] = *currSlice->partArr[i].bitstream;
    }

    //=== contexts for binary arithmetic coding ===
    *cs->mot_ctx = *currSlice->mot_ctx;
    *cs->tex_ctx = *currSlice->tex_ctx;
  }
  else
  {
    //=== important variables of data partition array ===
    for (i = 0; i < i_last; i++)
    {
      cs->bitstream[i] = *currSlice->partArr[i].bitstream;
    }
  }
  //=== syntax element number and bitcounters ===
  memcpy (cs->bitcounter, currMB->bitcounter, MAX_BITCOUNTER_MB * sizeof(int));

  //=== elements of current macroblock ===
  memcpy (cs->mvd, currMB->mvd, BLOCK_CONTEXT * sizeof(short));
  memcpy (cs->cbp_bits, currMB->cbp_bits, 3 * sizeof(int64));
  
  if (img->P444_joined)
  memcpy (cs->cbp_bits_8x8, currMB->cbp_bits_8x8, 3 * sizeof(int64));
}


/*!
 ************************************************************************
 * \brief
 *    restore coding state (for rd-optimized mode decision)
 ************************************************************************
 */
void reset_coding_state (Macroblock *currMB, CSptr cs)
{
  int  i;
  int  i_last = img->currentPicture->idr_flag? 1:cs->no_part;
  Slice *currSlice = img->currentSlice;

  if (!params->rdopt)  return;

  if (cs->symbol_mode==CABAC)
  {
    //=== important variables of data partition array ===
    //only one partition for IDR img
    for (i = 0; i < i_last; i++)
    {
      //--- parameters of encoding environments ---
      currSlice->partArr[i].ee_cabac = cs->encenv[i];
      *currSlice->partArr[i].bitstream = cs->bitstream[i];
    }

    //=== contexts for binary arithmetic coding ===
    *currSlice->mot_ctx = *cs->mot_ctx;
    *currSlice->tex_ctx = *cs->tex_ctx;
  }
  else
  {
    //=== important variables of data partition array ===
    //only one partition for IDR img
    for (i = 0; i < i_last; i++)
    {
      //--- parameters of encoding environments ---
      *currSlice->partArr[i].bitstream = cs->bitstream[i];
    }
  }

  //=== syntax element number and bit counters ===
  memcpy (currMB->bitcounter, cs->bitcounter, MAX_BITCOUNTER_MB * sizeof(int));

  //=== elements of current macroblock ===
  memcpy (currMB->mvd, cs->mvd, BLOCK_CONTEXT * sizeof(short));
  memcpy (currMB->cbp_bits, cs->cbp_bits, 3 * sizeof(int64));
  if(img->P444_joined)
    memcpy (currMB->cbp_bits_8x8, cs->cbp_bits_8x8, 3 * sizeof(int64));
}


