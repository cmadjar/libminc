/* ----------------------------- MNI Header -----------------------------------
@NAME       : gyro_read.c
@DESCRIPTION: Code to read gyrocom files and get info from them.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 25, 1993 (Peter Neelin)
@MODIFIED   : $Log: gyro_read.c,v $
@MODIFIED   : Revision 1.1  1993-11-30 14:41:04  neelin
@MODIFIED   : Initial revision
@MODIFIED   :
@COPYRIGHT  :
              Copyright 1993 Peter Neelin, McConnell Brain Imaging Centre, 
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */

#include <gcomserver.h>

/* ----------------------------- MNI Header -----------------------------------
@NAME       : read_gyro
@INPUT      : filename - name of gyrocom file to read
              max_group - maximum group number to read
@OUTPUT     : (none)
@RETURNS    : group list read in from file
@DESCRIPTION: Routine to read in a group list from a file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 25, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Group read_gyro(char *filename, int max_group)
{
   FILE *fp;
   Acr_File *afp;
   Acr_Group group_list;

   /* Open the file */
   fp = fopen(filename, "r");
   if (fp == NULL) return NULL;

   /* Connect to input stream */
   afp=acr_file_initialize(fp, 0, acr_stdio_read);
   (void) acr_test_byte_ordering(afp);

   /* Read in group list */
   (void) acr_input_group_list(afp, &group_list, max_group);

   /* Close the file */
   acr_file_free(afp);
   (void) fclose(fp);

   return group_list;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_file_info
@INPUT      : group_list - input data
@OUTPUT     : file_info - file-specific info
              general_info - general information about files
