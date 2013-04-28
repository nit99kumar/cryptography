
   /*************************** 
   * 
   *   cips2.c 
   *   COMPOSITE FILE COMPRISING: 
   *   gpcips.c 
   *   numcvrt.c 
   *   mymsc.c 
   * 
   ***************************\ 




    /******************************************************
    *
    *  file d:\cips\gpcips.c
    *
    *  Functions: This file contains
    *      get_parameters
    *      show_parameters
    *      my_clear_text_screen
    *
    *  Purpose - These functions get image parameters.
    *
    *  External Calls:
    *      intcvrt.c - get_integer
    *
    *  Modifications:
    *      19 February 1987 - These functions were 
    *           taken out of the file ip.c.
    *      28 June 1990 - changed to gpcips and the
    *           channel parameter was removed.
    *      14 June 1993 - added the function
    *           my_clear_text_screen
    *
    ******************************************************/


#include "cips.h"




get_parameters(il, ie, ll, le)
        int *il, *ie, *le, *ll;
{
        int choice, not_finished;

        not_finished = 1;
        while(not_finished){
           show_parameters(il, ie, ll, le);
           printf("\n\nEnter choice to change (enter 0 for no changes) __\b\b");
           get_integer(&choice);

           switch (choice){
              case 0:
                 not_finished = 0;
                 break;

              case 1:
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

           }  /* ends switch choice       */
        }     /* ends while not_finished  */
}             /* ends get_parameters      */


show_parameters(il, ie, ll, le)
        int *il, *ie, *le, *ll;
{
   printf("\n\nThe image parameters are:");
   printf("\n\t2.      il = %4d", *il);
   printf("\n\t3.      ie = %4d", *ie);
   printf("\n\t4.      ll = %4d", *ll);
   printf("\n\t5.      le = %4d", *le);
}  /* ends show_parameters  */





   /*************************************************
   *
   *   my_clear_text_screen()
   *
   *   This calls Microsoft C functions to clear
   *   the text screen and set a blue background
   *   with gray text.
   *
   **************************************************/

my_clear_text_screen()
{
   my_setvideomode(TEXTC80);/* MSC 6.0 statements */
   my_setbkcolor(1);
   my_settextcolor(7);
   my_clearscreen(GCLEARSCREEN);
}  /* ends clear_text_screen */

    /******************************************
    *
    *  file d:\cips\numcvrt.c
    *
    *  Functions:
    *     get_integer
    *     get_short
    *     get_long
    *     get_float
    *
    *  Purpose: 
    *     These functions read numbers from
    *     the keyboard.
    *
    *  Modifications:
    *     12 May 1993 - recreated
    *
    *******************************************/




get_integer(n)
   int *n;
{
   char string[80];

   gets(string);
   *n = atoi(string);
}


get_short(n)
   short *n;
{
   char string[80];

   gets(string);
   *n = atoi(string);
}


get_long(n)
   long *n;
{
   char string[80];

   gets(string);
   *n = atol(string);
}


get_float(f)
   float *f;
{
   char string[80];

   gets(string);
   *f = atof(string);
}

    /***********************************************
    *
    *  file d:\cips\mymsc.c
    *
    *  Functions: This file contains
    *     my_clearscreen
    *     my_setvideomode
    *     my_setlinestyle
    *     my_setcolor
    *     my_setpixel
    *     my_moveto
    *     my_lineto
    *     my_settextcolor
    *     my_setbkcolor
    *     my_settextposition
    *     my_outtext
    *     my_remappalette
    *     my_map_64_shades_of_gray
    *
    *  Purpose:
    *     This file contains subroutines that 
    *     call Microsoft C 6.0 specific 
    *     routines.  I put them all here in one
    *     file to make it easier to port the
    *     C Image Processing System software
    *     to other C compilers and non-IBM PC
    *     computers.
    *     
    *     Each subroutine in this file is a "wrapper"
    *     for a Microsoft C 6.0 routine.  I put
    *     the letters "my" in front of the Microsoft
    *     routine name.  For example, my_clearscreen
    *     calls Microsoft's _clearscreen.
    *     
    *     The one exception is 
    *     my_map_64_shades_of_gray.
    *
    *  External Calls: all MSC calls
    *     _clearscreen
    *     _setvideomode
    *     _setlinestyle
    *     _setcolor
    *     _setpixel
    *     _moveto
    *     _lineto
    *     _settextcolor
    *     _setbkcolor
    *     _settextposition
    *     _outtext
    *     _remappalette
    *
    *  Modifications:
    *     5 June 1993 - created
    *
    *************************************************/




   /***********************************
	*
	*   This ifdef makes all the MSC
	*   calls work.  The MSC variable
	*   must be defined in cips.h.
	*
	************************************/

#ifdef MSC



my_clearscreen()
{
   _clearscreen(_GCLEARSCREEN);
}

my_lineto(x, y)
   short x, y;
{
   _lineto(x, y);
}

my_moveto(x, y)
   short x, y;
{
   _moveto(x, y);
}

my_outtext(text)
   char *text;
{
   _outtext(text);
}

my_remappalette(index, color)
   short index;
   long  color;
{
   _remappalette(index, color);
}

my_setbkcolor(color)
   long color;
{
   _setbkcolor(color);
}

my_setcolor(color)
   short color;
{
   _setcolor(color);
}

my_setlinestyle(mode)
   unsigned short mode;
{
   _setlinestyle(mode);
}

my_setpixel(x, y)
   short x, y;
{
   _setpixel(x, y);
}

my_settextcolor(index)
   short index;
{
   _settextcolor(index);
}

my_settextposition(row, column)
   short row, column;
{
   _settextposition(row, column);
}

my_setvideomode(mode)
   short mode;
{
   _setvideomode(mode);
}


my_map_64_shades_of_gray()
{
   map_64_shades_of_gray();
}



   /*******************************************
   *
   *   map_64_shades_of_gray()
   *
   *   This function maps 256 DAC registers to
   *   gray shades.  Taken from p. 73 of
   *   Sutty and Blair's text on superVGA
   *
   ********************************************/


map_64_shades_of_gray()
{

   _asm{
      mov ax,0013h ;mod 13h is 320x200x256
      int 10h

      mov ah,10h   ; function 10h
      mov al,1bh   ; sub function 1bh
      mov bx,0h    ; first DAC register to change
      mov cx,100h  ; change 256 DAC registers
      int 10h

   } /* ends asm */
}  /* ends map_64_shades_of_gray */

#endif /* ends ifdef MSC */




#ifdef OTHERC


my_clearscreen()
{
}

my_lineto(x, y)
   short x, y;
{
}

my_moveto(x, y)
   short x, y;
{
}

my_outtext(text)
   char *text;
{
}

my_remappalette(index, color)
   short index;
   long  color;
{
}

my_setbkcolor(color)
   long color;
{
}

my_setcolor(color)
   short color;
{
}

my_setlinestyle(mode)
   unsigned short mode;
{
}

my_setpixel(x, y)
   short x, y;
{
}

my_settextcolor(index)
   short index;
{
}

my_settextposition(row, column)
   short row, column;
{
}

my_setvideomode(mode)
   short mode;
{
}


my_map_64_shades_of_gray()
{
}

#endif /* ends ifdef OTHER C */
