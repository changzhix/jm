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
 **************************************************************************************
 * \file
 *    parset.c
 * \brief
 *    Picture and Sequence Parameter set generation and handling
 *  \date 25 November 2002
 * \author
 *    Main contributors (see contributors.h for copyright, address and affiliation details)
 *      - Stephan Wenger        <stewe@cs.tu-berlin.de>
 *
 **************************************************************************************
 */
#include "global.h"
#include "memalloc.h"

/*!
 *************************************************************************************
 * \brief
 *    Allocates memory for a pps
 *
 * \return
 *    pointer to a pps
 *************************************************************************************
 */
pic_parameter_set_rbsp_t *AllocPPS ()
 {
   pic_parameter_set_rbsp_t *p;

   if ((p=calloc (sizeof (pic_parameter_set_rbsp_t), 1)) == NULL)
     no_mem_exit ("AllocPPS: PPS");
   p->slice_group_id = NULL;
   return p;
 }


/*!
 *************************************************************************************
 * \brief
 *    Allocates memory for am sps
 *
 * \return
 *    pointer to a sps
 *************************************************************************************
 */
seq_parameter_set_rbsp_t *AllocSPS ()
 {
   seq_parameter_set_rbsp_t *p;

   if ((p=calloc (sizeof (seq_parameter_set_rbsp_t), 1)) == NULL)
     no_mem_exit ("AllocSPS: SPS");
   return p;
 }


/*!
 *************************************************************************************
 * \brief
 *    Frees a pps
 *
 * \param pps
 *     pps to be freed
 *
 * \return
 *    none
 *************************************************************************************
 */

 void FreePPS (pic_parameter_set_rbsp_t *pps)
 {
   assert (pps != NULL);
   if (pps->slice_group_id != NULL)
     free (pps->slice_group_id);
   free (pps);
 }


 /*!
 *************************************************************************************
 * \brief
 *    Frees a sps
 *
 * \param sps
 *     sps to be freed
 *
 * \return
 *    none
 *************************************************************************************
 */

 void FreeSPS (seq_parameter_set_rbsp_t *sps)
 {
   assert (sps != NULL);
   free (sps);
 }
