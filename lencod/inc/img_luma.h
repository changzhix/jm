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
 *    img_luma.h
 *
 * \author
 *    Athanasios Leontaris           <aleon@dolby.com>
 *    Alexis Michael Tourapis        <alexis.tourapis@dolby.com>
 *
 * \date
 *    4. October 2006
 *
 * \brief
 *    Headerfile for luma interpolation functions
 **************************************************************************
 */

#ifndef _IMG_LUMA_H_
#define _IMG_LUMA_H_

void getSubImagesLuma       ( StorablePicture *s );
void getSubImageInteger     ( StorablePicture *s, imgpel **dstImg, imgpel **srcImg);
void getHorSubImageSixTap   ( StorablePicture *s, imgpel **dst_imgY, imgpel **ref_imgY);
void getVerSubImageSixTap   ( StorablePicture *s, imgpel **dst_imgY, imgpel **ref_imgY);
void getVerSubImageSixTapTmp( StorablePicture *s, imgpel **dst_imgY, imgpel **ref_imgY);
void getSubImageBiLinear    ( StorablePicture *s, imgpel **dstImg, imgpel **srcImgL, imgpel **srcImgR);
void getHorSubImageBiLinear ( StorablePicture *s, imgpel **dstImg, imgpel **srcImgL, imgpel **srcImgR);
void getVerSubImageBiLinear ( StorablePicture *s, imgpel **dstImg, imgpel **srcImgT, imgpel **srcImgB);
void getDiagSubImageBiLinear( StorablePicture *s, imgpel **dstImg, imgpel **srcImgT, imgpel **srcImgB);
#endif // _IMG_LUMA_H_
