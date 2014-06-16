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
 *    mode_decision.h
 *
 * \author
 *    Alexis Michael Tourapis
 *
 * \date
 *    21. February 2005
 *
 * \brief
 *    Headerfile for mode decision
 **************************************************************************
 */

#ifndef _MODE_DECISION_H_
#define _MODE_DECISION_H_

extern CSptr cs_mb, cs_b8, cs_cm, cs_ib8, cs_ib4;
extern RD_8x8DATA tr4x4, tr8x8;

// Adaptive Lagrangian variables
extern double mb16x16_cost;
extern double lambda_mf_factor;


extern int    ****cofAC8x8ts[3];        // [8x8block][4x4block][level/run][scan_pos]
extern int    ****cofAC, ****cofAC8x8;        // [8x8block][4x4block][level/run][scan_pos]
extern int   ****cofAC8x8CbCr[2];
extern int    QP2QUANT[40];
extern int    cbp_blk8x8;
extern int    cbp, cbp8x8, cnt_nonz_8x8;
extern int64  cbp_blk8_8x8ts;
extern int    cbp8_8x8ts;
extern int    cnt_nonz8_8x8ts;
extern int    qp_mbaff[2][2], qp_mbaff[2][2];
extern int    delta_qp_mbaff[2][2],delta_qp_mbaff[2][2];

// Residue Color Transform
extern char   b4_ipredmode[16], b4_intra_pred_modes[16];

extern short  bi_pred_me;
extern short  best_mode;
extern short  best8x8mode          [4]; // [block]
extern char   best8x8pdir [MAXMODE][4]; // [mode][block]
extern char   best8x8l0ref[MAXMODE][4]; // [mode][block]
extern char   best8x8l1ref[MAXMODE][4]; // [mode][block]
extern imgpel pred[16][16];

extern void   set_stored_macroblock_parameters (Macroblock *currMB);
extern void   StoreMV8x8(int);
extern void   RestoreMV8x8(int);
extern void   store_macroblock_parameters (Macroblock *currMB, int);
extern void   SetModesAndRefframeForBlocks (Macroblock *currMB, int);
extern void   SetRefAndMotionVectors (Macroblock *currMB, int, int, int, int, int);
extern void   StoreNewMotionVectorsBlock8x8(int, int, int, int, int, int, int);
extern void   assign_enc_picture_params(int, char, int, int, int, int, int);
extern void   update_refresh_map(int intra, int intra1, Macroblock *currMB);
extern void   SetMotionVectorsMB (Macroblock*, int);
extern void   SetCoeffAndReconstruction8x8 (Macroblock*);

extern int    GetBestTransformP8x8(void);
extern int    I16Offset (int, int);
extern int    CheckReliabilityOfRef (int, int, int, int);
extern int    Mode_Decision_for_Intra4x4Macroblock (Macroblock *currMB, double, double*);
extern int    RDCost_for_macroblocks (Macroblock  *currMB, double, int, double*, double*, double*, int);
extern double RDCost_for_8x8blocks (Macroblock  *currMB, int*, int64*, double, int, int, short, short, short);
extern double *mb16x16_cost_frame;

extern const int  b8_mode_table[6];
extern const int  mb_mode_table[9];

void rc_store_diff(int cpix_x, int cpix_y, imgpel prediction[16][16]);
void submacroblock_mode_decision(RD_PARAMS, RD_8x8DATA *, Macroblock *,int ***, int ***, int ***, int *, short, int, int *, int *, int *, int);
void init_enc_mb_params(Macroblock* currMB, RD_PARAMS *enc_mb, int intra, int bslice);
void list_prediction_cost(Macroblock *currMB, int list, int block, int mode, RD_PARAMS enc_mb, int bmcost[5], char best_ref[2]);
void determine_prediction_list(int, int [5], char [2], char *, int *, short *);
void compute_mode_RD_cost(int mode, Macroblock *currMB, RD_PARAMS enc_mb,
                               double *min_rdcost, double *min_dcost, double *min_rate,
                               int i16mode, short bslice, short *inter_skip);
extern void update_lambda_costs(RD_PARAMS *enc_mb, int lambda_mf[3]);


void get_initial_mb16x16_cost(Macroblock* currMB);
void adjust_mb16x16_cost(int);

int64 (*getDistortion)( Macroblock *currMB );
int64 distortionSSE   ( Macroblock *currMB );
#endif

