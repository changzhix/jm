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
 *    parset.h
 * \brief
 *    Picture and Sequence Parameter Sets, encoder operations
 *
 * \date 25 November 2002
 * \author
 *    Main contributors (see contributors.h for copyright, address and affiliation details)
 *      - Stephan Wenger        <stewe@cs.tu-berlin.de>
 ***************************************************************************************
 */


#ifndef _PARSET_H_
#define _PARSET_H_

#include "parsetcommon.h"
#include "nalu.h"
#include "sei.h"

void GenerateParameterSets (void);
void FreeParameterSets (void);

NALU_t *GenerateSeq_parameter_set_NALU (void);
NALU_t *GeneratePic_parameter_set_NALU (int);
NALU_t *GenerateSEImessage_NALU();

// The following are local helpers, but may come handy in the future, hence public
void GenerateSequenceParameterSet(seq_parameter_set_rbsp_t *sps, int SPS_id);
void GeneratePictureParameterSet( pic_parameter_set_rbsp_t *pps, seq_parameter_set_rbsp_t *sps, int PPS_id,
                                 int WeightedPrediction, int WeightedBiprediction,
                                 int cb_qp_index_offset, int cr_qp_index_offset);

int  Scaling_List(short *scalingListinput, short *scalingList, int sizeOfScalingList, short *UseDefaultScalingMatrix, Bitstream *bitstream);
int  GenerateSeq_parameter_set_rbsp (seq_parameter_set_rbsp_t *sps, byte *buf);
int  GeneratePic_parameter_set_rbsp (pic_parameter_set_rbsp_t *pps, byte *buf);
int  GenerateSEImessage_rbsp (int id, byte *buf);
void FreeSPS (seq_parameter_set_rbsp_t *sps);
void FreePPS (pic_parameter_set_rbsp_t *pps);

int  WriteHRDParameters(seq_parameter_set_rbsp_t *sps, Bitstream *bitstream);
void GenerateVUIParameters(seq_parameter_set_rbsp_t *sps);

pic_parameter_set_rbsp_t *AllocPPS (void);
seq_parameter_set_rbsp_t *AllocSPS (void);


#endif
