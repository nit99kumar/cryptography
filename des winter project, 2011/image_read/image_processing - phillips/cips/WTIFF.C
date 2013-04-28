
       /**********************************************
       *
       *  file d:\cips\wtiff.c
       *
       *  Functions: This file contains
       *      create_file_if_needed
       *      create_allocate_tiff_file
       *      write_array_into_tiff_image
       *      write_line
       *      insert_short_into_buffer
       *      insert_long_into_buffer
       *      round_off_image_size
       *      does_not_exist
       *
       *  Purpose:
       *     These functions create TIFF image files 
       *     on disk and insert a ROWSxCOLS array
       *     into a tiff image already stored on disk.
       *
       *  External Calls:
       *     rtiff.c - seek_to_first_line
       *               seek_to_end_of_line
       *     tiff.c - read_tiff_header
       *
       *  Modifications:
       *     29 January 1991 - created
       *     28 March 1993 - replaced open, lseek
       *         etc. with fopen, fseek, etc.
       *     10 May 1993 - added a number of tags
       *         to make the TIFF files I create
       *         TIFF 6.0 Gray Scale image compliant.
       *
       *********************************************/

#include "cips.h"





       /**********************************************
       *
       *   create_file_if_needed(...
       *
       *   This function creates a file on disk
       *   if it does not exist.  The out file is
       *   patterned after the in file.
       *
       ***********************************************/

create_file_if_needed(in_name, out_name, out_image)
   char in_name[], out_name[];
   short out_image[ROWS][COLS];
{
   int    length, width;
   struct tiff_header_struct image_header;

   if(does_not_exist(out_name)){
      printf("\n\n output file does not exist %s",
               out_name);
      read_tiff_header(in_name, &image_header);
      round_off_image_size(&image_header,
                           &length, &width);
      image_header.image_length = length*ROWS;
      image_header.image_width  = width*COLS;
      create_allocate_tiff_file(out_name, &image_header,
                                out_image);
   }  /* ends if does_not_exist */
}  /* ends create_file_if_needed */





   /**********************************************
   *
   *   create_alllocate_tiff_file(...
   *
   *   This function creates a file on disk that will be
   *   large enough to hold a tiff image.  The input
   *   tiff_header_struct describes the desired tiff file.
   *   This function writes the tiff header and then
   *   writes a blank image array out to disk the proper
   *   number of times.  This has the effect of allocating
   *   the correct number of bytes on the disk.
   *
   *   There will be 18 entries in the IFD.
   *   The image data will begin at byte 296.
   *   I will use LSB first data.
   *   I will have one strip for the entire image.
   *   Black is zero.
   *   The component values for the image are CHUNKY
   *      (Planer configuration = 1).
   *
   ***************************************************/


create_allocate_tiff_file(file_name, 
                          image_header, image)
   char   file_name[];
   short  image[ROWS][COLS];
   struct tiff_header_struct *image_header;
{
   char  buffer[12], long_buffer[50];
   FILE  *image_file;
   int   bytes_written,
         i,
         j,
         l,
         w;

   long  k;



      /***************************************
      *
      *   Create the image file in binary mode
      *   for both reading and writing.
      *
      ****************************************/

   image_file = fopen(file_name, "wb");

      /***************************************
      *
      *   Write out the first 8 bytes of the
      *   header.  The meaning of the
      *   bytes (HEX) is:
      *      0-1 = 49 49 - LSB first
      *      2-3 = 2A 00 - version #
      *      4-7 = 08 00 00 00 - go to offset
      *           8 for the first
      *           Image File
      *           Directory
      *
      ****************************************/

   buffer[0] = 0x49;
   buffer[1] = 0x49;
   buffer[2] = 0x2A;
   buffer[3] = 0x00;
   buffer[4] = 0x08;
   buffer[5] = 0x00;
   buffer[6] = 0x00;
   buffer[7] = 0x00;

   bytes_written = fwrite(buffer, 1, 8, image_file);

   printf("\n wrote %d bytes", bytes_written);

      /***************************************
      *
      *   Write out the first 2 bytes of the
      *   Image File Directory.  These tell
      *   the number of entries in the IFD.
      *
      ****************************************/