@RETURNS    : (nothing)
@DESCRIPTION: Routine to extract information from a group list
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 25, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void get_file_info(Acr_Group group_list, File_Info *file_info,
                          General_Info *general_info)
{
   Acr_Element element;
   int data_set_type;
   Mri_Index imri;
   World_Index iworld, jworld;
   char *string, *end;
   int nrows;
   int ncolumns;
   int image_group;
   int length, maxlen;
   int cur_index;
   int index;
   double default_slice_pos;
   double orientation_factor;
   Acr_Element_Id elid;
   Acr_Element_Id mri_index_list[MRI_NDIMS];
   Acr_Element_Id mri_total_list[MRI_NDIMS];

   /* Array of elements for mri dimensions */
   mri_index_list[SLICE] = SPI_Slice_number;
   mri_index_list[ECHO] = SPI_Echo_number;
   mri_index_list[TIME] = SPI_Dynamic_scan_number;
   mri_index_list[PHASE] = SPI_Phase_number;
   mri_index_list[CHEM_SHIFT] = SPI_Chemical_shift_number;
   mri_total_list[SLICE] = SPI_Number_of_slices;
   mri_total_list[ECHO] = SPI_Number_of_echoes;
   mri_total_list[TIME] = SPI_Number_of_dynamic_scans;
   mri_total_list[PHASE] = SPI_Number_of_phases;
   mri_total_list[CHEM_SHIFT] = SPI_Nr_of_chemical_shifts;

   /* Look for data set type */
   element = acr_find_group_element(group_list, ACR_Data_set_type);
   if (element != NULL)
      data_set_type = acr_get_element_short(element);

   if ((element == NULL) || (data_set_type != ACR_IMAGE_OBJECT)) {
      file_info->valid = FALSE;
      return;
   }

   /* Get stuff that must be in file */
   file_info->bits_alloc = find_short(group_list, ACR_Bits_allocated, 0);
   file_info->bits_stored = find_short(group_list, ACR_Bits_stored, 0);
   nrows = find_short(group_list, ACR_Rows, 0);
   ncolumns = find_short(group_list, ACR_Columns, 0);
   if (nrows <= 0)
      nrows = find_int(group_list, SPI_Recon_resolution, 0);
   if (ncolumns <= 0)
      ncolumns = find_int(group_list, SPI_Recon_resolution, 0);
   image_group = find_short(group_list, ACR_Image_location, INT_MIN);

   /* Check for necessary values not found */
   if ((nrows <= 0) || (ncolumns <= 0) ||
       (file_info->bits_stored <= 0) ||
       (file_info->bits_alloc <= 0) || 
       (image_group < 0)) {
      file_info->valid = FALSE;
      return;
   }

   /* Get pixel value information */
   file_info->pixel_min = find_short(group_list, ACR_Smallest_pixel_value, 0);
   file_info->pixel_max = find_short(group_list, ACR_Largest_pixel_value, 
                                     (1 << file_info->bits_stored) - 1);
   file_info->slice_min = find_double(group_list, SPI_Ext_scale_minimum,
                                      (double) file_info->pixel_min);
   file_info->slice_max = find_double(group_list, SPI_Ext_scale_maximum, 
                                      (double) file_info->pixel_max);

   for (imri=0; imri < MRI_NDIMS; imri++) {
      file_info->index[imri] = 
         find_int(group_list, mri_index_list[imri], 1) - 1;
      if (file_info->index[imri] < 0)
         file_info->index[imri] = 0;
   }

   /* Set up general info on first pass */
   if (!general_info->initialized) {

      /* Get row and columns sizes */
      general_info->nrows = nrows;
      general_info->ncolumns = ncolumns;

      /* Get dimension information */
      for (imri=0; imri < MRI_NDIMS; imri++) {
         general_info->size[imri] = 1;
         general_info->first[imri] = file_info->index[imri];
         general_info->total_size[imri] = 
            find_int(group_list, mri_total_list[imri], 1);
         if (general_info->total_size[imri] < 1)
            general_info->total_size[imri] = 1;
         general_info->position[imri] = NULL;
         general_info->image_index[imri] = -1;
      }

      /* Get spatial orientation */
      switch (find_int(group_list, SPI_Slice_orientation, 0)) {
      case SPI_SAGITTAL_ORIENTATION:
         general_info->orientation = SAGITTAL;
         general_info->slice_world = XCOORD;
         general_info->row_world = ZCOORD;
         general_info->column_world = YCOORD;
         break;
      case SPI_CORONAL_ORIENTATION:
         general_info->orientation = CORONAL;
         general_info->slice_world = YCOORD;
         general_info->row_world = ZCOORD;
         general_info->column_world = XCOORD;
         break;
      case SPI_TRANSVERSE_ORIENTATION:
      default:
         general_info->orientation = TRANSVERSE;
         general_info->slice_world = ZCOORD;
         general_info->row_world = YCOORD;
         general_info->column_world = XCOORD;
         break;
      }

      /* Get step information. Use field-of-view since pixel size doesn't
         seem to be correct */
#if 1
      string = find_string(group_list, SPI_Field_of_view, "");
      general_info->step[general_info->column_world] = strtod(string, &end);
      if (end != string)
         general_info->step[general_info->column_world] /= (double) nrows;
      else
         general_info->step[general_info->column_world] = 1.0;
      general_info->step[general_info->row_world] = 
         general_info->step[general_info->column_world];
#else      
      string = find_string(group_list, ACR_Pixel_size, "");
      general_info->step[general_info->column_world] = strtod(string, &end);
      if (end == string) {
         general_info->step[general_info->column_world] = 1.0;
      }
      else {
         string = end;
         if ((*string == '\\') || (*string == ',')) string++;
         general_info->step[general_info->row_world] = strtod(string, &end);
         if (end == string)
            general_info->step[general_info->row_world] = 1.0;
      }
#endif
      general_info->step[general_info->slice_world] = 
         find_double(group_list, ACR_Slice_thickness, 1.0);
      general_info->step[XCOORD] *= SPI_X_ORIENTATION;
      general_info->step[YCOORD] *= SPI_Y_ORIENTATION;
      general_info->step[ZCOORD] *= SPI_Z_ORIENTATION;

      /* Get start information */
      general_info->start[XCOORD] = SPI_X_ORIENTATION *
         find_double(group_list, SPI_Off_center_lr, 0.0);
      general_info->start[YCOORD] = SPI_Y_ORIENTATION *
         find_double(group_list, SPI_Off_center_ap, 0.0);
      general_info->start[ZCOORD] = SPI_Z_ORIENTATION *
         find_double(group_list, SPI_Off_center_cc, 0.0);

      /* Set direction cosines to default for now */
      for (iworld=0; iworld < WORLD_NDIMS; iworld++) {
         for (jworld=0; jworld < WORLD_NDIMS; jworld++) {
            if (iworld == jworld)
               general_info->dircos[iworld][jworld] = 1.0;
            else
               general_info->dircos[iworld][jworld] = 0.0;
         }
      }

      /* Save slice step info */
      iworld = general_info->slice_world;
      general_info->slice_start = general_info->start[iworld];
      general_info->slice_step = general_info->step[iworld];

      /* Set data type and range */
      if (file_info->bits_alloc <= 8)
         general_info->datatype = NC_BYTE;
      else
         general_info->datatype = NC_SHORT;
      general_info->is_signed = ((general_info->datatype == NC_SHORT) &&
                                 (file_info->bits_stored < 16));
      general_info->pixel_min = file_info->pixel_min;
      general_info->pixel_max = file_info->pixel_max;

      /* Keep track of range of slices */
      general_info->slicepos_min = DBL_MAX;
      general_info->slicepos_max = -DBL_MAX;

      /* Maximum length for strings */
      maxlen = sizeof(Cstring) - 1;

      /* Get intensity units */
      (void) strncpy(general_info->units,
              find_string(group_list, SPI_Ext_scale_units, ""), maxlen);

      /* Get patient info */
      (void) strncpy(general_info->patient.name,
              find_string(group_list, ACR_Patient_name, ""), maxlen);
      (void) strncpy(general_info->patient.identification,
              find_string(group_list, ACR_Patient_identification, ""), maxlen);
      (void) strncpy(general_info->patient.birth_date,
              find_string(group_list, ACR_Patient_birth_date, ""), maxlen);
      string = find_string(group_list, ACR_Patient_sex, "");
      if (*string == 'M') 
         (void) strncpy(general_info->patient.sex, MI_MALE, maxlen);
      else if (*string == 'F') 
         (void) strncpy(general_info->patient.sex, MI_FEMALE, maxlen);
      else if (*string == 'O') 
         (void) strncpy(general_info->patient.sex, MI_OTHER, maxlen);
      else 
         (void) strncpy(general_info->patient.sex, "", maxlen);
      general_info->patient.weight = 
         find_double(group_list, ACR_Patient_weight, -DBL_MAX);

      /* Get study info */
      (void) strncpy(general_info->study.start_time, 
              find_string(group_list, ACR_Study_date, ""), maxlen - 1);
      length = strlen(general_info->study.start_time);
      general_info->study.start_time[length] = ' ';
      length++;
      (void) strncpy(&general_info->study.start_time[length], 
              find_string(group_list, ACR_Study_time, ""), maxlen - length);
      string = find_string(group_list, ACR_Modality, "");
      if (strcmp(string, ACR_MODALITY_MR) == 0)
         (void) strncpy(general_info->study.modality, MI_MRI, maxlen);
      (void) strncpy(general_info->study.manufacturer, 
              find_string(group_list, ACR_Manufacturer, ""), maxlen);
      (void) strncpy(general_info->study.model, 
              find_string(group_list, ACR_Manufacturer_model, ""), maxlen);
      (void) strncpy(general_info->study.institution, 
              find_string(group_list, ACR_Institution_id, ""), maxlen);
      (void) strncpy(general_info->study.station_id, 
              find_string(group_list, ACR_Station_id, ""), maxlen);
      (void) strncpy(general_info->study.ref_physician, 
              find_string(group_list, ACR_Referring_physician, ""), maxlen);
      (void) strncpy(general_info->study.procedure, 
              find_string(group_list, ACR_Procedure_description, ""), maxlen);
      (void) sprintf(general_info->study.study_id, "%d",
                     find_int(group_list, ACR_Study, 0));
      (void) sprintf(general_info->study.acquisition_id, "%d",
                     find_int(group_list, ACR_Acquisition, 0));

      /* Get acquisition information */
      (void) strncpy(general_info->acq.scan_seq,
              find_string(group_list, ACR_Scanning_sequence, ""), maxlen);
      general_info->acq.rep_time = 
         find_double(group_list, ACR_Repetition_time, -DBL_MAX);
      general_info->acq.echo_time = 
         find_double(group_list, ACR_Echo_time, -DBL_MAX);
      general_info->acq.inv_time = 
         find_double(group_list, ACR_Inversion_time, -DBL_MAX);
      general_info->acq.num_avg = 
         find_double(group_list, ACR_Nr_of_averages, -DBL_MAX);
      general_info->acq.imaging_freq = 
         find_double(group_list, ACR_Imaging_frequency, -DBL_MAX);
      (void) strncpy(general_info->acq.imaged_nucl,
         find_string(group_list, ACR_Imaged_nucleus, ""), maxlen);

      /* Copy the group list */
      general_info->group_list = acr_copy_group_list(group_list);

      /* Set initialized flag */
      general_info->initialized = TRUE;
   }           /* Set up file info */

   /* Update general info and validate file on later passes */
   else {

      /* Check for consistent data type */
      if (((general_info->datatype == NC_BYTE) &&
           (file_info->bits_alloc > 8)) || 
          ((general_info->datatype == NC_SHORT) &&
           (file_info->bits_alloc <= 8))) {
         file_info->valid = FALSE;
         return;
      }

      /* Check row and columns sizes */
      if ((nrows != general_info->nrows) &&
          (ncolumns != general_info->ncolumns))  {
         file_info->valid = FALSE;
         return;
      }

      /* Check indices for range */
      for (imri=0; imri < MRI_NDIMS; imri++) {
         cur_index = file_info->index[imri];
         if ((cur_index < 0) || 
             (cur_index >= general_info->total_size[imri])) {
            file_info->valid = FALSE;
            return;
         }
      }

      /* Look to see if indices have changed */
      for (imri=0; imri < MRI_NDIMS; imri++) {

         /* Get current index */
         cur_index = file_info->index[imri];

         /* If we only have one index and this index is different, then 
            allocate a list */
         if ((general_info->size[imri] == 1) && 
             (cur_index != general_info->first[imri])) {
            general_info->position[imri] = 
               MALLOC(general_info->total_size[imri] * sizeof(int));
            for (index=0; index < general_info->total_size[imri]; index++)
               general_info->position[imri][index] = -1;
            general_info->position[imri][general_info->first[imri]] = 0;
            general_info->position[imri][cur_index] = 0;
            if (cur_index < general_info->first[imri])
               general_info->first[imri] = cur_index;
            general_info->size[imri]++; 
        }

         /* If we have more than one index and this one has not been found
            yet, then set it to true */
         else if ((general_info->size[imri] > 1) &&
                  (general_info->position[imri][cur_index] < 0)) {
            general_info->position[imri][cur_index] = 0;
            if (cur_index < general_info->first[imri])
               general_info->first[imri] = cur_index;
            general_info->size[imri]++;
         }

         /* Update position list */
         if (general_info->position[imri] != NULL) {
            cur_index = 0;
            for (index=0; index < general_info->total_size[imri]; index++) {
               if (general_info->position[imri][index] >= 0) {
                  general_info->position[imri][index] = cur_index;
                  cur_index++;
               }
            }
         }

      }              /* Loop over Mri_Index */

   }              /* Update general info for this file */

   /* Get other slice specific information */
   iworld = general_info->slice_world;
   switch (general_info->slice_world) {
   case XCOORD: 
      elid = SPI_Off_center_lr; 
      orientation_factor = SPI_X_ORIENTATION;
      break;
   case YCOORD: 
      elid = SPI_Off_center_ap; 
      orientation_factor = SPI_Y_ORIENTATION;
      break;
   case ZCOORD: 
      elid = SPI_Off_center_cc; 
      orientation_factor = SPI_Z_ORIENTATION; 
      break;
   }
   orientation_factor = 1.0;
   default_slice_pos = general_info->slice_start + 
      file_info->index[SLICE] * general_info->slice_step;
   default_slice_pos *= orientation_factor;
   file_info->slice_position = orientation_factor * 
      find_double(group_list, elid, default_slice_pos);
   file_info->dyn_begin_time = 
      find_double(group_list, SPI_dynamic_scan_begin_time, 0.0);

   /* Update slice position information for general_info */
   if (file_info->slice_position < general_info->slicepos_min)
      general_info->slicepos_min = file_info->slice_position;
   if (file_info->slice_position > general_info->slicepos_max)
      general_info->slicepos_max = file_info->slice_position;
   if (general_info->size[SLICE] > 1) {
      general_info->step[iworld] = orientation_factor *
         (general_info->slicepos_max - general_info->slicepos_min) /
            (general_info->size[SLICE] - 1);
   }
   general_info->start[iworld] = general_info->slicepos_min;

   /* If we get to here, then we have a valid file */
   file_info->valid = TRUE;

   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_gyro_image
@INPUT      : group_list - input data
@OUTPUT     : image - image data structure (user must free data)
@RETURNS    : (nothing)
@DESCRIPTION: Routine to get an image from a group list
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 25, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void get_gyro_image(Acr_Group group_list, Image_Data *image)
{
   Acr_Element element;
   int nrows, ncolumns;
   int bits_alloc;
   int bits_stored;
   int image_group;
   void *data = NULL;
   long imagepix, ipix;
   struct Acr_Element_Id elid;
   nc_type datatype;

   /* Get the image information */
   bits_alloc = find_short(group_list, ACR_Bits_allocated, 0);
   bits_stored = find_short(group_list, ACR_Bits_stored, 0);
   nrows = find_short(group_list, ACR_Rows, 0);
   ncolumns = find_short(group_list, ACR_Columns, 0);
   if (nrows <= 0)
      nrows = find_int(group_list, SPI_Recon_resolution, 0);
   if (ncolumns <= 0)
      ncolumns = find_int(group_list, SPI_Recon_resolution, 0);
   image_group = find_short(group_list, ACR_Image_location, INT_MIN);

   /* Figure out type */
   if (bits_alloc > CHAR_BIT)
      datatype = NC_SHORT;
   else 
      datatype = NC_BYTE;

   /* Set image info */
   image->nrows = nrows;
   image->ncolumns = ncolumns;
   imagepix = nrows * ncolumns;
   image->data = (unsigned short *) MALLOC(imagepix * sizeof(short));
   image->free = TRUE;

   /* Get image pointer */
   elid.group_id = image_group;
   elid.element_id = SPI_IMAGE_ELEMENT;
   element = acr_find_group_element(group_list, &elid);
   if (element == NULL) {
      (void) memset(image->data, 0, imagepix * sizeof(short));
      return;
   }
   data = acr_get_element_data(element);

   /* Convert the data according to type */
   if (datatype == NC_BYTE) {
      for (ipix=0; ipix < imagepix; ipix++) {
         image->data[ipix] = *((unsigned char *) data + ipix);
      }
   }
   else {
      if (bits_alloc != nctypelen(datatype) * CHAR_BIT) {
         (void) memset(image->data, 0, imagepix * sizeof(short));
         return;
      }
      acr_get_short(nrows*ncolumns, data, image->data);
   }

   return;
}

public int find_short(Acr_Group group_list, Acr_Element_Id elid, 
                      int default_value)
{
   Acr_Element element;

   element = acr_find_group_element(group_list, elid);
   if (element != NULL) 
      return (int) acr_get_element_short(element);
   else
      return default_value;
}

public int find_int(Acr_Group group_list, Acr_Element_Id elid, 
                    int default_value)
{
   Acr_Element element;

   element = acr_find_group_element(group_list, elid);
   if (element != NULL) 
      return (int) acr_get_element_numeric(element);
   else
      return default_value;
}

public double find_double(Acr_Group group_list, Acr_Element_Id elid, 
                          double default_value)
{
   Acr_Element element;

   element = acr_find_group_element(group_list, elid);
   if (element != NULL) 
      return acr_get_element_numeric(element);
   else
      return default_value;
}

public char *find_string(Acr_Group group_list, Acr_Element_Id elid, 
                         char *default_value)
{
   Acr_Element element;

   element = acr_find_group_element(group_list, elid);
   if (element != NULL) 
      return acr_get_element_string(element);
   else
      return default_value;
}
