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
 * \file sei.h
 *
 * \brief
 *    Prototypes for sei.c
 *************************************************************************************
 */

#ifndef SEI_H
#define SEI_H

typedef enum {
  SEI_BUFFERING_PERIOD = 0,
  SEI_PIC_TIMING,
  SEI_PAN_SCAN_RECT,
  SEI_FILLER_PAYLOAD,
  SEI_USER_DATA_REGISTERED_ITU_T_T35,
  SEI_USER_DATA_UNREGISTERED,
  SEI_RECOVERY_POINT,
  SEI_DEC_REF_PIC_MARKING_REPETITION,
  SEI_SPARE_PIC,
  SEI_SCENE_INFO,
  SEI_SUB_SEQ_INFO,
  SEI_SUB_SEQ_LAYER_CHARACTERISTICS,
  SEI_SUB_SEQ_CHARACTERISTICS,
  SEI_FULL_FRAME_FREEZE,
  SEI_FULL_FRAME_FREEZE_RELEASE,
  SEI_FULL_FRAME_SNAPSHOT,
  SEI_PROGRESSIVE_REFINEMENT_SEGMENT_START,
  SEI_PROGRESSIVE_REFINEMENT_SEGMENT_END,
  SEI_MOTION_CONSTRAINED_SLICE_GROUP_SET,
  SEI_FILM_GRAIN_CHARACTERISTICS,
  SEI_DEBLOCKING_FILTER_DISPLAY_PREFERENCE,
  SEI_STEREO_VIDEO_INFO,
  SEI_POST_FILTER_HINTS,
  SEI_TONE_MAPPING,

  SEI_MAX_ELEMENTS  //!< number of maximum syntax elements
} SEI_type;

#define MAX_FN 256
// tone mapping information
#define MAX_CODED_BIT_DEPTH  12
#define MAX_SEI_BIT_DEPTH    12
#define MAX_NUM_PIVOTS     (1<<MAX_CODED_BIT_DEPTH)

#if (ENABLE_OUTPUT_TONEMAPPING)
typedef struct
{
  Boolean seiHasTone_mapping;
  unsigned int  tone_map_repetition_period;
  unsigned char coded_data_bit_depth;
  unsigned char sei_bit_depth;
  unsigned int  model_id;
  unsigned int count;
  
  imgpel lut[1<<MAX_CODED_BIT_DEPTH];                 //<! look up table for mapping the coded data value to output data value

  Bitstream *data;
  int payloadSize;
} tone_mapping_struct;

extern tone_mapping_struct seiToneMapping;
#endif

void InterpretSEIMessage(byte* msg, int size, ImageParameters *img);
void interpret_spare_pic( byte* payload, int size, ImageParameters *img );
void interpret_subsequence_info( byte* payload, int size, ImageParameters *img );
void interpret_subsequence_layer_characteristics_info( byte* payload, int size, ImageParameters *img );
void interpret_subsequence_characteristics_info( byte* payload, int size, ImageParameters *img );
void interpret_scene_information( byte* payload, int size, ImageParameters *img ); // JVT-D099
void interpret_user_data_registered_itu_t_t35_info( byte* payload, int size, ImageParameters *img );
void interpret_user_data_unregistered_info( byte* payload, int size, ImageParameters *img );
void interpret_pan_scan_rect_info( byte* payload, int size, ImageParameters *img );
void interpret_recovery_point_info( byte* payload, int size, ImageParameters *img );
void interpret_filler_payload_info( byte* payload, int size, ImageParameters *img );
void interpret_dec_ref_pic_marking_repetition_info( byte* payload, int size, ImageParameters *img );
void interpret_full_frame_freeze_info( byte* payload, int size, ImageParameters *img );
void interpret_full_frame_freeze_release_info( byte* payload, int size, ImageParameters *img );
void interpret_full_frame_snapshot_info( byte* payload, int size, ImageParameters *img );
void interpret_progressive_refinement_start_info( byte* payload, int size, ImageParameters *img );
void interpret_progressive_refinement_end_info( byte* payload, int size, ImageParameters *img );
void interpret_motion_constrained_slice_group_set_info( byte* payload, int size, ImageParameters *img );
void interpret_reserved_info( byte* payload, int size, ImageParameters *img );
void interpret_buffering_period_info( byte* payload, int size, ImageParameters *img );
void interpret_picture_timing_info( byte* payload, int size, ImageParameters *img );
void interpret_film_grain_characteristics_info( byte* payload, int size, ImageParameters *img );
void interpret_deblocking_filter_display_preference_info( byte* payload, int size, ImageParameters *img );
void interpret_stereo_video_info_info( byte* payload, int size, ImageParameters *img );
void interpret_post_filter_hints_info( byte* payload, int size, ImageParameters *img );
// functions for tone mapping SEI message
void interpret_tone_mapping( byte* payload, int size, ImageParameters *img );

#if (ENABLE_OUTPUT_TONEMAPPING)
void tone_map(imgpel** imgX, imgpel* lut, int size_x, int size_y);
void init_tone_mapping_sei(void);
void update_tone_mapping_sei(void);
#endif
#endif
