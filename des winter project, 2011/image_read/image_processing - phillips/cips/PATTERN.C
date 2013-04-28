
    /***********************************************
    *
    *    file d:\cips\pattern.c
    *
    *    Functions: This file contains
    *       main
    *
    *    Purpose:
    *       I use this program to create simple
    *       patterns in images for illustrations.
    *
    *    External Calls:
    *       gpcips.c - my_clear_text_screen
    *       imageio.c
    *
    *    Modifications:
    *       31 March 1991 - created
    *       17 May 1993 - made this a command
    *           line driven program.
    *         19 September 1998 - modified to work with 
    *              all I O routines in imageio.c.
    *
    ***********************************************/

#include "cips.h"
#undef STUFF
#define DEGREESPERRADIAN 57.29577952





short **out_image;

main(argc, argv)
   int argc;
   char *argv[];
{
   char     name[80];
   char     *cc;
   double   da, db, dc, dd;
   int      alpha, beta;
   int      count, i, ie, ii, il, j, jj, le, l, ll, k,
            new_ie, new_il, set_i, set_j, square_size, w;
   int      ok = 0;
   long     cols, rows;
   struct   tiff_header_struct image_header;
   struct bmpfileheader      bmp_file_header;
   struct bitmapheader       bmheader;



   if(argc < 4 ){
      printf("\n usage: pattern file-name "
             "length width\n");
      exit(-1);
   }

   strcpy(name, argv[1]);
   l = atoi(argv[2]);
   w = atoi(argv[3]);

      /**************************************
      *                      
      *   Pre Processing
      *                      
      **************************************/

   cc = strstr(argv[1], ".tif");
   if(cc != NULL){  /* create a tif */
      ok = 1;
      image_header.lsb            = 1;
      image_header.bits_per_pixel = 8;
      image_header.image_length   = l;
      image_header.image_width    = w;;
      image_header.strip_offset   = 1000;
      rows = image_header.image_length;
      cols = image_header.image_width;
      create_allocate_tiff_file(argv[1], 
                                &image_header);
   }  /* ends tif */

   cc = strstr(argv[1], ".bmp");
   if(cc != NULL){  /* create a bmp */
      ok = 1;
      bmheader.height = l;
      bmheader.width  = w;
      rows = bmheader.height;
      cols = bmheader.width;
      create_allocate_bmp_file(argv[1], 
                               &bmp_file_header, 
                               &bmheader);
   }  /* ends tif */

   if(ok == 0){
      printf("\nERROR input file neither tiff nor bmp");
      exit(0);
   }

   out_image = allocate_image_array(l, w);

      /**************************************
      *                      
      *   Processing
      *                      
      **************************************/


   for(i=0; i<rows; i++)
      for(j=0; j<cols; j++)
        out_image[i][j] = 0;


   make_square(30, 30, 30, 10, out_image);


      /**************************************
      *                      
      *   Post Processing
      *                      
      **************************************/

   write_image_array(name, out_image);
   free_image_array(out_image, l, w);

}  /* ends main */



make_square(x, y, size, level, image)
   short **image;
   int   x, y, size, level;
{
   int i, j;

   for(i=y; i<y+size; i++)
      for(j=x; j<x+size; j++)
	     out_image[i][j] = level;

}  /* ends make_square */





int distance(x1, x2, y1, y2)
   int x1, x2, y1, y2;
{
   double n;
   int result;
   unsigned long a, b, c, d;

   a = x1;
   b = x2;
   c = y1;
   d = y2;

   n = (a-b)*(a-b) + (c-d)*(c-d);
   result = sqrt(n);
   return(result);


}  /* ends distance */





#ifdef STUFF