   buffer[0] = 0x12;
   buffer[1] = 0x00;
   bytes_written = fwrite(buffer, 1, 2, image_file);

   printf("\n wrote %d bytes", bytes_written);

      /***************************************
      *
      *   Write out the entries into the
      *   Image File Directory.
      *
      ****************************************/


      /* New Subfile Type */
   buffer[0]  = 0xFE;
   buffer[1]  = 0x00;
   buffer[2]  = 0x03;
   buffer[3]  = 0x00;
   buffer[4]  = 0x01;
   buffer[5]  = 0x00;
   buffer[6]  = 0x00;
   buffer[7]  = 0x00;
   buffer[8]  = 0x00;
   buffer[9]  = 0x00;
   buffer[10] = 0x00;
   buffer[11] = 0x00;
   bytes_written = fwrite(buffer, 1, 12, image_file);
   printf("\n wrote %d bytes", bytes_written);


      /* Subfile Type */
   buffer[0]  = 0xFF;
   buffer[1]  = 0x00;
   buffer[2]  = 0x03;
   buffer[3]  = 0x00;
   buffer[4]  = 0x01;
   buffer[5]  = 0x00;
   buffer[6]  = 0x00;
   buffer[7]  = 0x00;
   buffer[8]  = 0x01;
   buffer[9]  = 0x00;
   buffer[10] = 0x00;
   buffer[11] = 0x00;
   bytes_written = fwrite(buffer, 1, 12, image_file);
   printf("\n wrote %d bytes", bytes_written);


      /* Image Width */
   insert_short_into_buffer(buffer, 0, 256);
   insert_short_into_buffer(buffer, 2, 3);
   insert_short_into_buffer(buffer, 4, 1);
   insert_short_into_buffer(buffer, 8, 
                     image_header->image_width);
   bytes_written = fwrite(buffer, 1, 12, image_file);
   printf("\n wrote %d bytes", bytes_written);


      /* Image Length */
   insert_short_into_buffer(buffer, 0, 257);
   insert_short_into_buffer(buffer, 2, 3);
   insert_short_into_buffer(buffer, 4, 1);
   insert_short_into_buffer(buffer, 8, 
                     image_header->image_length);
   bytes_written = fwrite(buffer, 1, 12, image_file);
   printf("\n wrote %d bytes", bytes_written);


      /* Bits Per Sample */
   insert_short_into_buffer(buffer, 0, 258);
   insert_short_into_buffer(buffer, 2, 3);
   insert_short_into_buffer(buffer, 4, 1);
   insert_short_into_buffer(buffer, 8, 
                     image_header->bits_per_pixel);
   bytes_written = fwrite(buffer, 1, 12, image_file);
   printf("\n wrote %d bytes", bytes_written);


      /* Compression - None */
   insert_short_into_buffer(buffer, 0, 259);
   insert_short_into_buffer(buffer, 2, 3);
   insert_short_into_buffer(buffer, 4, 1);
   insert_short_into_buffer(buffer, 8, 1);
   bytes_written = fwrite(buffer, 1, 12, image_file);
   printf("\n wrote %d bytes", bytes_written);


      /* Photometric Interpretation */
      /* set to 1 because BLACK is ZERO */
   insert_short_into_buffer(buffer, 0, 262);
   insert_short_into_buffer(buffer, 2, 3);
   insert_short_into_buffer(buffer, 4, 1);
   insert_short_into_buffer(buffer, 8, 1);
   bytes_written = fwrite(buffer, 1, 12, image_file);
   printf("\n wrote %d bytes", bytes_written);


      /* Strip Offset */
      /* start after software name at 296 */
   insert_short_into_buffer(buffer, 0, 273);
   insert_short_into_buffer(buffer, 2, 3);
   insert_short_into_buffer(buffer, 4, 1);
   insert_short_into_buffer(buffer, 8, 296);
   bytes_written = fwrite(buffer, 1, 12, image_file);
   printf("\n wrote %d bytes", bytes_written);


