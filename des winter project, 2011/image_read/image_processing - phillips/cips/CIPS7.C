
   /*************************** 
   * 
   *   cips7.c 
   *   COMPOSITE FILE COMPRISING: 
   *   ed.c 
   *   skeleton.c 
   *   segment.c 
   *   segment2.c 
   * 
   ***************************\ 



       /**********************************************
       *
       *   file d:\cips\ed.c
       *
       *   Functions: This file contains
       *      erosion
       *      dilation
       *      mask_erosion
       *      mask_dilation
       *      interior_outline
       *      exterior_outline
       *      copy_3_x_3
       *      opening
       *      closing
       *      get_shape_options
       *
       *   Purpose:
       *      These functions perform the erosion,
       *      dilation, outlining, opening and
       *      closing operations.
       *
       *   External Calls:
       *      wtiff.c - round_off_image_size
       *                create_file_if_needed
       *                write_array_into_tiff_image
       *      tiff.c - read_tiff_header
       *      rtiff.c - read_tiff_image
       *      numcvrt.c - get_integer
       *
       *   Modifications:
       *      14 March 1993 - created
       *
       ************************************************/

#include "cips.h"



short edmask1[3][3] = {{0, 1, 0},
                       {0, 1, 0},
                       {0, 1, 0}};

short edmask2[3][3] = {{0, 0, 0},
                       {1, 1, 1},
                       {0, 0, 0}};

short edmask3[3][3] = {{0, 1, 0},
                       {1, 1, 1},
                       {0, 1, 0}};

short edmask4[3][3] = {{1, 1, 1},
                       {1, 1, 1},
                       {1, 1, 1}};






     /*******************************************
     *
     *   mask_dilation(...
     *
     *   This function performs the dilation
     *   operation using the erosion-dilation
     *   3x3 masks given above.  It works on
     *   0-value images.
     *
     *******************************************/

mask_dilation(in_name, out_name, the_image, out_image,
              il, ie, ll, le, value, mask_type)
   char   in_name[], out_name[];
   int    il, ie, ll, le;
   short  the_image[ROWS][COLS],
          out_image[ROWS][COLS],
          mask_type, value;
{
   int    a, b, count, i, j, k;
   short  mask[3][3], max;

      /**************************************
      *
      *   Copy the 3x3 erosion-dilation mask
      *   specified by the mask_type.
      *
      ***************************************/

   switch(mask_type){
      case 1:
         copy_3_x_3(mask, edmask1);
         break;
      case 2:
         copy_3_x_3(mask, edmask2);
         break;
      case 3:
         copy_3_x_3(mask, edmask3);
         break;
      case 4:
         copy_3_x_3(mask, edmask4);
         break;
      default:
         printf("\nInvalid mask type, using mask 4");
         copy_3_x_3(mask, edmask4);
         break;
   }

   create_file_if_needed(in_name, out_name, out_image);

   read_tiff_image(in_name, the_image, il, ie, ll, le);

      /***************************
      *
      *   Loop over image array
      *
      ****************************/

   printf("\n");

   for(i=1; i<ROWS-1; i++){
      if( (i%10) == 0) printf("%3d", i);
      for(j=1; j<COLS-1; j++){
         max = 0;
         for(a=-1; a<=1; a++){
             for(b=-1; b<=1; b++){
                if(mask[a+1][b+1] == 1){
                   if(the_image[i+a][j+b] > max)
                      max = the_image[i+a][j+b];
                }  /* ends if mask == 1 */
             }  /*  ends loop over b */
         }  /* ends loop over a */
         out_image[i][j] = max;
      }  /* ends loop over j */
   }  /* ends loop over i */

   fix_edges(out_image, 3);

   write_array_into_tiff_image(out_name, out_image,
                               il, ie, ll, le);

}  /* ends mask_dilation */





     /*******************************************
     *
     *   mask_erosion(...
     *
     *   This function performs the erosion
     *   operation using the erosion-dilation
     *   3x3 masks given above.  It works on
     *   0-value images.
     *
     *******************************************/

mask_erosion(in_name, out_name, the_image, out_image,
             il, ie, ll, le, value, mask_type)
   char   in_name[], out_name[];
   int    il, ie, ll, le;
   short  the_image[ROWS][COLS],
          out_image[ROWS][COLS],
          mask_type, value;
{
   int    a, b, count, i, j, k;
   short  mask[3][3], min;

      /**************************************
      *
      *   Copy the 3x3 erosion-dilation mask
      *   specified by the mask_type.
      *
      ***************************************/

   switch(mask_type){
      case 1:
         copy_3_x_3(mask, edmask1);
         break;
      case 2:
         copy_3_x_3(mask, edmask2);
         break;
      case 3:
         copy_3_x_3(mask, edmask3);
         break;
      case 4:
         copy_3_x_3(mask, edmask4);
         break;
      default:
         printf("\nInvalid mask type, using mask 4");
         copy_3_x_3(mask, edmask4);
         break;
   }

   create_file_if_needed(in_name, out_name, out_image);

   read_tiff_image(in_name, the_image, il, ie, ll, le);

      /***************************
      *
      *   Loop over image array
      *
      ****************************/

   printf("\n");

   for(i=1; i<ROWS-1; i++){
      if( (i%10) == 0) printf("%3d", i);
      for(j=1; j<COLS-1; j++){
         min = value;
         for(a=-1; a<=1; a++){
             for(b=-1; b<=1; b++){
                if(mask[a+1][b+1] == 1){
                   if(the_image[i+a][j+b] < min)
                      min = the_image[i+a][j+b];
                }  /* ends if mask == 1 */
             }  /*  ends loop over b */
         }  /* ends loop over a */
         out_image[i][j] = min;
      }  /* ends loop over j */
   }  /* ends loop over i */

   fix_edges(out_image, 3);

   write_array_into_tiff_image(out_name, out_image,
                               il, ie, ll, le);

}  /* ends mask_erosion */





     /*******************************************
     *
     *   erosion(...
     *
     *   This function performs the erosion
     *   operation.  If a value pixel has more
     *   than the threshold number of 0
     *   neighbors, you erode it by setting it
     *   to 0.
     *
     *******************************************/

erosion(in_name, out_name, the_image, out_image,
        il, ie, ll, le, value, threshold)
   char   in_name[], out_name[];
   int    il, ie, ll, le;
   short  the_image[ROWS][COLS],
          out_image[ROWS][COLS],
          threshold, value;
{
   int    a, b, count, i, j, k;

   create_file_if_needed(in_name, out_name, out_image);

   read_tiff_image(in_name, the_image, il, ie, ll, le);

      /***************************
      *
      *   Loop over image array
      *
      ****************************/

   for(i=0; i<ROWS; i++)
      for(j=0; j<COLS; j++)
         out_image[i][j] = the_image[i][j];

   printf("\n");

   for(i=1; i<ROWS-1; i++){
      if( (i%10) == 0) printf("%3d", i);
      for(j=1; j<COLS-1; j++){
         if(the_image[i][j] == value){
            count = 0;
            for(a=-1; a<=1; a++){
                for(b=-1; b<=1; b++){
                      if(the_image[i+a][j+b] == 0)
                         count++;
                }  /*  ends loop over b */
            }  /* ends loop over a */
            if(count > threshold) out_image[i][j] = 0;
         }  /* ends if the_image == value */
      }  /* ends loop over j */
   }  /* ends loop over i */

   fix_edges(out_image, 3);

   write_array_into_tiff_image(out_name, out_image,
                               il, ie, ll, le);

}  /* ends erosion */





     /*******************************************
     *
     *   dilation(...
     *
     *   This function performs the dilation
     *   operation.  If a 0 pixel has more than
     *   threshold number of value neighbors,
     *   you dilate it by setting it to value.
     *
     *******************************************/

dilation(in_name, out_name, the_image, out_image,
         il, ie, ll, le, value, threshold)
   char   in_name[], out_name[];
   int    il, ie, ll, le;
   short  the_image[ROWS][COLS],
          out_image[ROWS][COLS],
          threshold, value;
{
   int    a, b, count, i, j, k;

   create_file_if_needed(in_name, out_name, out_image);

   read_tiff_image(in_name, the_image, il, ie, ll, le);

      /***************************
      *
      *   Loop over image array
      *
      ****************************/

   printf("\n");

   for(i=1; i<ROWS-1; i++){
      if( (i%10) == 0) printf("%3d", i);
      for(j=1; j<COLS-1; j++){
         out_image[i][j] = the_image[i][j];
         if(the_image[i][j] == 0){
            count = 0;
            for(a=-1; a<=1; a++){
                for(b=-1; b<=1; b++){
                   if(a!=0  &&  b!=0){
                      if(the_image[i+a][j+b] == value)
                         count++;
                   }  /* ends avoid the center pixel */
                }  /*  ends loop over b */
            }  /* ends loop over a */
            if(count > threshold)
               out_image[i][j] = value;
         }  /* ends if the_image == 0 */
      }  /* ends loop over j */
   }  /* ends loop over i */

   fix_edges(out_image, 3);

   write_array_into_tiff_image(out_name, out_image,
                               il, ie, ll, le);

}  /* ends dilation */





     /*******************************************
     *
     *   interior_outline(...
     *
     *   This function produces the outline of
     *   any "holes" inside an object.  The
     *   method is:
     *      output = erosion of input
     *      final output = input - output
     *
     *******************************************/

interior_outline(in_name, out_name, the_image,
                 out_image, il, ie, ll, le, value,
                 mask_type)
   char   in_name[], out_name[];
   int    il, ie, ll, le;
   short  the_image[ROWS][COLS],
          out_image[ROWS][COLS],
          mask_type, value;
{
   int    a, b, count, i, j, k;
   short  mask[3][3], max;

      /**************************************
      *
      *   Copy the 3x3 erosion-dilation mask
      *   specified by the mask_type.
      *
      ***************************************/

   switch(mask_type){
      case 1:
         copy_3_x_3(mask, edmask1);
         break;
      case 2:
         copy_3_x_3(mask, edmask2);
         break;
      case 3:
         copy_3_x_3(mask, edmask3);
         break;
      case 4:
         copy_3_x_3(mask, edmask4);
         break;
      default:
         printf("\nInvalid mask type, using mask 4");
         copy_3_x_3(mask, edmask4);
         break;
   }

   create_file_if_needed(in_name, out_name, out_image);

   read_tiff_image(in_name, the_image, il, ie, ll, le);

   mask_erosion(in_name, out_name, the_image,
                out_image, il, ie, ll, le,
                value, mask_type);

   for(i=0; i<ROWS; i++)
      for(j=0; j<COLS; j++)
         the_image[i][j] =
            the_image[i][j] - out_image[i][j];

   write_array_into_tiff_image(out_name, the_image,
                               il, ie, ll, le);

}  /* ends interior_outline */





     /*******************************************
     *
     *   exterior_outline(...
     *
     *   This function produces the outline of
     *   exterior of an object.  The
     *   method is:
     *      output = dilation of input
     *      final output = output - input
     *
     *******************************************/


exterior_outline(in_name, out_name, the_image, out_image,
          il, ie, ll, le, value, mask_type)
   char   in_name[], out_name[];
   int    il, ie, ll, le;
   short  the_image[ROWS][COLS],
          out_image[ROWS][COLS],
          mask_type, value;
{
   int    a, b, count, i, j, k;
   short  mask[3][3], max;

      /**************************************
      *
      *   Copy the 3x3 erosion-dilation mask
      *   specified by the mask_type.
      *
      ***************************************/

   switch(mask_type){
      case 1:
         copy_3_x_3(mask, edmask1);
         break;
      case 2:
         copy_3_x_3(mask, edmask2);
         break;
      case 3:
         copy_3_x_3(mask, edmask3);
         break;
      case 4:
         copy_3_x_3(mask, edmask4);
         break;
      default:
         printf("\nInvalid mask type, using mask 4");
         copy_3_x_3(mask, edmask4);
         break;
   }

   create_file_if_needed(in_name, out_name, out_image);

   read_tiff_image(in_name, the_image, il, ie, ll, le);

   mask_dilation(in_name, out_name, the_image,
                 out_image, il, ie, ll, le,
                 value, mask_type);

   for(i=0; i<ROWS; i++)
      for(j=0; j<COLS; j++)
         the_image[i][j] =
            out_image[i][j] - the_image[i][j];

   write_array_into_tiff_image(out_name, the_image,
                               il, ie, ll, le);

}  /* ends exterior_outline */


     /***********************************************
     *
     *   copy_3_x_3(a, b)
     *
     *   This function copies a 3x3 array of shorts
     *   from one array to another.  It copies array
     *   b into array a.
     *
     ***********************************************/

copy_3_x_3(a, b)
   short a[3][3], b[3][3];
{
   int i, j;
   for(i=0; i<3; i++)
      for(j=0; j<3; j++)
         a[i][j] = b[i][j];
}  /* ends copy_3_x_3 */





     /*******************************************
     *
     *   opening(...
     *
     *   Opening is erosion followed by dilation.
     *   This routine will use the mask erosion
     *   and dilation.  You could use the other
     *   types and you could mix the two types.
     *
     *   The number parameter specifies how
     *   erosions to perform before doing one
     *   dilation.
     *
     *******************************************/

