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
 * \file block.h
 *
 * \brief
 *    constant arrays for single block processing
 *
 * \author
 *    Inge Lille-Langoy               <inge.lille-langoy@telenor.com>    \n
 *    Telenor Satellite Services                                         \n
 *    P.O.Box 6914 St.Olavs plass                                        \n
 *    N-0130 Oslo, Norway
 *
 ************************************************************************
 */

#ifndef _BLOCK_H_
#define _BLOCK_H_

//! make chroma QP from quant
extern const byte QP_SCALE_CR[52] ;


//! single scan pattern
const byte SNGL_SCAN[16][2] =
{
  {0,0},{1,0},{0,1},{0,2},
  {1,1},{2,0},{3,0},{2,1},
  {1,2},{0,3},{1,3},{2,2},
  {3,1},{3,2},{2,3},{3,3}
};

//! field scan pattern
const byte FIELD_SCAN[16][2] =
{
  {0,0},{0,1},{1,0},{0,2},
  {0,3},{1,1},{1,2},{1,3},
  {2,0},{2,1},{2,2},{2,3},
  {3,0},{3,1},{3,2},{3,3}
};


//! array used to find expencive coefficients
const byte COEFF_COST4x4[2][16] =
{
  {3,2,2,1,1,1,0,0,0,0,0,0,0,0,0,0},
  {9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9}
};

//! single scan pattern
const byte SCAN_YUV422  [8][2] =
{
  {0,0},{0,1},
  {1,0},{0,2},
  {0,3},{1,1},
  {1,2},{1,3}
};

//! look up tables for FRExt-chroma support
const unsigned char hor_offset[4][4][4] =
  {{{0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0}},

  {{0, 4, 0, 4},
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0}},

  {{0, 4, 0, 4},
  {0, 4, 0, 4},
  {0, 0, 0, 0},
  {0, 0, 0, 0}},

  {{0, 4, 0, 4},
  {8,12, 8,12},
  {0, 4, 0, 4},
  {8,12, 8,12}}};

const unsigned char ver_offset[4][4][4] =
  { {{0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0}},

  {{0, 0, 4, 4},
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0}},

  {{0, 0, 4, 4},
  {8, 8,12,12},
  {0, 0, 0, 0},
  {0, 0, 0, 0}},

  {{0, 0, 4, 4},
  {0, 0, 4, 4},
  {8, 8,12,12},
  {8, 8,12,12}}};

static unsigned char cbp_blk_chroma[8][4] =
  { {16, 17, 18, 19},
    {20, 21, 22, 23},
    {24, 25, 26, 27},
    {28, 29, 30, 31},
    {32, 33, 34, 35},
    {36, 37, 38, 39},
    {40, 41, 42, 43},
    {44, 45, 46, 47} };

#endif

