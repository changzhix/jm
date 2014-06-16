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
* \file symbol.c
*
* \brief
*    Generic Symbol writing interface
*
* \date
*    18 Jan 2006
*
* \author
*    Karsten Suehring   suehring@hhi.de
**************************************************************************/

#include "global.h"
#include "symbol.h"

void (*writeMB_typeInfo)      (SyntaxElement *se, DataPartition *dP);
void (*writeIntraPredMode)    (SyntaxElement *se, DataPartition *dP);
void (*writeB8_typeInfo)      (SyntaxElement *se, DataPartition *dP);
void (*writeRefFrame[6])      (SyntaxElement *se, DataPartition *dP);
void (*writeMVD)              (SyntaxElement *se, DataPartition *dP);
void (*writeCBP)              (Macroblock* currMB, SyntaxElement *se, DataPartition *dP);
void (*writeDquant)           (SyntaxElement *se, DataPartition *dP);
void (*writeCIPredMode)       (SyntaxElement *se, DataPartition *dP);
void (*writeFieldModeInfo)    (SyntaxElement *se, DataPartition *dP);
void (*writeMB_transform_size)(SyntaxElement *se, DataPartition *dP);