opening(in_name, out_name, the_image, out_image,
        il, ie, ll, le, value, mask_type, number)
   char   in_name[], out_name[];
   int    il, ie, ll, le, number;
   short  the_image[ROWS][COLS],
          out_image[ROWS][COLS],
          mask_type, value;
{
   int    a, b, count, i, j, k;
   short  mask[3][3], max;

      /**************************************
      *
      *   Copy the 3x3 erosion-dilation mask
      *   specified by the mask_type.
      *
      ***************************************/

   switch(mask_type){
      case 1:
         copy_3_x_3(mask, edmask1);
         break;
      case 2:
         copy_3_x_3(mask, edmask2);
         break;
      case 3:
         copy_3_x_3(mask, edmask3);
         break;
      case 4:
         copy_3_x_3(mask, edmask4);
         break;
      default:
         printf("\nInvalid mask type, using mask 4");
         copy_3_x_3(mask, edmask4);
         break;
   }

   create_file_if_needed(in_name, out_name, out_image);

   read_tiff_image(in_name, the_image, il, ie, ll, le);

   mask_erosion(in_name, out_name, the_image,
                out_image, il, ie, ll, le,
                value, mask_type);

   if(number > 1){
      count = 1;
      while(count < number){
         count++;
         mask_erosion(out_name, out_name, the_image,
                       out_image, il, ie, ll, le,
                       value, mask_type);
      }  /* ends while */
   }  /* ends if number > 1 */

   mask_dilation(out_name, out_name, the_image,
                 out_image, il, ie, ll, le,
                 value, mask_type);

   write_array_into_tiff_image(out_name, out_image,
                               il, ie, ll, le);

}  /* ends opening */





     /*******************************************
     *
     *   closing(...
     *
     *   Closing is dilation followed by erosion.
     *   This routine will use the mask erosion
     *   and dilation.  You could use the other
     *   types and you could mix the two types.
     *
     *   The number parameter specifies how
     *   dilations to perform before doing one
     *   erosion.
     *
     *******************************************/

closing(in_name, out_name, the_image, out_image,
        il, ie, ll, le, value, mask_type, number)
   char   in_name[], out_name[];
   int    il, ie, ll, le, number;
   short  the_image[ROWS][COLS],
          out_image[ROWS][COLS],
          mask_type, value;
{
   int    a, b, count, i, j, k;
   short  mask[3][3], max;

      /**************************************
      *
      *   Copy the 3x3 erosion-dilation mask
      *   specified by the mask_type.
      *
      ***************************************/

   switch(mask_type){
      case 1:
         copy_3_x_3(mask, edmask1);
         break;
      case 2:
         copy_3_x_3(mask, edmask2);
         break;
      case 3:
         copy_3_x_3(mask, edmask3);
         break;
      case 4:
         copy_3_x_3(mask, edmask4);
         break;
      default:
         printf("\nInvalid mask type, using mask 4");
         copy_3_x_3(mask, edmask4);
         break;
   }

   create_file_if_needed(in_name, out_name, out_image);

   read_tiff_image(in_name, the_image, il, ie, ll, le);

   mask_dilation(in_name, out_name, the_image,
                 out_image, il, ie, ll, le,
                 value, mask_type);

   if(number > 1){
      count = 1;
      while(count < number){
         count++;
         mask_dilation(out_name, out_name, the_image,
                        out_image, il, ie, ll, le,
                        value, mask_type);
      }  /* ends while */
   }  /* ends if number > 1 */

   mask_erosion(out_name, out_name, the_image,
                out_image, il, ie, ll, le,
                value, mask_type);

   write_array_into_tiff_image(out_name, out_image,
                               il, ie, ll, le);

}  /* ends closing */





     /*******************************************
     *
     *   get_shape_options(...
     *
     *   This function interacts with the user
     *   to obtain the parameters for calling
     *   the shape routines.
     *
     *******************************************/

get_shape_options(type, value, threshold, number)
   char   type[];
   int    *number, *threshold;
   short  *value;
{
   int not_finished = 1, response;

   while(not_finished){

   printf("\nThe shape options are:");
   printf("\n\t1. Type is %s", type);
   printf("\n\t    recall: EROsion DILation Mask-ERosion"
          "\n\t            Mask_DIlation INTerior-outline"
          "\n\t            EXTerior-outline THInning"
          "\n\t            Dilate-Not-Join OPEning"
          "\n\t            CLOsing SPecial-Opening"
          "\n\t            SPecial-Closing"
          "\n\t            Euclidean-Distance-Measure"
          "\n\t            Medial-Axis-Transform");
   printf("\n\t2. value is %d", *value);
   printf("\n\t3. threshold or mask type is %d",
           *threshold);
   printf("\n\t4. number of iterations is %d", *number);
   printf("\n\t   (used only in opening and closing)");
   printf("\n\nEnter choice (0 = no change) _\b");

      get_integer(&response);

      if(response == 0){
        not_finished = 0;
      }

      if(response == 1){
      printf("\nEnter type of operation");
      printf("\n\t    recall: EROsion DILation Mask-ERosion"
             "\n\t            Mask_DIlation INTerior-outline"
             "\n\t            EXTerior-outline THInning"
             "\n\t            Dilate-Not-Join OPEning"
             "\n\t            CLOsing SPecial-Opening"
             "\n\t            SPecial-Closing"
             "\n\t            Euclidean-Distance-Measure"
             "\n\t            Medial-Axis-Transform");
      printf("\n\t:");
      gets(type);
      }

      if(response == 2){
         printf("\nEnter value: ___\b\b\b");
         get_integer(value);
      }

      if(response == 3){
         printf("\nEnter threshold or mask type: ___");
         printf("\b\b\b");
         get_integer(threshold);
      }

      if(response == 4){
         printf("\nEnter number of iterations: ___");
         printf("\b\b\b");
         get_integer(number);
      }

   }  /* ends while not_finished */

}  /* ends get_shape_options */

       /**********************************************
       *
       *       file d:\cips\skeleton.c
       *
       *       Functions: This file contains
       *          thinning
       *          can_thin
       *          dilate_not_join
       *          can_dilate
       *          little_label_and_check
       *          special_closing
       *          special_opening
       *          edm
       *          distanc_8
       *          mat
       *          mat_d
       *
       *       Purpose:
       *          These functions thin objects in
       *          an image, dilate objects, and
       *          perform opening and closing
       *          without removing or joining
       *          objects.
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
       *          7 March 1993 - created
       *
       ************************************************/






     /*******************************************
     *
     *   thinning(...
     *
     *   Use a variation of the grass fire
     *   wave front approach.
     *
     *   Raster scan the image left to right
     *   and examine and thin the left edge pixels
     *   (a 0 to value transition).  Process them
     *   normally and "save" the result.  Next,
     *   raster scan the image right to left and
     *   save.  Raster scan top to bottom and save.
     *   Raster scan bottom to top and save.
     *
     *   That is one complete pass.
     *
     *   Keep track of pixels thinned for a
     *   pass and quit when you make a complete
     *   pass without thinning any pixels.
     *
     *******************************************/


thinning(in_name, out_name, the_image, out_image,
         il, ie, ll, le, value, threshold, once_only)
   char   in_name[], out_name[];
   int    il, ie, ll, le, once_only;
   short  the_image[ROWS][COLS],
          out_image[ROWS][COLS],
          threshold, value;
{
   int    a, b, big_count, count, i, j, k,
          not_finished;

   create_file_if_needed(in_name, out_name, out_image);

   read_tiff_image(in_name, the_image, il, ie, ll, le);


   for(i=0; i<ROWS; i++)
      for(j=0; j<COLS; j++)
         out_image[i][j] = the_image[i][j];

   not_finished = 1;
   while(not_finished){

      if(once_only == 1)
        not_finished = 0;
      big_count = 0;

         /***************************
         *
         *   Scan left to right
         *   Look for 0-value transition
         *
         ****************************/

      printf("\n");
      for(i=1; i<ROWS-1; i++){
         if( (i%10) == 0) printf("%3d", i);
         for(j=1; j<COLS-1; j++){
            if(the_image[i][j-1] == 0   &&
               the_image[i][j]   == value){
               count = 0;
               for(a=-1; a<=1; a++){
                   for(b=-1; b<=1; b++){
                         if(the_image[i+a][j+b] == 0)
                            count++;
                   }  /*  ends loop over b */
               }  /* ends loop over a */
               if(count > threshold){
                  if(can_thin(the_image, i, j, value)){
                     out_image[i][j] = 0;
                     big_count++;
                  }  /* ends if can_thin */
               }  /* ends if count > threshold */
            }  /* ends if the_image == value */
         }  /* ends loop over j */
      }  /* ends loop over i */

         /**************************************
         *
         *   Copy the output back to the input.
         *
         **************************************/

      for(i=0; i<ROWS; i++)
         for(j=0; j<COLS; j++)
            the_image[i][j] = out_image[i][j];


         /***************************
         *
         *   Scan right to left
         *   Do this by scanning left
         *   to right and look for
         *   value-0 transition.
         *
         ****************************/

      printf("\n");
      for(i=1; i<ROWS-1; i++){
         if( (i%10) == 0) printf("%3d", i);
         for(j=1; j<COLS-1; j++){
            if(the_image[i][j+1] == 0   &&
               the_image[i][j]   == value){
               count = 0;
               for(a=-1; a<=1; a++){
                   for(b=-1; b<=1; b++){
                         if(the_image[i+a][j+b] == 0)
                            count++;
                   }  /*  ends loop over b */
               }  /* ends loop over a */
               if(count > threshold){
                  if(can_thin(the_image, i, j, value)){
                     out_image[i][j] = 0;
                     big_count++;
                  }  /* ends if can_thin */
               }  /* ends if count > threshold */
            }  /* ends if the_image == value */
         }  /* ends loop over j */
      }  /* ends loop over i */

         /**************************************
         *
         *   Copy the output back to the input.
         *
         **************************************/

      for(i=0; i<ROWS; i++)
         for(j=0; j<COLS; j++)
            the_image[i][j] = out_image[i][j];


         /***************************
         *
         *   Scan top to bottom
         *   Look for 0-value transition
         *
         ****************************/

      printf("\n");
      for(j=1; j<COLS-1; j++){
         if( (j%10) == 0) printf("%3d", j);
         for(i=1; i<ROWS-1; i++){
            if(the_image[i-1][j] == 0   &&
               the_image[i][j]   == value){
               count = 0;
               for(a=-1; a<=1; a++){
                   for(b=-1; b<=1; b++){
                         if(the_image[i+a][j+b] == 0)
                            count++;
                   }  /*  ends loop over b */
               }  /* ends loop over a */
               if(count > threshold){
                  if(can_thin(the_image, i, j, value)){
                     out_image[i][j] = 0;
                     big_count++;
                  }  /* ends if can_thin */
               }  /* ends if count > threshold */
            }  /* ends if the_image == value */
         }  /* ends loop over i */
      }  /* ends loop over j */

         /**************************************
         *
         *   Copy the output back to the input.
         *
         **************************************/

      for(i=0; i<ROWS; i++)
         for(j=0; j<COLS; j++)
            the_image[i][j] = out_image[i][j];



         /***************************
         *
         *   Scan bottom to top
         *   Do this by scanning top
         *   to bottom and look for
         *   value-0 transition.
         *
         ****************************/

      printf("\n");
      for(j=1; j<COLS-1; j++){
         if( (j%10) == 0) printf("%3d", j);
         for(i=1; i<ROWS-1; i++){
            if(the_image[i+1][j] == 0   &&
               the_image[i][j]   == value){
               count = 0;
               for(a=-1; a<=1; a++){
                   for(b=-1; b<=1; b++){
                         if(the_image[i+a][j+b] == 0)
                            count++;
                   }  /*  ends loop over b */
               }  /* ends loop over a */
               if(count > threshold){
                  if(can_thin(the_image, i, j, value)){
                     out_image[i][j] = 0;
                     big_count++;
                  }  /* ends if can_thin */
               }  /* ends if count > threshold */
            }  /* ends if the_image == value */
         }  /* ends loop over i */
      }  /* ends loop over j */

         /**************************************
         *
         *   Copy the output back to the input.
         *
         **************************************/

      for(i=0; i<ROWS; i++)
         for(j=0; j<COLS; j++)
            the_image[i][j] = out_image[i][j];

         /**************************************
         *
         *   Now look at the result of this big
         *   pass through the image.
         *
         ***************************************/

      printf("\n\nThinned %d pixels", big_count);
      if(big_count == 0)
         not_finished = 0;
      else{
         for(i=0; i<ROWS; i++)
            for(j=0; j<COLS; j++)
               the_image[i][j] = out_image[i][j];
      }  /* ends else */

   }  /* ends while not_finished */

   fix_edges(out_image, 3);
   write_array_into_tiff_image(out_name, out_image,
                               il, ie, ll, le);
}  /* ends thinning */





     /*******************************************
     *
     *   can_thin(...
     *
     *   Look at the neighbors of the center pixel.
     *   If a neighbor == value, then it must
     *   have a neighbor == value other than the
     *   center pixel.
     *
     *   Procedure:
     *   . Copy the 3x3 area surrounding pixel
     *     i,j to a temp array.
     *   . Set the center pixel to zero.
     *   . Look at each non-zero pixel in temp.
     *      . If you cannot find a non-zero
     *        neighbor.
     *      . Then you cannot thin.
     *
     *******************************************/

