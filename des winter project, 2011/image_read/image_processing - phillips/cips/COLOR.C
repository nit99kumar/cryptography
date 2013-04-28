


       /***************************************************************
       *
       *       file c:\lsu\color.c
       *
       *       Functions: This file contains
       *           main
       *
       *       Purpose: This program sets the screen to any color
       *	   combination you want.
       *
       *       Modifications:
       *	   1 September 1996 - created
       *
       ***************************************************************/



#include <stdio.h>
#include <graph.h>
#include <io.h>




main()
{

   char response[80];
   long  back, fore; 
   int   i, j, quit, not_done = 1;

   _setvideomode(_TEXTC80);

   while(not_done){
      printf("\nEnter background color (0 - 7): ");
      gets(response);
      back = atoi(response);
      printf("\nEnter foreground color (0 - 15): ");
      gets(response);
      fore = atoi(response);

      if(back >= 0   &&  back <= 7   &&  
         fore >= 0   &&  fore <= 15  &&
         back != fore){
           _setbkcolor(back);
           _settextcolor(fore);
           _clearscreen(_GCLEARSCREEN);
      }

      if(back == fore)
         printf("\nDon't want the letters and the background "
                "to be the same, try again");

      printf("\nIf you want to quit, enter 0");
      printf("\nIf you want to try another, enter 1: ");
      gets(response);
      quit = atoi(response);

      if(quit != 1)
         not_done = 0;

   }  /* ends while not done */

}  /* ends main */
