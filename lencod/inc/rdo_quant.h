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
 *    rdo_quant.h
 *
 * \brief
 *    Headerfile for trellis based mode decision
 *
 ***************************************************************************
 */


#ifndef _RDO_QUANT_H_
#define _RDO_QUANT_H_

#include <math.h>


#define MAX_PREC_COEFF    25
#define SIGN_BITS    1

typedef struct 
{
  int  significantBits[16][2];
  int  lastBits[16][2];
  int     greaterOneBits[2][5][2]; // c1 and c2
  int        greaterOneState[5];
  int     blockCbpBits[4][2]; // c1 and c2
} estBitsCabacStruct;

typedef struct 
{
  int level[3];
  int  levelDouble;
  double  errLevel[3];
  int noLevels;
} levelDataStruct;

int precalcUnaryLevelTab[128][MAX_PREC_COEFF];
estBitsCabacStruct estBitsCabac[NUM_BLOCK_TYPES];

void precalculate_unary_exp_golomb_level();
int est_unary_exp_golomb_level_bits(unsigned int symbol, int bits0, int bits1);
int est_exp_golomb_encode_eq_prob(unsigned int symbol);

void estRunLevel_CABAC (Macroblock* currMB, int context);
void est_CBP_block_bit (Macroblock* currMB, EncodingEnvironmentPtr eep_dp, int type);
void est_significance_map(Macroblock* currMB, EncodingEnvironmentPtr eep_dp, int type);
void est_significant_coefficients (Macroblock* currMB, EncodingEnvironmentPtr eep_dp,  int type);
int biari_no_bits(EncodingEnvironmentPtr eep, signed short symbol, BiContextTypePtr bi_ct );
int biari_state(EncodingEnvironmentPtr eep, signed short symbol, BiContextTypePtr bi_ct );

int est_write_and_store_CBP_block_bit(Macroblock* currMB, int type, int block_y, int block_x);
void est_writeRunLevel_CABAC(levelDataStruct levelData[], int levelTabMin[], int type, double lambda, int kStart, 
                             int kStop, int noCoeff, int estCBP);
int est_unary_exp_golomb_level_encode(unsigned int symbol, int ctx, int type);

void RDOQ_update_mode(RD_PARAMS *enc_mb, int bslice);
void copy_rddata_trellis (RD_DATA *dest, RD_DATA *src);
void updateMV_mp(int *m_cost, short ref, int list, int h, int v, int blocktype, int *lambda_factor, int block8x8);

#endif  // _RDO_QUANT_H_

