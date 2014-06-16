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
 * \file input.h
 *
 * \brief
 *    Input related definitions
 *
 * \author
 *
 ************************************************************************
 */

#ifndef _INPUT_H_
#define _INPUT_H_

int testEndian(void);
void initInput(FrameFormat *source, FrameFormat *output);
int AllocateFrameMemory (ImageParameters *img, int size);
void DeleteFrameMemory (void);

void ReadOneFrame (int FrameNoInFile, int HeaderSize, FrameFormat *source, FrameFormat *output);
extern void (*buf2img) ( imgpel** imgX, unsigned char* buf, int size_x, int size_y, int osize_x, int o_size_y, int symbol_size_in_bytes, int bitshift);

#endif

