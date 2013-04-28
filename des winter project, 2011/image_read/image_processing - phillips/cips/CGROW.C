


       /***********************************************************
       *
       *       file c:\lsu\cgrow.c
       *
       *       Functions: This file contains
       *           grow
       *
       *       Purpose: This is the C version of the subroutine
       *           grow. It is a region growing object detector.
       *
       *       External Calls:
       *           lcn.c - label_and_check_neighbor
       *           cpa.c - compute_principle_axis
       *           thresh.c - threshold_image
       *           sfo.c - pop_data_off_of_stack_file
       *
       *       Modifications:
       *           July 1986 - ported to the IBM-PC
       *           10 November 1986 - uses vision3.h instead
       *               of vision2.h, also it has been broken up
       *               into separate files - lcn.c cpa.c thresh.c
       *           28 November 1986 - changed the variables
       *               area, coordinates, number, x_length, and y_length
       *               from ints to longs.
       *           10 February 1987 - change the image from an int
       *               to a short.
       *           27 August 1987 - changed the lcn_stack from an
       *               int to short.
       *           27 August 1987 - Modifying the stack operations
       *               so that when the lcn_stack becomes full,
       *               elements will be stored into a file.
       *               cgrow.c does the POP operation. If the pointer
       *               goes to zero and the stack_file_in_use then
       *               read elements from the stack file.
       *               The parameters stack_file_in_use and
       *               stack_file_name were added to
       *               the call to label_and_check_neighbors.
       *               The PUSH operation takes place in file lcn.c.
       *           23 January 1988 - the parameter first_call
       *               was added to the function label_and_check_
       *               neighbor as part of the stack file
       *               operations.
       *          25 January 1988 - added the parameter stack_file_
       *              in_use to the function pop_data_off_of_
       *              stack_file in file sfo.c. This is needed
       *              to indicate when the stack file is empty.
       *          27 January 1988 - added stack_file_name to the
       *              parameters.
       *          16 June 1988 - added multiple calls to threshold_
       *              image so that it can threshold multi spectral
       *              images.
       *          20 June 1988 - Still trying to fix the multi
       *              spectral threshold problem. Now grow calls
       *              read2_image multiple times if needed. region
       *              added as a parameter to grow. ht and wid
       *              removed as parameters.
       *          21 June 1988 - added image_name to the
       *	      parameter list.
       *	  24 July 1989 - added resolution to the parameter
       *	      list.  This allows multi-resolution analysis.
       *	      At the higher levels in the hierarchy perform
       *	      analysis at a lower resolution.
       *	   9 October 1989 - changed the way in which the
       *	      i_element and i_line variables are calculated.
       *
       ***********************************************************/


#include "d:\lsu\vision3.h"


        /******************************************************
        *
        *       grow(...
        *
        *       This function is an object detector. Originally
        *       written as an elas FORTRAN overlay, it has been
        *       modified into a C subroutine.
        *
        *       image - original array of pixels.
        *       binary - output array. 0's in background and
        *               numbers as objects.
        *
        ********************************************************/



grow(image, binary, limits, area, coordinates, x_length, y_length,
     alpha, whratios, number, stack_file_name, image_name, region,
     resolution)

        char    image_name[],
                stack_file_name[];

        float   alpha[],
		whratios[];

	int	resolution;

        long    area[],
                coordinates[GROW_SIZE][2],
                *number,
                region[],
                x_length[],
                y_length[];

        short   binary[ROWS][COLS],
                image[ROWS][COLS];

        struct descriptor_structure *limits;
{

            /********************************************************
            *
            *     NOTES:
            *     - y_length (height) will always be greater than
            *       x_length (width)
            *
            *     - the angle will be between +90 and -90 inclusive
            *
	    ********************************************************/

	char	name[80];

        float   sx,
                sy,
                x_to_y;

        int     call_number,
                counter,
                first_call,
                g_label,
                ht,
                i,
                ie,
                i_element,
                ii,
                il,
                i_line,
                j,
                jj,
                le,
                ll,
                mask[3][3],
                max_x,
                max_y,
                min_x,
                min_y,
                object_found,
                pointer,
                pop_i,
                pop_j,
                removed,
                size[5],
                stack_empty,
                stack_file_in_use,
                wid;

