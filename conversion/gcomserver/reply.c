/* ----------------------------- MNI Header -----------------------------------
@NAME       : reply.c
@DESCRIPTION: Routines for dealing with spi messages.
@GLOBALS    : 
@CREATED    : November 22, 1993 (Peter Neelin)
@MODIFIED   : $Log: reply.c,v $
@MODIFIED   : Revision 1.3  1993-11-25 13:26:58  neelin
@MODIFIED   : Working version.
@MODIFIED   :
 * Revision 1.2  93/11/24  12:09:32  neelin
 * Changed to use new acr-nema dump function. spi_reply returns a group list
 * instead of a message.
 * 
 * Revision 1.1  93/11/23  14:12:06  neelin
 * Initial revision
 * 
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

extern int Do_logging;

/* ----------------------------- MNI Header -----------------------------------
@NAME       : make_message
@INPUT      : group_list
@OUTPUT     : (nothing)
@RETURNS    : output message.
@DESCRIPTION: Convert a group list into a message.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 24, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private Acr_Message make_message(Acr_Group group_list)
{
   Acr_Group next_group, group;
   Acr_Message output_message;

   /* Create the output message */
   output_message = acr_create_message();

   /* Loop through groups, adding them to the message */
   group = group_list;
   while (group != NULL) {
      next_group = acr_get_group_next(group);
      acr_set_group_next(group, NULL);
      acr_message_add_group(output_message, group);
      group = next_group;
   }

   return output_message;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : spi_reply
