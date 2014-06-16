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
* \file weighted_prediction.c
*
* \brief
*    Estimate weights for WP
*
* \author
*    Main contributors (see contributors.h for copyright, address and affiliation details)
*     - Alexis Michael Tourapis         <alexismt@ieee.org>
*     - Athanasios Leontaris            <aleon@dolby.com>
*************************************************************************************
*/
#include "contributors.h"

#include "global.h"
#include "image.h"
#include "wp.h"

/*!
************************************************************************
* \brief
*    Initialize weighting parameter functions
************************************************************************
*/
void InitWP(InputParameters *params)
{
  //if (params->WPMethod == 0)
  {
    EstimateWPPSlice = estimate_weighting_factor_P_slice;
    EstimateWPBSlice = estimate_weighting_factor_B_slice;
    TestWPPSlice = test_wp_P_slice;
    TestWPBSlice = test_wp_B_slice;
  }
}

/*!
************************************************************************
* \brief
*    Compute sum of samples in a picture
************************************************************************
*/
double ComputeImgSum(imgpel **CurrentImage, int height, int width)
{
  int i, j;
  double sum_value = 0.0;
  for (i = 0; i < height; i++)
  {
    for (j = 0; j < width; j++)
    {
      sum_value += (double) CurrentImage[i][j];
    }
  }
  return sum_value;
}

/*!
************************************************************************
* \brief
*    Estimates reference picture weighting factors for P slices
************************************************************************
*/

void estimate_weighting_factor_P_slice(int select_offset)
{
  int i, j, n;

  int index;
  int comp;
  double dc_org = 0.0;
  double dc_ref[MAX_REFERENCE_PICTURES] = { 0.0 };

  int default_weight[3];

  int list_offset   = ((img->MbaffFrameFlag)&&(img->mb_data[img->current_mb_nr].mb_field))? (img->current_mb_nr & 0x01) ? 4 : 2 : 0;
  int weight[2][MAX_REFERENCE_PICTURES][3];
  int offset[2][MAX_REFERENCE_PICTURES][3];
  int clist;

  imgpel **tmpPtr;

  luma_log_weight_denom = 5;
  chroma_log_weight_denom = 5;
  wp_luma_round = 1 << (luma_log_weight_denom - 1);
  wp_chroma_round = 1 << (chroma_log_weight_denom - 1);
  default_weight[0] = 1 << luma_log_weight_denom;
  default_weight[1] = default_weight[2] = 1 << chroma_log_weight_denom;

  /* set all values to defaults */
  for (i = 0; i < 2 + list_offset; i++)
  {
    for (j = 0; j < listXsize[i]; j++)
    {
      for (n = 0; n < 3; n++)
      {
        weight[i][j][n] = default_weight[n];
        wp_weight[i][j][n] = default_weight[n];
        wp_offset[i][j][n] = 0;
        offset[i][j][n] = 0;
      }
    }
  }

  dc_org = ComputeImgSum(pCurImg, img->height, img->width);
  
  for (clist = 0; clist < 2 + list_offset; clist++)
  {
    for (n = 0; n < listXsize[clist]; n++)
    {
      // Y
      tmpPtr = listX[clist][n]->p_curr_img;      
      dc_ref[n] = ComputeImgSum(tmpPtr, img->height, img->width);

      if (select_offset == 0)
      {
        if (dc_ref[n] != 0.0)
          weight[clist][n][0] = (int) (default_weight[0] * dc_org / dc_ref[n] + 0.5);
        else
          weight[clist][n][0] = default_weight[0];  // only used when reference picture is black
        weight[clist][n][0] = iClip3(-128, 127, weight[clist][n][0]);
      }
      else
      {
        offset[clist][n][0] = (int) ((dc_org - dc_ref[n])/(img->size)+0.5);
        offset[clist][n][0] = (offset[clist][n][0]+((img->bitdepth_luma-8)>>1))>>(img->bitdepth_luma-8);
        offset[clist][n][0] = iClip3( -128, 127, offset[clist][n][0]);
        offset[clist][n][0] = offset[clist][n][0]<<(img->bitdepth_luma-8);
        weight[clist][n][0] = default_weight[0];
      }

      weight[clist][n][1] = default_weight[1];
      weight[clist][n][2] = default_weight[2];
      offset[clist][n][1] = 0;
      offset[clist][n][2] = 0;
    }
  }

  for (clist=0; clist<2 + list_offset; clist++)
  {
    for (index = 0; index < listXsize[clist]; index++)
    {
      for (comp=0; comp < 3; comp ++)
      {
        wp_weight[clist][index][comp] = weight[clist][index][comp];
        wp_offset[clist][index][comp] = offset[clist][index][comp];
        // printf("index %d component %d weight %d offset %d\n",index,comp,weight[0][index][comp],offset[0][index][comp]);
      }
    }
  }
}