/*
   The following are samples patterns you can stick
   into the output image.
*/
   /********************************************/

   for(i=0; i<rows; i++)
      for(j=0; j<cols; j++)
        out_image[i][j] = 0;


   /* this makes a little volcano */
   for(i=0; i<rows; i++){
      for(j=0; j<cols; j++){
         count = distance(i, 49, j, 49);
         da = count/8.0;
         db = sin(da);
         db = db*12;
         if(db < 0.0) db = 0.0;
         count = db;
         out_image[i][j] = count;
      }
   }

   /* ANOTHER VOLCANO             */
   /* this makes a little volcano */
   for(i=0; i<rows; i++){
      for(j=0; j<cols; j++){
         count = distance(i, 49, j, 49);
         da = 2*count/DEGREESPERRADIAN;
         db = sin(da);
         db = (db+1.5)*100;
         if(db < 0.0) db = 0.0;
         count = db;
         out_image[i][j] = count;
      }
   }






   /********************************************/

   count = 1;
   for(i=0; i<l; i++){
      for(j=0; j<w; j++){
         printf("\nRunning %d of %d", count, l*w);
         count++;
         write_array_into_tiff_image(argv[1], out_image,
                         new_il + i*rows,
                         new_ie + j*cols,
                         new_il + i*rows + rows,
                         new_ie + j*cols + cols);
      }
   }


   make_square(20, 20, 30, 2, out_image);
   make_square(60, 60, 30, 5, out_image);
   make_square(20, 60, 30, 8, out_image);
   make_square(60, 20, 30, 4, out_image);
   make_square(40, 40, 30, 10, out_image);

   /********************************************/


make_square(x, y, size, level, image)
   short image[rows][cols];
   int   x, y, size, level;
{
   int i, j;

   for(i=y; i<y+size; i++)
      for(j=x; j<x+size; j++)
	     out_image[i][j] = level;

}  /* ends make_square */


make_rectangle(x1, x2, y1, y2, level, image)
   short image[rows][cols];
   int   x1, x2, y1, y2, level;
{
   int i, j;

   for(i=y1; i<y2; i++)
      for(j=x1; j<x2; j++)
	     out_image[i][j] = level;

}  /* ends make rectangle */


make_ccpp(xc, yc, level, image)
   short image[rows][cols];
   int xc, yc;
{
   make_square(xc,    yc, 20, level, image);
   make_square(xc+22, yc, 20, level, image);

   make_rectangle(xc+ 8, xc+10, yc+4 , yc+16, 0    , image);
   make_rectangle(xc+30, xc+32, yc+4 , yc+16, 0    , image);
   make_rectangle(xc+ 8, xc+20, yc+10, yc+12, 0    , image);
   make_rectangle(xc+30, xc+42, yc+10, yc+12, 0    , image);
   make_rectangle(xc+44, xc+54, yc+8 , yc+14, level, image);
   make_rectangle(xc+56, xc+66, yc+8 , yc+15, level, image);
   make_rectangle(xc+46, xc+52, yc+6 , yc+16, level, image);
   make_rectangle(xc+58, xc+64, yc+6 , yc+16, level, image);

}  /* ends make_ccpp */