@INPUT      : input_message
@OUTPUT     : (nothing)
@RETURNS    : group_list for output message.
@DESCRIPTION: Composes reply for SPI messages
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 22, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private Acr_Group spi_reply(Acr_Message input_message)
{
   Acr_Group group, group_list;
   Acr_Element element;
   unsigned short his_message_id = 0;
   static unsigned short my_message_id = 0;
   char *his_station_id = "CANPHIMR000000";
   char *my_station_id =  "CANMNIPT010101";
   unsigned short my_session_id = 0;
   int acr_command = ACR_UNKNOWN_COMMAND;
   int spi_command = SPI_UNKNOWN_COMMAND;

   /* Get the command */
   group = acr_get_message_group_list(input_message);
   element = acr_find_group_element(group, ACR_Command);
   if (element != NULL) 
      acr_command = acr_get_element_short(element);
   element = acr_find_group_element(group, SPI_Command);
   if (element != NULL) 
      spi_command = acr_get_element_short(element);

   /* Get the appropriate reply command */
   if (acr_command == CANCELq) {
      acr_command = CANCELp;
   }
   else {
      acr_command = SENDp;
      switch (spi_command) {
      case GCBEGINq:
         spi_command = GCBEGINp; break;
      case READYq:
         spi_command = READYp; break;
      case SENDq:
         spi_command = SENDp; break;
      case GCENDq:
         spi_command = GCENDp; break;
      default:
         spi_command = SENDp; break;
      }
   }

   /* Get information from input_message */
   element = acr_find_group_element(group, ACR_Message_id);
   if (element != NULL)
      his_message_id = acr_get_element_short(element);
   element = acr_find_group_element(group, ACR_Initiator);
   if (element != NULL)
      his_station_id = acr_get_element_string(element);

   /* Compose the reply. Increment message id number and increment
      session id number if GCBEGINp */
   my_message_id++;
   if ((acr_command == SENDp) && (spi_command == GCBEGINp))
      my_session_id++;

   /* Acr-nema group */
   group_list = group = acr_create_group(ACR_MESSAGE_GID);
   acr_group_add_element(group,
      acr_create_element_long(ACR_Length_to_eom, (unsigned short) 0));
   acr_group_add_element(group,
      acr_create_element_string(ACR_Recognition_code, ACR_RECOGNITION_CODE));
   acr_group_add_element(group,
      acr_create_element_short(ACR_Command, (unsigned short) acr_command));
   acr_group_add_element(group,
      acr_create_element_short(ACR_Message_id, my_message_id));
   acr_group_add_element(group,
      acr_create_element_short(ACR_Message_id_brt, his_message_id));
   acr_group_add_element(group,
      acr_create_element_string(ACR_Initiator, my_station_id));
   acr_group_add_element(group,
      acr_create_element_string(ACR_Receiver, his_station_id));
   acr_group_add_element(group,
      acr_create_element_short(ACR_Dataset_type,
                               (unsigned short) ACR_NULL_DATASET));
   acr_group_add_element(group,
      acr_create_element_short(ACR_Status, 
                               (unsigned short) ACR_SUCCESS));

   /* SPI group */
   if (acr_command == SENDp) {
      acr_set_group_next(group, acr_create_group(SPI_MESSAGE_GID));
      group = acr_get_group_next(group);
      acr_group_add_element(group,
         acr_create_element_string(SPI_Recognition_code, 
                                   SPI_RECOGNITION_CODE));
      acr_group_add_element(group,
         acr_create_element_short(SPI_Command, (unsigned short) spi_command));
      acr_group_add_element(group,
         acr_create_element_long(SPI_Status, (long) ACR_SUCCESS));
      acr_group_add_element(group,
         acr_create_element_short(SPI_Session_id, my_session_id));
      acr_group_add_element(group,
         acr_create_element_short(SPI_Dataset_type,
                                  (unsigned short) ACR_NULL_DATASET));
   }

   return group_list;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : gcbegin_reply
@INPUT      : input_message
@OUTPUT     : num_files - number of files needed
@RETURNS    : output_message
@DESCRIPTION: Responds to GCBEGINq message
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 22, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Message gcbegin_reply(Acr_Message input_message, int *num_files)
{
   Acr_Group group, group_list;
   Acr_Element element;

   /* Print log message */
   if (Do_logging >= HIGH_LOGGING) {
      (void) fprintf(stderr, "\n\nReceived GCBEGINq message:\n");
      acr_dump_message(stderr, input_message);
   }

   /* Get the group list */
   group = acr_get_message_group_list(input_message);

   /* Get the number of files */
   element = acr_find_group_element(group, SPI_Nr_data_objects);
   if (element != NULL)
      *num_files = acr_get_element_short(element);
   else
      *num_files = 1;

   /* Create the reply */
   group_list = spi_reply(input_message);

   /* Add to the reply */
   group = acr_get_group_next(group_list);
   acr_group_add_element(group,
      acr_create_element_short(SPI_Delay_time, (unsigned short) 0));
   acr_group_add_element(group,
      acr_create_element_short(SPI_Nr_data_objects, *num_files));
   acr_group_add_element(group,
      acr_create_element_string(SPI_Operator_text, "OK"));
   acr_group_add_element(group,
      acr_create_element_string(SPI_Log_info, "OK"));
   acr_group_add_element(group,
      acr_create_element_string(SPI_Volume_name, "PET"));
   acr_group_add_element(group,
      acr_create_element_string(SPI_Creation_date, "1993.11.23"));
   acr_group_add_element(group,
      acr_create_element_string(SPI_Creation_time, "10:00:0000"));
   acr_group_add_element(group,
      acr_create_element_string(SPI_Volume_type, "FOREIGN"));
   acr_group_add_element(group,
      acr_create_element_string(SPI_Volume_status, "OPEN"));
   acr_group_add_element(group,
      acr_create_element_numeric(SPI_Space_left, (double) 1000000));
   acr_group_add_element(group,
      acr_create_element_numeric(SPI_Nr_exams, (double) 10));
   acr_group_add_element(group,
      acr_create_element_numeric(SPI_Nr_images, (double) 200));
   acr_group_add_element(group,
      acr_create_element_string(SPI_Closing_date, "2010.11.23"));
   acr_group_add_element(group,
      acr_create_element_numeric(SPI_Perc_space_used, (double) 20));

   return make_message(group_list);

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : ready_reply
@INPUT      : input_message
@OUTPUT     : (nothing)
@RETURNS    : output_message
@DESCRIPTION: Responds to READYq message
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 22, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Message ready_reply(Acr_Message input_message)
{
   Acr_Group group_list, group;

   /* Print log message */
   if (Do_logging >= HIGH_LOGGING) {
      (void) fprintf(stderr, "\n\nReceived READYq message:\n");
      acr_dump_message(stderr, input_message);
   }

   /* Create the reply */
   group_list = spi_reply(input_message);

   /* Add to the reply */
   group = acr_get_group_next(group_list);
   acr_group_add_element(group,
      acr_create_element_short(SPI_Delay_time, (unsigned short) 0));
   acr_group_add_element(group,
      acr_create_element_string(SPI_Operator_text, "OK"));
   acr_group_add_element(group,
      acr_create_element_string(SPI_Log_info, "OK"));

   return make_message(group_list);

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : send_reply
@INPUT      : input_message
              file_prefix
@OUTPUT     : new_file_name (must be freed by caller)
@RETURNS    : output_message
@DESCRIPTION: Responds to SENDq message
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 22, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Message send_reply(Acr_Message input_message)
{
   Acr_Group group_list, group;

   /* Print log message */
   if (Do_logging >= HIGH_LOGGING) {
      (void) fprintf(stderr, "\n\nReceived SENDq message:\n");
      acr_dump_message(stderr, input_message);
   }

   /* Create the reply */
   group_list = spi_reply(input_message);

   /* Add to the reply */
   group = acr_get_group_next(group_list);
   acr_group_add_element(group,
      acr_create_element_string(SPI_Operator_text, "OK"));
   acr_group_add_element(group,
      acr_create_element_string(SPI_Log_info, "OK"));

   return make_message(group_list);

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : gcend_reply
@INPUT      : input_message
@OUTPUT     : (nothing)
@RETURNS    : output_message
@DESCRIPTION: Responds to GCENDq message
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 22, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Message gcend_reply(Acr_Message input_message)
{
   Acr_Group group_list, group;

   /* Print log message */
   if (Do_logging >= HIGH_LOGGING) {
      (void) fprintf(stderr, "\n\nReceived GCENDq message:\n");
      acr_dump_message(stderr, input_message);
   }

   /* Create the reply */
   group_list = spi_reply(input_message);

   /* Add to the reply */
   group = acr_get_group_next(group_list);
   acr_group_add_element(group,
      acr_create_element_string(SPI_Operator_text, "OK"));
   acr_group_add_element(group,
      acr_create_element_string(SPI_Log_info, "OK"));
   acr_group_add_element(group,
      acr_create_element_string(SPI_Volume_name, "PET"));
   acr_group_add_element(group,
      acr_create_element_string(SPI_Creation_date, "1993.11.23"));
   acr_group_add_element(group,
      acr_create_element_string(SPI_Creation_time, "10:00:0000"));
   acr_group_add_element(group,
      acr_create_element_string(SPI_Volume_type, "FOREIGN"));
   acr_group_add_element(group,
      acr_create_element_string(SPI_Volume_status, "OPEN"));
   acr_group_add_element(group,
      acr_create_element_numeric(SPI_Space_left, (double) 1000000));
   acr_group_add_element(group,
      acr_create_element_numeric(SPI_Nr_exams, (double) 10));
   acr_group_add_element(group,
      acr_create_element_numeric(SPI_Nr_images, (double) 200));
   acr_group_add_element(group,
      acr_create_element_string(SPI_Closing_date, "2010.11.23"));
   acr_group_add_element(group,
      acr_create_element_numeric(SPI_Perc_space_used, (double) 20));

   return make_message(group_list);

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : cancel_reply
@INPUT      : input_message
@OUTPUT     : (nothing)
@RETURNS    : output_message
@DESCRIPTION: Responds to CANCELq message
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 22, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Message cancel_reply(Acr_Message input_message)
{
   /* Print log message */
   if (Do_logging >= HIGH_LOGGING) {
      (void) fprintf(stderr, "\n\nReceived CANCELq message:\n");
      acr_dump_message(stderr, input_message);
   }

   /* Create the reply */
   return make_message(spi_reply(input_message));

}
