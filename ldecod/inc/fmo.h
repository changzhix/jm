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
 *
 * \file fmo.h
 *
 * \brief
 *    Support for Flexilble Macroblock Ordering (FMO)
 *
 * \date
 *    19 June, 2002
 *
 * \author
 *    Stephan Wenger   stewe@cs.tu-berlin.de
 **************************************************************************/

#ifndef _FMO_H_
#define _FMO_H_


int FmoInit (pic_parameter_set_rbsp_t* pps, seq_parameter_set_rbsp_t* sps);
int FmoFinit (void);

int FmoGetNumberOfSliceGroup(void);
int FmoGetLastMBOfPicture(void);
int FmoGetLastMBInSliceGroup(int SliceGroup);
int FmoGetSliceGroupId (int mb);
int FmoGetNextMBNr (int CurrentMbNr);

#endif
