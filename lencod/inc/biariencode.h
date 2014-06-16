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
 *    biariencode.h
 *
 * \brief
 *    Headerfile for binary arithmetic encoding routines
 *
 * \author
 *    - Detlev Marpe
 *    - Gabi Blaettermann
 *    - Gunnar Marten
 *
 *    Copyright (C) 2000 HEINRICH HERTZ INSTITUTE All Rights Reserved.
 *
 * \date
 *    21. Oct 2000
 **************************************************************************
 */


#ifndef _BIARIENCOD_H_
#define _BIARIENCOD_H_


/************************************************************************
 * D e f i n i t i o n s
 ***********************************************************************
 */

// some definitions to increase the readability of the source code

#define Elow                      (eep->Elow)
#define Erange                    (eep->Erange)
#define Ebuffer                   (eep->Ebuffer)
#define Ebits_to_go               (eep->Ebits_to_go)
#define Ecodestrm                 (eep->Ecodestrm)
#define Ecodestrm_len             (eep->Ecodestrm_len)
#define Ecodestrm_laststartcode   (eep->Ecodestrm_laststartcode)

#define B_BITS                    10 // Number of bits to represent the whole coding interval

#define Echunks_outstanding       (eep->Echunks_outstanding)
#define Epbuf                     (eep->Epbuf)
#define BITS_TO_LOAD   16
#define MAX_BITS       (B_BITS + BITS_TO_LOAD)
#define ONE            (1 << MAX_BITS)
#define HALF           (1 << (B_BITS-1))
#define QUARTER        (1 << (B_BITS-2))
#define MIN_BITS_TO_GO  0
#define B_LOAD_MASK    ((1<<BITS_TO_LOAD) - 1)



// Range table for LPS
const byte rLPS_table_64x4[64][4]=
{
        { 128, 176, 208, 240},
        { 128, 167, 197, 227},
        { 128, 158, 187, 216},
        { 123, 150, 178, 205},
        { 116, 142, 169, 195},
        { 111, 135, 160, 185},
        { 105, 128, 152, 175},
        { 100, 122, 144, 166},
        {  95, 116, 137, 158},
        {  90, 110, 130, 150},
        {  85, 104, 123, 142},
        {  81,  99, 117, 135},
        {  77,  94, 111, 128},
        {  73,  89, 105, 122},
        {  69,  85, 100, 116},
        {  66,  80,  95, 110},
        {  62,  76,  90, 104},
        {  59,  72,  86,  99},
        {  56,  69,  81,  94},
        {  53,  65,  77,  89},
        {  51,  62,  73,  85},
        {  48,  59,  69,  80},
        {  46,  56,  66,  76},
        {  43,  53,  63,  72},
        {  41,  50,  59,  69},
        {  39,  48,  56,  65},
        {  37,  45,  54,  62},
        {  35,  43,  51,  59},
        {  33,  41,  48,  56},
        {  32,  39,  46,  53},
        {  30,  37,  43,  50},
        {  29,  35,  41,  48},
        {  27,  33,  39,  45},
        {  26,  31,  37,  43},
        {  24,  30,  35,  41},
        {  23,  28,  33,  39},
        {  22,  27,  32,  37},
        {  21,  26,  30,  35},
        {  20,  24,  29,  33},
        {  19,  23,  27,  31},
        {  18,  22,  26,  30},
        {  17,  21,  25,  28},
        {  16,  20,  23,  27},
        {  15,  19,  22,  25},
        {  14,  18,  21,  24},
        {  14,  17,  20,  23},
        {  13,  16,  19,  22},
        {  12,  15,  18,  21},
        {  12,  14,  17,  20},
        {  11,  14,  16,  19},
        {  11,  13,  15,  18},
        {  10,  12,  15,  17},
        {  10,  12,  14,  16},
        {   9,  11,  13,  15},
        {   9,  11,  12,  14},
        {   8,  10,  12,  14},
        {   8,   9,  11,  13},
        {   7,   9,  11,  12},
        {   7,   9,  10,  12},
        {   7,   8,  10,  11},
        {   6,   8,   9,  11},
        {   6,   7,   9,  10},
        {   6,   7,   8,   9},
        {   2,   2,   2,   2}
};

const byte AC_next_state_MPS_64[64] =
{
                1,2,3,4,5,6,7,8,9,10,
                11,12,13,14,15,16,17,18,19,20,
                21,22,23,24,25,26,27,28,29,30,
                31,32,33,34,35,36,37,38,39,40,
                41,42,43,44,45,46,47,48,49,50,
                51,52,53,54,55,56,57,58,59,60,
                61,62,62,63
};

const byte AC_next_state_LPS_64[64] =
{
                 0, 0, 1, 2, 2, 4, 4, 5, 6, 7,
                 8, 9, 9,11,11,12,13,13,15,15,
                 16,16,18,18,19,19,21,21,22,22,
                 23,24,24,25,26,26,27,27,28,29,
                 29,30,30,30,31,32,32,33,33,33,
                 34,34,35,35,35,36,36,36,37,37,
                 37,38,38,63
};

const byte renorm_table_32[32]={6,5,4,4,3,3,3,3,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};

#endif  // BIARIENCOD_H