/*!
************************************************************************
* \brief
*    Estimates reference picture weighting factors
************************************************************************
*/
void estimate_weighting_factor_B_slice()
{
  int i, j, n;

  int tx,DistScaleFactor;

  int index;
  int comp;
  double dc_org = 0.0;
  double dc_ref[6][MAX_REFERENCE_PICTURES] = { {0.0} };

  int log_weight_denom;

  int default_weight[3];
  int list_offset   = ((img->MbaffFrameFlag)&&(img->mb_data[img->current_mb_nr].mb_field))? (img->current_mb_nr & 0x01) ? 4 : 2 : 0;
  int weight[6][MAX_REFERENCE_PICTURES][3];
  int offset[6][MAX_REFERENCE_PICTURES][3];
  int im_weight[6][MAX_REFERENCE_PICTURES][MAX_REFERENCE_PICTURES][3];
  int im_offset[6][MAX_REFERENCE_PICTURES][MAX_REFERENCE_PICTURES][3];
  int clist;
  int wf_weight, wf_offset;
  imgpel **tmpPtr;

  if (active_pps->weighted_bipred_idc == 2) //! implicit mode
  {
    luma_log_weight_denom = 5;
    chroma_log_weight_denom = 5;
  }
  else
  {
    //luma_log_weight_denom = 6;
    //chroma_log_weight_denom = 6;
    luma_log_weight_denom = 5;
    chroma_log_weight_denom = 5;
  }

  wp_luma_round     = 1 << (luma_log_weight_denom - 1);
  wp_chroma_round   = 1 << (chroma_log_weight_denom - 1);
  default_weight[0] = 1 << luma_log_weight_denom;
  default_weight[1] = 1 << chroma_log_weight_denom;
  default_weight[2] = 1 << chroma_log_weight_denom;

  /* set all values to defaults */
  for (i = 0; i < 2 + list_offset; i++)
  {
    for (j = 0; j < listXsize[i]; j++)
    {
      for (n = 0; n < 3; n++)
      {
        wp_weight[i][j][n] = default_weight[n];
        wp_offset[i][j][n] = 0;
        offset   [i][j][n] = 0;
        weight   [i][j][n] = default_weight[n];
      }
    }
  }

  for (i = 0; i < listXsize[LIST_0]; i++)
  {
    for (j = 0; j < listXsize[LIST_1]; j++)
    {
      int td, tb;
      td = iClip3(-128, 127,(listX[LIST_1][j]->poc - listX[LIST_0][i]->poc));
      tb = iClip3(-128, 127,(enc_picture->poc - listX[LIST_0][i]->poc));
      for (comp = 0; comp < 3; comp++)
      {
        // implicit weights
        if (td == 0)
        {
          im_weight[1][i][j][comp] = default_weight[comp];
          im_weight[0][i][j][comp] = default_weight[comp];
          im_offset[1][i][j][comp] = 0;
          im_offset[0][i][j][comp] = 0;
        }
        else
        {
          tx = (16384 + iabs(td/2))/td;
          DistScaleFactor = iClip3(-1024, 1023, (tx*tb + 32 )>>6);
          im_weight[1][i][j][comp] = DistScaleFactor>>2;
          if (im_weight[1][i][j][comp] < -64 || im_weight[1][i][j][comp] >128)
            im_weight[1][i][j][comp] = default_weight[comp];
          im_weight[0][i][j][comp] = 64 - im_weight[1][i][j][comp];
          im_offset[1][i][j][comp] = 0;
          im_offset[0][i][j][comp] = 0;
        }
      }
      /*
      printf ("%d imp weight[%d][%d] = %d  , %d (%d %d %d) (%d %d) (%d %d)\n",enc_picture->poc, i, j,  im_weight[0][i][j][0], im_weight[1][i][j][0],
      enc_picture->poc,listX[LIST_0][i]->poc, listX[LIST_1][j]->poc,
      DistScaleFactor ,tx,td,tb);
      */
    }
  }

  if (active_pps->weighted_bipred_idc == 2) //! implicit mode
  {
    for (i = 0; i < listXsize[LIST_0]; i++)
    {
      for (j = 0; j < listXsize[LIST_1]; j++)
      {
        for (comp = 0; comp < 3; comp++)
        {
          log_weight_denom = (comp == 0) ? luma_log_weight_denom : chroma_log_weight_denom;
          wbp_weight[1][i][j][comp] = im_weight[1][i][j][comp] ;
          wbp_weight[0][i][j][comp] = im_weight[0][i][j][comp];
        }
      }
    }

    for (clist=0; clist<2 + list_offset; clist++)
    {
      for (index = 0; index < listXsize[clist]; index++)
      {
        wp_weight[clist][index][0] = default_weight[0];
        wp_weight[clist][index][1] = default_weight[1];
        wp_weight[clist][index][2] = default_weight[2];
        wp_offset[clist][index][0] = 0;
        wp_offset[clist][index][1] = 0;
        wp_offset[clist][index][2] = 0;
      }
    }
  }
  else
  {
    dc_org = ComputeImgSum(pCurImg, img->height, img->width);

    for (clist=0; clist<2 + list_offset; clist++)
    {
      for (n = 0; n < listXsize[clist]; n++)
      {
        // To simplify these computations we may wish to perform these after a reference is 
        // stored in the reference buffer and attach them to the storedimage structure!!!
        // Y
        tmpPtr = listX[clist][n]->p_curr_img;
        dc_ref[clist][n] = ComputeImgSum(tmpPtr, img->height, img->width);

        if (dc_ref[clist][n] != 0.0)
          wf_weight = (int) (default_weight[0] * dc_org / dc_ref[clist][n] + 0.5);
        else
          wf_weight = default_weight[0];  // only used when reference picture is black
        wf_weight = iClip3(-128, 127, wf_weight);
        wf_offset = 0;

        //    printf("dc_org = %d, dc_ref = %d, weight[%d] = %d\n",dc_org, dc_ref[n],n,weight[n][0]);

        weight[clist][n][0] = wf_weight;
        offset[clist][n][0] = wf_offset;

        weight[clist][n][1] = default_weight[1];
        weight[clist][n][2] = default_weight[2];        
        offset[clist][n][1] = 0;
        offset[clist][n][2] = 0;
      }
    }

    if (active_pps->weighted_bipred_idc == 1)
    {
      for (clist=0; clist<2 + list_offset; clist++)
      {
        for (index = 0; index < listXsize[clist]; index++)
        {
          for (comp = 0; comp < 3; comp++)
          {
            wp_weight[clist][index][comp] = weight[clist][index][comp];
            wp_offset[clist][index][comp] = offset[clist][index][comp];
            //printf("%d %d\n",wp_weight[clist][index][comp],wp_offset[clist][index][comp]);
          }
        }
      }
    }
    else
    {
      for (clist=0; clist<2 + list_offset; clist++)
      {
        for (index = 0; index < listXsize[clist]; index++)
        {
          memcpy(wp_weight[clist][index], default_weight, 3 * sizeof(int));
          memset(wp_offset[clist][index], 0, 3 * sizeof(int));
        }
      }
    }
    for (i = 0; i < listXsize[LIST_0]; i++)
    {
      for (j = 0; j < listXsize[LIST_1]; j++)
      {
        for (comp = 0; comp < 3; comp++)
        {
          log_weight_denom = (comp == 0) ? luma_log_weight_denom : chroma_log_weight_denom;
          wbp_weight[0][i][j][comp] = wp_weight[0][i][comp];
          wbp_weight[1][i][j][comp] = wp_weight[1][j][comp];
        }
        /*
        printf ("bpw weight[%d][%d] = %d  , %d (%d %d %d) (%d %d) (%d %d)\n", i, j, wbp_weight[0][i][j][0], wbp_weight[1][i][j][0],
        enc_picture->poc,listX[LIST_0][i]->poc, listX[LIST_1][j]->poc,
        DistScaleFactor ,tx,tx,tx);
        */
      }
    }
  }
}


