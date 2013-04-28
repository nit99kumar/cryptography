
/*****************************************************
 *
 * File: lzwutil.c
 * Author: Dwayne Phillips
 * Compiler: Microsoft C 6.0
 * Functions: setup
 *            display_table
 *
 * Purpose: Utility routines for the LZW data compression
 *          and decompression program.
 *
 * Date: October 1991
 *
******************************************************/


#include "d:\cips\lzwinc.h"


/**************************************************
 *
 * setup(...
 *
 * A.   Set the first 256 items in the string table
 *      to the char number.
 *
 * B.   Set the remaining items to the null char.
 *
***************************************************/

setup(string_table)
   struct item string_table[];
{
   int i;
/* A. */
   for(i=0; i<256; i++){
      string_table[i].code_num = 0;
      string_table[i].code_char = i;
   }

/* B. */
   for(i=256; i<TABLE; i++){
      string_table[i].code_num = 0;
      string_table[i].code_char = '\0';
   }
}


/**************************************************
 *
 * display_table(...
 *
 * This is a debug routine that shows you the
 * string table.
 *
 * A. If there is an entry in the string table, then:
 *
 * B. Print the ASCII character if it is printable,
 *
 * C. or print the decimal value of the character
 *    if it is not printable.
 *
********************************************************/


/* debug routine */
display_table(string_table)
   struct item string_table[];
{
   int s, k;
   printf("\n");
   s=256;
   k=1;

   while(k){
/* A. */
      if(string_table[s].code_num !=0 ){
/* B. */
         if((string_table[s].code_char > 33)  &&
            (string_table[s].code_char < 127))
            printf("\nLZW:TEST: table[%d]  %3d %c",
                    s, string_table[s].code_num,
                    string_table[s].code_char);
/* C. */
         else
            printf("\nLZW:TEST: table[%d]  %3d %2d(d)",
                    s, string_table[s].code_num, string_table[s].code_char);
         s++;
      }
      else
         k = 0;
   }
}
