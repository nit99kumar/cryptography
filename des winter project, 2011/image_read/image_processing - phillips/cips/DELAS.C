



       /*****************************************************
       *
       *       file d:\cips\delas.c
       *
       *       Functions: This file contains
       *          main
       *          display_image_portion
       *          read_string
       *          read_image
       *          my_read
       *          get_image_name
       *          get_parameters
       *          show_parameters
       *          read_header
       *          color_transform
       *          display_menu
       *
       *       Purpose: This program displays images on the EGA 
       *          monitor.
       *          It can display either a 300x500 section of a 
       *          single
       *          channel of an image, or three channels of a 
       *          100x500
       *	  section of an image. This can use either a straight
       *	  mapping of 256 gray levels to 16 colors or a modified
       *	  mapping that brings out selected objects in the image.
       *
       *       Modifications:
       *          17 June 1987 - created
       *          8 August 1987 - added the ability to draw a
       *              grid over the image and label areas with
       *              a C or an L.
       *	  30 August 1987 - major changes made so that this is
       *              the only display routine needed.
       *           9 April 1988 - moved to Borland's Turbo C 1.5.
       *              This has a library of graphics routines that
       *	      are more reliable and execute faster.
       *	  13 September 1990 - put in function
       *	      blank_out_display to help reduce
       *	      the contrast when taking photos.
       *	  15 September 1990 - put the x_offset and
       *	      y_offset parameters in to center the
       *	      image display.
       *
       ********************************************************/


#include "d:\lsu\vision3.h"

   short image[ROWS][COLS];