can_thin(the_image, i, j, value)
   int   i, j;
   short the_image[ROWS][COLS], value;
{
   int   a, b, c, d, count,
         no_neighbor, one=1, zero=0;
   short temp[3][3];

       /**************************************
       *
       *   Copy the center pixel and its
       *   neighbors to the temp array.
       *
       ***************************************/

   for(a=-1; a<2; a++)
      for(b=-1; b<2; b++)
         temp[a+1][b+1] = the_image[i+a][b+j];

       /**************************************
       *
       *   Set the center of temp to 0.
       *
       ***************************************/

   temp[1][1] = 0;

       /**************************************
       *
       *   Check the non-zero pixels in temp.
       *
       ***************************************/

   for(a=0; a<3; a++){
      for(b=0; b<3; b++){
         if(temp[a][b] == value){
            temp[a][b] = 0;

                /**************************************
                *
                *   Check the neighbors of this pixel
                *   If there is a single non-zero
                *   neighbor, set no_neighbor = 0.
                *
                ***************************************/

            no_neighbor = 1;
            for(c=-1; c<2; c++){
               for(d=-1; d<2; d++){
                  if( ((a+c) >= 0)   &&
                      ((a+c) <= 2)   &&
                      ((b+d) >= 0)   &&
                      ((b+d) <= 2)){
                     if(temp[a+c][b+d] == value){
                        no_neighbor = 0;
                     }  /* ends if temp == value */
                  }  /* ends if part of temp array */
               }  /* ends loop over d */
            }  /* ends loop over c */
            temp[a][b] = value;

                /**************************************
                *
                *   If the non-zero pixel did not
                *   have any non-zero neighbors,
                *   no_neighbor still equals 1,
                *   and we cannot thin, so return
                *   zero.
                *
                ***************************************/

            if(no_neighbor){
               return(zero);
            }
         }  /* ends if temp[a][b] == value */
      }  /* ends loop over b */
   }  /* ends loop over a */

       /**************************************
       *
       *   First, ensure the object is more
       *   than two wide.  If it is two wide,
       *   you will thin out the entire object.
       *   Check in all eight directions.
       *   If the distance to a zero is 0 or
       *   >= 2, then ok you can thin so go
       *   on to the remainder of this routine.
       *   If not, you cannot thin so return
       *   zero.
       *
       ***************************************/

   return(one);

}  /* ends can_thin */





     /*******************************************
     *
     *   dilate_not_join(...
     *
     *   Use a variation of the grass fire
     *   wave front approach.
     *
     *   Raster scan the image left to right
     *   and examine and dilate the left edge pixels
     *   (a value to 0 transition).  Process them
     *   normally and "save" the result.  Next,
     *   raster scan the image right to left and
     *   save.  Raster scan top to bottom and save.
     *   Raster scan bottom to top and save.
     *
     *   That is one complete pass.
     *
     *******************************************/

dilate_not_join(in_name, out_name, the_image, out_image,
                il, ie, ll, le, value, threshold)
   char   in_name[], out_name[];
   int    il, ie, ll, le;
   short  the_image[ROWS][COLS],
          out_image[ROWS][COLS],
          threshold, value;
{
   int    a, b, count, i, j, k;

   create_file_if_needed(in_name, out_name, out_image);

   read_tiff_image(in_name, the_image, il, ie, ll, le);


   for(i=0; i<ROWS; i++)
      for(j=0; j<COLS; j++)
         out_image[i][j] = the_image[i][j];

         /***************************
         *
         *   Scan left to right
         *   Look for value-0 transition
         *
         ****************************/

      printf("\n");
      for(i=1; i<ROWS-1; i++){
         if( (i%10) == 0) printf("%3d", i);
         for(j=1; j<COLS-1; j++){
            if(the_image[i][j-1] == value  &&
               the_image[i][j]   == 0){
               count = 0;
               for(a=-1; a<=1; a++){
                   for(b=-1; b<=1; b++){
                         if(the_image[i+a][j+b]==value)
                            count++;
                   }  /*  ends loop over b */
               }  /* ends loop over a */
               if(count > threshold){
                  if(can_dilate(the_image,i,j,value)){
                     out_image[i][j] = value;
                  }  /* ends if can_dilate */
               }  /* ends if count > threshold */
            }  /* ends if the_image == value */
         }  /* ends loop over j */
      }  /* ends loop over i */

         /**************************************
         *
         *   Copy the output back to the input.
         *
         **************************************/

      for(i=0; i<ROWS; i++)
         for(j=0; j<COLS; j++)
            the_image[i][j] = out_image[i][j];


         /***************************
         *
         *   Scan right to left
         *   Do this by scanning left
         *   to right and look for
         *   0-value transition.
         *
         ****************************/

      printf("\n");
      for(i=1; i<ROWS-1; i++){
         if( (i%10) == 0) printf("%3d", i);
         for(j=1; j<COLS-1; j++){
            if(the_image[i][j+1] == value   &&
               the_image[i][j]   == 0){
               count = 0;
               for(a=-1; a<=1; a++){
                   for(b=-1; b<=1; b++){
                         if(the_image[i+a][j+b]==value)
                            count++;
                   }  /*  ends loop over b */
               }  /* ends loop over a */
               if(count > threshold){
                  if(can_dilate(the_image,i,j,value)){
                     out_image[i][j] = value;
                  }  /* ends if can_dilate */
               }  /* ends if count > threshold */
            }  /* ends if the_image == value */
         }  /* ends loop over j */
      }  /* ends loop over i */

         /**************************************
         *
         *   Copy the output back to the input.
         *
         **************************************/

      for(i=0; i<ROWS; i++)
         for(j=0; j<COLS; j++)
            the_image[i][j] = out_image[i][j];


         /***************************
         *
         *   Scan top to bottom
         *   Look for value-0 transition
         *
         ****************************/

      printf("\n");
      for(j=1; j<COLS-1; j++){
         if( (j%10) == 0) printf("%3d", j);
         for(i=1; i<ROWS-1; i++){
            if(the_image[i-1][j] == value   &&
               the_image[i][j]   == 0){
               count = 0;
               for(a=-1; a<=1; a++){
                   for(b=-1; b<=1; b++){
                         if(the_image[i+a][j+b]==value)
                            count++;
                   }  /*  ends loop over b */
               }  /* ends loop over a */
               if(count > threshold){
                  if(can_dilate(the_image,i,j,value)){
                     out_image[i][j] = value;
                  }  /* ends if can_dilate */
               }  /* ends if count > threshold */
            }  /* ends if the_image == value */
         }  /* ends loop over i */
      }  /* ends loop over j */

         /**************************************
         *
         *   Copy the output back to the input.
         *
         **************************************/

      for(i=0; i<ROWS; i++)
         for(j=0; j<COLS; j++)
            the_image[i][j] = out_image[i][j];



         /***************************
         *
         *   Scan bottom to top
         *   Do this by scanning top
         *   to bottom and look for
         *   0-value transition.
         *
         ****************************/

      printf("\n");
      for(j=1; j<COLS-1; j++){
         if( (j%10) == 0) printf("%3d", j);
         for(i=1; i<ROWS-1; i++){
            if(the_image[i+1][j] == value   &&
               the_image[i][j]   == 0){
               count = 0;
               for(a=-1; a<=1; a++){
                   for(b=-1; b<=1; b++){
                         if(the_image[i+a][j+b]==value)
                            count++;
                   }  /*  ends loop over b */
               }  /* ends loop over a */
               if(count > threshold){
                  if(can_dilate(the_image,i,j,value)){
                     out_image[i][j] = value;
                  }  /* ends if can_dilate */
               }  /* ends if count > threshold */
            }  /* ends if the_image == value */
         }  /* ends loop over i */
      }  /* ends loop over j */

         /**************************************
         *
         *   Copy the output back to the input.
         *
         **************************************/

      for(i=0; i<ROWS; i++)
         for(j=0; j<COLS; j++)
            the_image[i][j] = out_image[i][j];


   fix_edges(out_image, 3);

   write_array_into_tiff_image(out_name, out_image,
                               il, ie, ll, le);

}  /* ends dilate_not_join */





     /*******************************************
     *
     *   can_dilate(...
     *
     *   This function decides if you can dilate
     *   (set to value) a pixel without joining
     *   two separate objects in a 3x3 area.
     *
     *   First, you grow regions inside the 3x3
     *   area.  Next, check if the center pixel
     *   has neighbors with differing values.
     *   If it does, you cannot dilate it because
     *   that would join two separate objects.
     *
     *******************************************/

can_dilate(the_image, i, j, value)
   int   i, j;
   short the_image[ROWS][COLS], value;
{
   int   a, b, c, d, count, found=0,
         no_neighbor,
         stack_pointer=-1,
         stack_empty=1,
         stack[12][2],
         pop_a, pop_b,
         one=1,
         zero=0;
   short first_value, label = 2, temp[3][3];

       /**************************************
       *
       *   Copy the center pixel and its
       *   neighbors to the temp array.
       *
       ***************************************/

   for(a=-1; a<2; a++)
      for(b=-1; b<2; b++)
         temp[a+1][b+1] = the_image[i+a][b+j];

       /**************************************
       *
       *   Grow objects inside the temp array.
       *
       ***************************************/

   for(a=0; a<3; a++){
      for(b=0; b<3; b++){
         stack_empty   = 1;
         stack_pointer = -1;
         if(temp[a][b] == value){
            little_label_and_check(temp, stack, label,
                                   &stack_empty,
                                   &stack_pointer,
                                   a, b, value);
            found = 1;
         }  /* ends if temp == value */

         while(stack_empty == 0){
            pop_a = stack[stack_pointer][0]; /* POP */
            pop_b = stack[stack_pointer][1]; /* POP */
            --stack_pointer;
            if(stack_pointer <= 0){
               stack_pointer = 0;
               stack_empty   = 1;
            }  /* ends if stack_pointer */
            little_label_and_check(temp, stack, label,
                                   &stack_empty,
                                   &stack_pointer,
                                   pop_a, pop_b, value);
         }  /* ends while stack_empty == 0 */

         if(found){
            found = 0;
            label++;
         }  /* ends if object_found */
      }  /* ends loop over b */
   }  /* ends loop over a */

       /**************************************
       *
       *   Look at the center pixel.  If it
       *   has two non-zero neigbors whose
       *   pixels are not the same, then
       *   you cannot dilate.
       *
       ***************************************/

   first_value = -1;
   for(a=0; a<3; a++){
      for(b=0; b<3; b++){
         if(temp[a][b]  != 0 &&
            first_value == -1){
            first_value = temp[a][b];
         }
         if(temp[a][b]  != 0  &&
            first_value != -1){
            if(temp[a][b] != first_value){
               return(zero);
            }
         }
      }  /* ends loop over b */
   }  /* ends loop over a */

   return(one);

}  /* ends can_dilate */





     /*******************************************
     *
     *   little_label_and_check(...
     *
     *   This function labels the objects in
     *   in a 3x3 area.
     *
     *******************************************/

little_label_and_check(temp, stack, label, stack_empty,
                       stack_pointer, a, b, value)
   int   a, b, stack[12][2], 
         *stack_empty, *stack_pointer;
   short temp[3][3], label, value;
{
   int c, d;

   temp[a][b] = label;
   for(c=a-1; c<=a+1; c++){
      for(d=b-1; d<=b+1; d++){
         if(c >= 0      &&
            c <= 2      &&
            d >= 0      &&
            d <= 2)
            if(temp[c][d] == value){  /* PUSH */
               *stack_pointer = *stack_pointer + 1;
               stack[*stack_pointer][0] = c;
               stack[*stack_pointer][1] = d;
               *stack_empty = 0;
            }  /* ends if temp == value */
      }  /* ends loop over d */
   }  /* ends loop over c */

}  /* ends little_label_and_check */





     /*******************************************
     *
     *   special_closing(...
     *
     *   Closing is dilation followed by erosion.
     *   This routine will use the dilate_not_join
     *   dilation routine.  This will not allow
     *   two separate objects to join.
     *
     *   The number parameter specifies how
     *   dilations to perform before doing one
     *   erosion.
     *
     *******************************************/

special_closing(in_name, out_name, the_image,
                out_image, il, ie, ll, le,
                value, threshold, number)
   char   in_name[], out_name[];
   int    il, ie, ll, le, number;
   short  the_image[ROWS][COLS],
          out_image[ROWS][COLS],
          threshold, value;
{
   int    a, b, count, i, j, k;

   create_file_if_needed(in_name, out_name, out_image);

   read_tiff_image(in_name, the_image, il, ie, ll, le);

   dilate_not_join(in_name, out_name, the_image,
                   out_image, il, ie, ll, le,
                   value, threshold);

   if(number > 1){
      count = 1;
      while(count < number){
         count++;
         dilate_not_join(out_name, out_name, the_image,
                         out_image, il, ie, ll, le,
                         value, threshold);
      }  /* ends while */
   }  /* ends if number > 1 */

   erosion(out_name, out_name, the_image,
           out_image, il, ie, ll, le,
           value, threshold);

   write_array_into_tiff_image(out_name, out_image,
                               il, ie, ll, le);

}  /* ends special_closing */





     /*******************************************
     *
     *   special_opening(...
     *
     *   Opening is erosion followed by dilation.
     *   This routine will use the thinning
     *   erosion routine.  This will not allow
     *   an object to erode to nothing.
     *
     *   The number parameter specifies how
     *   erosions to perform before doing one
     *   dilation.
     *
     *******************************************/

