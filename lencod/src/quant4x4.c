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
 * \file quant4x4.c
 *
 * \brief
 *    Quantization process for a 4x4 block
 *
 * \author
 *    Main contributors (see contributors.h for copyright, address and affiliation details)
 *    - Alexis Michael Tourapis                  <alexismt@ieee.org>
 *
 *************************************************************************************
 */

#include "contributors.h"
#include "global.h"
#include "quant4x4.h"

/*!
************************************************************************
* \brief
*    Quantization initialization function
*
************************************************************************
*/
void init_quant_4x4(ImageParameters *img)
{
  if (params->UseRDOQuant == 1)
  {
    quant_4x4     = quant_4x4_trellis;
    quant_ac4x4   = quant_ac4x4_trellis;
    if (params->RDOQ_CR == 1)
      quant_ac4x4cr = quant_ac4x4_trellis;
    else
      quant_ac4x4cr = quant_ac4x4_normal;
  }
  else if (img->AdaptiveRounding)
  {
    quant_4x4   = quant_4x4_around;
    quant_ac4x4 = quant_ac4x4_around;
    quant_ac4x4cr = quant_ac4x4_around;
  }
  else
  {
    quant_4x4   = quant_4x4_normal;
    quant_ac4x4 = quant_ac4x4_normal;
    quant_ac4x4cr = quant_ac4x4_normal;
  }
}