main()
{
   char  channels[80],
         color_transform[80],
         file_name[80],
         response[80];

   int   a,
         b,
         c,
	 channel,
	 double_display,
	 horizontal,
         ie,
         il,
         key,
         le,
         ll,
         not_finished,
	 r,
	 vertical,
	 x_offset,
	 y_offset;


   unsigned int block,
                color,
                i,
                j,
                x,
                y;

   unsigned long histogram[256];


   _setvideomode(_TEXTC80);         /* MSC 6.0 statements */
   _setbkcolor(1);
   _settextcolor(7);
   _clearscreen(_GCLEARSCREEN);

   strcpy(file_name, "d:/images/nbrite.dat");
   strcpy(channels, "Single channel");
   strcpy(color_transform, "Modified transform");
   channel = 1;
   il      = 1;
   ll      = 100;
   ie      = 1;
   le	   = 100;
   horizontal = 5;
   vertical   = 4;
   double_display = 0;

   printf("\nThis is the display program.");
   printf("\nThis program can either display a 300x500 section of");
   printf("\nan image or three channels of a 100x500 section of an");
   printf("\nimage. The parameters entered describe the upper left");
   printf("\n100x100 corner of the display.");

   not_finished = 1;
   while(not_finished){

      display_menu(channels, color_transform);
      get_image_name(file_name);
      get_parameters(&channel, &il, &ie, &ll, &le,
		     &horizontal, &vertical, &double_display);


	/*******************************************
	*
	*   we want to set an offset so the image is
	*   centered on the CRT
	*
	********************************************/

	   x_offset = 0;
	   y_offset = 0;
	   x_offset = (6-horizontal)*100;
	   x_offset = 20 + x_offset/2;
	   y_offset = (4-vertical)*100;
	   y_offset = 40 + y_offset/2;



           /*********************************************
           *
           *   If the color_transform requires histogram
           *   equalization, then calculate the histogram
           *   from the four corners of the image.  You
           *   will use this in the display loop below
           *   to equalize the image.
           *
           **********************************************/

      if(color_transform[0] == 'H'){
         printf("\nCalculating histograms");
         zero_long_histogram(histogram);
         printf("\n\t1");
         read_image(image, file_name, channel, il, ie, ll, le);
         calculate_long_histogram(image, histogram);
         printf("\n\t2");
	 read_image(image, file_name, channel, il+400, ie,
		    ll+400, le);
         calculate_long_histogram(image, histogram);
         printf("\n\t3");
	 read_image(image, file_name, channel, il,
		    ie+400, ll, le+400);
         calculate_long_histogram(image, histogram);
         printf("\n\t4");
         read_image(image, file_name, channel, il+400, ie+400,
		    ll+400, le+400);
         calculate_long_histogram(image, histogram);
for(a=0; a<256; a++)
printf("\nh[%4d] = %8d", a, histogram[a]);
      }  /* ends if color_transform == H */


           /*********************************************
           *
           *   Find out what the key should be. This
           *   requires looking at the name of the image
           *   that is being displayed. Use a different
           *   key for the three different channels of
           *   the image.
           *
           **********************************************/

      key = 0;
      if(file_name[16] == '1')
         key = 1;
      if(file_name[16] == '2')
         key = 2;
      if(file_name[16] == '3')
         key = 3;
      if( (file_name[16] == '.') &&
          (channel == 1))
         key = 1;
      if( (file_name[16] == '.') &&
          (channel == 2))
         key = 2;
      if( (file_name[16] == '.') &&
          (channel == 3))
         key = 3;

  /* set graphics mode */
/*******
     map_16_shades_of_gray(_VRES16COLOR);
     blank_out_display();
********/

my_set_colors();




	  /***************************************************
          *
          *   If a single channel is desired then display a 300x500
	  *   section of one channel. If multiple channels
	  *   are desired then display one 100x500 section
	  *   of the image three times.
          *
	  *****************************************************/

      if( (channels[0] == 'S')  ||
	  (channels[0] == 's')){

	 for(a=0; a<horizontal; a++){
	    for(b=0; b<vertical; b++){
               x = a*100;
               y = b*100;
               read_image(image, file_name, channel, il+y, ie+x,
			   ll+y, le+x);

               if(color_transform[0] == 'H')
                  perform_histogram_equalization(image, histogram,
					      16, 40000);

	       if(double_display == 1){

		  if( (a==0) && (b==0)){
		    x_offset = 120;
		    y_offset = 40;
		  }

		  if( (a==0) && (b==1)){
		    x_offset = 120;
		    y_offset = 140;
		  }

		  if( (a==1) && (b==0)){
		    x_offset = 220;
		    y_offset = 40;
		  }

		  if( (a==1) && (b==1)){
		    x_offset = 220;
		    y_offset = 140;
		  }
	       }

               display_image_portion(image, key, color_transform,
				x, y, x_offset, y_offset,
				double_display);
            }  /* ends loop over b                   */
	 }     /* ends loop over a		     */

      }        /* ends if channels == Single channel */



      else{    /* else display three channels        */
         for(a=0; a<5; a++){    /*  Channel 1  */
            x = a*100;
            y =     0;
            read_image(image, file_name, 1, il+y, ie+x, ll+y,
			le+x);
            display_image_portion(image, key, color_transform, x,
				  y, x_offset, y_offset,
				  double_display);
         }  /* ends loop over a  */

         for(a=0; a<5; a++){    /*  Channel 2  */
            x = a*100;
            y =     0;
            color_transform[0] = 'S';
            read_image(image, file_name, 2, il+y, ie+x, ll+y,
		       le+x);
            display_image_portion(image, key, color_transform, x,
				  y+100, x_offset, y_offset,
				  double_display);
         }  /* ends loop over a  */

         for(a=0; a<5; a++){    /*  Channel 3  */
            x = a*100;
            y =     0;
            color_transform[0] = 'S';
            read_image(image, file_name, 3, il+y, ie+x, ll+y,
			le+x);
            display_image_portion(image, key, color_transform, x,
				  y+200, x_offset, y_offset,
				  double_display);
         }  /* ends loop over a  */
      }     /* ends else display multiple channels  */


/******************
      setlinestyle(SOLID_LINE, 0, THICK_WIDTH);
      for(i=0; i<5; i++){
         line(0, i*100, 500, i*100);
      }

      for(i=0; i<6; i++){
         line(i*100, 0, i*100, 300);
      }


      setlinestyle(SOLID_LINE, 0, NORM_WIDTH);
      for(i=0; i<13; i++){
         line(0, i*25, 500, i*25);
      }

      for(i=0; i<21; i++){
         line(i*25, 0, i*25, 300);
      }
******************/


/***********
setusercharsize(1, 2, 1, 2);
settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 0);
outtextxy(10, 320, "done");
***********/


/******************
    The following label the objects in the lower portion of the
image
outtextxy(400, 200, "041 house");
outtextxy(100, 25, "041 house");
outtextxy(100, 200, "040 apartments/hotel");
outtextxy(350, 50, "040 apartments/hotel");
outtextxy(10, 100, "200 decidous");
outtextxy(175, 150, "200 decidous");
*******************/


      read_string(response);
      printf("\nEnter 0 to quit 1 to do again");
      get_integer(&not_finished);

          /* set display back to text mode */
   _setvideomode(_TEXTC80);         /* MSC 6.0 statements */
   _setbkcolor(1);
   _settextcolor(7);
   _clearscreen(_GCLEARSCREEN);


   }  /* ends while not_finished  */

}  /* ends main  */



