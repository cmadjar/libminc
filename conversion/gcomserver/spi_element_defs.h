/* ----------------------------- MNI Header -----------------------------------
@NAME       : spi_element_defs.h
@DESCRIPTION: Element definitions for spi
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 23, 1993 (Peter Neelin)
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

/* Element id's for ACR-NEMA */
GLOBAL_ELEMENT(ACR_Length_to_eom   , 0x0000, 0x0001);
GLOBAL_ELEMENT(ACR_Recognition_code, 0x0000, 0x0010);
GLOBAL_ELEMENT(ACR_Command         , 0x0000, 0x0100);
GLOBAL_ELEMENT(ACR_Message_id      , 0x0000, 0x0110);
GLOBAL_ELEMENT(ACR_Message_id_brt  , 0x0000, 0x0120);
GLOBAL_ELEMENT(ACR_Initiator       , 0x0000, 0x0200);
GLOBAL_ELEMENT(ACR_Receiver        , 0x0000, 0x0300);
GLOBAL_ELEMENT(ACR_Dataset_type    , 0x0000, 0x0800);
GLOBAL_ELEMENT(ACR_Status          , 0x0000, 0x0900);
GLOBAL_ELEMENT(ACR_Data_set_type   , 0x0008, 0x0040);
GLOBAL_ELEMENT(ACR_Data_set_subtype, 0x0008, 0x0041);
GLOBAL_ELEMENT(ACR_Patient_name    , 0x0010, 0x0010);
GLOBAL_ELEMENT(ACR_Study           , 0x0020, 0x0010);
GLOBAL_ELEMENT(ACR_Acquisition     , 0x0020, 0x0012);
GLOBAL_ELEMENT(ACR_Image           , 0x0020, 0x0013);
GLOBAL_ELEMENT(ACR_Image_location  , 0x0028, 0x0200);

GLOBAL_ELEMENT(ACR_Study_date            , 0x0008, 0x0020);
GLOBAL_ELEMENT(ACR_Study_time            , 0x0008, 0x0030);
GLOBAL_ELEMENT(ACR_Modality              , 0x0008, 0x0060);
GLOBAL_ELEMENT(ACR_Manufacturer          , 0x0008, 0x0070);
GLOBAL_ELEMENT(ACR_Institution_id        , 0x0008, 0x0080);
GLOBAL_ELEMENT(ACR_Referring_physician   , 0x0008, 0x0090);
GLOBAL_ELEMENT(ACR_Station_id            , 0x0008, 0x1010);
GLOBAL_ELEMENT(ACR_Procedure_description , 0x0008, 0x1030);
GLOBAL_ELEMENT(ACR_Manufacturer_model    , 0x0008, 0x1090);
GLOBAL_ELEMENT(ACR_Patient_identification, 0x0010, 0x0020);
GLOBAL_ELEMENT(ACR_Patient_birth_date    , 0x0010, 0x0030);
GLOBAL_ELEMENT(ACR_Patient_sex           , 0x0010, 0x0040);
GLOBAL_ELEMENT(ACR_Patient_weight        , 0x0010, 0x1030);
GLOBAL_ELEMENT(ACR_Scanning_sequence     , 0x0018, 0x0020);
GLOBAL_ELEMENT(ACR_Slice_thickness       , 0x0018, 0x0050);
GLOBAL_ELEMENT(ACR_Repetition_time       , 0x0018, 0x0080);
GLOBAL_ELEMENT(ACR_Echo_time             , 0x0018, 0x0081);
GLOBAL_ELEMENT(ACR_Inversion_time        , 0x0018, 0x0082);
GLOBAL_ELEMENT(ACR_Nr_of_averages        , 0x0018, 0x0083);
GLOBAL_ELEMENT(ACR_Imaging_frequency     , 0x0018, 0x0084);
GLOBAL_ELEMENT(ACR_Imaged_nucleus        , 0x0018, 0x0085);
GLOBAL_ELEMENT(ACR_Rows                  , 0x0028, 0x0010);
GLOBAL_ELEMENT(ACR_Columns               , 0x0028, 0x0011);
GLOBAL_ELEMENT(ACR_Pixel_size            , 0x0028, 0x0030);
GLOBAL_ELEMENT(ACR_Bits_allocated        , 0x0028, 0x0100);
GLOBAL_ELEMENT(ACR_Bits_stored           , 0x0028, 0x0101);
GLOBAL_ELEMENT(ACR_Smallest_pixel_value  , 0x0028, 0x0104);
GLOBAL_ELEMENT(ACR_Largest_pixel_value   , 0x0028, 0x0105);

/* Element id's for SPI */
GLOBAL_ELEMENT(SPI_Recognition_code, 0x0001, 0x0010);
GLOBAL_ELEMENT(SPI_Command         , 0x0001, 0x0018);
GLOBAL_ELEMENT(SPI_Status          , 0x0001, 0x0019);
GLOBAL_ELEMENT(SPI_Session_id      , 0x0001, 0x0028);
GLOBAL_ELEMENT(SPI_Dataset_type    , 0x0001, 0x0040);
GLOBAL_ELEMENT(SPI_Delay_time      , 0x0001, 0x0060);
GLOBAL_ELEMENT(SPI_Nr_data_objects , 0x0001, 0x0081);
GLOBAL_ELEMENT(SPI_Operator_text   , 0x0001, 0x1060);
GLOBAL_ELEMENT(SPI_Log_info        , 0x0001, 0x1070);
GLOBAL_ELEMENT(SPI_Volume_name     , 0x0001, 0x1080);
GLOBAL_ELEMENT(SPI_Creation_date   , 0x0001, 0x1081);
GLOBAL_ELEMENT(SPI_Creation_time   , 0x0001, 0x1082);
GLOBAL_ELEMENT(SPI_Volume_type     , 0x0001, 0x1083);
GLOBAL_ELEMENT(SPI_Volume_status   , 0x0001, 0x1084);
GLOBAL_ELEMENT(SPI_Space_left      , 0x0001, 0x1085);
GLOBAL_ELEMENT(SPI_Nr_exams        , 0x0001, 0x1086);
GLOBAL_ELEMENT(SPI_Nr_images       , 0x0001, 0x1087);
GLOBAL_ELEMENT(SPI_Closing_date    , 0x0001, 0x1088);
GLOBAL_ELEMENT(SPI_Perc_space_used , 0x0001, 0x1089);

