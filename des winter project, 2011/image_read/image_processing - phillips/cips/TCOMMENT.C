
   /***********************************************
   *
   *  file tcomment.c
   *
   *  Functions: This file contains
   *     main
   *
   *  Purpose:
   *     This is the main calling program for 
   *     subroutines that read comments from
   *     a TIFF header and insert comments into
   *     a TIFF header.
   *
   *  External Calls:
   *     tcommsub.c - read_comments
   *                  insert_comments
   *
   *  Modifications:
   *     9 May 1995 - created
   *
   *************************************************/


#include "cips.h"




main(argc, argv)
   char *argv[];
   int  argc;
{
   char all_comments[1000],
        comments[MAX_NAME_LENGTH],
        file_name[MAX_NAME_LENGTH];

   int  i,
        should_insert = 0,
        should_read   = 0;


   if(argc < 3){
      printf("\nusage: tcomment file-name -c comments...");
      printf("\n       tcomment file-name -r");
      exit(1);
   }  /* ends if argc */

   strcpy(file_name, argv[1]);
   if(does_not_exist(file_name)){
      printf("\ntcomment> File %s does not exist", 
             file_name);
      exit(2);
   }  /* ends if does not exist */


   i = 2;
   while(i <= argc){

      if(strcmp(argv[i], "-r") == 0)
         should_read = 1;

      if(strcmp(argv[i], "-c") == 0){
         should_insert = 1;
         i++;
         comments[0] = '\0';
         while(i < argc){
            strcat(comments, argv[i]);
            strcat(comments, " ");
            i++;
         }  /* ends while i < argc */
      }  /* ends if argv == -c */
      
      i++;
   }  /* ends while i < argc */

   if(should_read){
      read_comments(file_name, all_comments);
      printf("\nThe comments in file %s are:", 
         file_name);
      printf("\n     ------------------------");
      printf("\n%s", all_comments);
      printf("\n     ------------------------");
   }  /* ends if should_read */

   if(should_insert)
      insert_comments(file_name, comments);

}  /* ends main */