special_opening(in_name, out_name, the_image,
                out_image, il, ie, ll, le,
                value, threshold, number)
   char   in_name[], out_name[];
   int    il, ie, ll, le, number;
   short  the_image[ROWS][COLS],
          out_image[ROWS][COLS],
          threshold, value;
{
   int    a, b, count, i, j, k;

   create_file_if_needed(in_name, out_name, out_image);

   read_tiff_image(in_name, the_image, il, ie, ll, le);

   thinning(in_name, out_name, the_image,
            out_image, il, ie, ll, le,
            value, threshold, 1);

   if(number > 1){
      count = 1;
      while(count < number){
         count++;
         thinning(out_name, out_name, the_image,
                  out_image, il, ie, ll, le,
                  value, threshold, 1);
      }  /* ends while */
   }  /* ends if number > 1 */

   dilation(out_name, out_name, the_image,
            out_image, il, ie, ll, le,
            value, threshold);

   write_array_into_tiff_image(out_name, out_image,
                               il, ie, ll, le);

}  /* ends special_opening */





     /*******************************************
     *
     *   edm(..
     *
     *   This function calculates the Euclidean
     *   distance measure for objects in an image.
     *   It calculates the distance from any
     *   pixel=value to the nearest zero pixel
     *
     *******************************************/


edm(in_name, out_name, the_image, out_image,
          il, ie, ll, le, value)
   char   in_name[], out_name[];
   int    il, ie, ll, le;
   short  the_image[ROWS][COLS],
          out_image[ROWS][COLS],
          value;
{
   int    a, b, count, i, j, k;

   create_file_if_needed(in_name, out_name, out_image);

   read_tiff_image(in_name, the_image, il, ie, ll, le);

   for(i=0; i<ROWS; i++)
      for(j=0; j<COLS; j++)
         out_image[i][j] = 0;

      /***************************
      *
      *   Loop over image array
      *
      ****************************/

   printf("\n");

   for(i=0; i<ROWS; i++){
      if( (i%10) == 0) printf("%3d", i);
      for(j=0; j<COLS; j++){
         if(the_image[i][j] == value)
            out_image[i][j] = distance_8(the_image, 
                                         i, j, value);
      }  /* ends loop over j */
   }  /* ends loop over i */

   write_array_into_tiff_image(out_name, out_image,
                               il, ie, ll, le);

}  /* ends edm */



     /*******************************************
     *
     *   distance_8(..
     *
     *   This function finds the distance from
     *   a pixel to the nearest zero pixel.
     *   It search in all eight directions.
     *
     *******************************************/

distance_8(the_image, a, b, value)
   int   a, b;
   short the_image[ROWS][COLS], value;
{
   int i, j, measuring;
   short dist1  = 0,
         dist2  = 0,
         dist3  = 0,
         dist4  = 0,
         dist5  = 0,
         dist6  = 0,
         dist7  = 0,
         dist8  = 0,
         result = 0;

      /* straight up */
   measuring = 1;
   i = a;
   j = b;
   while(measuring){
      i--;
      if(i >= 0){
         if(the_image[i][j] == value)
            dist1++;
         else
            measuring = 0;
      }
      else
         measuring = 0;
   }  /* ends while measuring */
   result = dist1;

      /* straight down */
   measuring = 1;
   i = a;
   j = b;
   while(measuring){
      i++;
      if(i <= ROWS-1){
         if(the_image[i][j] == value)
            dist2++;
         else
            measuring = 0;
      }
      else
         measuring = 0;
   }  /* ends while measuring */
   if(dist2 <= result)
      result = dist2;

      /* straight left */
   measuring = 1;
   i = a;
   j = b;
   while(measuring){
      j--;
      if(j >= 0){
         if(the_image[i][j] == value)
            dist3++;
         else
            measuring = 0;
      }
      else
         measuring = 0;
   }  /* ends while measuring */
   if(dist3 <= result)
      result = dist3;

      /* straight right */
   measuring = 1;
   i = a;
   j = b;
   while(measuring){
      j++;
      if(j <= COLS-1){
         if(the_image[i][j] == value)
            dist4++;
         else
            measuring = 0;
      }
      else
         measuring = 0;
   }  /* ends while measuring */
   if(dist4 <= result)
      result = dist4;

      /* left and up */
   measuring = 1;
   i = a;
   j = b;
   while(measuring){
      j--;
      i--;
      if(j >= 0 && i>=0){
         if(the_image[i][j] == value)
            dist5++;
         else
            measuring = 0;
      }
      else
         measuring = 0;
   }  /* ends while measuring */
   dist5 = (dist5*14)/10;
   if(dist5 <= result)
      result = dist5;

      /* right and up */
   measuring = 1;
   i = a;
   j = b;
   while(measuring){
      j++;
      i--;
      if(j <=COLS-1 && i>=0){
         if(the_image[i][j] == value)
            dist6++;
         else
            measuring = 0;
      }
      else
         measuring = 0;
   }  /* ends while measuring */
   dist6 = (dist6*14)/10;
   if(dist6 <= result)
      result = dist6;

      /* right and down */
   measuring = 1;
   i = a;
   j = b;
   while(measuring){
      j++;
      i++;
      if(j <=COLS-1 && i<=ROWS-1){
         if(the_image[i][j] == value)
            dist7++;
         else
            measuring = 0;
      }
      else
         measuring = 0;
   }  /* ends while measuring */
   dist7 = (dist7*14)/10;
   if(dist7 <= result)
      result = dist7;

      /* left and down */
   measuring = 1;
   i = a;
   j = b;
   while(measuring){
      j--;
      i++;
      if(j >=0 && i<=ROWS-1){
         if(the_image[i][j] == value)
            dist8++;
         else
            measuring = 0;
      }
      else
         measuring = 0;
   }  /* ends while measuring */
   dist8 = (dist8*14)/10;
   if(dist8 <= result)
      result = dist8;

   return(result);
}  /* ends distance_8 */





     /*******************************************
     *
     *   mat(..
     *
     *   This function finds the medial axis
     *   transform for objects in an image.
     *   The mat are those points that are
     *   minimally distant to more than one
     *   boundary point.
     *
     *******************************************/


mat(in_name, out_name, the_image, out_image,
    il, ie, ll, le, value)
   char   in_name[], out_name[];
   int    il, ie, ll, le;
   short  the_image[ROWS][COLS],
          out_image[ROWS][COLS],
          value;
{
   int    a, b, count, i, j, k,
          length, width;

   create_file_if_needed(in_name, out_name, out_image);

   read_tiff_image(in_name, the_image, il, ie, ll, le);

   for(i=0; i<ROWS; i++)
      for(j=0; j<COLS; j++)
         out_image[i][j] = 0;

      /***************************
      *
      *   Loop over image array
      *
      ****************************/

   printf("\n");

   for(i=0; i<ROWS; i++){
      if( (i%10) == 0) printf("%3d", i);
      for(j=0; j<COLS; j++){
         if(the_image[i][j] == value)
            out_image[i][j] = mat_d(the_image, 
                                    i, j, value);
      }  /* ends loop over j */
   }  /* ends loop over i */

   write_array_into_tiff_image(out_name, out_image,
                               il, ie, ll, le);

}  /* ends mat */



     /*******************************************
     *
     *   mat_d(..
     *
     *   This function helps find the medial
     *   axis transform.
     *
     *   This function measures the distances
     *   from the point to a zero pixel in all
     *   eight directions.  Look for the two
     *   shortest distances in the eight distances.
     *   If the two shortest distances are
     *   equal, then the point in question
     *   is minimally distant to more than one
     *   boundary point.  Therefore, it is
     *   on the medial axis so return a value.
     *   Otherwise, return zero.
     *
     *******************************************/

mat_d(the_image, a, b, value)
   int   a, b;
   short the_image[ROWS][COLS], value;
{
   int i, j, measuring;
   short dist1  = 0,
         dist2  = 0,
         dist3  = 0,
         dist4  = 0,
         dist5  = 0,
         dist6  = 0,
         dist7  = 0,
         dist8  = 0,
         min1   = 255,
         min2   = 255,
         result = 0;

      /* straight up */
   measuring = 1;
   i = a;
   j = b;
   while(measuring){
      i--;
      if(i >= 0){
         if(the_image[i][j] == value)
            dist1++;
         else
            measuring = 0;
      }
      else
         measuring = 0;
   }  /* ends while measuring */
   result = dist1;
   min1   = dist1;

      /* straight down */
   measuring = 1;
   i = a;
   j = b;
   while(measuring){
      i++;
      if(i <= ROWS-1){
         if(the_image[i][j] == value)
            dist2++;
         else
            measuring = 0;
      }
      else
         measuring = 0;
   }  /* ends while measuring */
   if(dist2 <= result)
      result = dist2;
   if(dist2 < min1){
      min2 = min1;
      min1 = dist2;
   }
   else
      if(dist2 < min2)
         min2 = dist2;

      /* straight left */
   measuring = 1;
   i = a;
   j = b;
   while(measuring){
      j--;
      if(j >= 0){
         if(the_image[i][j] == value)
            dist3++;
         else
            measuring = 0;
      }
      else
         measuring = 0;
   }  /* ends while measuring */
   if(dist3 <= result)
      result = dist3;
   if(dist3 < min1){
      min2 = min1;
      min1 = dist3;
   }
   else
      if(dist3 < min2)
         min2 = dist3;

      /* straight right */
   measuring = 1;
   i = a;
   j = b;
   while(measuring){
      j++;
      if(j <= COLS-1){
         if(the_image[i][j] == value)
            dist4++;
         else
            measuring = 0;
      }
      else
         measuring = 0;
   }  /* ends while measuring */
   if(dist4 <= result)
      result = dist4;
   if(dist4 < min1){
      min2 = min1;
      min1 = dist4;
   }
   else
      if(dist4 < min2)
         min2 = dist4;

      /* left and up */
   measuring = 1;
   i = a;
   j = b;
   while(measuring){
      j--;
      i--;
      if(j >= 0 && i>=0){
         if(the_image[i][j] == value)
            dist5++;
         else
            measuring = 0;
      }
      else
         measuring = 0;
   }  /* ends while measuring */
   dist5 = ((dist5*14)+7)/10;
   if(dist5 <= result)
      result = dist5;
   if(dist5 < min1){
      min2 = min1;
      min1 = dist5;
   }
   else
      if(dist5 < min2)
         min2 = dist5;

      /* right and up */
   measuring = 1;
   i = a;
   j = b;
   while(measuring){
      j++;
      i--;
      if(j <=COLS-1 && i>=0){
         if(the_image[i][j] == value)
            dist6++;
         else
            measuring = 0;
      }
      else
         measuring = 0;
   }  /* ends while measuring */
   dist6 = ((dist6*14)+7)/10;
   if(dist6 <= result)
      result = dist6;
   if(dist6 < min1){
      min2 = min1;
      min1 = dist6;
   }
   else
      if(dist6 < min2)
         min2 = dist6;

      /* right and down */
   measuring = 1;
   i = a;
   j = b;
   while(measuring){
      j++;
      i++;
      if(j <=COLS-1 && i<=ROWS-1){
         if(the_image[i][j] == value)
            dist7++;
         else
            measuring = 0;
      }
      else
         measuring = 0;
   }  /* ends while measuring */
   dist7 = ((dist7*14)+7)/10;
   if(dist7 <= result)
      result = dist7;
   if(dist7 < min1){
      min2 = min1;
      min1 = dist7;
   }
   else
      if(dist7 < min2)
         min2 = dist7;

      /* left and down */
   measuring = 1;
   i = a;
   j = b;
   while(measuring){
      j--;
      i++;
      if(j >=0 && i<=ROWS-1){
         if(the_image[i][j] == value)
            dist8++;
         else
            measuring = 0;
      }
      else
         measuring = 0;
   }  /* ends while measuring */
   dist8 = ((dist8*14)+7)/10;
   if(dist8 <= result)
      result = dist8;
   if(dist8 < min1){
      min2 = min1;
      min1 = dist8;
   }
   else
      if(dist8 < min2)
         min2 = dist8;

   if(min1 == min2)
      result = value;
   else
      result = 0;

   if(min1 == 0)
      result = 0;

   return(result);
}  /* ends mat_d */

   /*********************************************
   *
   *       file d:\cips\segment.c
   *
   *       Functions: This file contains
   *           adaptive_threshold_segmentation
   *           append_stack_files
   *           copy_stack_files
   *           find_peaks
   *           find_valley_point
   *           grow
   *           insert_into_peaks
   *           insert_into_deltas
   *           label_and_check_neighbors
   *           manual_threshold_segmentation
   *           peak_threshold_segmentation
   *           peaks_high_low
   *           push_data_onto_stack_file
   *           pop_data_off_of_stack_file
   *           threshold_image_array
   *           valley_high_low
   *           valley_threshold_segmentation
   *           get_segmentation_options
   *
   *       Purpose:
   *           These functions are part of histogram
   *           based image segmentation.
   *
   *       External Calls:
   *          wtiff.c - round_off_image_size
   *                    create_file_if_needed
   *                    write_array_into_tiff_image
   *          tiff.c - read_tiff_header
   *          rtiff.c - read_tiff_image
   *          numcvrt.c - get_integer
   *                      get_short
   *
   *       Modifications:
   *           October 1992 - created
   *
   ************************************************/




   /**************************************************
   *
   *   manual_threshold_segmentation(...
   *
   *   This function segments an image using thresholding
   *   given the hi and low values of the threshold
   *   by the calling routine.  It reads in an image
   *   and writes the result to the output image.
   *
   *   If the segment parameter is 0, you only
   *   threshold the array - you do not segment.
   *
   ***************************************************/