display_menu(channels, color_transform)
   char channels[], color_transform[];
{
   char response[80];
   int  not_finished, r;

   not_finished = 1;
   while(not_finished){
      printf("\n\nDISPLAY> Enter choice (0 for no change) ");
      printf("\nDISPLAY> 1. Channels-- %s", channels);
      printf("\nDISPLAY> 2. Color Transform-- %s",
	  color_transform);
      printf("\nDISPLAY> _\b");
      get_integer(&r);

      if(r == 0){
         not_finished = 0;
      }

      if(r == 1){
         printf("\nDISPLAY> Enter the new channel mode ");
	 printf("\nDISPLAY> (S) Single channel   (M) Multiple channel");
         printf("\nDISPLAY> _\b");
         read_string(response);
         if((response[0] == 'S') ||
            (response[0] == 's'))
               strcpy(channels, "Single channel");
         else
               strcpy(channels, "Multiple channels");
      }  /* ends if r == 1 */

      if(r == 2){
         printf("\nDISPLAY> Enter the new color transform mode ");
	 printf("\nDISPLAY> (S) Straight mode");
	 printf(" (M) Modified mode");
	 printf(" (R) Results mode");
	 printf(" (H) Histogram Equalization");
         printf("\nDISPLAY> _\b");
         read_string(response);
         if((response[0] == 'S') ||
            (response[0] == 's'))
               strcpy(color_transform, "Straight mode");
         if((response[0] == 'M') ||
            (response[0] == 'm'))
               strcpy(color_transform, "Modified mode");
         if((response[0] == 'H') ||
            (response[0] == 'h'))
	       strcpy(color_transform,
		   "Histogram Equalization Mode");
	 if((response[0] == 'R') ||
	    (response[0] == 'r'))
	       strcpy(color_transform,
		   "Results Mode");
      }  /* ends if r == 2  */

   }  /* ends while not_finished  */

}  /* ends display_menu  */






display_image_portion(image, key, desired_color_transform,
		      x, y, x_offset, y_offset,
		      double_display)
   char     desired_color_transform[];
   int	    key, x_offset, y_offset, double_display;
   short    image[ROWS][COLS];
   unsigned int x, y;
{
   unsigned int color, i, j;

      for(i=0; i<ROWS; i++){
         for(j=0; j<COLS; j++){


            if(desired_color_transform[0] == 'H')
               color = image[i][j];
            else
               color = color_transform(image[i][j], key,
				    desired_color_transform);

	    /*putpixel(j+x, i+y, color);*/

	    _setcolor(color);

	    if(double_display == 1){
	       _setpixel( (j*2)+1 + x + x_offset, (i*2)+1 + y + y_offset);
	       _setpixel( (j*2)+1 + x + x_offset + 1, (i*2)+1 + y + y_offset);
	       _setpixel( (j*2)+1 + x + x_offset, (i*2)+1 + y + y_offset + 1);
	       _setpixel( (j*2)+1 + x + x_offset + 1, (i*2)+1 + y + y_offset + 1);
	    }
	    else
	       /***_setpixel(j+x+x_offset, i+y+y_offset);***/
my_set_pixel(j+x+x_offset, i+y+y_offset, lookup(color));

         }  /* ends loop over j  */
      }     /* ends loop over i  */

}  /* ends display_image_portion  */




read_string(string)
        char *string;
{
        int     eof,
                letter,
                no_error;

        eof = -1;
        no_error = 0;

        while((letter = getchar()) != '\n' &&
               letter !=  eof)
           *string++ = letter;

        *string = '\0';

        return((letter == eof) ? eof : no_error);

}       /* ends read_string */



clear_buffer(string)
   char string[];
{
   int i;
   for(i=0; i<MAX_NAME_LENGTH; i++)
      string[i] = ' ';
}



long_clear_buffer(string)
   char string[];
{
   int i;
   for(i=0; i<300; i++)
      string[i] = ' ';
}


       /***********************************************************
       *
       *       file c:\lsu\rdimage.c
       *
       *       Functions: This file contains
       *           read_image
       *
       *       Purpose: These functions read images from and write
       *          images to disk files using the ELAS image format.
       *
       *       External Calls:
       *          rhead.c - read_header
       *          whead.c - write_header
       *          mrw.c - my_read,
       *                  my_write
       *
       *       Modifications:
       *          July 86 - ported to IBM-PC
       *          29 September 86 - uses vision3.h instead of
       *              vision2.h
       *	   10 February 1987 - changed the images from int to
       *              short and the int_char_union was changed to
a
       *              short_char_union.
       *          12 June 1987 - ported to Borland Turbo C. The
       *              call to read and write were changed to
       *              my_read and my_write.
       *           12 June 1987 - one function, read_image, moved
       *              from the file rwimage.c to this file.
       *
      
*************************************************************/


        /****************************************************
        *
        *       read_image(image, file_name, channel, il, ie, ll,
le)
        *
        *       This function will read in an elas image.
        *       The il and ie are given with the idea that
        *       the il and ie of the image are 1,1.
        *
        *       The image is packed into the array starting at
        *       location [0][0].
        *
        ****************************************************/


read_image(image, file_name, channel, il, ie, ll, le)

        int   ie, il, channel, ll, le;
        char  file_name[];
        short image[ROWS][COLS];