	long	max_area,
		max_height,
		max_width,
		temp;

        short channel,
              lcn_stack[STACK_SIZE][2];

        struct header_parameters par;

        int threshold_image(),
            label_and_check_neighbor(),
            compute_principle_axis();

/*****************
printf("\nhi=%d   low=%d <---> ht=%d   wid=%d",
        limits->spectrum[0].high_threshold,
        limits->spectrum[0].low_threshold,ht,wid);
printf("\nmax area=%ld  min area=%ld",
        limits->max_area,
        limits->min_area);
******************/

        first_call = 1;

             /* zero out the vectors  */
        for(i=0; i<GROW_SIZE; i++){
           alpha[i]          = 0.0;
           area[i]           =   0;
           coordinates[i][0] = 0.0;
           coordinates[i][1] = 0.0;
           whratios[i]       = 0.0;
           x_length[i]       =   0;
           y_length[i]       =   0;
        }  /* ends loop over i  */


	i_element = 0;
	i_line	  = 0;


	    /*****************************************
	    *
	    *	24 July 1989
	    *
	    *	For multi-resolution analysis you need
	    *	to alter the max_width, max_height,
	    *	and max_area.  This is because when
	    *	you go to a lower resolution objects
	    *	can be joined together.  They would
	    *	then be eliminated.
	    *
	    ******************************************/

	max_area   = limits->max_area * resolution;
	max_width  = limits->max_width * resolution;
	max_height = limits->max_height * resolution;


           /************************************************************
           *
           *   20 June 1988 - Added the ability to threshold multi
           *   spectral images.
           *
           ************************************************************/

        ie  = region[0];
        le  = ie + (region[2] - 1);
        ll  = region[1];
        il  = ll - (region[3] - 1);
        ht  = ll - il + 1;
        wid = le - ie + 1;

        for(i=0; i<limits->num_of_channels; i++){
           read2_image(image, image_name, limits->spectrum[i].channel,
		       il, ie, ll, le);
	   change_image_resolution(image, resolution);
printf("\nGROW> read image");
           call_number = i;
	   channel     = call_number;
           threshold_image(image, binary, limits, &counter,
			   ht, wid, call_number, channel);
strcpy(name, "1.dat");
write_image_array(binary, name);
        }
                      /*  must use channel-1 when calling  */
                      /*  threshold   21 June 1988         */
printf("\nCGROW> %d points set to 1",counter);

                /****************************************
                *
                *       Now begin the process of growing
                *       regions.
                *
                *****************************************/

        g_label       = 2;
        object_found  = FALSE;

