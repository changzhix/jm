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
 * \file vlc.h
 *
 * \brief
 *    Prototypes for VLC coding funtions
 * \author
 *     Karsten Suehring
 *************************************************************************************
 */

#ifndef _VLC_H_
#define _VLC_H_

Boolean u_1  (char *tracestring, int value, Bitstream *bitstream);
int se_v (char *tracestring, int value, Bitstream *bitstream);
int ue_v (char *tracestring, int value, Bitstream *bitstream);
int u_v  (int n, char *tracestring, int value, Bitstream *bitstream);

void levrun_linfo_c2x2(int level,int run,int *len,int *info);
void levrun_linfo_inter(int level,int run,int *len,int *info);

void writeSE_Fix    (SyntaxElement *se, Bitstream *bitstream);
void writeSE_UVLC   (SyntaxElement *se, DataPartition *dp);
void writeSE_SVLC   (SyntaxElement *se, DataPartition *dp);
void writeSE_Flag   (SyntaxElement *se, DataPartition *dp);
void writeSE_invFlag(SyntaxElement *se, DataPartition *dp);
void writeSE_Dummy  (SyntaxElement *se, DataPartition *dp);

void writeCBP_VLC   (Macroblock* currMB, SyntaxElement *se, DataPartition *dp);
void writeIntraPredMode_CAVLC(SyntaxElement *se, DataPartition *dp);

int   writeSyntaxElement2Buf_UVLC(SyntaxElement *se, Bitstream* this_streamBuffer );
void  writeUVLC2buffer(SyntaxElement *se, Bitstream *currStream);
int   writeSyntaxElement2Buf_Fixed(SyntaxElement *se, Bitstream* this_streamBuffer );
int   symbol2uvlc(SyntaxElement *se);
void  ue_linfo(int n, int dummy, int *len,int *info);
void  se_linfo(int mvd, int dummy, int *len,int *info);
void  cbp_linfo_intra(int cbp, int dummy, int *len,int *info);
void  cbp_linfo_inter(int cbp, int dummy, int *len,int *info);

// CAVLC
void  CAVLC_init(void);

int   writeSyntaxElement_VLC(SyntaxElement *se, DataPartition *this_dataPart);
int   writeSyntaxElement_TotalZeros(SyntaxElement *se, DataPartition *this_dataPart);
int   writeSyntaxElement_TotalZerosChromaDC(SyntaxElement *se, DataPartition *this_dataPart);
int   writeSyntaxElement_Run(SyntaxElement *se, DataPartition *this_dataPart);
int   writeSyntaxElement_NumCoeffTrailingOnes(SyntaxElement *se, DataPartition *this_dataPart);
int   writeSyntaxElement_NumCoeffTrailingOnesChromaDC(SyntaxElement *se, DataPartition *this_dataPart);
int   writeSyntaxElement_Level_VLC1(SyntaxElement *se, DataPartition *this_dataPart, int profile_idc);
int   writeSyntaxElement_Level_VLCN(SyntaxElement *se, int vlc, DataPartition *this_dataPart, int profile_idc);

#endif

