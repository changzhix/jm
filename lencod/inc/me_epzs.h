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
 * \file
 *     me_epzs.h
 *
 * \author
 *    Alexis Michael Tourapis        <alexis.tourapis@dolby.com>
 *
 * \date
 *    11. August 2006
 *
 * \brief
 *    Headerfile for EPZS motion estimation
 **************************************************************************
 */


#ifndef _ME_EPZS_H_
#define _ME_EPZS_H_


#define CHECK_RANGE  ((cand.mv_x >= 0) && (cand.mv_x < img_width  - blocksize_x) &&(cand.mv_y >= 0) && (cand.mv_y < img_height - blocksize_y))


typedef struct
{
  int         mb_adaptive_frame_field_flag;
  int         size_x, size_y;

  // Frame
  short ****  mv;            //!< motion vector       [list][subblock_x][subblock_y][component]
  // Top field
  short ****  top_mv;        //!< motion vector       [list][subblock_x][subblock_y][component]
  // Bottom field params
  short ****  bottom_mv;     //!< motion vector       [list][subblock_x][subblock_y][component]

} EPZSColocParams;

typedef struct
{
  MotionVector motion;
  int start_nmbr;
  int next_points;
}
SPoint;

typedef struct MEPatternNode
{
  int    searchPoints;
  SPoint *point;
  int    stopSearch;
  int    nextLast;
  struct MEPatternNode *nextpattern;
}
EPZSStructure;

typedef enum
{
  SDIAMOND  = 0,
  SQUARE    = 1,
  EDIAMOND  = 2,
  LDIAMOND  = 3,
  SBDIAMOND = 4
} EPZSPatterns;

extern EPZSColocParams *EPZSCo_located;
extern int ***EPZSDistortion;  //!< Array for storing SAD Values

extern int  EPZSInit(void);
extern void EPZSDelete (void);
extern void EPZSOutputStats(FILE *stat,short stats_file);
extern void EPZSSliceInit(EPZSColocParams* p, StorablePicture **listX[6]);
extern int  EPZSPelBlockMotionSearch (Macroblock *, imgpel *, short, int, int, char ***, short ****,
                                     int, int, int, short[2], short[2], int, int, int, int);

extern int  EPZSBiPredBlockMotionSearch (Macroblock *, imgpel *, short, int, int, char  ***, short  ****,
                                        int, int, int, short[2], short[2],
                                        short[2], short[2], int, int, int, int);

extern int EPZSSubPelBlockMotionSearch (imgpel *, short, int, int, int, int, int, short[2],
                                        short[2], int, int, int, int*, int);

extern int EPZSSubPelBlockSearchBiPred  (imgpel* orig_pic, short ref, int list, int pic_pix_x, int pic_pix_y,
                                         int blocktype, short pred_mv1[2], short pred_mv2[2], short mv1[2], short mv2[2],
                                         int search_pos2, int search_pos4, int min_mcost, int *lambda_factor, int apply_weights);

#endif