      /* Samples per Pixel */
   insert_short_into_buffer(buffer, 0, 277);
   insert_short_into_buffer(buffer, 2, 3);
   insert_short_into_buffer(buffer, 4, 1);
   insert_short_into_buffer(buffer, 8, 1);
   bytes_written = fwrite(buffer, 1, 12, image_file);
   printf("\n wrote %d bytes", bytes_written);


      /* clear buffer */
   for(i=0; i<12; i++) buffer[i] = 0x00;

      /* Rows Per Strip 1 strip for the entire image */
      /* use 2E32 - 1, which is max */
   insert_short_into_buffer(buffer, 0, 278);
   insert_short_into_buffer(buffer, 2, 4);
   insert_short_into_buffer(buffer, 4, 1);
   insert_long_into_buffer(buffer, 8, 4294967295);
   bytes_written = fwrite(buffer, 1, 12, image_file);
   printf("\n wrote %d bytes", bytes_written);


      /* Strip Byte Counts */
      /* this = image width times length */
   insert_short_into_buffer(buffer, 0, 279);
   insert_short_into_buffer(buffer, 2, 4);
   insert_short_into_buffer(buffer, 4, 1);
   insert_long_into_buffer(buffer, 8, 
    (long)(image_header->image_length *
           image_header->image_width));
   bytes_written = fwrite(buffer, 1, 12, image_file);
   printf("\n wrote %d bytes", bytes_written);


      /* Min Sample Value */
   insert_short_into_buffer(buffer, 0, 280);
   insert_short_into_buffer(buffer, 2, 3);
   insert_short_into_buffer(buffer, 4, 1);
   insert_short_into_buffer(buffer, 8, 0);
   bytes_written = fwrite(buffer, 1, 12, image_file);
   printf("\n wrote %d bytes", bytes_written);


      /* Max Sample Value */
   insert_short_into_buffer(buffer, 0, 281);
   insert_short_into_buffer(buffer, 2, 3);
   insert_short_into_buffer(buffer, 4, 1);
   if(image_header->bits_per_pixel == 8)
      insert_short_into_buffer(buffer, 8, 255);
   else
      insert_short_into_buffer(buffer, 8, 15);
   bytes_written = fwrite(buffer, 1, 12, image_file);
   printf("\n wrote %d bytes", bytes_written);


      /* X Resolution */
      /* Store the 8 bytes for this value
         starting at 230 */
   insert_short_into_buffer(buffer, 0, 282);
   insert_short_into_buffer(buffer, 2, 5);
   insert_short_into_buffer(buffer, 4, 1);
   insert_short_into_buffer(buffer, 8, 230);
   bytes_written = fwrite(buffer, 1, 12, image_file);
   printf("\n wrote %d bytes", bytes_written);


      /* Y Resolution */
      /* Store the 8 bytes for this value
         starting at 238 */
   insert_short_into_buffer(buffer, 0, 283);
   insert_short_into_buffer(buffer, 2, 5);
   insert_short_into_buffer(buffer, 4, 1);
   insert_short_into_buffer(buffer, 8, 238);
   bytes_written = fwrite(buffer, 1, 12, image_file);
   printf("\n wrote %d bytes", bytes_written);



      /* clear buffer */
   for(i=0; i<12; i++) buffer[i] = 0x00;

      /* Planer Configuration */
      /* chunky */
   insert_short_into_buffer(buffer, 0, 284);
   insert_short_into_buffer(buffer, 2, 3);
   insert_short_into_buffer(buffer, 4, 1);
   insert_short_into_buffer(buffer, 8, 1);
   bytes_written = fwrite(buffer, 1, 12, image_file);
   printf("\n wrote %d bytes", bytes_written);


      /* Resolution Unit */
      /* inches */
   insert_short_into_buffer(buffer, 0, 296);
   insert_short_into_buffer(buffer, 2, 3);
   insert_short_into_buffer(buffer, 4, 1);
   insert_short_into_buffer(buffer, 8, 2);
   bytes_written = fwrite(buffer, 1, 12, image_file);
   printf("\n wrote %d bytes", bytes_written);


      /* Software */
      /* Put this a 246, 50 bytes */
   insert_short_into_buffer(buffer, 0, 305);
   insert_short_into_buffer(buffer, 2, 2);
   insert_short_into_buffer(buffer, 4, 50);
   insert_short_into_buffer(buffer, 8, 246);
   bytes_written = fwrite(buffer, 1, 12, image_file);
   printf("\n wrote %d bytes", bytes_written);


