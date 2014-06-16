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
 * \file refbuf.c
 *
 * \brief
 *    Declarations of teh reference frame buffer types and functions
 ************************************************************************
 */

#include "global.h"
#include "refbuf.h"


/*!
 ************************************************************************
 * \brief
 *    Yields a pel line _pointer_ from one of the 16 sub-images
 *    Input does not require subpixel image indices
 ************************************************************************
 */
imgpel *FastLine4X (imgpel ****Pic, int y, int x)
{
  return &(Pic[(y & 0x03)][(x & 0x03)][y >> 2][x >> 2]);
}

/*!
 ************************************************************************
 * \brief
 *    Yields a pel line _pointer_ from one of the 16 sub-images
 *    Input does not require subpixel image indices
 ************************************************************************
 */
imgpel *UMVLine4X (imgpel ****Pic, int y, int x)
{
  return &(Pic[(y & 0x03)][(x & 0x03)][iClip3( 0, height_pad, y >> 2)][iClip3( 0, width_pad , x >> 2)]);
}

/*!
 ************************************************************************
 * \brief
 *    Yields a pel line _pointer_ from one of the 16 (4:4:4), 32 (4:2:2),
 *    or 64 (4:2:0) sub-images
 *    Input does not require subpixel image indices
 ************************************************************************
 */
imgpel *UMVLine8X_chroma (imgpel ****Pic, int y, int x)
{
  return &(Pic[y & chroma_mask_mv_y][x & chroma_mask_mv_x][iClip3 (0, height_pad_cr, y >> chroma_shift_y)][iClip3 (0, width_pad_cr , x >> chroma_shift_x)]);
}

/*!
 ************************************************************************
 * \brief
 *    Yields a pel line _pointer_ from one of the 16 (4:4:4), 32 (4:2:2),
 *    or 64 (4:2:0) sub-images
 *    Input does not require subpixel image indices
 ************************************************************************
 */
imgpel *FastLine8X_chroma (imgpel ****Pic, int y, int x)
{
  return &(Pic[y & chroma_mask_mv_y][x & chroma_mask_mv_x][y >> chroma_shift_y][x >> chroma_shift_x]);
}



