
   /***********************************************
   *
   *  file tcommsub.c
   *
   *  Functions: This file contains
   *     read_comments
   *     insert_comments
   *
   *  Purpose:
   *     This file contains subroutines that will
   *     read the comments in a TIFF header and
   *     insert comments into a TIFF header.
   *
   *  External Calls:
   *     tiff.c - read_tiff_header
   *     rtiff.c - extract_long_from_buffer
   *               extract_short_from_buffer
   *     wtiff.c - insert_long_into_buffer
   *               insert_short_into_buffer
   *
   *  Modifications:
   *     9 May 1995 - created  
   *
   *************************************************/


#include "cips.h"

   /*************************************************
   *
   *   read_comments(...
   *
   *   This function reads the comments out of a 
   *   TIFF file header and displays them on the
   *   screen.
   *
   **************************************************/

read_comments(file_name, all_comments)
   char all_comments[], file_name[];
{
   char  buffer[12], *comments;

   FILE  *image_file;

   int   bytes_read,
         comment_location,
         i,
         j,
         lsb,
         reading_header;

   long  offset_to_ifd,
         position;

   short comment_length,
         entry_count,
         location,
         tag_type;

   strcpy(all_comments, "\n");
   image_file = fopen(file_name, "rb");

        /*************************************
        *
        *   Read the first four bytes to 
        *   determine if the file uses MSB
        *   first or LSB first.
        *
        *************************************/

   bytes_read = fread(buffer, 1, 4, image_file);
   if(buffer[0] == 0x49)
      lsb = 1;
   else
      lsb = 0;
   
   reading_header = 1;
   while(reading_header){

         /*************************************
         *
         *   Read the offset to the IFD
         *
         *************************************/
        
      bytes_read = fread(buffer, 1, 4, image_file);
      extract_long_from_buffer(buffer, lsb, 0, 
                               &offset_to_ifd);

      if(offset_to_ifd == 0)
         reading_header = 0;
      else{  /* else offset_to_ifd != 0 */

            /*************************************
            *
            *   Seek to the IFD and read the
            *   entry_count, i.e. the number of
            *   entries in the IFD.
            *
            *************************************/

         fseek(image_file, offset_to_ifd, SEEK_SET);
         bytes_read = fread(buffer, 1, 2, image_file);
         extract_short_from_buffer(buffer, lsb, 0, 
                                   &entry_count);


            /***************************************
            *
            *   Now loop over the directory entries.
            *   Look only for the comment tag
            *   305.
            *
            *****************************************/

         for(i=0; i<entry_count; i++){
            bytes_read = fread(buffer, 1, 12, image_file);
            extract_short_from_buffer(buffer, lsb, 0, 
                                      &tag_type);

            if(tag_type == 305){
               position = ftell(image_file);
               extract_short_from_buffer(buffer, lsb, 4,
                                         &comment_length);
               extract_short_from_buffer(buffer, lsb, 8,
                                         &comment_location);
               fseek(image_file, (long)(comment_location), 
                     SEEK_SET);
               comments   = malloc(comment_length+1);
               bytes_read = fread(comments, 1, 
                                  comment_length, 
                                  image_file);
               comments[comment_length] = '\0';
/**printf("\nREAD> %s", comments);**/
               strcat(all_comments, comments);
               strcat(all_comments, " \n");
               free(comments);
               fseek(image_file, position, SEEK_SET);
            }  /* ends if tag_type == 305 */
         }  /* ends loop over i directory entries */
      }  /* ends if else offset_to_ifd != 0 */
   }  /* ends while reading_header */

   fclose(image_file);

}  /* ends read_comments */





   /*************************************************
   *
   *   insert_comments(...
   *
   *   This function inserts comments into a 
   *   TIFF file header.
   *
   **************************************************/