      /* Offset to next IFD (0 means no more IFD's) */
   for(i=0; i<12; i++) buffer[i] = 0x00;
   bytes_written = fwrite(buffer, 1, 4, image_file);
   printf("\n wrote %d bytes", bytes_written);


      /* clear buffer */
   for(i=0; i<12; i++) buffer[i] = 0x00;

      /* Now store the X Resolution
         first long is numerator
         second long is denominator */
   insert_long_into_buffer(buffer, 0, 300L);
   insert_long_into_buffer(buffer, 4, 1L);
   bytes_written = fwrite(buffer, 1, 8, image_file);
   printf("\n wrote %d bytes", bytes_written);


      /* Now store the Y Resolution
         first long is numerator
         second long is denominator */
   insert_long_into_buffer(buffer, 0, 300L);
   insert_long_into_buffer(buffer, 4, 1L);
   bytes_written = fwrite(buffer, 1, 8, image_file);
   printf("\n wrote %d bytes", bytes_written);


      /* Now store the software tag */
   for(i=0; i<50; i++) long_buffer[i] = '\0';
   strcpy(long_buffer,
   "Dwayne Phillips C Image Processing System 1993");
   long_buffer[46] = '\0';
   long_buffer[47] = '\0';
   long_buffer[48] = '\0';
   long_buffer[49] = '\0';
   bytes_written = fwrite(long_buffer, 1, 50, 
                          image_file);
   printf("\n wrote %d bytes", bytes_written);
   printf("\n%s", long_buffer);



      /***************************************
      *
      *   Now write the image data.
      *
      ****************************************/

   printf("\n length is %ld", 
          image_header->image_length);
   printf("\n width is %ld", 
          image_header->image_width);

   round_off_image_size(image_header, &l, &w);
   k = l * w;

   if(image_header->bits_per_pixel == 8)
      k = k/2;
   else
      k = k/4;
   k++;

   for(i=0; i<ROWS; i++)
      for(j=0; j<COLS; j++)
        image[i][j] = 0;

   j = sizeof(short) * ROWS * COLS;

   for(i=0; i<k; i++){
      bytes_written = fwrite(image, 1, j, image_file);
      printf("\n wrote %d bytes", bytes_written);
   }


   fclose(image_file);

}  /* ends create_allocate_tiff_file */





       /*********************************************
       *
       * write_array_into_tiff_file(...
       *
       * This function takes an array of shorts and 
       * writes them into an existing tiff image file.
       *
       **********************************************/

write_array_into_tiff_image(image_file_name, array,
                            il, ie, ll, le)

        char    image_file_name[];
        int     il, ie, ll, le;
        short   array[ROWS][COLS];
{

   char  buffer[COLS];
   FILE  *image_file;
   int   bytes_written,
         closed,
         i,
          position,
         written;

   float a;

   long  line_length,
         offset;

   struct tiff_header_struct image_header;



   read_tiff_header(image_file_name, &image_header);


      /***********************************************
      *
      *   Procedure:
      *   Seek to the strip offset where the data begins.
      *   Seek to the first line you want.
      *   Loop over the lines you want to write.
      *      Seek to the first element of the line.
      *      Write the line.
      *      Seek to the end of the data in that line.
      *
      ************************************************/

   image_file = fopen(image_file_name, "rb+");
   position   = fseek(image_file,
                      image_header.strip_offset, 
                      SEEK_SET);
   position   = seek_to_first_line(image_file,
                                   &image_header, il);

   for(i=0; i<(ll-il); i++){
      offset        = (ie-1)/
                      (8/image_header.bits_per_pixel);
      position      = fseek(image_file, offset, 
                            SEEK_CUR);
      bytes_written = write_line(image_file, array,
                                 i, &image_header, 
                                 ie, le);
      position      = seek_to_end_of_line(image_file,
                                          le, 
                                          &image_header);
      position      = fseek(image_file, 1, SEEK_CUR);
   }  /* ends loop over i  */

   closed = fclose(image_file);

}  /*  ends write_array_into_tiff_image */




       /*********************************************
       *
       *   write_line(...
       *
       *   This function takes an array of shorts, 
       *   extracts the numbers and puts them into 
       *   a buffer, then writes this buffer into a 
       *   tiff file on disk. The process depends on 
       *   the number of bits per pixel used in the 
       *   file (4 or 8).
       *
       **********************************************/

