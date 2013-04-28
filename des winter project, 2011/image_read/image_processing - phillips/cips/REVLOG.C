/*
   revlog.c
   The Revision Control System

   Dwayne Phillips
   November 1991
*/


#include "rcs.h"



main(argc, argv)
   int argc;
   char *argv[];
{
   char   rcs_name[80], source_name[80], string[80];
   int    printing;
   FILE   *rcs_file;

   if(argc <  2   ||   argc > 2){
      printf("\n\n\tusage: revlog source-file\n");
      exit(1);
   }

   strcpy(source_name, argv[1]);
   create_rcs_file_name(source_name, rcs_name);

   if((rcs_file = fopen(rcs_name, "r")) == '\0'){
      printf("\nrevlog>> "
             "cannot open the rcs file >>%s<<",
             rcs_name);
      exit(-1);
   }

   printf("\n----------------------------------\n");

   while(fgets(string, 80, rcs_file) != '\0'){
      if( strncmp(string, FIRST_LINE, 5) == 0){
         printing = 1;
         while(printing){
            fgets(string, 80, rcs_file);
            if(strncmp(string, DELIMETER, 5) != 0){
               printf("%s", string);
            }  /* ends if != DELIMETER */
            else /* else you hit DELIMETER stop printing */
               printing = 0;
         }  /* ends while printing */
         printf("\n----------------------------------\n");
      }  /* ends if FIRST_LINE */
   }  /* ends while reading rcs file */

   fclose(rcs_file);
} 