manual_threshold_segmentation(in_name, out_name,
                              the_image, out_image,
                              il, ie, ll, le,
                              hi, low, value, segment)
   char   in_name[], out_name[];
   int    il, ie, ll, le, segment;
   short  hi, low, the_image[ROWS][COLS],
          out_image[ROWS][COLS], value;
{
   int    length, width;
   struct tiff_header_struct image_header;

   create_file_if_needed(in_name, out_name, out_image);

   read_tiff_image(in_name, the_image, il, ie, ll, le);
   threshold_image_array(the_image, out_image,
            hi, low, value);
   if(segment == 1)
      grow(out_image, value);
   write_array_into_tiff_image(out_name, out_image,
                               il, ie, ll, le);

}  /* ends manual_threshold_segmentation */





   /************************************************
   *
   *   peak_threshold_segmentation(...
   *
   *   This function segments an image using
   *   thresholding.  It uses the histogram peaks
   *   to find the hi and low values of the
   *   threshold.
   *
   *   If the segment parameter is 0, you only
   *   threshold the array - you do not segment.
   *
   *************************************************/

peak_threshold_segmentation(in_name, out_name,
                            the_image, out_image,
                            il, ie, ll, le,
                            value, segment)
   char   in_name[], out_name[];
   int    il, ie, ll, le, segment;
   short  the_image[ROWS][COLS],
          out_image[ROWS][COLS], value;
{
   int      length, peak1, peak2, width;
   short    hi, low;
   struct   tiff_header_struct image_header;
   unsigned long histogram[GRAY_LEVELS+1];

   create_file_if_needed(in_name, out_name, out_image);

   read_tiff_image(in_name, the_image, il, ie, ll, le);
   zero_histogram(histogram);
   calculate_histogram(the_image, histogram);
   smooth_histogram(histogram);
   find_peaks(histogram, &peak1, &peak2);
   peaks_high_low(histogram, peak1, peak2,
                  &hi, &low);
   threshold_image_array(the_image, out_image,
                         hi, low, value);
   if(segment == 1)
      grow(out_image, value);
   write_array_into_tiff_image(out_name, out_image,
                               il, ie, ll, le);

}  /* ends peak_threshold_segmentation */





   /************************************************
   *
   *   valley_threshold_segmentation(...
   *
   *   This function segments an image using
   *   thresholding.  It uses the histogram valleys
   *   to find the hi and low values of the
   *   threshold.
   *
   *   If the segment parameter is 0, you only
   *   threshold the array - you do not segment.
   *
   *************************************************/

valley_threshold_segmentation(in_name, out_name,
                              the_image, out_image,
                              il, ie, ll, le,
                              value, segment)
   char   in_name[], out_name[];
   int    il, ie, ll, le, segment;
   short  the_image[ROWS][COLS],
          out_image[ROWS][COLS], value;
{
   int      length, peak1, peak2, width;
   short    hi, low;
   struct   tiff_header_struct image_header;
   unsigned long histogram[GRAY_LEVELS+1];

   create_file_if_needed(in_name, out_name, out_image);

   read_tiff_image(in_name, the_image, il, ie, ll, le);
   zero_histogram(histogram);
   calculate_histogram(the_image, histogram);
   smooth_histogram(histogram);
   find_peaks(histogram, &peak1, &peak2);
   valley_high_low(histogram, peak1, peak2,
                   &hi, &low);
   threshold_image_array(the_image, out_image,
                         hi, low, value);
   if(segment == 1)
      grow(out_image, value);
   write_array_into_tiff_image(out_name, out_image,
                               il, ie, ll, le);

}  /* ends valley_threshold_segmentation */





   /************************************************
   *
   *   adaptive_threshold_segmentation(...
   *
   *   This function segments an image using
   *   thresholding.  It uses two passes
   *   to find the hi and low values of the
   *   threshold.  The first pass uses the peaks
   *   of the histogram to find the hi and low
   *   threshold values.  It thresholds the image
   *   using these hi lows and calculates the means
   *   of the object and background.  Then we use
   *   these means as new peaks to calculate new
   *   hi and low values.  Finally, we threshold
   *   the image again using these second hi low
   *   hi low values.
   *
   *   If the segment parameter is 0, you only
   *   threshold the array - you do not segment.
   *
   *************************************************/

adaptive_threshold_segmentation(in_name, out_name,
                                the_image, out_image,
                                il, ie, ll, le,
                                value, segment)
   char   in_name[], out_name[];
   int    il, ie, ll, le, segment;
   short  the_image[ROWS][COLS],
          out_image[ROWS][COLS], value;
{
   int      length, peak1, peak2, width;
   short    background, hi, low, object;
   struct   tiff_header_struct image_header;
   unsigned long histogram[GRAY_LEVELS+1];

   create_file_if_needed(in_name, out_name, out_image);

   read_tiff_image(in_name, the_image, il, ie, ll, le);
   zero_histogram(histogram);
   calculate_histogram(the_image, histogram);
   smooth_histogram(histogram);
   find_peaks(histogram, &peak1, &peak2);
   peaks_high_low(histogram, peak1, peak2,
                  &hi, &low);
   threshold_and_find_means(the_image, out_image,
                            hi, low, value,
                            &object, &background);
   peaks_high_low(histogram, object, background,
                  &hi, &low);
   threshold_image_array(the_image, out_image,
                         hi, low, value);
   if(segment == 1)
      grow(out_image, value);
   write_array_into_tiff_image(out_name, out_image,
                               il, ie, ll, le);

}  /* ends adaptive_threshold_segmentation */





   /**************************************************
   *
   *   threshold_image_array(...
   *
   *   This function thresholds an input image array
   *   and produces a binary output image array.
   *   If the pixel in the input array is between
   *   the hi and low values, then it is set to value.
   *   Otherwise, it is set to 0.
   *
   ***************************************************/


threshold_image_array(in_image, out_image, hi, low, value)
   short hi, low, in_image[ROWS][COLS],
         out_image[ROWS][COLS], value;
{
   int   counter = 0, i, j;
   for(i=0; i<ROWS; i++){
      for(j=0; j<COLS; j++){
         if(in_image[i][j] >= low  &&
            in_image[i][j] <= hi){
            out_image[i][j] = value;
            counter++;
         }
         else
            out_image[i][j] = 0;
      }  /* ends loop over j */
   }  /* ends loop over i */
   printf("\n\tTIA> set %d points", counter);
}  /* ends threshold_image_array */





   /**************************************************
   *
   *   threshold_and_find_means(...
   *
   *   This function thresholds an input image array
   *   and produces a binary output image array.
   *   If the pixel in the input array is between
   *   the hi and low values, then it is set to value.
   *   Otherwise, it is set to 0.
   *
   ***************************************************/

threshold_and_find_means(in_image, out_image, hi,
                         low, value, object_mean,
                         background_mean)
   short *background_mean, hi, low,
         in_image[ROWS][COLS], *object_mean,
         out_image[ROWS][COLS], value;
{
   int      counter = 0,
            i,
            j;
   unsigned long object     = 0,
                 background = 0;

   for(i=0; i<ROWS; i++){
      for(j=0; j<COLS; j++){
         if(in_image[i][j] >= low  &&
            in_image[i][j] <= hi){
            out_image[i][j] = value;
            counter++;
            object = object + in_image[i][j];
         }
         else{
            out_image[i][j] = 0;
            background      = background + in_image[i][j];
         }
      }  /* ends loop over j */
   }  /* ends loop over i */
   object     = object/counter;
   background = background/((ROWS*COLS)-counter);
   *object_mean     = (short)(object);
   *background_mean = (short)(background);
   printf("\n\tTAFM> set %d points", counter);
   printf("\n\tTAFM> object=%d background=%d",
          *object_mean, *background_mean);
}  /* ends threshold_and_find_means */





    /**********************************************
    *
    *  grow(...
    *
    *  This function is an object detector.
    *  Its input is an binary image array
    *  containing 0's and value's.
    *  It searches through the image and connects
    *  the adjacent values.
    *
    ***********************************************/

grow(binary, value)
   short binary[ROWS][COLS],
         value;
{
   char name[80];

   int first_call,
       i,
       j,
       object_found,
       pointer,
       pop_i,
       pop_j,
       stack_empty,
       stack_file_in_use;

   short g_label, stack[STACK_SIZE][2];

            /*************************************
            *
            *   Now begin the process of growing
            *   regions.
            *
            **************************************/

   g_label       = 2;
   object_found  = 0;
   first_call    = 1;

   for(i=0; i<ROWS; i++){
      for(j=0; j<COLS; j++){

         stack_file_in_use =  0;
         stack_empty       =  1;
         pointer           = -1;

               /**********************************
               *
               *  Search for the first pixel of
               *  a region.
               *
               ***********************************/

         if(binary[i][j] == value){
            label_and_check_neighbor(binary, stack, 
                      g_label, &stack_empty, &pointer, 
                      i, j, value, &stack_file_in_use,
                      &first_call);
            object_found = 1;
         }  /* ends if binary[i]j] == value */

               /*****************************
               *
               *  If the stack is not empty,
               *  pop the coordinates of
               *  the pixel off the stack
               *  and check its 8 neighbors.
               *
               *******************************/

         while(stack_empty == 0){
            pop_i = stack[pointer][0]; /* POP       */
            pop_j = stack[pointer][1]; /* OPERATION */
            --pointer;
            if(pointer <= 0){
               if(stack_file_in_use){
                  pop_data_off_of_stack_file(
                                 stack,
                                 &pointer,
                                 &stack_file_in_use);
               }  /* ends if stack_file_in_use  */
               else{
                  pointer     = 0;
                  stack_empty = 1;
               }  /* ends else stack file is
                     not in use  */
            }  /*  ends if point <= 0  */

            label_and_check_neighbor(binary,
                        stack, g_label,
                        &stack_empty,
                        &pointer, pop_i,
                        pop_j, value,
                        &stack_file_in_use,
                        &first_call);
         }  /* ends while stack_empty == 0 */

         if(object_found == 1){
            object_found = 0;
            ++g_label;
         }  /* ends if object_found == 1 */

      }   /* ends loop over j */
   }  /* ends loop over i */

   printf("\nGROW> found %d objects", g_label);

} /* ends grow  */





   /********************************************
   *
   *  label_and_check_neighbors(...
   *
   *  This function labels a pixel with an object
   *  label and then checks the pixel's 8
   *  neighbors.  If any of the neigbors are
   *  set, then they are also labeled.
   *
   ***********************************************/

label_and_check_neighbor(binary_image, stack,
                         g_label, stack_empty,
                         pointer, r, e, value,
                         stack_file_in_use,
                         first_call)
int   e,
      *first_call,
      *pointer,
      r,
      *stack_empty,
      *stack_file_in_use;

short binary_image[ROWS][COLS],
      g_label,
      stack[STACK_SIZE][2],
      value;
{
   int already_labeled = 0,
       i, j;

   if (binary_image[r][e] == g_label)
      already_labeled = 1;

   binary_image[r][e] = g_label;

      /***************************************
      *
      *   Look at the 8 neighors of the
      *   point r,e.
      *
      *   Ensure the points you are checking
      *   are in the image, i.e. not less
      *   than zero and not greater than
      *   ROWS-1 or COLS-1.
      *
      ***************************************/

   for(i=(r-1); i<=(r+1); i++){
      for(j=(e-1); j<=(e+1); j++){

         if((i>=0)   &&
            (i<=ROWS-1)  &&
            (j>=0)   &&
            (j<=COLS-1)){

            if(binary_image[i][j] == value){
               *pointer           = *pointer + 1;
               stack[*pointer][0] = i; /* PUSH      */
               stack[*pointer][1] = j; /* OPERATION */
               *stack_empty       = 0;

               if(*pointer >= (STACK_SIZE -
                               STACK_FILE_LENGTH)){
                  push_data_onto_stack_file(stack,
                            pointer, first_call);
                  *stack_file_in_use = 1;
               }  /* ends if *pointer >=
                     STACK_SIZE - STACK_FILE_LENGTH*/

            }  /* end of if binary_image == value */
         }  /* end if i and j are on the image */
      }  /* ends loop over i rows           */
   }  /* ends loop over j columns        */
}  /* ends label_and_check_neighbors  */





   /****************************************
   *
   *   push_data_onto_stack_file(...
   *
   *   This function takes the stack array
   *   and pushes it onto the stack file.
   *
   *****************************************/

push_data_onto_stack_file(stack, pointer, first_call)
   int   *first_call, *pointer;
   short stack[STACK_SIZE][2];
{
   char  backup_file_name[MAX_NAME_LENGTH];
   FILE  *backup_file_pointer, *stack_file_pointer;
   int   diff, i;
   short holder[STACK_FILE_LENGTH][2];

   printf("\nSFO> Start of push_data_onto_stack ");

   diff = STACK_SIZE - STACK_FILE_LENGTH;

       /*******************************************
       *
       *   Copy the elements to be stored to the
       *   stack file into holder
       *
       ********************************************/

   for(i=0; i<STACK_FILE_LENGTH; i++){
      holder[i][0] = stack[i][0];
      holder[i][1] = stack[i][1];
   }

       /*******************************************
       *
       *   Move the elements of the stack down
       *
       *******************************************/

   for(i=0; i<diff; i++){
      stack[i][0] = stack[i + STACK_FILE_LENGTH][0];
      stack[i][1] = stack[i + STACK_FILE_LENGTH][1];
   }

       /*******************************************
       *
       *   Fill the top of the stack with zeros
       *
       *******************************************/

   for(i=diff; i<STACK_SIZE; i++){
      stack[i][0] = 0;
      stack[i][1] = 0;
   }

   *pointer = *pointer - STACK_FILE_LENGTH;

   /************************************************
   *
   *   Store the holder array into the stack file.
   *   Open the stack file for writing in binary
   *   mode. If the file does not exist it will be
   *   created. If the file does exist it will be
   *   over written.
   *
   *   PUSH - IF first_time == 1 then write to stack
   *          ELSE write to stack.bak
   *          append stack onto stack.bak
   *          copy stack.bak to stack
   *          this has the effect of writing
   *          to the beginning of the stack.
   *
   ************************************************/

   if(*first_call == 1){

      *first_call = *first_call + 1;
      if((stack_file_pointer = fopen(STACK_FILE,"wb"))
                                     == NULL)
         printf("\nSFO> Could not open stack file");
      else{
         /*printf("\n\nSFO> Writing to stack file");*/
         fwrite(holder, sizeof(holder),
                1, stack_file_pointer);
         fclose(stack_file_pointer);
      }  /*  ends else could not open stack_file  */

   }  /*  ends if *first_call == 1  */
   else{  /* else stack file has been used already  */
      strcpy(backup_file_name, STACK_FILE);
      strcat(backup_file_name, ".bak");
      if((backup_file_pointer =
          fopen(backup_file_name, "wb")) == NULL)
         printf("\nSFO> Could not open backup file");
      else{
         /*printf("\n\nSFO> Writing to backup file");*/
         fwrite(holder, sizeof(holder),
                1, backup_file_pointer);
         fclose(backup_file_pointer);
      }  /*  ends else could not open backup_file  */

      append_stack_files(backup_file_name,
                         STACK_FILE, holder);
      copy_stack_files(backup_file_name,
                       STACK_FILE, holder);

   }  /*  ends else first_call != 1  */

   printf("--- End of push_data_onto_stack");

}  /* ends push_data_onto_stack_file  */





   /****************************************
   *
   *   pop_data_off_of_stack_file(...
   *
   *   This function pops the stack array
   *   off of the stack file.
   *
   *****************************************/

