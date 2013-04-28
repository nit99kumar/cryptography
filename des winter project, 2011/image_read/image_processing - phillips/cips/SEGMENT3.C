
       /***********************************************
       *
       *       file d:\cips\segment2.c
       *
       *       Functions: This file contains
       *
       *
       *
       *
       *
       *
       *
       *       Purpose:
       *
       *       External Calls:
       *          wtiff.c - round_off_image_size
       *                    create_file_if_needed
       *                    write_array_into_tiff_image
       *          tiff.c - read_tiff_header
       *          rtiff.c - read_tiff_image
       *          numcvrt.c - get_integer
       *
       *
       *       Modifications:
       *          5 December 1992 - created
       *
       *************************************************/

#include "cips.h"


short skeleton01[3][3] = {{1, 1, 1},
                          {0, 1, 1},
                          {0, 0, 1}};

short skeleton02[3][3] = {{1, 0, 0},
                          {1, 1, 0},
                          {1, 1, 1}};

short skeleton03[3][3] = {{0, 0, 1},
                          {0, 1, 1},
                          {1, 1, 1}};

short skeleton04[3][3] = {{1, 1, 1},
                          {1, 1, 0},
                          {1, 0, 0}};

short skeleton05[3][3] = {{1, 1, 1},
                          {0, 1, 0},
                          {0, 0, 0}};

short skeleton06[3][3] = {{1, 0, 0},
                          {1, 1, 0},
                          {1, 0, 0}};

short skeleton07[3][3] = {{0, 0, 1},
                          {0, 1, 1},
                          {0, 0, 1}};

short skeleton08[3][3] = {{0, 0, 0},
                          {0, 1, 0},
                          {1, 1, 1}};

short skeleton09[3][3] = {{1, 1, 1},
                          {1, 1, 1},
                          {0, 0, 0}};

short skeleton10[3][3] = {{0, 0, 0},
                          {1, 1, 1},
                          {1, 1, 1}};

short skeleton11[3][3] = {{1, 1, 0},
                          {1, 1, 0},
                          {1, 1, 0}};

short skeleton12[3][3] = {{0, 1, 1},
                          {0, 1, 1},
                          {0, 1, 1}};

short skeleton13[3][3] = {{1, 1, 0},
                          {1, 1, 0},
                          {0, 0, 0}};

short skeleton14[3][3] = {{0, 1, 1},
                          {0, 1, 1},
                          {0, 0, 0}};

short skeleton15[3][3] = {{0, 0, 0},
                          {1, 1, 0},
                          {1, 1, 0}};

short skeleton16[3][3] = {{0, 0, 0},
                          {0, 1, 1},
                          {0, 1, 1}};

short skeleton17[3][3] = {{0, 1, 0},
                          {1, 1, 0},
                          {0, 0, 0}};

short skeleton18[3][3] = {{0, 1, 0},
                          {0, 1, 1},
                          {0, 0, 0}};

short skeleton19[3][3] = {{0, 0, 0},
                          {1, 1, 0},
                          {0, 1, 0}};

short skeleton20[3][3] = {{0, 0, 0},
                          {0, 1, 1},
                          {0, 1, 0}};




     /*******************************************
     *
     *   skeleton(..
     *
     *   This function performs the range operation.
     *
     *******************************************/


