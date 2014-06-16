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
 *    me_distortion.h
 *
 * \author
 *    Alexis Michael Tourapis        <alexis.tourapis@dolby.com>
 *    Athanasios Leontaris           <aleon@dolby.com>
 *
 * \date
 *    11. August 2006
 *
 * \brief
 *    Headerfile for motion estimation distortion
 **************************************************************************
 */

#ifndef _ME_DISTORTION_H_
#define _ME_DISTORTION_H_

extern imgpel *(*get_line[2]) (imgpel****, int, int);
extern imgpel *(*get_line1[2]) (imgpel****, int, int);
extern imgpel *(*get_line2[2]) (imgpel****, int, int);

extern imgpel *(*get_crline[2]) (imgpel****, int, int);
extern imgpel *(*get_crline1[2]) (imgpel****, int, int);
extern imgpel *(*get_crline2[2]) (imgpel****, int, int);

extern int ref_access_method;
extern int bipred1_access_method;
extern int bipred2_access_method;

extern SubImageContainer ref_pic_sub;
extern SubImageContainer ref_pic1_sub;
extern SubImageContainer ref_pic2_sub;

extern short weight1, weight2, offsetBi;
extern int weight_luma, weight_cr[2], offset_luma, offset_cr[2];
extern short weight1_cr[2], weight2_cr[2], offsetBi_cr[2];
extern short img_width, img_height;
extern int test8x8transform;
extern int ChromaMEEnable;

extern int HadamardSAD4x4(int* diff);
extern int HadamardSAD8x8(int* diff);

extern int computeSAD(imgpel* , int, int, int, int, int);
extern int computeSADWP(imgpel* , int, int, int, int, int);
extern int computeBiPredSAD1(imgpel* , int, int, int, int, int, int, int);
extern int computeBiPredSAD2(imgpel* , int, int, int, int, int, int, int);
extern int computeSATD(imgpel* , int, int, int, int, int);
extern int computeSATDWP(imgpel* , int, int, int, int, int);
extern int computeBiPredSATD1(imgpel* , int, int, int, int, int, int, int);
extern int computeBiPredSATD2(imgpel* , int, int, int, int, int, int, int);
extern int computeSSE(imgpel* , int, int, int, int, int);
extern int computeSSEWP(imgpel* , int, int, int, int, int);
extern int computeBiPredSSE1(imgpel* , int, int, int, int, int, int, int);
extern int computeBiPredSSE2(imgpel* , int, int, int, int, int, int, int);
// Function
extern int (*computeUniPred[6])(imgpel* , int , int , int , int , int );
extern int (*computeBiPred) (imgpel* , int , int , int , int , int, int , int);
extern int (*computeBiPred1[3])(imgpel* , int , int , int , int , int, int , int);
extern int (*computeBiPred2[3])(imgpel* , int , int , int , int , int, int , int);

#endif
