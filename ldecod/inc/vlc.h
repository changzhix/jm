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
 * \file vlc.h
 *
 * \brief
 *    header for (CA)VLC coding functions
 *
 * \author
 *    Karsten Suehring
 *
 ************************************************************************
 */

#ifndef _VLC_H_
#define _VLC_H_

int se_v (char *tracestring, Bitstream *bitstream);
int ue_v (char *tracestring, Bitstream *bitstream);
Boolean u_1 (char *tracestring, Bitstream *bitstream);
int u_v (int LenInBits, char *tracestring, Bitstream *bitstream);
int i_v (int LenInBits, char *tracestring, Bitstream *bitstream);

// UVLC mapping
void linfo_ue(int len, int info, int *value1, int *dummy);
void linfo_se(int len, int info, int *value1, int *dummy);

void linfo_cbp_intra(int len,int info,int *cbp, int *dummy);
void linfo_cbp_inter(int len,int info,int *cbp, int *dummy);
void linfo_levrun_inter(int len,int info,int *level,int *irun);
void linfo_levrun_c2x2(int len,int info,int *level,int *irun);

int  readSyntaxElement_VLC (SyntaxElement *sym, Bitstream *currStream);
int  readSyntaxElement_UVLC(SyntaxElement *sym, ImageParameters *img, struct datapartition *dp);
int  readSyntaxElement_Intra4x4PredictionMode(SyntaxElement *sym, ImageParameters *img, Bitstream   *currStream);

int  GetVLCSymbol (byte buffer[],int totbitoffset,int *info, int bytecount);
int  GetVLCSymbol_IntraMode (byte buffer[],int totbitoffset,int *info, int bytecount);

int readSyntaxElement_FLC                         (SyntaxElement *sym, Bitstream *currStream);
int readSyntaxElement_NumCoeffTrailingOnes(SyntaxElement *sym,  Bitstream *currStream,
                                           char *type);
int readSyntaxElement_NumCoeffTrailingOnesChromaDC(SyntaxElement *sym, Bitstream *currStream);
int readSyntaxElement_Level_VLC0                  (SyntaxElement *sym, Bitstream *currStream);
int readSyntaxElement_Level_VLCN                  (SyntaxElement *sym, int vlc, Bitstream *currStream);
int readSyntaxElement_TotalZeros                  (SyntaxElement *sym, Bitstream *currStream);
int readSyntaxElement_TotalZerosChromaDC          (SyntaxElement *sym, Bitstream *currStream);
int readSyntaxElement_Run                         (SyntaxElement *sym, Bitstream *currStream);
int GetBits (byte buffer[],int totbitoffset,int *info, int bytecount,
             int numbits);
int ShowBits (byte buffer[],int totbitoffset,int bytecount, int numbits);
int ShowBitsThres (byte *curbyte,int totbitoffset,int bytecount, int numbits, int code);

int more_rbsp_data (byte buffer[],int totbitoffset,int bytecount);


#endif