{
        int     file_closed,
                file_descriptor,
                i,
                j,
                last_i,
                last_j,
                nc,
                origin,
                position,
                read_file;

        char    in_buffer[512];

        long    current_position,
                line_length,
                offset;

        struct header_parameters par;

        union short_char_union scu;


        read_header(file_name, &par);

        line_length = par.le - par.ie + 1;
        if((line_length % 256) != 0)
           line_length = ((line_length/256) + 1) * 256;

        nc     = par.nc;

        last_i = ll;
        if(ll > ROWS)
           last_i = ROWS;
        last_j = le;
        if(le > COLS)
           last_j = COLS;

        file_descriptor = my_open(file_name);

        origin = 0;     /* seek from beginning of file */
        offset = (long)(header_size) + ((il-1)*nc +
(channel-1))*line_length;

        position = lseek(file_descriptor, offset, origin);

        for(i=0; i<last_i; i++){
           read_file = my_read(file_descriptor, in_buffer,
line_length);
           for(j=0; j<last_j; j++){
              scu.s_num      =            0;
              scu.s_alpha[0] = in_buffer[j+ie-1]; /********/
              image[i][j]    =    scu.s_num;
           }    /* ends loop over j COLS */

                        /*****************************************
                        *
                        *       If more than one channel in the
                        *       image then skip over the other
                        *       channels.
                        *
                        ******************************************/
           if(nc != 1){
              origin =                    1; /* seek from current
position */
              offset = (nc - 1)*line_length;
              lseek(file_descriptor, offset, origin);
           }

        }       /* ends loop over i ROWS */

        file_closed = close(file_descriptor);

}       /* ends read_image */

       /*****************************************************
       *
       *       file c:\lsu\mrw.c
       *
       *       Functions: This file contains
       *           my_read
       *           my_write
       *
       *       Purpose: These two functions call the Turbo C
       *           functions _read and _write. All software
       *           will use my_read and my_write so that if
       *           the software is ported to another system
       *           that uses read and write changing the
       *           two functions in this file will take care
       *           of the move.
       *
       *       External Call:
       *           _read
       *           _write
       *
       *       Modifications:
       *           10 June 1987 - created
       *
       *
      
************************************************************/


my_read(file_descriptor, buffer, number_of_bytes)
   int  file_descriptor, number_of_bytes;
   char *buffer;
{
   int read();
   read(file_descriptor, buffer, number_of_bytes);
}


my_write(file_descriptor, buffer, number_of_bytes)
   int  file_descriptor, number_of_bytes;
   char *buffer;
{
   int write();
   write(file_descriptor, buffer, number_of_bytes);
}
       
/***************************************************************
        *
        *       file c:\lsu\gin.c
        *
        *       Functions: This file contains
        *           get_image_name
        *
        *       Purpose - This function prompts the user to enter
the
        *            name of an image.
        *
        *       External Calls:
        *           rstring.c - read_string
        *                       clear_buffer
        *
        *       Modifications:
        *           26 September 86 - now uses vision3.h instead of
        *               vision2.h and the read_string and
get_integer
        *               instead of  scanf.
        *           11 March 1987 - this function was removed from
the
        *               file ip.c and put in file gin.c.
        *
       
***************************************************************/




get_image_name(name)

        char name[];
{
        char response[80];
        printf("\n\nImage name is--%s", name);
        printf("\nDo you want to change this name (Y/N)? _\b");
        clear_buffer(response);
        read_string(response);
        if((response[0] == 'Y') ||
           (response[0] == 'y')){
             printf("\n\nEnter image name\n--");
             read_string(name);
        }
}       /* ends get_image_name  */

       
/***************************************************************
        *
        *       file c:\lsu\gp.c
        *
        *       Functions: This file contains
        *           get_parameters
        *           show_parameters
        *
        *       Purpose - These functions get image parameters.
        *
        *       External Calls:
        *           rstring.c - read_string
        *           intcvrt.c - get_integer
        *
        *       Modifications:
        *           19 February 1987 - These functions were taken
out
        *               of the file ip.c.
        *
       
***************************************************************/



get_parameters(channel, il, ie, ll, le, h, v, dd)
	int *channel, *il, *ie, *le, *ll, *h, *v, *dd;
{
        int choice, not_finished;

        not_finished = 1;
        while(not_finished){
	   show_parameters(channel, il, ie, ll, le, h, v, dd);
	   printf("\n\nEnter choice to change");
	   printf(" (enter 0 for no changes) __\b\b");
           get_integer(&choice);

           switch (choice){
              case 0:
                 not_finished = 0;
                 break;

              case 1:
                 printf("\nEnter channel number\n_\b");
                 get_integer(channel);
                 break;

              case 2:
                 printf("\nEnter initial line\n___\b\b\b");
                 get_integer(il);
                 break;

              case 3:
                 printf("\nEnter initial element\n___\b\b\b");
                 get_integer(ie);
                 break;

              case 4:
                 printf("\nEnter last line\n___\b\b\b");
                 get_integer(ll);
                 break;

              case 5:
                 printf("\nEnter last element\n___\b\b\b");
                 get_integer(le);
		 break;

	      case 6:
		 printf("\nEnter horizontal \n___\b\b\b");
		 get_integer(h);
		 break;

	      case 7:
		 printf("\nEnter vertical \n___\b\b\b");
		 get_integer(v);
		 break;

	      case 8:
		 printf("\nEnter double display (1=on 0=off)");
		 printf("\n___\b\b\b");
		 get_integer(dd);
		 break;

           }  /* ends switch choice       */
        }     /* ends while not_finished  */
}             /* ends get_parameters      */