int distance(x1, x2, y1, y2)
   int x1, x2, y1, y2;
{
   double n;
   int result;
   unsigned long a, b, c, d;

   a = x1;
   b = x2;
   c = y1;
   d = y2;

   n = (a-b)*(a-b) + (c-d)*(c-d);
   result = sqrt(n);
   return(result);


}  /* ends distance */



      /* writes c c++ several times */
   make_ccpp( 6,  6,  5, out_image);
   make_ccpp(20, 20,  8, out_image);
   make_ccpp(30, 35, 12, out_image);
   make_ccpp(6 , 75,  5, out_image);
   make_ccpp(20, 60,  8, out_image);
   make_ccpp(30, 55, 12, out_image);
   make_ccpp(20, 40, 20, out_image);


      /* small thin hill */
   da = 1.75;
   for(i=0; i<rows; i++){
      for(j=0; j<cols; j++){
         count = distance(i, 49, j, 49);
         db = count;
         dc = (2.0*da)/(da*da + db*db);
         dc = dc*15.0;
         count = dc;
         out_image[i][j] = count;
      }
   }

   /* this makes a little volcano */
   for(i=0; i<rows; i++){
      for(j=0; j<cols; j++){
         count = distance(i, 49, j, 49);
         da = count/8.0;
         db = sin(da);
         db = db*12;
         if(db < 0.0) db = 0.0;
         count = db;
         out_image[i][j] = count;
      }
   }

     /* This is a funnel going down away from you */
   for(i=0; i<rows; i++){
      for(j=0; j<cols; j++){
         count = distance(i, 49, j, 49);
         count = count*2;
         count = count/7;
         out_image[i][j] = count;
      }
   }



      /* a Rect in the upper right corner,
         A side wall moving towards you on
         the left side, and the floor moving
         towards you on the bottom. */

   for(i=50, j=1; i>0; i=i-2)
      make_rectangle(i-2, i, 0, 100, j++, out_image); 

   
   for(i=50, j=50, k=1; i>0, j<100; i=i-2, j=j+2)
      make_rectangle(i, 100, j, j+2, k++, out_image); 

   make_square(33, 33, 44, 7, out_image);


     /* depth file with four levels */
   for(i=0; i<rows; i++)
      for(j=0; j<cols; j++)
        out_image[i][j] = 0;


   make_square(20, 20, 20, 2, out_image);
   make_square(20, 25, 20, 3, out_image);
   make_square(30, 30, 20, 4, out_image);
   make_square(30, 35, 20, 5, out_image);
   make_square(40, 40, 20, 6, out_image);
   make_square(40, 45, 20, 7, out_image);
   make_square(50, 50, 20, 8, out_image);
   make_square(50, 55, 20, 9, out_image);
   make_square(60, 60, 20, 10, out_image);
   make_square(60, 65, 20, 11, out_image);
   make_square(70, 70, 20, 12, out_image);
   make_square(70, 75, 20, 13, out_image);


     /* depth file, increasing steps */
   for(i=0; i<rows; i++)
      for(j=0; j<cols; j++)
        out_image[i][j] = 0;


   for(i=0; i<100; i++)
      for(j=30; j<100; j++)
        out_image[i][j] = 2;
   for(i=0; i<100; i++)
      for(j=50; j<100; j++)
        out_image[i][j] = 4;
   for(i=0; i<100; i++)
      for(j=70; j<100; j++)
        out_image[i][j] = 6;






     /* another two square for a depth file */
   for(i=0; i<rows; i++)
      for(j=0; j<cols; j++)
        out_image[i][j] = 0;


   for(i=50; i<80; i++)
      for(j=50; j<80; j++)
        out_image[i][j] = 2;

   for(i=30; i<60; i++)
      for(j=30; j<60; j++)
        out_image[i][j] = 5;



     /* depth file with four levels */
   for(i=0; i<rows; i++)
      for(j=0; j<cols; j++)
        out_image[i][j] = 0;

   for(i=20; i<80; i++)
      for(j=20; j<80; j++)
        out_image[i][j] = 2;

   for(i=30; i<70; i++)
      for(j=30; j<70; j++)
        out_image[i][j] = 3;

   for(i=40; i<60; i++)
      for(j=40; j<60; j++)
        out_image[i][j] = 4;


     /* basic two square for a depth file */
   for(i=0; i<rows; i++)
      for(j=0; j<cols; j++)
        out_image[i][j] = 0;

   for(i=30; i<60; i++)
      for(j=30; j<60; j++)
        out_image[i][j] = 2;

   for(i=50; i<80; i++)
      for(j=50; j<80; j++)
        out_image[i][j] = 4;


	 
	 /* depth file with four levels */
   for(i=0; i<rows; i++)
      for(j=0; j<cols; j++)
        out_image[i][j] = 0;

   for(i=20; i<80; i++)
      for(j=20; j<80; j++)
        out_image[i][j] = 2;

   for(i=35; i<65; i++)
      for(j=35; j<65; j++)
        out_image[i][j] = 4;

   for(i=45; i<45; i++)
      for(j=45; j<45; j++)
        out_image[i][j] = 6;



     /* depth file with two levels */
   for(i=0; i<rows; i++)
      for(j=0; j<cols; j++)
        out_image[i][j] = 0;

   for(i=20; i<60; i++)
      for(j=20; j<60; j++)
        out_image[i][j] = 2;

   for(i=40; i<80; i++)
      for(j=40; j<80; j++)
        out_image[i][j] = 5;


     /* basic square for a depth file */
   for(i=0; i<rows; i++)
      for(j=0; j<cols; j++)
        out_image[i][j] = 0;

   for(i=30; i<70; i++)
      for(j=30; j<70; j++)
        out_image[i][j] = 5;



     /* make a step pattern of gray shades */
   for(i=0; i<rows; i++)
      for(j=0; j<cols; j++)
        out_image[i][j] = 0;
   for(i=0; i<rows; i++)
      for(j=10; j<20; j++)
        out_image[i][j] = 5;
   for(i=0; i<rows; i++)
      for(j=20; j<30; j++)
        out_image[i][j] = 10;
   for(i=0; i<rows; i++)
      for(j=30; j<40; j++)
        out_image[i][j] = 20;


     /*  make a club to look at thinning and mat */
   for(i=0; i<rows; i++){
      for(j=0; j<cols; j++){
        if( i>4 && i<11 && j>4 && j<17)
          out_image[i][j] = 200;
        else
          out_image[i][j] = 0;
    }
   }
   for(i=0; i<rows; i++){
      for(j=0; j<cols; j++){
        if( i>1 && i<17 && j>4 && j<11)
          out_image[i][j] = 200;
    }
   }


      /*  make a rectangle with a hole in it */
   for(i=0; i<rows; i++){
      for(j=0; j<cols; j++){
        if( i>=5 && i<=8 && j>=5 && j<=18)
          out_image[i][j] = 200;
    }
   }
   for(i=0; i<rows; i++){
      for(j=0; j<cols; j++){
        if( i>=15 && i<=18 && j>=5 && j<=18)
          out_image[i][j] = 200;
    }
   }
   for(i=0; i<rows; i++){
      for(j=0; j<cols; j++){
        if( j>=5 && j<=8 && i>=5 && i<=18)
          out_image[i][j] = 200;
    }
   }
   for(i=0; i<rows; i++){
      for(j=0; j<cols; j++){
        if( j>=15 && j<=18 && i>=5 && i<=18)
          out_image[i][j] = 200;
    }
   }

     /* make the entire area white with a black hole
     in it good for interior outline */
   for(i=0; i<rows; i++){
      for(j=0; j<cols; j++){
         if( i>=10 && i<=15 && j>=10 && j<=18)
            out_image[i][j] = 0;
         else
            out_image[i][j] = 200;
      }
   }


      /* make a square  */
   for(i=0; i<rows; i++){
      for(j=0; j<cols; j++){
         if( i>=8 && i<=18 && j>=8 && j<=18)
            out_image[i][j] = 200;
         else
            out_image[i][j] = 0;
      }
   }

      /* make a square  with a hole in
        it for closing practice */
   for(i=0; i<rows; i++){
      for(j=0; j<cols; j++){
         if( i>=8 && i<=13 && j>=8 && j<=13)
            out_image[i][j] = 200;
         else
            out_image[i][j] = 0;
      }
   }
   out_image[10][10] = 0;
   out_image[11][10] = 0;

      /* make 2 small squares
        illustrate closing and special closing */
   for(i=0; i<rows; i++)
      for(j=0; j<cols; j++)
         out_image[i][j] = 0;
   for(i=0; i<rows; i++){
      for(j=0; j<cols; j++){
         if( i>=8 && i<=13 && j>=8 && j<=9)
            out_image[i][j] = 200;
         if( i>=8 && i<=13 && j>=11 && j<=13)
            out_image[i][j] = 200;
      }
   }

      /* make a rectangle  */
   for(i=0; i<rows; i++){
      for(j=0; j<cols; j++){
         if( i>=8 && i<=18 && j>=4 && j<=18)
            out_image[i][j] = 200;
         else
            out_image[i][j] = 0;
      }
   }

      /* make a block letter A */
   for(i=0; i<rows; i++)
      for(j=0; j<cols; j++)
         out_image[i][j] = 0;
   for(i=0; i<rows; i++)
      for(j=0; j<cols; j++)
         if(i>=10 && i<=20 && j>=10 && j<=50)
          out_image[i][j] = 200;
   for(i=0; i<rows; i++)
      for(j=0; j<cols; j++)
         if(i>=40 && i<=50 && j>=10 && j<=50)
          out_image[i][j] = 200;
   for(i=0; i<rows; i++)
      for(j=0; j<cols; j++)
         if(i>=10 && i<=90 && j>=10 && j<=20)
          out_image[i][j] = 200;
   for(i=0; i<rows; i++)
      for(j=0; j<cols; j++)
         if(i>=10 && i<=90 && j>=40 && j<=50)
          out_image[i][j] = 200;

      /* make a broken line */
   for(i=0; i<rows; i++)
      for(j=0; j<cols; j++)
         out_image[i][j] = 0;
   for(i=0; i<rows; i++)
      for(j=0; j<cols; j++)
         if(i>=10 && i<=11 && j>=5 && j<=18)
          out_image[i][j] = 200;
   out_image[10][11] = 0;
   out_image[11][11] = 0;


      /* make two squares joined together */
   for(i=0; i<rows; i++)
      for(j=0; j<cols; j++)
         out_image[i][j] = 0;
   for(i=0; i<rows; i++){
      for(j=0; j<cols; j++){
         if( i>=5 && i<=15 && j>=5 && j<=9)
            out_image[i][j] = 200;
         if( i>=5 && i<=15 && j>=12 && j<=17)
            out_image[i][j] = 200;
      }
   }
   out_image[10][10] = 200;
   out_image[10][11] = 200;




   /* make a texture, checkerboard of 100-200
      with each square the size given by the 
      variable square_size 
      I'll do this a little different and let
      the user specify the square size as the
      last parameter on the line. */

   sqaure_size = atoi(argv[4]);
   ii=0; 
   jj=0; 
   set_i=0; 
   set_j=0;
   for(i=0; i<rows; i++){
      for(j=0; j<cols; j++){

            /* odd rows */
         if( (set_i%2) == 0){
            if( (set_j%2) == 0)
               out_image[i][j] = 100;
            else
               out_image[i][j] = 200;
         }
            /* even rows */
         else{
            if( (set_j%2) == 0)
               out_image[i][j] = 200;
            else
               out_image[i][j] = 100;
         }

         jj++;
         if(jj > square_size){
            jj = 0;
            set_j++;
         }
      }  /* ends loop over j */

      jj=0; 
      set_j=0;
      ii++;
      if(ii > square_size){
         ii = 0;
         set_i++;
      }
   }  /* ends loop over i */


      /* create a random image 0-255 */
   for(i=0; i<rows; i++)
      for(j=0; j<cols; j++)
        out_image[i][j] = (rand() % 255);


      /* create a random image 100-150  */
   for(i=0; i<rows; i++)
      for(j=0; j<cols; j++)
        out_image[i][j] = 100 + (rand() %  50);


      /* make vertical stripes */
   for(i=0; i<rows; i++){
      for(j=0; j<cols; j++){
		   if( (j > 10  &&  j < 30)    ||
		       (j > 50  &&  j < 70)    ||
		       (j > 90  &&  j < 99) )
			   out_image[i][j] = 200;
			else
			   out_image[i][j] = 50;
	   }
   }


      /* make a blank image that is not zero */
   for(i=0; i<rows; i++){
      for(j=0; j<cols; j++){
		   out_image[i][j] = 150;
	   }
   }


      /* make a random pattern background */
   for(i=0; i<rows/2; i++)
      for(j=0; j<cols/2; j++)
        out_image[i][j] = (abs(rand()) ) %  255;

      /* make a horizontal stripe */
   for(i=10; i<20; i++)
      for(j=0; j<cols/2; j++)
        out_image[i][j] = (abs(rand()) ) % 25   + 50;

      /* make a horizontal stripe */
   for(i=30; i<40; i++)
      for(j=0; j<cols/2; j++)
        out_image[i][j] = (abs(rand()) ) % 25   + 50;

      /* make a vertical stripe */
   for(i=0; i<rows/2; i++)
      for(j=10; j<20; j++)
        out_image[i][j] = (abs(rand()) ) % 25   + 50;

      /* make a vertical stripe */
   for(i=0; i<rows/2; i++)
      for(j=30; j<40; j++)
        out_image[i][j] = (abs(rand()) ) % 25   + 50;

      /* copy the 50x50 to the rest of the 100x100 */
   for(i=0; i<50; i++)
      for(j=50; j<cols; j++)
	     out_image[i][j] = out_image[i][j-50];

   for(i=50; i<rows; i++)
      for(j=0; j<50; j++)
	     out_image[i][j] = out_image[i-50][j];

   for(i=50; i<rows; i++)
      for(j=50; j<cols; j++)
	     out_image[i][j] = out_image[i-50][j-50];

      /* make the left edge look like the right edge */
   for(i=0; i<rows; i++)
      for(j=90; j<cols; j++)
	     out_image[i][j] = out_image[i][j-90];

      /* make the bottom edge look like the top edge */
   for(i=90; i<rows; i++)
      for(j=0; j<cols; j++)
	     out_image[i][j] = out_image[i-90][j];


