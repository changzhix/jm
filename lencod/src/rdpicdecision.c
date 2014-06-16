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
* \file rdpicdecision.c
*
* \brief
*    Perform RD optimal decisions between multiple coded versions of the same picture
*
* \author
*    Main contributors (see contributors.h for copyright, address and affiliation details)
*     - Alexis Michael Tourapis         <alexismt@ieee.org>
*************************************************************************************
*/

#include "global.h"
#include <math.h>


/*!
 ************************************************************************
 * \brief
 *    RD decision between possible encoding cases
 ************************************************************************
 */
int rd_pic_decision(double snrY_version1, double snrY_version2, int bits_version1, int bits_version2, double lambda_picture)
{
  double cost_version1, cost_version2;

  cost_version1 = (double) bits_version1 * lambda_picture + snrY_version1;
  cost_version2 = (double) bits_version2 * lambda_picture + snrY_version2;
  //printf("%d %d %.2f %.2f %.2f %.2f \n",bits_version1,bits_version2,snrY_version1,snrY_version2,cost_version1,cost_version2);
  if (cost_version2 > cost_version1 || (cost_version2 == cost_version1 && snrY_version2 >= snrY_version1) )
    return (0);
  else
    return (1);
}

/*!
 ************************************************************************
 * \brief
 *    Picture Coding Decision
 ************************************************************************
 */
int picture_coding_decision (Picture *picture1, Picture *picture2, int qp)
{
  double lambda_picture;
  int spframe = (img->type == SP_SLICE);
  int bframe = (img->type == B_SLICE);
  double snr_picture1, snr_picture2;
  int bit_picture1, bit_picture2;

  if (params->successive_Bframe)
    lambda_picture = (qp < 20 ? 0.55 : 0.68) * pow (2, (qp - SHIFT_QP) / 3.0) * (bframe || spframe ? 2 : 1);
  else
    lambda_picture = (qp < 20 ? 0.55 : 0.68) * pow (2, (qp - SHIFT_QP) / 3.0);

  snr_picture1 = picture1->distortion_y + picture1->distortion_u + picture1->distortion_v;
  snr_picture2 = picture2->distortion_y + picture2->distortion_u + picture2->distortion_v;
  bit_picture2 = picture2->bits_per_picture;
  bit_picture1 = picture1->bits_per_picture;
  
  return rd_pic_decision(snr_picture1, snr_picture2, bit_picture1, bit_picture2, lambda_picture);
}