pop_data_off_of_stack_file(stack, pointer,
                           stack_file_in_use)
   int   *pointer, *stack_file_in_use;
   short stack[STACK_SIZE][2];
{
   char  backup_file_name[MAX_NAME_LENGTH];
   FILE  *backup_file_pointer, *stack_file_pointer;
   int   i;
   long  write_counter;
   short holder[STACK_FILE_LENGTH][2],
         holder2[STACK_FILE_LENGTH][2];

       /*******************************************
       *
       *   POP - Read 1 time from stack
       *         Copy the remainder of stack to
       *            stack.bak
       *         Copy stack.bak to stack
       *         This has the effect of popping off
       *         of the stack.
       *
       *   Read the holder array from the stack file.
       *   Open the stack file for reading in binary
       *   mode.
       *
       *   If it requires more than one write to
       *   copy the remainder of stack to
       *   stack.bak then there is still data in the
       *   stack file so set stack_file_in_use = 1.
       *   Else set it to 0.
       *
       **********************************************/

   printf("\nSFO> Start of pop_data_off_of_stack ");
   write_counter = 0;

   strcpy(backup_file_name, STACK_FILE);
   strcat(backup_file_name, ".bak");

   if( (stack_file_pointer =
          fopen(STACK_FILE, "rb")) == NULL)
      printf("\nSFO> Could not open stack file");
   else{
      /*printf("\n\nSFO> Reading from stack file");*/
      fread(holder, sizeof(holder),
            1, stack_file_pointer);

      backup_file_pointer =
            fopen(backup_file_name, "wb");
      while( fread(holder2, sizeof(holder2),
                   1, stack_file_pointer) ){
         fwrite(holder2, sizeof(holder2),
                1, backup_file_pointer);
         ++write_counter;
      }  /* ends while reading  */
      if(write_counter > 0)
         *stack_file_in_use = 1;
      else
         *stack_file_in_use = 0;

      fclose(backup_file_pointer);
      fclose(stack_file_pointer);
   }  /* ends else could not open stack file  */

   copy_stack_files(backup_file_name,
                    STACK_FILE, holder2);

   for(i=0; i<STACK_FILE_LENGTH; i++){
      stack[i][0] = holder[i][0];
      stack[i][1] = holder[i][1];
   }

   *pointer = *pointer + STACK_FILE_LENGTH - 1;

   printf("--- End of pop_data_off_of_stack");
}  /* ends pop_data_off_of_stack_file  */





   /*********************************************
   *
   *   append_stack_files(...
   *
   *   Append the second file onto the end
   *   of the first.
   *
   ***********************************************/

append_stack_files(first_file, second_file, holder)
   char first_file[], second_file[];
   short holder[STACK_FILE_LENGTH][2];
{
   FILE  *first, *second;
   int   i;

   if((first = fopen(first_file, "r+b")) == NULL)
      printf("\n\nSFO> Cannot open file %s",
             first_file);

   if((second = fopen(second_file, "rb")) == NULL)
      printf("\n\nSFO> Cannot open file %s",
             second_file);

          /*****************************************
          *
          *  Seek to the end of the first file and
          *  to the beginning of the second file.
          *
          *******************************************/

   fseek(first, 0L, 2);
   fseek(second, 0L, 0);

   while(fread(holder, sizeof(holder), 1, second) ){
      fwrite(holder, sizeof(holder), 1, first);
   }  /* ends while reading  */

   fclose(first);
   fclose(second);

}  /*  ends append_stack_files  */





   /********************************************
   *
   *   copy_stack_files(...
   *
   *   Copy the first file to the second.
   *
   **********************************************/

copy_stack_files(first_file, second_file, holder)
   char first_file[], second_file[];
   short holder[STACK_FILE_LENGTH][2];
{
   FILE  *first, *second;
   int   i;

   if( (first = fopen(first_file, "rb")) == NULL)
      printf("\n\nSFO> Cannot open file %s",
             first_file);

   if( (second = fopen(second_file, "wb")) == NULL)
      printf("\n\nSFO> Cannot open file %s",
             second_file);

          /******************************************
          *
          *  Seek to the beginning of the first file.
          *
          *******************************************/

   fseek(first, 0L, 0);

   while( fread(holder, sizeof(holder), 1, first) ){
      fwrite(holder, sizeof(holder), 1, second);
   }  /* ends while reading  */

   fclose(first);
   fclose(second);

}  /*  ends copy_stack_files */




   /********************************************
   *
   *   find_peaks(...
   *
   *   This function looks through the histogram
   *   array and finds the two highest peaks.
   *   The peaks must be separated, cannot be
   *   next to each other, by a spacing defined
   *   in cips.h.
   *
   *   The peaks array holds the peak value
   *   in the first place and its location in
   *   the second place.
   *
   *********************************************/

find_peaks(histogram, peak1, peak2)
   unsigned long histogram[];
   int *peak1, *peak2;
{
   int distance[PEAKS], peaks[PEAKS][2];
   int i, j=0, max=0, max_place=0;

   for(i=0; i<PEAKS; i++){
      distance[i] =  0;
      peaks[i][0] = -1;
      peaks[i][1] = -1;
   }

   for(i=0; i<=GRAY_LEVELS; i++){
      max       = histogram[i];
      max_place = i;
      insert_into_peaks(peaks, max, max_place);
   }  /* ends loop over i */

   for(i=1; i<PEAKS; i++){
      distance[i] = peaks[0][1] - peaks[i][1];
      if(distance[i] < 0)
         distance[i] = distance[i]*(-1);
   }

   *peak1 = peaks[0][1];
   for(i=PEAKS-1; i>0; i--)
    if(distance[i] > PEAK_SPACE) *peak2 = peaks[i][1];

}  /* ends find_peaks */





   /********************************************
   *
   *   insert_into_peaks(...
   *
   *   This function takes a value and its
   *   place in the histogram and inserts them
   *   into a peaks array.  This helps us rank
   *   the the peaks in the histogram.
   *
   *   The objective is to build a list of
   *   histogram peaks and thier locations.
   *
   *   The peaks array holds the peak value
   *   in the first place and its location in
   *   the second place.
   *
   *********************************************/

insert_into_peaks(peaks, max, max_place)
   int max, max_place, peaks[PEAKS][2];
{
   int i, j;

      /* first case */
   if(max > peaks[0][0]){
      for(i=PEAKS-1; i>0; i--){
         peaks[i][0] = peaks[i-1][0];
         peaks[i][1] = peaks[i-1][1];
      }
      peaks[0][0] = max;
      peaks[0][1] = max_place;
   }  /* ends if */

      /* middle cases */
   for(j=0; j<PEAKS-3; j++){
      if(max < peaks[j][0]  && max > peaks[j+1][0]){
         for(i=PEAKS-1; i>j+1; i--){
            peaks[i][0] = peaks[i-1][0];
            peaks[i][1] = peaks[i-1][1];
         }
         peaks[j+1][0] = max;
         peaks[j+1][1] = max_place;
      }  /* ends if */
   }  /* ends loop over j */
      /* last case */
   if(max < peaks[PEAKS-2][0]  && 
      max > peaks[PEAKS-1][0]){
      peaks[PEAKS-1][0] = max;
      peaks[PEAKS-1][1] = max_place;
   }  /* ends if */

}  /* ends insert_into_peaks */





   /********************************************
   *
   *   peaks_high_low(...
   *
   *   This function uses the histogram array
   *   and the peaks to find the best high and
   *   low threshold values for the threshold
   *   function.  You want the hi and low values
   *   so that you will threshold the image around
   *   the smaller of the two "humps" in the
   *   histogram.  This is because the smaller
   *   hump represents the objects while the
   *   larger hump represents the background.
   *
   *********************************************/

peaks_high_low(histogram, peak1, peak2, hi, low)
   int  peak1, peak2;
   short *hi, *low;
   unsigned long histogram[];
{
   int i, mid_point;
   unsigned long sum1 = 0, sum2 = 0;

   if(peak1 > peak2)
      mid_point = ((peak1 - peak2)/2) + peak2;
   if(peak1 < peak2)
      mid_point = ((peak2 - peak1)/2) + peak1;

   for(i=0; i<mid_point; i++)
      sum1 = sum1 + histogram[i];

   for(i=mid_point; i<=GRAY_LEVELS; i++)
      sum2 = sum2 + histogram[i];
   if(sum1 >= sum2){
      *low = mid_point;
      *hi  = GRAY_LEVELS;
   }
   else{
      *low = 0;
      *hi  = mid_point;
   }

}  /* ends peaks_high_low */





      /********************************************
      *
      *   valley_high_low(...
      *
      *   This function uses the histogram array
      *   and the valleys to find the best high and
      *   low threshold values for the threshold
      *   function.  You want the hi and low values
      *   so that you will threshold the image around
      *   the smaller of the two "humps" in the
      *   histogram.  This is because the smaller
      *   hump represents the objects while the
      *   larger hump represents the background.
      *
      *********************************************/

valley_high_low(histogram, peak1, peak2, hi, low)
   int  peak1, peak2;
   short *hi, *low;
   unsigned long histogram[];
{
   int  i, valley_point;
   unsigned long sum1 = 0, sum2 = 0;

   find_valley_point(histogram, peak1, peak2,
                     &valley_point);
   /*printf("\nVHL> valley point is %d",
            valley_point);*/

   for(i=0; i<valley_point; i++)
      sum1 = sum1 + histogram[i];
   for(i=valley_point; i<=GRAY_LEVELS; i++)
      sum2 = sum2 + histogram[i];

   if(sum1 >= sum2){
      *low = valley_point;
      *hi  = GRAY_LEVELS;
   }
   else{
      *low = 0;
      *hi  = valley_point;
   }

}  /* ends valley_high_low */





   /********************************************
   *
   *   find_valley_point(...
   *
   *   This function finds the low point of
   *   the valley between two peaks in a
   *   histogram.  It starts at the lowest
   *   peak and works its way up to the
   *   highest peak.  Along the way, it looks
   *   at each point in the histogram and inserts
   *   them into a list of points.  When done,
   *   it has the location of the smallest histogram
   *   point - that is the valley point.
   *
   *   The deltas array holds the delta value
   *   in the first place and its location in
   *   the second place.
   *
   *********************************************/

find_valley_point(histogram, peak1, 
                  peak2, valley_point)
   int  peak1, peak2, *valley_point;
   unsigned long histogram[];
{
   int  deltas[PEAKS][2], delta_hist, i;

   for(i=0; i<PEAKS; i++){
      deltas[i][0] = 10000;
      deltas[i][1] =    -1;
   }

   if(peak1 < peak2){
      for(i=peak1+1; i<peak2; i++){
         delta_hist = (int)(histogram[i]);
         insert_into_deltas(deltas, delta_hist, i);
      }  /* ends loop over i */
   }  /* ends if peak1 < peak2 */

   if(peak2 < peak1){
      for(i=peak2+1; i<peak1; i++){
         delta_hist = (int)(histogram[i]);
         insert_into_deltas(deltas, delta_hist, i);
      }  /* ends loop over i */
   }  /* ends if peak2 < peak1 */

   *valley_point = deltas[0][1];

}  /* ends find_valley_point */






   /********************************************
   *
   *   insert_into_deltas(...
   *
   *   This function inserts histogram deltas
   *   into a deltas array.  The smallest delta
   *   will be at the top of the array.
   *
   *   The objective is to build a list of
   *   histogram area deltas and thier locations.
   *
   *   The deltas array holds the delta value
   *   in the first place and its location in
   *   the second place.
   *
   *********************************************/