/*!
************************************************************************
* \brief
*    Tests P slice weighting factors to perform or not WP RD decision
************************************************************************
*/

int test_wp_P_slice(int select_offset)
{
  int i, j, n;

  int index;
  int comp;
  double dc_org = 0.0;
  double dc_ref[MAX_REFERENCE_PICTURES] = { 0.0 };

  int default_weight[3];

  int list_offset   = ((img->MbaffFrameFlag)&&(img->mb_data[img->current_mb_nr].mb_field))? (img->current_mb_nr & 0x01) ? 4 : 2 : 0;
  int weight[2][MAX_REFERENCE_PICTURES][3];
  int offset[2][MAX_REFERENCE_PICTURES][3];
  int clist;
  int perform_wp = 0;
  imgpel **tmpPtr;

  luma_log_weight_denom = 5;
  chroma_log_weight_denom = 5;
  wp_luma_round = 1 << (luma_log_weight_denom - 1);
  wp_chroma_round = 1 << (chroma_log_weight_denom - 1);
  default_weight[0] = 1 << luma_log_weight_denom;
  default_weight[1] = default_weight[2] = 1 << chroma_log_weight_denom;

  /* set all values to defaults */
  for (i = 0; i < 2 + list_offset; i++)
  {
    for (j = 0; j < listXsize[i]; j++)
    {
      for (n = 0; n < 3; n++)
      {
        weight[i][j][n] = default_weight[n];
        wp_weight[i][j][n] = default_weight[n];
        wp_offset[i][j][n] = 0;
        offset[i][j][n] = 0;
      }
    }
  }

  dc_org = ComputeImgSum(pCurImg, img->height, img->width);

  for (clist = 0; clist < 2 + list_offset; clist++)
  {
    for (n = 0; n < listXsize[clist]; n++)
    {
      tmpPtr = listX[clist][n]->p_curr_img;
      dc_ref[n] = ComputeImgSum(tmpPtr, img->height, img->width);

      if (select_offset == 0)
      {
        if (dc_ref[n] != 0.0)
          weight[clist][n][0] = (int) (default_weight[0] * dc_org / dc_ref[n] + 0.5);
        else
          weight[clist][n][0] = default_weight[0];  // only used when reference picture is black
        weight[clist][n][0] = iClip3(-128, 127, weight[clist][n][0]);
      }
      else
      {
        offset[clist][n][0] = (int) ((dc_org - dc_ref[n])/(img->size)+0.5);
        offset[clist][n][0] = (offset[clist][n][0]+((img->bitdepth_luma-8)>>1))>>(img->bitdepth_luma-8);
        offset[clist][n][0] = iClip3( -128, 127, offset[clist][n][0]);
        offset[clist][n][0] = offset[clist][n][0]<<(img->bitdepth_luma-8);
        weight[clist][n][0] = default_weight[0];
      }
    }
  }

  for (clist=0; clist<2 + list_offset; clist++)
  {
    for (index = 0; index < listXsize[clist]; index++)
    {
      for (comp=0; comp < 3; comp ++)
      {
        int offset_test = params->RDPSliceBTest && active_sps->profile_idc != 66
          ? iabs(offset[clist][index][comp]) > 2
          : offset[clist][index][comp] != 0;

        if (weight[clist][index][comp] != default_weight[0] ||  offset_test)
        {
          perform_wp = 1;
          break;
        }
      }
      if (perform_wp == 1) break;
    }
    if (perform_wp == 1) break;
  }

  return perform_wp;
}