write_line(image_file, array, line_number, 
           image_header, ie, le)
   FILE   *image_file;
   int    ie, le, line_number;
   short  array[ROWS][COLS];
   struct tiff_header_struct *image_header;
{
   char     buffer[COLS], first, second;
   float    a, b;
   int      bytes_written, i;
   unsigned int bytes_to_write;
   union    short_char_union scu;

   for(i=0; i<COLS; i++)
      buffer[i] = '\0';

   bytes_to_write = (le-ie)/
                    (8/image_header->bits_per_pixel);

   for(i=0; i<bytes_to_write; i++){

        /**********************************************
        *
        *   Use unions defined in cips.h to stuff shorts
        *   into bytess.
        *
        **********************************************/

      if(image_header->bits_per_pixel == 8){
       scu.s_num = 0;
       scu.s_num = array[line_number][i];
       buffer[i] = scu.s_alpha[0];
      }  /* ends if bits_per_pixel == 8 */


      if(image_header->bits_per_pixel == 4){

       scu.s_num = 0;
       scu.s_num = array[line_number][i*2];
       first     = scu.s_alpha[0] << 4;

       scu.s_num = 0;
       scu.s_num = array[line_number][i*2];
       second    = scu.s_alpha[0] & 0X000F;

       buffer[i] = first | second;
      }  /* ends if bits_per_pixel == 4 */

   }  /*  ends loop over i  */


   bytes_written = fwrite(buffer, 1, bytes_to_write, 
                          image_file);

   return(bytes_written);

}  /* ends write_line  */






   /***************************************
   *
   *   insert_short_into_buffer(...
   *
   *   This inserts a two byte short into a
   *   buffer of characters.  It does this
   *   is LSB order.
   *
   ***************************************/


insert_short_into_buffer(buffer, start, number)
    char  buffer[];
    int   start;
    short number;
{
    union short_char_union lsu;

    lsu.s_num       = number;
    buffer[start+0] = lsu.s_alpha[0];
    buffer[start+1] = lsu.s_alpha[1];

}  /* ends insert_short_into_buffer */




   /***************************************
   *
   *   insert_long_into_buffer(...
   *
   *   This inserts a four byte long into a
   *   buffer of characters.  It does this
   *   is LSB order.
   *
   ***************************************/



insert_long_into_buffer(buffer, start, number)
    char buffer[];
    int  start;
    long number;
{
    union long_char_union lsu;

    lsu.l_num       = number;
    buffer[start+0] = lsu.l_alpha[0];
    buffer[start+1] = lsu.l_alpha[1];
    buffer[start+2] = lsu.l_alpha[2];
    buffer[start+3] = lsu.l_alpha[3];

}  /* ends insert_short_into_buffer */




   /***************************************
   *
   *   round_off_image_size(...
   *
   *   This takes the image header and rounds
   *   it off to a multiple of ROWS and COLS.
   *   e.g. if width=123 it returns 1.
   *
   ***************************************/


round_off_image_size(image_header, length, width)
    int    *length, *width;
    struct tiff_header_struct *image_header;
{
   *length = (ROWS-10 + image_header->image_length)/ROWS;
   *width  = (COLS-10 + image_header->image_width)/COLS;
} /* ends round_off_image_size */





   /***********************************************
    *
    *    does_not_exist(...
    *
    *    This function checks the disk to see if
    *    a file exists.  If the file is there this
    *    function returns a 0, if it does not exist
    *    this function returns a 1.
    *
    ***********************************************/

does_not_exist(file_name)
    char file_name[];
{
   FILE *image_file;
   int  result;

   result = 1;
   image_file = fopen(file_name, "rb");
   if(image_file != NULL){
      result = 0;
      fclose(image_file);
   }
   return(result);
}  /* ends does_not_exist */
