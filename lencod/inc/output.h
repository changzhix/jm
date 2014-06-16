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
 *    output.h
 * \brief
 *    Picture writing routine headers
 * \author
 *    Main contributors (see contributors.h for copyright, address and affiliation details)
 *      - Karsten Suehring        <suehring@hhi.de>
 ***************************************************************************************
 */

#ifndef _OUTPUT_H_
#define _OUTPUT_H_

void flush_direct_output(int p_out);
void direct_output_paff(StorablePicture *p, int p_out);
void write_out_picture(StorablePicture *p, int p_out);
void write_stored_frame(FrameStore *fs, int p_out);
void direct_output(StorablePicture *p, int p_out);
void direct_output_paff(StorablePicture *p, int p_out);
void init_out_buffer(void);
void uninit_out_buffer(void);


#endif //_OUTPUT_H_
