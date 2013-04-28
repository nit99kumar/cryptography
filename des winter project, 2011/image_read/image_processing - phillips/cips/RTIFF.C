
       /*********************************************
       *
       *  file d:\cips\rtiff.c
       *
       *  Functions: This file contains
       *          read_tiff_image
       *          read_line
       *          seek_to_first_line
       *          seek_to_end_of_line
       *
       *  Purpose:
       *  These functions read a TIFF image and insert
       *  the data into a ROWSxCOLS array of short.
       *
       *       NOTE: The fseek constants are
       *             0=SEEK_SET = beginning of file
       *             1=SEEK_CUR = current offset
       *             2=SEEK_END = end of file
       *
       *  External Calls:
       *  tiff.c - read_tiff_header
       *
       *  Modifications:
       *       25 June 1990 - created
       *       27 March 1993 - use fopen, fread, fseek
       *           instead of the earlier open, read,
       *           seek, etc.
       *
       **********************************************/

#include "cips.h"





read_tiff_image(image_file_name, array, il, ie, ll, le)
      char   image_file_name[];
      int    il, ie, ll, le;
      short   array[ROWS][COLS];
{
   char  buffer[COLS],
         rep[80];
   int   bytes_read,
         closed,
         position,
         i;
   FILE  *image_file;
   float a;
   long  line_length, offset;

   struct tiff_header_struct image_header;

   read_tiff_header(image_file_name, &image_header);

      /***********************************************
      *
      *   Procedure:
      *   Seek to the strip offset where the data begins.
      *   Seek to the first line you want.
      *   Loop over the lines you want to read:
      *      Seek to the first element of the line.
      *      Read the line.
      *      Seek to the end of the data in that line.
      *
      ************************************************/

   image_file = fopen(image_file_name, "rb");
   if(image_file != NULL){
      position = fseek(image_file, 
                       image_header.strip_offset, 
                       SEEK_SET);
      position = seek_to_first_line(image_file, 
                                    &image_header, il);

      for(i=0; i<(ll-il); i++){
         offset       = (ie-1)/
                        (8/image_header.bits_per_pixel);
         position     = fseek(image_file, offset, 
                              SEEK_CUR);
         bytes_read   = read_line(image_file, array, 
                                  i, &image_header, 
                                  ie, le);
         position     = seek_to_end_of_line(image_file, 
                                 le, &image_header);
         position     = fseek(image_file, 1, 
                              SEEK_CUR); 
      }  /* ends loop over i  */

      closed = fclose(image_file);
   }  /* ends if file opened ok */
   else{
      printf("\nRTIFF.C> ERROR - cannot open "
             "tiff file");
   }

}  /*  ends read_tiff_image */




       /**********************************************
       *
       *   read_line(...
       *
       *   This function reads bytes from the TIFF 
       *   file into a buffer, extracts the numbers 
       *   from that buffer, and puts them into a 
       *   ROWSxCOLS array of shorts. The process 
       *   depends on the number of bits per pixel used 
       *   in the file (4 or 8).
       *
       **********************************************/

read_line(image_file, array, line_number, 
          image_header, ie, le)
   FILE   *image_file;
   int    ie, le, line_number;
   short  array[ROWS][COLS];
   struct tiff_header_struct *image_header;
{
   char  buffer[COLS], first, second;
   float a, b;
   int bytes_read, i;
   unsigned int bytes_to_read;
   union short_char_union scu;

   for(i=0; i<COLS; i++)
      buffer[i] = '\0';

        /********************************************
        *
        *   Use the number of bits per pixel to 
        *   calculate how many bytes to read.
        *
        ********************************************/

   bytes_to_read = (le-ie)/
                   (8/image_header->bits_per_pixel);
   bytes_read    = fread(buffer, 1, bytes_to_read, 
                         image_file);

   for(i=0; i<bytes_read; i++){

        /*********************************************
        *
        *   Use unions defined in cips.h to stuff bytes
        *   into shorts.
        *
        **********************************************/

      if(image_header->bits_per_pixel == 8){
       scu.s_num          = 0;
       scu.s_alpha[0]        = buffer[i];
       array[line_number][i] = scu.s_num;
      }  /* ends if bits_per_pixel == 8 */

      if(image_header->bits_per_pixel == 4){

       scu.s_num             = 0;
       second                = buffer[i] & 0X000F;
       scu.s_alpha[0]        = second;
       array[line_number][i*2+1] = scu.s_num;

       scu.s_num             = 0;
       first                 = buffer[i] >> 4;
       first                 = first & 0x000F;
       scu.s_alpha[0]        = first;
       array[line_number][i*2] = scu.s_num;

      }  /* ends if bits_per_pixel == 4 */

   }  /*  ends loop over i  */

   return(bytes_read);

}  /* ends read_line  */





       /*********************************************
       *
       *   seek_to_first_line(...
       *
       **********************************************/

seek_to_first_line(image_file, image_header, il)
   FILE   *image_file;
   int    il;
   struct tiff_header_struct *image_header;
{
   long offset;
   int  position;

   offset   = (il-1)*image_header->image_width/
             (8/image_header->bits_per_pixel);
      /* seek from current position */
   position = fseek(image_file, offset, SEEK_CUR);
   return(position);
}  /* ends seek_to_first_line */





       /**********************************************
       *
       *   seek_to_end_of_line(...
       *
       ***********************************************/

seek_to_end_of_line(image_file, le, image_header)
   FILE   *image_file;
   int    le;
   struct tiff_header_struct *image_header;
{
   long  offset;
   int   position;

   offset   = (image_header->image_width-le)/
             (8/image_header->bits_per_pixel);
   position = fseek(image_file, offset, SEEK_CUR);
   return(position);
}  /* ends seek_to_end_of_line         */