show_parameters(channel, il, ie, ll, le, h, v, dd)
	int *channel, *il, *ie, *le, *ll, *h, *v, *dd;
{

   printf("\n\nThe image parameters are:");
   printf("\n\t1. channel = %4d", *channel);
   printf("\n\t2.      il = %4d", *il);
   printf("\n\t3.      ie = %4d", *ie);
   printf("\n\t4.      ll = %4d", *ll);
   printf("\n\t5.      le = %4d", *le);
   printf("\n\t6.      horizontal = %d", *h);
   printf("\n\t7.      vertical   = %d", *v);
   printf("\n\t8.      double display = %d", *dd);

}  /* ends show_parameters  */

       
/***********************************************************
        *
        *        file c:\lsu\rhead.c
        *
        *        Functions: This file contains
        *            read_header
        *            show_header
        *
        *        Purpose: These functions read an ELAS image header
        *           and display its contents to the screen.
        *
        *        External Calls:
        *           unions.c - long_equate
        *                      float_equate
        *           mrw.c - my_read
        *
        *        Modifications:
        *            June 1986 - created
        *            26 September 1986 - uses vision3.h instead of
        *                vision2.h
        *            12 June 1987 - ported to Borland Turbo C. The
        *                call to read was changed to my_read.
        *
       
***********************************************************/


        /*********************************************
        *
        *       This function will read the header of
        *       an elas image and report the 
        *       parameters.
        *
        *********************************************/

read_header(file_name, params)

        char file_name[];
        struct header_parameters *params;

{

        int     closed_file,
                file_descriptor,
                i,
                read_file,
                value,
                written;

        char    in_buffer[header_size],
                line1[64],
                line2[64],
                line3[64],
                line4[64];

        union long_char_union  lcu;
        union float_char_union fcu;

                /*****************************************
                *
                *       Open the file, read in 1024 bytes
                *       and then proceed to pull out the
                *       bytes from the in_buffer.
                *
                ******************************************/

        file_descriptor = my_open(file_name);

        read_file = my_read(file_descriptor, in_buffer,
header_size);


        value = 0;
        long_equate(in_buffer, &lcu, value);
        params->nbih = lcu.l_num;

        
        value = 4;
        long_equate(in_buffer, &lcu, value);
        params->nbpr = lcu.l_num;

        value = 8;
        long_equate(in_buffer, &lcu, value);
        params->il = lcu.l_num;

        value = 12;
        long_equate(in_buffer, &lcu, value);
        params->ll = lcu.l_num;

        value = 16;
        long_equate(in_buffer, &lcu, value);
        params->ie = lcu.l_num;

        value = 20;
        long_equate(in_buffer, &lcu, value);
        params->le = lcu.l_num;

        value = 24;
        long_equate(in_buffer, &lcu, value);
        params->nc = lcu.l_num;

        value = 28;
        long_equate(in_buffer, &lcu, value);
        params->f321 = lcu.l_num;

        value = 36;
        long_equate(in_buffer, &lcu, value);
        params->y_offset = lcu.l_num;

        value = 40;
        for(i=0; i<4; i++)
           params->desc2[i] = in_buffer[value + i];

        value = 44;
        long_equate(in_buffer, &lcu, value);
        params->x_offset = lcu.l_num;

        value = 48;
        float_equate(in_buffer, &fcu, value);
        params->y_spot_size = fcu.f_num;

        value = 52;
        float_equate(in_buffer, &fcu, value);
        params->x_spot_size = fcu.f_num;

        value = 56;
        for(i=0; i<4; i++){
           float_equate(in_buffer, &fcu, (value+i*4));
           params->transform[i] = fcu.f_num;
        }

        value = 96;
        for(i=0; i<=256; i++)
           params->comments[i] = in_buffer[i + value];
        value = 32;
        for(i=0; i<4; i++)
           params->desc1[i] = in_buffer[value + i];


        closed_file = close(file_descriptor);

}       /* end of read_header */



        /*****************************************************
        *
        *       show_header(name, params)
        *
        *       This function shows the header input to the
        *       function.
        *
        *****************************************************/




show_header(name, params)

        char   name[];
        struct header_parameters *params;

{

        int     i;