/*!
************************************************************************
* \brief
*    test_wp_B_slice:
*    Tests B slice weighting prediction
************************************************************************
*/
int test_wp_B_slice(int select_method)
{
  int i, j, n;

  int tx,DistScaleFactor;

  int index;
  int comp;
  double dc_org = 0.0;
  double dc_ref[6][MAX_REFERENCE_PICTURES] = { {0.0} };

  int log_weight_denom;

  int default_weight[3];
  // this needs to be fixed.
  int list_offset   = ((img->MbaffFrameFlag)&&(img->mb_data[img->current_mb_nr].mb_field))? (img->current_mb_nr & 0x01) ? 4 : 2 : 0;
  int weight[6][MAX_REFERENCE_PICTURES][3];
  int offset[6][MAX_REFERENCE_PICTURES][3];
  int im_weight[6][MAX_REFERENCE_PICTURES][MAX_REFERENCE_PICTURES][3];
  int im_offset[6][MAX_REFERENCE_PICTURES][MAX_REFERENCE_PICTURES][3];
  int clist;
  int wf_weight, wf_offset;
  int perform_wp = 0;
  imgpel **tmpPtr;

  if (select_method == 1) //! implicit mode
  {
    luma_log_weight_denom = 5;
    chroma_log_weight_denom = 5;
  }
  else
  {
    luma_log_weight_denom = 5;
    chroma_log_weight_denom = 5;
  }

  wp_luma_round     = 1 << (luma_log_weight_denom - 1);
  wp_chroma_round   = 1 << (chroma_log_weight_denom - 1);
  default_weight[0] = 1 << luma_log_weight_denom;
  default_weight[1] = 1 << chroma_log_weight_denom;
  default_weight[2] = 1 << chroma_log_weight_denom;

  /* set all values to defaults */
  for (i = 0; i < 2 + list_offset; i++)
  {
    for (j = 0; j < listXsize[i]; j++)
    {
      for (n = 0; n < 3; n++)
      {
        wp_weight[i][j][n] = default_weight[n];
        wp_offset[i][j][n] = 0;
        offset   [i][j][n] = 0;
        weight   [i][j][n] = default_weight[n];
      }
    }
  }

  for (i = 0; i < listXsize[LIST_0]; i++)
  {
    for (j = 0; j < listXsize[LIST_1]; j++)
    {
      int td, tb;
      td = iClip3(-128, 127,(listX[LIST_1][j]->poc - listX[LIST_0][i]->poc));
      tb = iClip3(-128, 127,(enc_picture->poc - listX[LIST_0][i]->poc));
      for (comp = 0; comp < 3; comp++)
      {
        // implicit weights
        if (td == 0)
        {
          im_weight[1][i][j][comp] = default_weight[comp];
          im_weight[0][i][j][comp] = default_weight[comp];
          im_offset[1][i][j][comp] = 0;
          im_offset[0][i][j][comp] = 0;
        }
        else
        {
          tx = (16384 + iabs(td/2))/td;
          DistScaleFactor = iClip3(-1024, 1023, (tx*tb + 32 )>>6);
          im_weight[1][i][j][comp] = DistScaleFactor>>2;
          if (im_weight[1][i][j][comp] < -64 || im_weight[1][i][j][comp] >128)
            im_weight[1][i][j][comp] = default_weight[comp];
          im_weight[0][i][j][comp] = 64 - im_weight[1][i][j][comp];
          im_offset[1][i][j][comp] = 0;
          im_offset[0][i][j][comp] = 0;
        }
      }
    }
  }


  if (select_method == 1) //! implicit mode
  {
    for (i = 0; i < listXsize[LIST_0]; i++)
    {
      for (j = 0; j < listXsize[LIST_1]; j++)
      {
        for (comp = 0; comp < 3; comp++)
        {
          log_weight_denom = (comp == 0) ? luma_log_weight_denom : chroma_log_weight_denom;
          wbp_weight[1][i][j][comp] = im_weight[1][i][j][comp] ;
          wbp_weight[0][i][j][comp] = im_weight[0][i][j][comp];
        }
      }
    }

    for (clist=0; clist<2 + list_offset; clist++)
    {
      for (index = 0; index < listXsize[clist]; index++)
      {
        wp_weight[clist][index][0] = default_weight[0];
        wp_weight[clist][index][1] = default_weight[1];
        wp_weight[clist][index][2] = default_weight[2];
        wp_offset[clist][index][0] = 0;
        wp_offset[clist][index][1] = 0;
        wp_offset[clist][index][2] = 0;
      }
    }
  }
  else
  {
    dc_org = ComputeImgSum(pCurImg, img->height, img->width);

    for (clist=0; clist<2 + list_offset; clist++)
    {
      for (n = 0; n < listXsize[clist]; n++)
      {
        // To simplify these computations we may wish to perform these after a reference is 
        // stored in the reference buffer and attach them to the storedimage structure!!!
        // Y
        tmpPtr = listX[clist][n]->p_curr_img;
        dc_ref[clist][n] = ComputeImgSum(tmpPtr, img->height, img->width);

        if (dc_ref[clist][n] != 0.0)
          wf_weight = (int) (default_weight[0] * dc_org / dc_ref[clist][n] + 0.5);
        else
          wf_weight = default_weight[0];  // only used when reference picture is black
        wf_weight = iClip3(-128, 127, wf_weight);
        wf_offset = 0;

        weight[clist][n][0] = wf_weight;
        offset[clist][n][0] = wf_offset;

        weight[clist][n][1] = default_weight[1];
        weight[clist][n][2] = default_weight[2];        
        offset[clist][n][1] = 0;
        offset[clist][n][2] = 0;
      }
    }

    if (select_method == 0) //! explicit mode
    {
      for (clist=0; clist<2 + list_offset; clist++)
      {
        for (index = 0; index < listXsize[clist]; index++)
        {
          memcpy(wp_weight[clist][index], weight[clist][index], 3 * sizeof(int));
          memcpy(wp_offset[clist][index], offset[clist][index], 3 * sizeof(int));
        }
      }
    }
    else
    {
      for (clist=0; clist<2 + list_offset; clist++)
      {
        for (index = 0; index < listXsize[clist]; index++)
        {
          memcpy(wp_weight[clist][index], default_weight, 3 * sizeof(int));
          memset(wp_offset[clist][index], 0, 3 * sizeof(int));
        }
      }
    }
    for (i = 0; i < listXsize[LIST_0]; i++)
    {
      for (j = 0; j < listXsize[LIST_1]; j++)
      {
        for (comp = 0; comp < 3; comp++)
        {
          log_weight_denom = (comp == 0) ? luma_log_weight_denom : chroma_log_weight_denom;
          wbp_weight[0][i][j][comp] = wp_weight[0][i][comp];
          wbp_weight[1][i][j][comp] = wp_weight[1][j][comp];
        }
        /*
        printf ("bpw weight[%d][%d] = %d  , %d (%d %d %d) (%d %d) (%d %d)\n", i, j, wbp_weight[0][i][j][0], wbp_weight[1][i][j][0],
        enc_picture->poc,listX[LIST_0][i]->poc, listX[LIST_1][j]->poc,
        DistScaleFactor ,tx,tx,tx);
        */
      }
    }
  }

  if (select_method == 0) //! implicit mode
  {
    int active_refs[2];

    active_refs[0] = (params->B_List0_refs == 0 ? listXsize[0] : imin(params->B_List0_refs,listXsize[0]));
    active_refs[1] = (params->B_List1_refs == 0 ? listXsize[1] : imin(params->B_List0_refs,listXsize[1]));

    for (clist=0; clist<2 + list_offset; clist++)
    {
      for (index = 0; index < active_refs[clist]; index++)
      {
        for (comp=0; comp < 3; comp ++)
        {
          if (wp_weight[clist][index][comp] != default_weight[comp])
          {
            perform_wp = 1;
            break;
          }
        }
        if (perform_wp == 1) break;
      }
      if (perform_wp == 1) break;
    }
  }
  return perform_wp;
}