        for(i=0; i<ht; i++){
            for(j=0; j<wid; j++){

                stack_file_in_use = FALSE;
                stack_empty       =  TRUE;
                pointer           =    -1;
                max_y             = i + 1;
                min_y             = i + 0;
                max_x             =     0;
                min_x             =   wid;
                area[g_label]     =     0;

                        /***************************************
                        *
                        *       Search for the first pixel of
                        *       a region.
                        *
                        ****************************************/

                if(binary[i][j] == (short)(1)){
                   sx = 0.0;
                   sy = 0.0;

                   label_and_check_neighbor(binary, lcn_stack, g_label,
                                    &stack_empty, &pointer, i, j,
                                    &max_x, &min_x, &max_y, &min_y,
                                    &sx, &sy, area, ht, wid,
                                    &stack_file_in_use, stack_file_name,
                                    &first_call);
                   object_found = TRUE;
                }


                        /***********************************************
                        *
                        *       If the stack is not empty, pop the
                        *       coordinates of the pixel off the stack
                        *       and check its 8 neighbors.
                        *
                        ***********************************************/

                /******************************************
                *
                *   This is the code to use without using
                *   the stack file
                *   17 January 1988
                *
                *while(stack_empty == FALSE){
                *   pop_i = lcn_stack[pointer][0];
                *   pop_j = lcn_stack[pointer][1];
                *   --pointer;
                *   if(pointer <= 0){
                *      pointer     =    0;
                *      stack_empty = TRUE;
                *   }
                *   label_and_check_neighbor(binary, lcn_stack, g_label,
                *                    &stack_empty, &pointer,
                *                    pop_i, pop_j,
                *                    &max_x, &min_x, &max_y,
                *                    &min_y, &sx, &sy, area, ht, wid,
                *                    &stack_file_in_use, stack_file_name);
                *}
                *
                **********************************************************/

                /****************************************************
                *
                *   The following code was inserted to use the
                *   stack file
                *   17 January 1988
                *
                *****************************************************/

                 while(stack_empty == FALSE){
                    pop_i = lcn_stack[pointer][0]; /* POP       */
                    pop_j = lcn_stack[pointer][1]; /* OPERATION */
                    --pointer;
                    if(pointer <= 0){
                       if(stack_file_in_use){
                          pop_data_off_of_stack_file(lcn_stack,
                                                     stack_file_name,
                                                     &pointer,
                                                     &stack_file_in_use);
                       }  /* ends if stack_file_in_use  */
                       else{
                          pointer     =    0;
                          stack_empty = TRUE;
                       }  /* ends else stack file is not in use  */
                    }  /*  ends if point <= 0  */

                    label_and_check_neighbor(binary, lcn_stack, g_label,
                                     &stack_empty, &pointer,
                                     pop_i, pop_j,
                                     &max_x, &min_x, &max_y,
                                     &min_y, &sx, &sy, area, ht, wid,
                                     &stack_file_in_use, stack_file_name,
                                     &first_call);
                 }  /* ends while stack_empty == FALSE */



                 if(object_found == TRUE){
                    object_found = FALSE;

/********************
printf("\n\n\nCGROW> maxx=%d minx=%d maxy=%d miny=%d sx=%f sy=%f",
    max_x,min_x,max_y,min_y,sx,sy);
printf("\nCGROW> Now calculate the principle axis");
********************/

                    compute_principle_axis(max_x, min_x, max_y, min_y,
                                             sx, sy, area, g_label,
                                             binary, alpha, x_length,
                                             y_length, whratios, ht, wid);

                /******************************************
                *
                *       If any of the following conditions
                *       hold then eliminate this region.
                *
                ********************************************/

/****************
printf("\nCGROW>Address of g_label is %u", &g_label);
printf("\nCGROW> g_label is %d",g_label);
printf("\nCGROW> angle is %f",alpha[g_label]);
printf("\nCGROW> lengths are x=%ld, y=%ld",x_length[g_label],
   y_length[g_label]);
printf("\nCGROW> area is %ld",area[g_label]);
*****************/


                    if(y_length[g_label] != 0){
                       x_to_y = ((float)x_length[g_label])/
                          ((float)y_length[g_label]);
                       if(x_to_y > 1.0){
                          printf("\nCGROW> Mistake in x_to_y ratio so invert it");
                          x_to_y            = 1.0/x_to_y;
                          temp              = y_length[g_label];
                          y_length[g_label] = x_length[g_label];
                          x_length[g_label] = temp;
                       }  /* ends if x_to_y > 1.0            */
                    }     /* ends if y_length[g_label] != 0  */

                    whratios[g_label] = x_to_y;

/************************
printf("\n\n");
printf("\nLimits area = %ld %ld",limits->max_area,limits->min_area);
printf("\nActual area=%ld", area[g_label]);
printf("\nLimits width= %ld %ld",limits->max_width,limits->min_width);
printf("\nActual x length=%ld", x_length[g_label]);
printf("\nLimits height=%ld %ld",limits->max_height,limits->min_height);
printf("\nActual y length=%ld", y_length[g_label]);
printf("\nLimits ratios=%f %f",
  limits->max_w_to_h_ratio,limits->min_w_to_h_ratio);
printf("\nActual x_to_y = %f",x_to_y);
printf("\nLimits angle= %f %f",limits->max_angle,limits->min_angle);
printf("\nActual alpha=%f", alpha[g_label]);
************************/


			/*****************************************
			*
			*   25 July 1989
			*   Replaced limits->max_area, max_width
			*   and max_hieght by just
			*   max_area, max_height, max_width
			*
			*   26 January 1990
			*   Put in if statement about max_w_to_h_
			*   ratio.  Problem with sidewalk ratio
			*   going to infinity.
			*
			******************************************/


		    removed = FALSE;
		    if(limits->max_w_to_h_ratio > 1.0)
		       whratios[g_label] = 0.0;

		    if( (area[g_label] > max_area)	    ||
			(area[g_label] < limits->min_area)	    ||
                        (x_length[g_label] < limits->min_width)     ||
			(x_length[g_label] > max_width)     ||
                        (y_length[g_label] < limits->min_height)    ||
			(y_length[g_label] > max_height)    ||
                        (whratios[g_label]>limits->max_w_to_h_ratio)||
                        (whratios[g_label]<limits->min_w_to_h_ratio)||
                        (alpha[g_label] > limits->max_angle)        ||
                        (alpha[g_label] < limits->min_angle)){


   printf("\n\n");
 if(area[g_label] > limits->max_area){
   printf("\n\tCGROW> area > max area");
   printf("\nLimits area = %ld %ld",limits->max_area,limits->min_area);
   printf("\nActual area=%ld", area[g_label]);
}
 if(area[g_label] < limits->min_area){
   printf("\n\tCGROW> area < min area");
   printf("\nLimits area = %ld %ld",limits->max_area,limits->min_area);
   printf("\nActual area=%ld", area[g_label]);
}
 if(x_length[g_label] < limits->min_width){
   printf("\n\tCGROW> x length < min width");
   printf("\nLimits width= %ld %ld",limits->max_width,limits->min_width);
   printf("\nActual x length=%ld", x_length[g_label]);
}
 if(x_length[g_label] > limits->max_width){
   printf("\n\tCGROW> x length > max width");
   printf("\nLimits width= %ld %ld",limits->max_width,limits->min_width);
   printf("\nActual x length=%ld", x_length[g_label]);
}
 if(y_length[g_label] < limits->min_height){
   printf("\n\tCGROW> y length < min height");
   printf("\nLimits height=%ld %ld",limits->max_height,limits->min_height);
   printf("\nActual y length=%ld", y_length[g_label]);
}
 if(y_length[g_label] > limits->max_height){
   printf("\n\tCGROW> y length > max height");
   printf("\nLimits height=%ld %ld",limits->max_height,limits->min_height);
   printf("\nActual y length=%ld", y_length[g_label]);
}
 if(x_to_y >  limits->max_w_to_h_ratio){
   printf("\n\tCGROW> x to y > max ratio");
   printf("\nLimits ratios=%f %f",
     limits->max_w_to_h_ratio,limits->min_w_to_h_ratio);
   printf("\nActual x_to_y = %f",x_to_y);
}
 if(x_to_y <  limits->min_w_to_h_ratio){
   printf("\n\tCGROW> x to y < min ratio");
   printf("\nLimits ratios=%f %f",
     limits->max_w_to_h_ratio,limits->min_w_to_h_ratio);
   printf("\nActual x_to_y = %f",x_to_y);
}
 if(alpha[g_label] > limits->max_angle){
   printf("\n\tCGROW>  alpha > max angle");
   printf("\nLimits angle= %f %f",limits->max_angle,limits->min_angle);
   printf("\nActual alpha=%f", alpha[g_label]);
}
 if(alpha[g_label] < limits->min_angle){
   printf("\n\tCGROW>  alpha < min angle");
   printf("\nLimits angle= %f %f",limits->max_angle,limits->min_angle);
   printf("\nActual alpha=%f", alpha[g_label]);
}



                        removed = TRUE;
printf("\nCGROW> set removed to TRUE");

                        for(ii=min_y; ii<=max_y; ii++){
                           for(jj=min_x; jj<=max_x; jj++){
                              if(binary[ii][jj] == (short)(g_label))
                                 binary[ii][jj] = 0;
                           }
                        }
                    }   /* ends large if */
                    else{       /* if the object was not removed then   */
                                /* calculate the center coordinates     */
                       coordinates[g_label][1] = (long)(i_line +
                                              (sy/(float)area[g_label]));
                       coordinates[g_label][0] = (long)(i_element +
                                              (sx/(float)area[g_label]));
                    }
/*************************
printf("\nCGROW> THE END, removed=%d",removed);
printf("\nCGROW> Coordinates are %ld %ld",
   coordinates[g_label][0],
   coordinates[g_label][1]);
**************************/

                    if(!removed){
                       ++g_label;
                    }   /* ends if !removed     */
                 }      /* ends if object_found */

           }        /* ends loop over j */ /* corresponds to 96 */
        }           /* ends loop over i */

        *number = (long)(g_label);

}       /* ends grow  */