insert_into_deltas(deltas, value, place)
   int value, place, deltas[PEAKS][2];
{
   int i, j;

      /* first case */
   if(value < deltas[0][0]){
      for(i=PEAKS-1; i>0; i--){
         deltas[i][0] = deltas[i-1][0];
         deltas[i][1] = deltas[i-1][1];
      }
      deltas[0][0] = value;
      deltas[0][1] = place;
   }  /* ends if */

      /* middle cases */
   for(j=0; j<PEAKS-3; j++){
      if(value > deltas[j][0]  &&
         value < deltas[j+1][0]){
         for(i=PEAKS-1; i>j+1; i--){
            deltas[i][0] = deltas[i-1][0];
            deltas[i][1] = deltas[i-1][1];
         }
         deltas[j+1][0] = value;
         deltas[j+1][1] = place;
      }  /* ends if */
   }  /* ends loop over j */

      /* last case */
   if(value > deltas[PEAKS-2][0]  &&
      value < deltas[PEAKS-1][0]){
      deltas[PEAKS-1][0] = value;
      deltas[PEAKS-1][1] = place;
   }  /* ends if */

}  /* ends insert_into_deltas */





   /********************************************
   *
   *   get_segmentation_options(...
   *
   *   This function interacts with the user
   *   to obtain the options for image
   *   segmentation.
   *
   *********************************************/


get_segmentation_options(method, hi, low, value)
   char   method[];
   short  *hi, *low, *value;
{
   int   i, not_finished = 1, response;

   while(not_finished){
      printf(
         "\n\nThe image segmentation options are:\n");
      printf("\n\t1. Method is %s", method);
      printf("\n\t   (options are manual peaks");
      printf(        " valleys adapative)");
      printf("\n\t2. Value is %d", *value);
      printf("\n\t3. Hi    is %d", *hi);
      printf("\n\t4. Low   is %d", *low);
      printf("\n\t   Hi and Low needed only for");
      printf(        " manual method");
      printf("\n\nEnter choice (0 = no change):_\b");

      get_integer(&response);

      if(response == 0)
         not_finished = 0;

      if(response == 1){
         printf("\nEnter method (options are:");
         printf(" manual peaks valleys adaptive)\n\t");
         gets(method);
      }

      if(response == 2){
         printf("\nEnter value: ___\b\b\b");
         get_short(value);
      }

      if(response == 3){
         printf("\nEnter hi: ___\b\b\b");
         get_short(hi);
      }
      if(response == 4){
         printf("\nEnter low: ___\b\b\b");
         get_short(low);
      }

   }  /* ends while not_finished */
}  /* ends get_segmentation_options */






   /********************************************
   *
   *   get_threshold_options(...
   *
   *   This function interacts with the user
   *   to obtain the options for image
   *   threshold.
   *
   *********************************************/


get_threshold_options(method, hi, low, value)
   char   method[];
   short  *hi, *low, *value;
{
   int   i, not_finished = 1, response;

   while(not_finished){
      printf("\n\nThe image threshold options are:\n");
      printf("\n\t1. Method is %s", method);
      printf("\n\t   (options are manual peaks");
      printf(        " valleys adapative)");
      printf("\n\t2. Value is %d", *value);
      printf("\n\t3. Hi    is %d", *hi);
      printf("\n\t4. Low   is %d", *low);
      printf("\n\t   Hi and Low needed only for");
      printf(        " manual method");
      printf("\n\nEnter choice (0 = no change):_\b");

      get_integer(&response);

      if(response == 0)
         not_finished = 0;

      if(response == 1){
         printf("\nEnter method (options are:");
         printf(" manual peaks valleys adaptive)\n\t");
         gets(method);
      }

      if(response == 2){
         printf("\nEnter value: ___\b\b\b");
         get_short(value);
      }

      if(response == 3){
         printf("\nEnter hi: ___\b\b\b");
         get_short(hi);
      }
      if(response == 4){
         printf("\nEnter low: ___\b\b\b");
         get_short(low);
      }

   }  /* ends while not_finished */
}  /* ends get_threshold_options */



       /***********************************************
       *
       *       file d:\cips\segment2.c
       *
       *       Functions: This file contains
       *          find_cutoff_point
       *          edge_region
       *          gray_shade_region
       *          edge_gray_shade_region
       *          pixel_grow
       *          pixel_label_and_check_neighbors
       *          is_close
       *          erode_image_array
       *          get_edge_region_options
       *
       *       Purpose:
       *          These function implement the three
       *          segmentation techniques in Image
       *          Processing part 10.
       *
       *       External Calls:
       *          wtiff.c - round_off_image_size
       *                    create_file_if_needed
       *                    write_array_into_tiff_image
       *          tiff.c - read_tiff_header
       *          rtiff.c - read_tiff_image
       *          numcvrt.c - get_integer
       *          edges.c - quick_edge
       *                    homogeneity
       *                    difference_edge
       *                    contrast_edge
       *                    gaussian_edge
       *                    range
       *                    variance
       *                    detect_edges
       *          hist.c - calculate_histogram
       *                   zero_histogram
       *          thresh.c - threshold_image_array
       *
       *       Modifications:
       *          5 December 1992 - created
       *
       *************************************************/




     /*******************************************
     *
     *   find_cutoff_point(..
     *
     *   This function looks at a histogram
     *   and sets a cuttoff point at a given
     *   percentage of pixels.
     *   For example, if percent=0.6, you
     *   start at 0 in the histogram and count
     *   up until you've hit 60% of the pixels.
     *   Then you stop and return that pixel
     *   value.
     *
     ********************************************/

find_cutoff_point(histogram, percent, cutoff)
   unsigned long histogram[];
   float    percent;
   short    *cutoff;
{
   float  fd, fsum, sum_div;
   int    i, looking;
   long   lc, lr, num=0, sum=0;

   sum     = 0;
   i       = 0;
   lr      = (long)(ROWS);
   lc      = (long)(COLS);
   num     = lr*lc;
   fd      = (float)(num);

   while(looking){
      fsum    = (float)(sum);
      sum_div = fsum/fd;
      if(sum_div >= percent)
         looking = 0;
      else
         sum = sum + histogram[i++];
   }  /* ends while looking */

   if(i >= 256) i = 255;
   *cutoff = i;
   printf("\nCutoff is %d sum=%ld", *cutoff, sum);
}  /* ends find_cutoff_point */





     /*******************************************
     *
     *   edge_region(..
     *
     *   This function segments an image by
     *   growing regions inside of edges.
     *   The steps are:
     *      . detect edges
     *      . threshold edge output to a
     *        percent value
     *      . remove edges from consideration
     *      . grow regions
     *
     *******************************************/


edge_region(in_name, out_name, the_image, out_image,
            il, ie, ll, le, edge_type, min_area,
            max_area, diff, percent, set_value,
            erode)
   char     in_name[], out_name[];
   float    percent;
   int      edge_type, il, ie, ll, le;
   short    diff, erode, 
            max_area, min_area, 
            set_value,
            the_image[ROWS][COLS],
            out_image[ROWS][COLS];
{

   int    a, b, count, i, j, k,
          length, width;
   short  cutoff;
   struct tiff_header_struct image_header;
   unsigned long histogram[GRAY_LEVELS+1];


   create_file_if_needed(in_name, out_name, out_image);

   read_tiff_image(in_name, the_image, il, ie, ll, le);

      /***************************
      *
      *   Detect the edges.  Do
      *   not threshold.
      *
      ****************************/

   if(edge_type == 1  ||
      edge_type == 2  ||
      edge_type == 3)
      detect_edges(in_name, out_name, the_image,
                   out_image, il, ie, ll, le,
                   edge_type, 0, 0);

   if(edge_type == 4){
      quick_edge(in_name, out_name, the_image,
                 out_image, il, ie, ll, le,
                 0, 0);
   }  /* ends if 4 */

   if(edge_type == 5){
      homogeneity(in_name, out_name, the_image,
                 out_image, il, ie, ll, le,
                 0, 0);
   }  /* ends if 5 */

   if(edge_type == 6){
      difference_edge(in_name, out_name, the_image,
                 out_image, il, ie, ll, le,
                 0, 0);
   }  /* ends if 6 */

   if(edge_type == 7){
      contrast_edge(in_name, out_name, the_image,
                 out_image, il, ie, ll, le,
                 0, 0);
   }  /* ends if 7 */

   if(edge_type == 8){
      gaussian_edge(in_name, out_name, the_image,
                 out_image, il, ie, ll, le,
                 3, 0, 0);
   }  /* ends if 8 */

   if(edge_type == 10){
      range(in_name, out_name, the_image,
            out_image, il, ie, ll, le,
            3, 0, 0);
   }  /* ends if 10 */

   if(edge_type == 11){
      variance(in_name, out_name, the_image,
               out_image, il, ie, ll, le,
               0, 0);
   }  /* ends if 11 */

/**write_array_into_tiff_image("f:e1.tif", out_image,
il, ie, ll, le);**/

      /* copy out_image to the_image */
   for(i=0; i<ROWS; i++)
      for(j=0; j<COLS; j++)
         the_image[i][j] = out_image[i][j];

      /******************************
      *
      *   Threshold the edge detector
      *   output at a given percent.
      *   This eliminates the weak
      *   edges.
      *
      *******************************/
   zero_histogram(histogram);
   calculate_histogram(the_image, histogram);
   find_cutoff_point(histogram, percent, &cutoff);
   threshold_image_array(the_image, out_image,
                         255, cutoff, set_value);
/**write_array_into_tiff_image("f:e2.tif", out_image,
il, ie, ll, le);**/

   if(erode != 0){
         /* copy out_image to the_image */
      for(i=0; i<ROWS; i++)
         for(j=0; j<COLS; j++)
            the_image[i][j] = out_image[i][j];
      erode_image_array(the_image, out_image,
                        set_value, erode);
   }  /* ends if erode */

/**write_array_into_tiff_image("f:e3.tif", out_image,
il, ie, ll, le);**/

      /*******************************
      *
      *   Set all the edge values to
      *   FORGET_IT so the region
      *   growing will not use those
      *   points.
      *
      *******************************/

   for(i=0; i<ROWS; i++)
      for(j=0; j<COLS; j++)
         if(out_image[i][j] == set_value)
            out_image[i][j] = FORGET_IT;

   for(i=0; i<ROWS; i++)
      for(j=0; j<COLS; j++)
         the_image[i][j] = out_image[i][j];

   pixel_grow(the_image, out_image, diff,
              min_area, max_area);

   write_array_into_tiff_image(out_name, out_image,
                               il, ie, ll, le);

}  /* ends edge_region */



     /*******************************************
     *
     *   gray_shade_region(...
     *
     *   This function segments an image by
     *   growing regions based only on gray
     *   shade.
     *
     *******************************************/


gray_shade_region(in_name, out_name, the_image,
                  out_image, il, ie, ll, le,
                  diff, min_area, max_area)
   char   in_name[], out_name[];
   int    il, ie, ll, le;
   short  the_image[ROWS][COLS],
          out_image[ROWS][COLS],
          diff, min_area, max_area;
{
   int    a, b, big_count, count, i, j, k, l,
          not_finished, length, width;
   short  temp[3][3];
   struct tiff_header_struct image_header;

   create_file_if_needed(in_name, out_name, out_image);

   read_tiff_image(in_name, the_image, il, ie, ll, le);
   pixel_grow(the_image, out_image, diff,
              min_area, max_area);
   write_array_into_tiff_image(out_name, out_image,
                               il, ie, ll, le);

}  /* ends gray_shade_region */





     /*******************************************
     *
     *   edge_gray_shade_region(..
     *
     *   This function segments an image by
     *   growing gray shade regions inside of
     *   edges.  It combines the techniques
     *   of the edge_region and gray_shade_region
     *   functions.
     *
     *   The steps are:
     *      . detect edges
     *      . threshold edge output to a
     *        percent value
     *      . lay the edges on top of the original
     *        image to eliminate them from
     *        consideration
     *      . grow regions
     *
     *******************************************/

