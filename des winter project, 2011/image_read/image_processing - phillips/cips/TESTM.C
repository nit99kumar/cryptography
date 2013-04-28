/*****************************************************/
/*****************************************************/
/*PAGE Includes and data structures */

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

#define L              100
#define LPP             66
#define CPL             85
#define FOOTER           5
#define LEFT_MARGIN     10 
#define RIGHT_MARGIN     5 
#define END_OF_LIST   0x00
#define SPACE          ' '
#define VERSION    "Pageit Version 1 - March 1996"

/*****************************************************/
/*****************************************************/

main(int argc, char *argv[])
{
   int i, size, *x;
   size = atoi(argv[1]);
   x = malloc(size*sizeof(int));
   for(i=0; i<size; i++){
      x[i] = i % 1000;
   }
   for(i=0; i<size; i++){
      if(i%100 == 0)
         printf("%5d", x[i]);
   }
   free(x);
}  /* ends main */
