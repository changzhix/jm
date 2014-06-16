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
 *    q_matrix.h
 *
 * \brief
 *    Headerfile for q_matrix array
 *
 * \date
 *    07. Apr 2004
 ***************************************************************************
 */

#ifndef _Q_MATRIX_H_
#define _Q_MATRIX_H_

extern int *****LevelScale4x4Comp;
extern int *****LevelScale8x8Comp;

extern int *****InvLevelScale4x4Comp;
extern int *****InvLevelScale8x8Comp;

extern short ScalingList4x4input[6][16];
extern short ScalingList8x8input[6][64];
extern short ScalingList4x4[6][16];
extern short ScalingList8x8[6][64];


extern short UseDefaultScalingMatrix4x4Flag[6];
extern short UseDefaultScalingMatrix8x8Flag[6];

extern int *qp_per_matrix;
extern int *qp_rem_matrix;


void Init_QMatrix (void);
void CalculateQuantParam(void);
void CalculateQuant8Param(void);
void free_QMatrix(void);

#endif
