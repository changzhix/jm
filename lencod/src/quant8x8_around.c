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
 * \file quant8x8_around.c
 *
 * \brief
 *    Quantization process for a 8x8 block with adaptive rounding
 *
 * \author
 *    Main contributors (see contributors.h for copyright, address and affiliation details)
 *    - Alexis Michael Tourapis                  <alexismt@ieee.org>
 *
 *************************************************************************************
 */

#include "contributors.h"

#include <math.h>

#include "global.h"

#include "image.h"
#include "mb_access.h"
#include "vlc.h"
#include "transform.h"
#include "mc_prediction.h"
#include "q_offsets.h"
#include "q_matrix.h"
#include "quant8x8.h"


/*!
 ************************************************************************
 * \brief
 *    Quantization process for All coefficients for a 8x8 block
 *
 * \par Input:
 *
 * \par Output:
 *
 ************************************************************************
 */
int quant_8x8_around(int (*tblock)[16], int block_y, int block_x, int  qp,                 
                     int*  ACLevel, int*  ACRun, 
                     int **fadjust8x8, int **levelscale, int **invlevelscale, int **leveloffset,
                     int *coeff_cost, const byte (*pos_scan)[2], const byte *c_cost)
{
  static int i,j, coeff_ctr;

  static int *m7;
  static int scaled_coeff;

  int   level, run = 0;
  int   nonzero = FALSE;
  int   qp_per = qp_per_matrix[qp];
  int   q_bits = Q_BITS_8 + qp_per;
  const byte *p_scan = &pos_scan[0][0];
  int*  ACL = &ACLevel[0];
  int*  ACR = &ACRun[0];

// Quantization
  for (coeff_ctr = 0; coeff_ctr < 64; coeff_ctr++)
  {
    i = *p_scan++;  // horizontal position
    j = *p_scan++;  // vertical position

    m7 = &tblock[j][block_x + i];
    scaled_coeff = iabs (*m7) * levelscale[j][i];
    level = (scaled_coeff + leveloffset[j][i]) >> q_bits;

    if (level != 0)
    {
      if (params->symbol_mode == CAVLC && img->qp < 10)
        level = imin(level, CAVLC_LEVEL_LIMIT);

      fadjust8x8[j][block_x + i] = rshift_rnd_sf((AdaptRndWeight * (scaled_coeff - (level << q_bits))), (q_bits + 1));

      nonzero = TRUE;

      *coeff_cost += (level > 1) ? MAX_VALUE : c_cost[run];

      level  = isignab(level, *m7);
      *m7    = rshift_rnd_sf(((level * invlevelscale[j][i]) << qp_per), 6);
      *ACL++ = level;
      *ACR++ = run; 
      // reset zero level counter
      run    = 0;
    }
    else
    {
      fadjust8x8[j][block_x + i] = 0;
      run++;
      *m7 = 0;
    }      
  }

  *ACL = 0;

  return nonzero;
}

/*!
 ************************************************************************
 * \brief
 *    Quantization process for All coefficients for a 8x8 block 
 *    CAVLC version
 *
 * \par Input:
 *
 * \par Output:
 *
 ************************************************************************
 */
int quant_8x8cavlc_around(int (*tblock)[16], int block_y, int block_x, int  qp,                 
                          int***  cofAC, 
                          int **fadjust8x8, int **levelscale, int **invlevelscale, int **leveloffset,
                          int *coeff_cost, const byte (*pos_scan)[2], const byte *c_cost)
{
  static int i,j, k, coeff_ctr;

  static int *m7;
  static int scaled_coeff;  

  int level, runs[4] = { 0 };
  int nonzero = FALSE; 
  int qp_per = qp_per_matrix[qp];  
  int q_bits = Q_BITS_8 + qp_per;
  const byte *p_scan = &pos_scan[0][0];
  int*  ACL[4];  
  int*  ACR[4];

  for (k = 0; k < 4; k++)
  {
    ACL[k] = &cofAC[k][0][0];
    ACR[k] = &cofAC[k][1][0];
  }

  // Quantization
  for (coeff_ctr = 0; coeff_ctr < 16; coeff_ctr++)
  {
    for (k = 0; k < 4; k++)
    {
      i = *p_scan++;  // horizontal position
      j = *p_scan++;  // vertical position

      m7 = &tblock[j][block_x + i];
      scaled_coeff = iabs (*m7) * levelscale[j][i];
      level = (scaled_coeff + leveloffset[j][i]) >> q_bits;

      if (level != 0)
      {
        if (params->symbol_mode == CAVLC && img->qp < 10)
          level = imin(level, CAVLC_LEVEL_LIMIT);

        fadjust8x8[j][block_x + i] = rshift_rnd_sf((AdaptRndWeight * (scaled_coeff - (level << q_bits))), (q_bits + 1));

        nonzero=TRUE;

        *coeff_cost += (level > 1) ? MAX_VALUE : c_cost[runs[k]];

        level  = isignab(level, *m7);
        *m7    = rshift_rnd_sf(((level * invlevelscale[j][i]) << qp_per), 6);

        *(ACL[k])++ = level;
        *(ACR[k])++ = runs[k];
        // reset zero level counter
        runs[k] = 0;
      }
      else
      {
        fadjust8x8[j][block_x + i] = 0;
        runs[k]++;
        *m7 = 0;      
      }
    }
  }

  for(k = 0; k < 4; k++)
    *(ACL[k]) = 0;

  return nonzero;
}