GLOBAL_ELEMENT(SPI_Field_of_view           , 0x0019, 0x1000);
GLOBAL_ELEMENT(SPI_Angulation_of_cc_axis   , 0x0019, 0x1005);
GLOBAL_ELEMENT(SPI_Angulation_of_ap_axis   , 0x0019, 0x1006);
GLOBAL_ELEMENT(SPI_Angulation_of_lr_axis   , 0x0019, 0x1007);
GLOBAL_ELEMENT(SPI_Slice_orientation       , 0x0019, 0x100a);
GLOBAL_ELEMENT(SPI_Off_center_lr           , 0x0019, 0x100b);
GLOBAL_ELEMENT(SPI_Off_center_cc           , 0x0019, 0x100c);
GLOBAL_ELEMENT(SPI_Off_center_ap           , 0x0019, 0x100d);
GLOBAL_ELEMENT(SPI_Number_of_slices        , 0x0019, 0x100f);
GLOBAL_ELEMENT(SPI_Number_of_dynamic_scans , 0x0019, 0x101b);
GLOBAL_ELEMENT(SPI_dynamic_scan_begin_time , 0x0019, 0x1022);
GLOBAL_ELEMENT(SPI_Number_of_phases        , 0x0019, 0x1069);
GLOBAL_ELEMENT(SPI_Nr_of_chemical_shifts   , 0x0019, 0x1080);
GLOBAL_ELEMENT(SPI_Number_of_echoes        , 0x0019, 0x10cf);
GLOBAL_ELEMENT(SPI_Recon_resolution        , 0x0019, 0x1145);
GLOBAL_ELEMENT(SPI_Slice_number            , 0x0021, 0x1020);
GLOBAL_ELEMENT(SPI_Echo_number             , 0x0021, 0x1030);
GLOBAL_ELEMENT(SPI_Chemical_shift_number   , 0x0021, 0x1035);
GLOBAL_ELEMENT(SPI_Phase_number            , 0x0021, 0x1040);
GLOBAL_ELEMENT(SPI_Dynamic_scan_number     , 0x0021, 0x1050);
GLOBAL_ELEMENT(SPI_Ext_scale_minimum       , 0x0029, 0x1010);
GLOBAL_ELEMENT(SPI_Ext_scale_maximum       , 0x0029, 0x1011);
GLOBAL_ELEMENT(SPI_Ext_scale_units         , 0x0029, 0x1020);
GLOBAL_ELEMENT(SPI_Fp_min                  , 0x0029, 0x1110);
GLOBAL_ELEMENT(SPI_Fp_max                  , 0x0029, 0x1120);

/* Define group numbers for message stuff */
#define ACR_MESSAGE_GID 0
#define SPI_MESSAGE_GID 1
#define ACR_IDENTIFYING_GID 0x0008
#define ACR_IMAGE_GID 0x0028
#define SPI_ACTUAL_IMAGE_GID 0x7fe0
#define SPI_SCANDATA_GID 0x8001
#define SPI_SPECTRADATA_GID 0x8101
#define SPI_RAWDATA_GID 0x8201
#define SPI_LABELDATA_GID 0x8301

/* Element number for images */
#define SPI_IMAGE_ELEMENT 0x0010

/* Define recognition codes */
#define ACR_RECOGNITION_CODE "ACR-NEMA 1.0"
#define SPI_RECOGNITION_CODE "SPI Release 1 "

/* Define commands */
#define ACR_UNKNOWN_COMMAND 0x0000
#define SENDq               0x0001
#define SENDp               0x8001
#define CANCELq             0x0FFF
#define CANCELp             0x8FFF
#define SPI_UNKNOWN_COMMAND 0x0000
#define GCBEGINq            0x0028
#define GCBEGINp            0x8028
#define READYq              0x0008
#define READYp              0x8008
#define GCENDq              0x0029
#define GCENDp              0x8029

/* Define dataset type */
#define ACR_NULL_DATASET 0x0101

/* Define status codes */
#define ACR_SUCCESS 0x0000

/* Define data object types */
#define ACR_IMAGE_OBJECT 0x0000
#define ACR_OTHER_OBJECT 0x0100

/* Define data object sub-types */
#define SPI_SCANDATA_OBJECT    "SCAN_DATA"
#define SPI_SPECTRADATA_OBJECT "SPECTRA_DATA"
#define SPI_RAWDATA_OBJECT     "RAW_DATA"
#define SPI_LABELDATA_OBJECT   "LAB_DATA"

/* Define acr-nema constants */
#define ACR_MODALITY_MR "MR"

/* Define spi constants */
#define SPI_TRANSVERSE_ORIENTATION 1
#define SPI_SAGITTAL_ORIENTATION   2
#define SPI_CORONAL_ORIENTATION    3
#define SPI_X_ORIENTATION (-1)
#define SPI_Y_ORIENTATION (-1)
#define SPI_Z_ORIENTATION (-1)