edge_gray_shade_region(in_name, out_name, the_image,
            out_image, il, ie, ll, le, edge_type,
            min_area, max_area, diff, percent,
            set_value, erode)
   char     in_name[], out_name[];
   float    percent;
   int      edge_type, il, ie, ll, le;
   short    diff, erode, 
            max_area, min_area, 
            set_value,
            the_image[ROWS][COLS],
            out_image[ROWS][COLS];
{
   int    a, b, count, i, j, k,
          length, width;
   short  cutoff;
   struct tiff_header_struct image_header;
   unsigned long histogram[GRAY_LEVELS+1];

   create_file_if_needed(in_name, out_name, out_image);

   read_tiff_image(in_name, the_image, il, ie, ll, le);

      /***************************
      *
      *   Detect the edges.  Do
      *   not threshold.
      *
      ****************************/

   if(edge_type == 1  ||
      edge_type == 2  ||
      edge_type == 3)
      detect_edges(in_name, out_name, the_image,
                   out_image, il, ie, ll, le,
                   edge_type, 0, 0);

   if(edge_type == 4){
      quick_edge(in_name, out_name, the_image,
                 out_image, il, ie, ll, le,
                 0, 0);
   }  /* ends if 4 */
   if(edge_type == 5){
      homogeneity(in_name, out_name, the_image,
                 out_image, il, ie, ll, le,
                 0, 0);
   }  /* ends if 5 */

   if(edge_type == 6){
      difference_edge(in_name, out_name, the_image,
                 out_image, il, ie, ll, le,
                 0, 0);
   }  /* ends if 6 */

   if(edge_type == 7){
      contrast_edge(in_name, out_name, the_image,
                 out_image, il, ie, ll, le,
                 0, 0);
   }  /* ends if 7 */

   if(edge_type == 8){
      gaussian_edge(in_name, out_name, the_image,
                 out_image, il, ie, ll, le,
                 3, 0, 0);
   }  /* ends if 8 */

   if(edge_type == 10){
      range(in_name, out_name, the_image,
            out_image, il, ie, ll, le,
            3, 0, 0);
   }  /* ends if 10 */

   if(edge_type == 11){
      variance(in_name, out_name, the_image,
               out_image, il, ie, ll, le,
               0, 0);
   }  /* ends if 11 */

/**write_array_into_tiff_image("f:e1.tif", out_image,
il, ie, ll, le);**/

      /* copy out_image to the_image */
   for(i=0; i<ROWS; i++)
      for(j=0; j<COLS; j++)
         the_image[i][j] = out_image[i][j];

      /******************************
      *
      *   Threshold the edge detector
      *   output at a given percent.
      *   This eliminates the weak
      *   edges.
      *
      *******************************/

   zero_histogram(histogram);
   calculate_histogram(the_image, histogram);
   find_cutoff_point(histogram, percent, &cutoff);
   threshold_image_array(the_image, out_image,
                         255, cutoff, set_value);

/**write_array_into_tiff_image("f:e2.tif", out_image,
il, ie, ll, le);**/

   if(erode != 0){
         /* copy out_image to the_image */
      for(i=0; i<ROWS; i++)
         for(j=0; j<COLS; j++)
            the_image[i][j] = out_image[i][j];
      erode_image_array(the_image, out_image,
                        set_value, erode);
   }  /* ends if erode */

/**write_array_into_tiff_image("f:e3.tif", out_image,
il, ie, ll, le);**/

      /*******************************
      *
      *   Read the original gray shade
      *   image back into the_image.
      *
      *******************************/

   read_tiff_image(in_name, the_image, il, ie, ll, le);

      /*******************************
      *
      *   Overlay the edge values
      *   on top of the original
      *   image by setting them to
      *   FORGET_IT so the region
      *   growing will not use those
      *   points.
      *
      *******************************/

   for(i=0; i<ROWS; i++)
      for(j=0; j<COLS; j++)
         if(out_image[i][j] == set_value)
            the_image[i][j] = FORGET_IT;

/**write_array_into_tiff_image("f:e4.tif", the_image,
il, ie, ll, le);**/

   pixel_grow(the_image, out_image, diff,
              min_area, max_area);

   write_array_into_tiff_image(out_name, out_image,
                               il, ie, ll, le);

}  /* ends edge_gray_shade_region */




    /**********************************************
    *
    *   pixel_grow(...
    *
    *   The function grows regions.  It is similar
    *   to the grow function in segment.c, but it
    *   has several new capabilities.  It can
    *   eliminate regions if they are too large or
    *   too small.
    *
    *   It ignores pixels = FORGET_IT.  This allows
    *   it to ignore edges or regions already
    *   eliminated from consideration.
    *
    *   It adds pixels to a growing region only if
    *   the pixel is close enough to the average gray
    *   level of that region.
    *
    ***********************************************/

pixel_grow(input, output, diff, min_area, max_area)
   short input[ROWS][COLS],
         output[ROWS][COLS],
         max_area,
         min_area,
         diff;
{
   char name[80];

   int count,
       first_call,
       i,
       ii,
       j,
       jj,
       object_found,
       pointer,
       pop_i,
       pop_j,
       stack_empty,
       stack_file_in_use;

   short g_label, target, sum, stack[STACK_SIZE][2];

   for(i=0; i<ROWS; i++)
      for(j=0; j<COLS; j++)
         output[i][j] = 0;

   g_label       = 2;
   object_found  = 0;
   first_call    = 1;

            /*************************************
            *
            *   Now begin the process of growing
            *   regions.
            *
            **************************************/

   for(i=0; i<ROWS; i++){
if( (i%4) == 0) printf("\n");
printf("-i=%3d label=%3d", i, g_label);
      for(j=0; j<COLS; j++){

         target            = input[i][j];
         sum               = target;
         count             = 0;
         stack_file_in_use = 0;
         stack_empty       = 1;
         pointer           = -1;
               /**********************************
               *
               *  Search for the first pixel of
               *  a region.  It must not equal
               *  FORGET_IT, and it must be close
               *  enough to the target (ave value).
               *
               ***********************************/

         if(input[i][j] != FORGET_IT            &&
            is_close(input[i][j], target, diff) &&
            output[i][j] == 0){
            pixel_label_and_check_neighbor(input,
                           output, &target, &sum,
                           &count, stack, g_label,
                           &stack_empty, &pointer,
                           i, j, diff,
                           &stack_file_in_use,
                           &first_call);
            object_found = 1;
         }  /* ends if is_close */

               /*****************************
               *
               *  If the stack is not empty,
               *  pop the coordinates of
               *  the pixel off the stack
               *  and check its 8 neighbors.
               *
               *******************************/

         while(stack_empty == 0){
            pop_i = stack[pointer][0]; /* POP       */
            pop_j = stack[pointer][1]; /* OPERATION */
            --pointer;
            if(pointer <= 0){
               if(stack_file_in_use){
                  pop_data_off_of_stack_file(
                                 stack,
                                 &pointer,
                                 &stack_file_in_use);
               }  /* ends if stack_file_in_use  */
               else{
                  pointer     = 0;
                  stack_empty = 1;
               }  /* ends else stack file is
                     not in use  */
            }  /*  ends if point <= 0  */
            pixel_label_and_check_neighbor(input,
                           output, &target, &sum,
                           &count, stack, g_label,
                           &stack_empty, &pointer,
                           pop_i, pop_j,
                           diff, &stack_file_in_use,
                           &first_call);
         }  /* ends while stack_empty == 0 */

         if(object_found == 1){
            object_found = 0;

                  /**********************************
                  *
                  *  The object must be in the
                  *  size constraints given by
                  *  min_area and max_area
                  *
                  *********************************/

            if(count >= min_area  &&
               count <= max_area)
               ++g_label;
                  /**********************************
                  *
                  *   Remove the object from the
                  *   output.  Set all pixels in the
                  *   object you are removing to
                  *   FORGET_IT.
                  *
                  **********************************/

            else{
               for(ii=0; ii<ROWS; ii++){
                  for(jj=0; jj<COLS; jj++){
                     if(output[ii][jj] == g_label){
                        output[ii][jj] = 0;
                        input[ii][jj]  = FORGET_IT;
                     }  /* ends if output == g_label */
                  }  /* ends loop over jj */
               }  /* ends loop over ii */
            }  /* ends else remove object */
         }  /* ends if object_found == 1 */

      }   /* ends loop over j */
   }  /* ends loop over i */

   printf("\nGROW> found %d objects", g_label);

} /* ends pixel_grow  */





   /********************************************
   *
   *  pixel_label_and_check_neighbors(...
   *
   *  This function labels a pixel with an object
   *  label and then checks the pixel's 8
   *  neighbors.  If any of the neigbors are
   *  set, then they are also labeled.
   *
   *  It also updates the target or ave pixel
   *  value of the pixels in the region being
   *  grown.
   *
   ***********************************************/
pixel_label_and_check_neighbor(input_image,
                         output_image, target,
                         sum, count, stack,
                         g_label, stack_empty,
                         pointer, r, e, diff,
                         stack_file_in_use,
                         first_call)
int   *count,
      e,
      *first_call,
      *pointer,
      r,
      *stack_empty,
      *stack_file_in_use;

short input_image[ROWS][COLS],
      output_image[ROWS][COLS],
      g_label,
      *sum,
      *target,
      stack[STACK_SIZE][2],
      diff;
{
   int already_labeled = 0,
       i, j;

   if (output_image[r][e] != 0)
      already_labeled = 1;

   output_image[r][e] = g_label;
   *count  = *count + 1;
   if(*count > 1){
      *sum    = *sum + input_image[r][e];
      *target = *sum / *count;
   }

      /***************************************
      *
      *   Look at the 8 neighors of the
      *   point r,e.
      *
      *   Ensure the points are close enough
      *   to the target and do not equal
      *   FORGET_IT.
      *
      *   Ensure the points you are checking
      *   are in the image, i.e. not less
      *   than zero and not greater than
      *   ROWS-1 or COLS-1.
      *
      ***************************************/

   for(i=(r-1); i<=(r+1); i++){
      for(j=(e-1); j<=(e+1); j++){

         if((i>=0)   &&
            (i<=ROWS-1)  &&
            (j>=0)   &&
            (j<=COLS-1)){

            if( input_image[i][j] != FORGET_IT   &&
                is_close(input_image[i][j],
                            *target, diff)       &&
                output_image[i][j] == 0){
               *pointer           = *pointer + 1;
               stack[*pointer][0] = i; /* PUSH      */
               stack[*pointer][1] = j; /* OPERATION */
               *stack_empty       = 0;

               if(*pointer >= (STACK_SIZE -
                               STACK_FILE_LENGTH)){
                  push_data_onto_stack_file(stack,
                            pointer, first_call);
                  *stack_file_in_use = 1;
               }  /* ends if *pointer >=
                     STACK_SIZE - STACK_FILE_LENGTH*/

            }  /* ends if is_close */
         }  /* end if i and j are on the image */
      }  /* ends loop over i rows           */
   }  /* ends loop over j columns        */
}  /* ends pixel_label_and_check_neighbors  */



   /********************************************
   *
   *  is_close(...
   *
   *  This function tests to see if two pixel
   *  values are close enough together.  It
   *  uses the delta parameter to make this
   *  judgement.
   *
   ***********************************************/

is_close(a, b, delta)
   short a, b, delta;
{
   int   result = 0;
   short diff;

   diff = a-b;
   if(diff < 0) diff = diff*(-1);
   if(diff < delta)
      result = 1;
   return(result);
}  /* ends is_close */




     /*******************************************
     *
     *   erode_image_array(..
     *
     *   This function erodes pixels.  If a pixel
     *   equals value and has more than threshold
     *   neighbors equal to 0, then set that
     *   pixel in the output to 0.
     *
     *******************************************/


erode_image_array(the_image, out_image,
                  value, threshold)
   short  the_image[ROWS][COLS],
          out_image[ROWS][COLS],
          threshold,
          value;
{
   int    a, b, count, i, j, k,
          length, width;

      /***************************
      *
      *   Loop over image array
      *
      ****************************/

   for(i=0; i<ROWS; i++)
      for(j=0; j<COLS; j++)
         out_image[i][j] = the_image[i][j];

   printf("\n");

   for(i=1; i<ROWS-1; i++){
      if( (i%10) == 0) printf("%3d", i);
      for(j=1; j<COLS-1; j++){
         if(the_image[i][j] == value){
            count = 0;
            for(a=-1; a<=1; a++){
                for(b=-1; b<=1; b++){
                      if(the_image[i+a][j+b] == 0)
                         count++;
                }  /*  ends loop over b */
            }  /* ends loop over a */
            if(count > threshold) out_image[i][j] = 0;
         }  /* ends if the_image == value */
      }  /* ends loop over j */
   }  /* ends loop over i */

}  /* ends erode_image_array */





   /********************************************
   *
   *  get_edge_region_options(...
   *
   *  This function interacts with the user to   
   *  get the options needed to call the 
   *  edge and region based segmentation 
   *  routines.
   *
   ********************************************/

get_edge_region_options(method, edge_type, 
         min_area, max_area, set_value, 
         diff, percent, erode)
   char  method[];
   float *percent;
   int   *edge_type;
   short *diff, *erode, 
         *min_area, *max_area, 
         *set_value;
{
   int not_finished = 1, response;

   while(not_finished){
      printf("\n\nEdge Region Segmentation Options:");
      printf("\n\t1.  Method is %s", method);
      printf("\n\t    Recall: Edge, Gray shade, "
                      "Combination");
      printf("\n\t2.  Edge type is %d", *edge_type);
      printf("\n\t    Recall: ");
      printf("\n\t     1=Prewitt     2=Kirsch");
      printf("\n\t     3=Sobel       4=quick");
      printf("\n\t     5=homogeneity 6=difference");
      printf("\n\t     7=contrast    8=gaussian");
      printf("\n\t     10=range      11=variance");
      printf("\n\t3.  Min area is %d", *min_area);
      printf("\n\t4.  Max area is %d", *max_area);
      printf("\n\t5.  Set value is %d", *set_value);
      printf("\n\t6.  Difference value is %d", *diff);
      printf("\n\t7.  Threshold percentage is %f",
                      *percent);
      printf("\n\t8.  Erode is %d", *erode);
      printf("\n\nEnter choice (0 = no change) _\b");

      get_integer(&response);

      if(response == 0){
        not_finished = 0;
      }

      if(response == 1){
         printf("\n\t    Recall: Edge, Gray shade, "
                         "Combination");
         printf("\n\t> ");
         gets(method);
      }

      if(response == 2){
         printf("\n\t    Recall:"); 
         printf("\n\t     1=Prewitt     2=Kirsch");
         printf("\n\t     3=Sobel       4=quick");
         printf("\n\t     5=homogeneity 6=difference");
         printf("\n\t     7=contrast    8=gaussian");
         printf("\n\t     10=range      11=variance");
         printf("\n\t__\b");
         get_integer(edge_type);
      }

      if(response == 3){
         printf("\nEnter min area:__\b\b");
         get_integer(min_area);
      }

      if(response == 4){
         printf("\nEnter max area:__\b\b");
         get_integer(max_area);
      }

      if(response == 5){
         printf("\nEnter set value:__\b\b");
         get_integer(set_value);
      }

      if(response == 6){
         printf("\nEnter difference:__\b\b");
         get_integer(diff);
      }

      if(response == 7){
         printf("\nEnter threshold percentage:__\b\b");
         get_float(percent);
      }

      if(response == 8){
         printf("\nEnter erode:__\b\b");
         get_integer(erode);
      }

   }  /* ends while not_finished */
}  /* ends get_edge_region_options */