        char    c,
                line1[64],
                line2[64],
                line3[64],
                line4[64],
                response[80];

        for(i=0; i<64; i++){
           line1[i] = params->comments[i];
           line2[i] = params->comments[i + 64];
           line3[i] = params->comments[i + 128];
           line4[i] = params->comments[i + 192];
        }
        line1[63] = '\0';
        line2[63] = '\0';
        line3[63] = '\0';
        line4[63] = '\0';


        printf("\nFile name is %s", name);
        printf("\nnbih=%d",params->nbih);
        printf("\nnbpr=%d",params->nbpr);
        printf("\nil=%d",params->il);
        printf("\nll=%d",params->ll);
        printf("\nie=%d",params->ie);
        printf("\nle=%d",params->le);
        printf("\nnc=%d",params->nc);
        printf("\n4321=%d",params->f321);
        printf("\nDescriptor1=%c%c%c%c",
           params->desc1[0],
           params->desc1[1],
           params->desc1[2],
           params->desc1[3]);
        printf("\ny offset=%d",params->y_offset);
        printf("\nDescriptor2=%c%c%c%c",
           params->desc2[0],
           params->desc2[1],
           params->desc2[2],
           params->desc2[3]);
        printf("\nx offset=%d",params->x_offset);
        printf("\ny spot size=%f",params->y_spot_size);
        printf("\nx spot size=%f",params->x_spot_size);
        printf("\nThe transform matrix is %f %f %f %f",
           params->transform[0],
           params->transform[1],
           params->transform[2],
           params->transform[3]);
        printf("\ncomments");
        printf("\n>%s",line1);
        printf("\n>%s",line2);
        printf("\n>%s",line3);
        printf("\n>%s",line4);

        printf("\nHit RETURN to continue");
        read_string(response);

}       /* end show_header */

        /*********************************************************
        *
        *       file c:\lsu\unions.c
        *
        *       Functions: This file contains
        *           long_equate
        *           float_equate
        *           store_long_into_buffer
        *           store_float_into_buffer
        *
        *       Purpose: These functions are used when reading and
        *                writing ELAS image headers. They pack and
        *                unpack floating point numbers and integers
        *                from a buffer of characters.
        *
        *       External Calls:
        *           none
        *
        *       Modifications:
        *           July 1986 - ported to IBM-PC
        *
        *********************************************************/


long_equate(buffer, lcu, start)

        char  buffer[];
        int   start;
        union long_char_union *lcu;
{
        int i;

        lcu->l_num = 0;
        for(i=0; i<4; i++){
           lcu->l_alpha[3-i] = buffer[start + i];
        }            /* this is i not 3-i on none 8086 systems   */
                     /* the same is true for the other functions */
                     /* in this file                             */

}


float_equate(buffer, fcu, start)

        char buffer[];
        int  start;
        union float_char_union *fcu;
{
        int i;
        for(i=0; i<4; i++)
           fcu->f_alpha[3-i] = buffer[start + i];
}



store_long_into_buffer(buffer, lcu, start)

        char  buffer[];
        int   start;
        union long_char_union *lcu;
{
        int i;
        for(i=0; i<4; i++)
           buffer[start + i] = lcu->l_alpha[3-i];
}



store_float_into_buffer(buffer, fcu, start)

        char buffer[];
        int  start;
        union float_char_union *fcu;
{
        int i;
        for(i=0; i<4; i++)
           buffer[start + i] = fcu->f_alpha[3-i];
}

               
/******************************************************
                *
                *                file c:\lsu\intcvrt.c
                *
                *                Functions: This file contains
                *           get_integer
                *                    int_convert
                *
                *                Purpose: These functions convert
a string of 
                *                characters to their number value.
                *
                *                Modifications: 
                *            Taken from Jamsa's software package
                *            and altered to fit into the computer
                *            vision programming 22 August 1986.
                *
               
*******************************************************/


#include "d:\tc\numdefs.h"


get_integer(n)
   int *n;
{
   char string[80];

   read_string(string);
   int_convert(string, n);
}





 int_convert (ascii_val, result)
    char *ascii_val;
    int *result;
  {
    int sign = 1;  /* -1 if negative */

    *result = 0;   /* value returned to the calling routine */

    /* read passed blanks */
    while (is_blank(*ascii_val))
       ascii_val++;              /* get next letter */

    /* check for sign */

    if (*ascii_val == '-' || *ascii_val == '+') 
       sign = (*ascii_val++ == '-') ? -1 : 1;  /* find sign */

   /*
    * convert the ASCII representation to the actual
    * decimal value by subtracting '0' from each character.
    *
    * for example, the ASCII '9' is equivalent to 57 in decimal.
    * by subtracting '0' (or 48 in decimal) we get the desired 
    * value.
    *
    * if we have already converted '9' to 9 and the next character
    * is '3', we must first multiply 9 by 10 and then convert '3'
    * to decimal and add it to the previous total yielding 93.
    *
    */

    while (*ascii_val)
     if (is_digit(*ascii_val))
       *result = *result * 10 + to_decimal(*ascii_val++);

     else
       return (IO_ERROR);


    *result = *result * sign;
       
    return (NO_ERROR);
  }