skeleton(in_name, out_name, the_image, out_image,
         il, ie, ll, le, value)
   char   in_name[], out_name[];
   int    il, ie, ll, le;
   short  the_image[ROWS][COLS],
          out_image[ROWS][COLS],
          value;
{
char namme[80];
   int    a, b, big_count, count, i, j, k, l,
          not_finished, length, width;
   short  temp[3][3];
   struct tiff_header_struct image_header;

      /**********************************************
      *
      *   Allocate the elements array large enough
      *   to hold size*size shorts.
      *
      **********************************************/

   create_file_if_needed(in_name, out_name, out_image);

   read_tiff_image(in_name, the_image, il, ie, ll, le);

      /***************************
      *
      *   Loop over image array
      *
      ****************************/

   big_count    = 0;
   not_finished = 1;
   while(not_finished){
      count = 0;
      printf("\n");

      for(i=1; i<ROWS-1; i++){
         if( (i%10) == 0) printf("%3d", i);
         for(j=1; j<COLS-1; j++){
            if(the_image[i][j] == value){
               for(a=-1; a<=1; a++){
                  for(b=-1; b<=1; b++){
                     temp[a+1][b+1] = the_image[i+a][j+b];
                  }  /* ends loop over b */
               }  /* ends loop over a */
               if(can_be_removed(temp)){
                  the_image[i][j] = 0;
                  count++;
               }  /* ends if can_be_removed */
            }  /* ends if the_image == value */
         }  /* ends loop over j */
      }  /* ends loop over i */

      printf("\nPass %d Eroded %d points", ++big_count, count);
sprintf(namme, "f:k%d.tif", big_count);
printf("\nwriting to file %s", namme);
write_array_into_tiff_image(namme, the_image, il, ie, ll, le);
      if(count == 0) not_finished = 0;
   }  /* ends while not_finished */

   fix_edges(the_image, 3);

   write_array_into_tiff_image(out_name, the_image,
                               il, ie, ll, le);

}  /* ends skeleton */



     /*******************************************
     *
     *   can_be_removed(..
     *
     *   This function
     *
     *******************************************/

can_be_removed(temp)
   short temp[3][3];
{
   int i, j, yes=1, no=0;
   short tt[3][3];

   copy_3_x_3(tt, skeleton01);
   if(compare_3_x_3(tt, temp))
      return(yes);

   copy_3_x_3(tt, skeleton02);
   if(compare_3_x_3(tt, temp))
      return(yes);

   copy_3_x_3(tt, skeleton03);
   if(compare_3_x_3(tt, temp))
      return(yes);

   copy_3_x_3(tt, skeleton04);
   if(compare_3_x_3(tt, temp))
      return(yes);

   copy_3_x_3(tt, skeleton05);
   if(compare_3_x_3(tt, temp))
      return(yes);

   copy_3_x_3(tt, skeleton06);
   if(compare_3_x_3(tt, temp))
      return(yes);

   copy_3_x_3(tt, skeleton07);
   if(compare_3_x_3(tt, temp))
      return(yes);

   copy_3_x_3(tt, skeleton08);
   if(compare_3_x_3(tt, temp))
      return(yes);

   copy_3_x_3(tt, skeleton09);
   if(compare_3_x_3(tt, temp))
      return(yes);

   copy_3_x_3(tt, skeleton10);
   if(compare_3_x_3(tt, temp))
      return(yes);

   copy_3_x_3(tt, skeleton11);
   if(compare_3_x_3(tt, temp))
      return(yes);

   copy_3_x_3(tt, skeleton12);
   if(compare_3_x_3(tt, temp))
      return(yes);

   copy_3_x_3(tt, skeleton13);
   if(compare_3_x_3(tt, temp))
      return(yes);

   copy_3_x_3(tt, skeleton14);
   if(compare_3_x_3(tt, temp))
      return(yes);

   copy_3_x_3(tt, skeleton15);
   if(compare_3_x_3(tt, temp))
      return(yes);

   copy_3_x_3(tt, skeleton16);
   if(compare_3_x_3(tt, temp))
      return(yes);

   copy_3_x_3(tt, skeleton17);
   if(compare_3_x_3(tt, temp))
      return(yes);

   copy_3_x_3(tt, skeleton18);
   if(compare_3_x_3(tt, temp))
      return(yes);

   copy_3_x_3(tt, skeleton19);
   if(compare_3_x_3(tt, temp))
      return(yes);

   copy_3_x_3(tt, skeleton20);
   if(compare_3_x_3(tt, temp))
      return(yes);

   return(no);

}  /* ends can_be_removed */


/* return a 1 if the two arrays match */
compare_3_x_3(a, b)
   short a[3][3], b[3][3];
{
   int i, j, match=1;
   for(i=0; i<3; i++){
      for(j=0; j<3; j++){
         if((a[i][j] != 0  &&
             b[i][j] == 0) ||
            (a[i][j] == 0  &&
             b[i][j] != 0))
            match = 0;
      }
   }
   return(match);
}