short p1[10][10] = 
   {{11, 11, 11, 11, 11, 11, 11, 11, 11, 11},
    {11, 11, 11, 11, 11, 11, 11, 11, 11, 11},
    {11, 11, 11, 11, 11, 11, 11, 11, 11, 11},
    {11, 11, 11, 11, 11, 11, 11, 11, 11, 11},
    {11, 11, 11, 11, 11, 11, 11, 11, 11, 11},
    {11, 11, 11, 11, 11, 11, 11, 11, 11, 11},
    {11, 11, 11, 11, 11, 11, 11, 11, 11, 11},
    {11, 11, 11, 11, 11, 11, 11, 11, 11, 11},
    {11, 11, 11, 11, 11, 11, 11, 11, 11, 11},
    {11, 11, 11, 11, 11, 11, 11, 11, 11, 11}};

short p2[10][10] = 
   {{11, 11, 11, 11, 11, 11, 11, 11, 11, 11},
    {11, 11, 11, 11, 11, 11, 11, 11, 11, 11},
    {11, 11, 11, 11, 11, 11, 11, 11, 11, 11},
    {11, 11, 11, 11, 11, 11, 11, 11, 11, 11},
    {11, 11, 11, 11, 11, 11, 11, 11, 11, 11},
    {11, 11, 11, 11, 11, 11, 11, 11, 11, 11},
    {11, 11, 11, 11, 11, 11, 11, 11, 11, 11},
    {11, 11, 11, 11, 11, 11, 11, 11, 11, 11},
    {11, 11, 11, 11, 11, 11, 11, 11, 11, 11},
    {11, 11, 11, 11, 11, 11, 11, 11, 11, 11}};

#endif