color_transform(pixel, key, type_of_transform)
   char  type_of_transform[];
   int   key;
   short pixel;
{
   unsigned int color;

   if( (type_of_transform[0] == 'M')  ||
       (type_of_transform[0] == 'm')){
      if(key == 1){
         if((pixel >= 0)   && (pixel <   16))  color =  0;
         if((pixel >= 16)  && (pixel <   32))  color =  1;
         if((pixel >= 32)  && (pixel <   48))  color =  2;
         if((pixel >= 48)  && (pixel <   64))  color =  3;
         if((pixel >= 64)  && (pixel <   80))  color =  4;
         if((pixel >= 80)  && (pixel <   96))  color =  5;
         if((pixel >= 96)  && (pixel <   190)) color =  8;
         /*if((pixel >= 112) && (pixel <   128)) color =  7;
         if((pixel >= 128) && (pixel <   144)) color =  8;
         if((pixel >= 150) && (pixel <   160)) color =  9;
         if((pixel >= 160) && (pixel <   176)) color = 10;
         if((pixel >= 176) && (pixel <   192)) color = 11;
         if((pixel >= 190) && (pixel <   208)) color = 12;
         if((pixel >= 208) && (pixel <   224)) color = 13;
         if((pixel >= 224) && (pixel <   240)) color = 14;*/
         if((pixel >= 190) && (pixel <=  255)) color = 15;
      }  /* ends if key == 1 */

      if(key == 2){
         if((pixel >= 0)   && (pixel <   16))  color =  0;
         if((pixel >= 16)  && (pixel <   32))  color =  1;
         if((pixel >= 32)  && (pixel <   48))  color =  2;
         if((pixel >= 48)  && (pixel <   64))  color =  3;
         if((pixel >= 64)  && (pixel <   80))  color =  4;
         if((pixel >= 80)  && (pixel <   96))  color =  5;
         if((pixel >= 96)  && (pixel <   190)) color =  6;
         if((pixel >= 112) && (pixel <   128)) color =  7;
         if((pixel >= 128) && (pixel <   144)) color =  8;
         if((pixel >= 150) && (pixel <   160)) color =  9;
         if((pixel >= 160) && (pixel <   176)) color = 10;
         if((pixel >= 176) && (pixel <   192)) color = 11;
         /*if((pixel >= 190) && (pixel <   208)) color = 12;
         if((pixel >= 208) && (pixel <   224)) color = 13;
         if((pixel >= 224) && (pixel <   240)) color = 14;*/
         if((pixel >= 192) && (pixel <=  255)) color = 15;
      }  /* ends if key == 2 */

      if(key == 3){
         if((pixel >= 0)   && (pixel <   16))  color =  0;
         if((pixel >= 16)  && (pixel <   30))  color =  1;
         if((pixel >= 30)  && (pixel <   55))  color =  2;
         if((pixel >= 55)  && (pixel <   64))  color =  3;
         if((pixel >= 64)  && (pixel <   80))  color =  4;
         if((pixel >= 80)  && (pixel <   96))  color =  5;
         if((pixel >= 96)  && (pixel <   190)) color =  6;
         if((pixel >= 112) && (pixel <   120)) color =  7;
         if((pixel >= 120) && (pixel <   140)) color =  8;
         if((pixel >= 140) && (pixel <   160)) color =  9;
         if((pixel >= 160) && (pixel <   176)) color = 10;
         if((pixel >= 176) && (pixel <   192)) color = 11;
         if((pixel >= 192) && (pixel <   208)) color = 12;
         if((pixel >= 208) && (pixel <   224)) color = 13;
         if((pixel >= 224) && (pixel <   240)) color = 14;
         if((pixel >= 240) && (pixel <   255)) color = 15;
      }  /* ends if key == 3 */

   }	 /* ends if type_of_transform == Modified  */

   if( (type_of_transform[0] == 'S')  ||
       (type_of_transform[0] == 's'))
      color = pixel/16;

   if( (type_of_transform[0] == 'R')  ||
       (type_of_transform[0] == 'r')){
      if(pixel == 0)  color = 3;
      if(pixel == 40) color = 6;
      if(pixel == 41) color = 9;
      if(pixel == 69) color = 12;
      if(pixel == 97) color = 15;
   }  /* ends results mode */

   return(color);

}  /* ends color_transform  */




           /*****************************************************
********
           *
           *       file c:\lsu\mof.c
           *
           *       Functions: This file contains
           *          my_open
           *
           *       Purpose: This function opens a file. Borland's
           *          Turbo C opens files a little different from
           *          the standard UNIX C. Instead of using this
           *          different method in a number of various
files,
           *          the method is placed in this one file. If the
           *          programs are moved to another system, all
changes
           *          will be located in this one place.
           *
           *       External Calls:
           *          none
           *
           *       Modifications:
           *          18 June 1987 - created
           *
           ******************************************************
*********/



