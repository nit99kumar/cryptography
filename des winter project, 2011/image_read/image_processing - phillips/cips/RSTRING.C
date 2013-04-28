
        /*****************************************************
        *
        *       file d:\cips\rstring.c
        *
        *       Functions: This file contains
        *            clear_buffer
        *            long_clear_buffer
        *            my_clear_text_screen
        *
        *       Purpose: This function reads a string of input
        *            from the keyboard.
        *
        *******************************************************/

#include "cips.h"





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
   _setvideomode(_TEXTC80);/* MSC 6.0 statements */
   _setbkcolor(1);
   _settextcolor(7);
   _clearscreen(_GCLEARSCREEN);
}  /* ends clear_text_screen */
