

       /*****************************************************
       *
       *       file d:\cips\wtiffh.c
       *
       *       Functions: This file contains
       *	  write_tiff_header
       *	  insert_long_into_buffer
       *	  insert_short_into_buffer
       *
       *       Purpose:
       *	  These functions take a tiff_header_struct
       *	  and write it to a file as the tiff
       *	  header.
       *
       *       External Calls:
       *
       *       Modifications:
       *	  30 January 1991 - created
       *
       ******************************************************/

#include "d:\cips\cips.h"


write_tiff_header(file_name, image_header)
   char   file_name[];
   struct tiff_header_struct *image_header;
{
   char   buffer[100];
   int	  bytes_written,
	  closed,
	  file_descriptor,
	  i,
	  j,
	  k,
	  not_finished;

   /*file_descriptor = my_open(file_name);*/





   printf("\nhellow from write tiff header");



}  /* ends write_tiff_header */