my_open(file_name)
   char file_name[];
{
   int file_descriptor;

   file_descriptor = open(file_name, O_RDWR | O_CREAT | O_BINARY,
S_IWRITE);

   return(file_descriptor);


}  /* ends my_open  */









   /**********************************************
   *
   *   map_16_shades_of_gray(...
   *
   *   This function maps 16 shades of gray into
   *   the first 16 color indices.  This allows
   *   you to display a true "black and white"
   *   image on a color monitor.
   *
   *********************************************/


map_16_shades_of_gray(display_mode)
   int display_mode;
{
   /* all MSC 6.0 statements */
_setvideomode(display_mode);


_remappalette(0,  0x000000L);
_remappalette(1,  0x040404L);
_remappalette(2,  0x080808L);
_remappalette(3,  0x0c0c0cL);
_remappalette(4,  0x101010L);
_remappalette(5,  0x141414L);
_remappalette(6,  0x181818L);
_remappalette(7,  0x1c1c1cL);
_remappalette(8,  0x202020L);
_remappalette(9,  0x242424L);
_remappalette(10, 0x282828L);
_remappalette(11, 0x2c2c2cL);
_remappalette(12, 0x303030L);
_remappalette(13, 0x343434L);
_remappalette(14, 0x383838L);
_remappalette(15, 0x3f3f3fL);
}








   /**********************************
   *
   *   Modes for the SigmaVGA Legend
   *               (hex)
   *   10 - 640x350x64?
   *   12 - 640x480x16
   *   29 - 800x600x16
   *   30 - 800x600x256
   *   38 - 1024x768x256
   *
   ***********************************/

my_set_video_mode()
{

   union REGS regs;

   regs.h.al = 0x29; /* mode */
   regs.h.ah = 0x00;
   int86(0x10, &regs, &regs);


}  /* ends my_set_video_mode */





my_set_pixel(x, y, color)
   unsigned int x, y, color;
{

union REGS regs;


   regs.h.ah = 0x0c;
   regs.x.dx = y;
   regs.x.cx = x;
   regs.h.al = color;
   regs.h.bh = 0;
   int86(0x10, &regs, &regs);


}  /* ends my_set_pixel */






lookup(color)
   int color;
{
   int table[16] =
	 {0, 1, 4, 5, 8, 2, 6, 9, 3, 12, 13, 7, 10, 11, 14, 15};
   return(table[color]);
}  /* ends lookup */







my_set_colors()
{
   _asm{
      mov ax,0029h ; 800x600x16
      int 10h

      mov ah,10h
      mov al,1bh
      mov bx,0h
      mov cx,100h
      int 10h
   }
}  /* ends my_set_colors */








        /*****************************************
        *
        *    perform_histogram_equalization(...
        *
        ******************************************/


perform_histogram_equalization(image, histogram, new_grays, area)
   float new_grays, area;
   short image[ROWS][COLS];
   unsigned long histogram[];
{
   int i,
       j,
       k;
   unsigned long sum,
                 sum_of_h[256];

   double constant;


   sum = 0;
   for(i=0; i<256; i++){
      sum          = sum + histogram[i];
      sum_of_h[i] = sum;
   }

      /* constant = new # of gray levels div by area */
   /*constant = new_grays/area;*/
   constant = 0.0004; /* 16/40000 = 0.0004 */

   for(i=0; i<ROWS; i++){
      for(j=0; j<COLS; j++){
         k                 = image[i][j];
         image[i][j] = sum_of_h[k] * constant;
      }
   }
}  /* ends perform_histogram_equalization */











        /*****************************************
        *
        *   zero_long_histogram(...
        *
        ******************************************/


zero_long_histogram(histogram)
   unsigned long histogram[];
{
   int i;
   for(i=0; i<256; i++)
      histogram[i] = 0;
}  /* ends zero_long_histogram */









        /*****************************************
        *
        *   calcualte_long_histogram(...
        *
        ******************************************/

calculate_long_histogram(image, histogram)
   short  image[ROWS][COLS];
   unsigned long histogram[];
{
   int i,j,k;
   for(i=0; i<ROWS; i++){
      for(j=0; j<COLS; j++){
         k = image[i][j];
         histogram[k] = histogram[k] + 1;
      }
   }
}  /* ends calculate_long_histogram */








blank_out_display()
{
   int i;

   _setlinestyle(0xFFFF);
   _setcolor(10);
   for(i=0; i<640;i++){
      _moveto(i, 0);
      _lineto(i, 480);
   }

}  /* ends blank_out_display */