insert_comments(file_name, comments)
   char file_name[], comments[];
{
   char   *buffer, 
          *buffer2,
          buffer3[12],
          image_buffer[1000], 
          temp_file_name[MAX_NAME_LENGTH];

   FILE   *in_image_file,
          *temp_file;

   int    base,
          bytes_read,
          bytes_written,
          i,
          index,
          length_of_comments,
          lsb,
          position,
          traversing;

   long   offset_to_ifd;

   short  entry_count        = 0,
          offset             = 0,
          old_comment_length = 0,
          tag_type           = 0;

   struct tiff_header_struct image_header;

   tmpnam(temp_file_name);
   temp_file = fopen(temp_file_name, "wb");

   length_of_comments = strlen(comments);
   read_tiff_header(file_name, &image_header);

   buffer        = malloc((int)
                          (image_header.strip_offset +
                           length_of_comments        +
                           12));
   in_image_file = fopen(file_name, "rb");
   bytes_read    = fread(buffer, 1, 
                         (int)
                         (image_header.strip_offset), 
                         in_image_file);

   if(buffer[0] == 0x49)
      lsb = 1;
   else 
      lsb = 0;

      /********************************************
      *
      *   Traverse through the current TIFF header.
      *   
      *********************************************/

   index      = 4;
   traversing = 1;

   while(traversing){

         /****************************************
         *
         *   Look at the offset_to_ifd.  If it
         *   is zero, this is the end of the TIFF
         *   header.  Set this offset_to_ifd
         *   to point to the new IFD that will
         *   hold the new comment.
         *   
         *****************************************/

      extract_long_from_buffer(buffer, lsb, index,
                               &offset_to_ifd);

      if(offset_to_ifd == 0L){
         traversing = 0;
         offset_to_ifd = (long)
                         (image_header.strip_offset);
         insert_long_into_buffer(buffer, 
                                 index,
                                 offset_to_ifd);
      }  /* ends if offset_to_ifd == 0 */
      else{  /* else offset_to_ifd != 0 */

         extract_short_from_buffer(buffer, lsb, 
                                (int)(offset_to_ifd),
                                &entry_count);

         /********************************************
         *
         *   The IFD's begin at buffer location
         *   base = offset_to_ifd + 2
         *   Each IFD is 12 bytes long.
         *
         *   Loop through the buffer to find the
         *   Strip Offset tag.  This tells where the
         *   header ends and the image data begins.
         *   We want to increase that number by the
         *   length_of_comments plus 12.
         *
         *   Also look for any comment tags.  We want
         *   to increase the offset to the comment
         *   by 12.
         *
         ********************************************/

         base = (int)(offset_to_ifd) + 2;


         for(i=0; i<entry_count; i++){
            index = base + i*12;
            extract_short_from_buffer(buffer, lsb, 
                                      index,
                                      &tag_type);

            if(tag_type == 273){
               extract_short_from_buffer(buffer, lsb, 
                                         index+8,
                                         &offset);
               offset = image_header.strip_offset +
                        length_of_comments        +
                        12 + 2 + 4;
                        /* 12 is length of entry
                            2 is # of entries in IFD
                            4 is final zero 
                              offset to IFD */
               insert_short_into_buffer(buffer, 
                                        index+8,
                                        offset);
            }  /* ends if found the strip offset tag */

         }  /* ends loop over i */
         index = index + 12;
      }  /* ends else offset_to_ifd != 0 */
   }  /* ends while traversing */

      /**********************************************
      *
      *   Now we have changed the strip offset 
      *   AND
      *   are sitting on a zero offset_to_ifd.
      *
      *   So, set this offset_to_ifd to point to 
      *   the new IFD (the one that will hold
      *   the new comment).
      *
      *   Write the original header to the 
      *   temp_file.
      *
      *
      **********************************************/

   insert_short_into_buffer(buffer, index, 
                            (short)
                            (image_header.strip_offset));

   bytes_written = fwrite(buffer, 1, 
                          (int)
                          (image_header.strip_offset),
                          temp_file);

      /********************************************
      *
      *   Write the number of entries in the
      *   new IFD (1).
      *
      ********************************************/

   insert_short_into_buffer(buffer, 0, 1);
   bytes_written = fwrite(buffer, 1, 2, temp_file);

      /********************************************
      *
      *   Write the entry.
      *
      ********************************************/

   for(i=0; i<12; i++) buffer3[i] = 0x00;
   insert_short_into_buffer(buffer3, 0, 305);
   insert_short_into_buffer(buffer3, 2, 2);
   insert_short_into_buffer(buffer3, 4, 
                            length_of_comments);
   insert_short_into_buffer(buffer3, 8, 
            image_header.strip_offset + 2 + 12 + 4);
   bytes_written = fwrite(buffer3, 1, 12, temp_file);

      /********************************************
      *
      *   Write zero offset to IFD
      *
      ********************************************/

   for(i=0; i<12; i++) buffer3[i] = 0x00;
   bytes_written = fwrite(buffer3, 1, 4, temp_file);

      /********************************************
      *
      *   Write the new comments.
      *
      ********************************************/

   bytes_written = fwrite(comments, 1, 
                          length_of_comments, 
                          temp_file);

      /*********************************************
      *
      *   Copy the image data from the image_file
      *   to the temp_file.
      *
      *********************************************/
   
   bytes_read = 1000;
   position   = fseek(in_image_file, 
                      image_header.strip_offset, 
                      SEEK_SET);
   while(bytes_read == 1000){
      bytes_read    = fread(image_buffer, 1, 1000, 
                            in_image_file);
      bytes_written = fwrite(image_buffer, 1, 
                             bytes_read, 
                             temp_file);
   }  /* ends while bytes_read */

   fclose(in_image_file);
   fclose(temp_file);
   free(buffer);

   remove(file_name);
   rename(temp_file_name, file_name);

}  /* ends insert_comments */
