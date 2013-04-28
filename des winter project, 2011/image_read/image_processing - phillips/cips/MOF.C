

           /******************************************************
           *
           *       file d:\cips\mof.c
           *
           *       Functions: This file contains
           *          my_open
           *          my_fopen
           *
           *       Purpose: This function opens a file. Borland's
           *          Turbo C opens files a little different from
           *          the standard UNIX C. Instead of using this
           *          different method in a number of various files,
           *          the method is placed in this one file. If the
           *          programs are moved to another system, all changes
           *          will be located in this one place.
           *
           *       External Calls:
           *          none
           *
           *       Modifications:
           *          18 June 1987 - created
           *          27 March 1993 - my_fopen added
           *
           ****************************************************/


#include "cips.h"

my_open(file_name)
   char file_name[];
{
   int file_descriptor;

   file_descriptor = open(file_name, O_RDWR | O_CREAT | O_BINARY, 
                          S_IWRITE);

   return(file_descriptor);


}  /* ends my_open  */




   /***************************************
   *
   *   my_fopen(file_name)
   *
   *   This function opens a file in 
   *   read-only binary mode and returns
   *   the file pointer.
   *
   **************************************/

my_fopen(file_name)
   char file_name[];
{
   FILE *result;

   result = fopen(file_name, "rb");

   return(result);
}  /* ends my_fopen  */
