
/*
     The Poor Man's Revision Control System Project

     Dwayne Phillips
     31 August 1991

     This file contains functions that are used by
     several of the RCS programs.  They are held here
     to avoid having multiple copies of the same
     function.

     create_rcs_file_name
     rev_number
     replace_slash
     go_to_correct_rev
     copy_rcs_to_source
     copy_latest_rcs_to_source
     get_header_lines


*/

#include "d:\cips\pmrcs.h"

/*
   create_rcs_file_name(char source_name[],
                       char rcs_name[])

   Add the RCS-slash-v onto the front of the source
   name and put this into the rcs name.
*/

create_rcs_file_name(char *source_name, char *rcs_name)
{
   char *place, temp[80];
   int  i, j, slash, slot;

      /* look for the last slash already in the
         file name */
   slash = 47;
   place = strrchr(source_name, slash);
   slash = 92;
   if(place == '\0')
         place = strrchr(source_name, slash);

      /* if no slash in the file name then just
         add RCS-slash-v */
   if(place == '\0'){
      strcpy(rcs_name, "RCS/v");
      strcat(rcs_name, source_name);
   }
      /* else put the RCS-slash-v into the appropriate
         slot in the file name */
   else{
         /* find last slash */
      slot = place - source_name;
         /* copy first part of name to temp */
      strncpy(temp, source_name, slot);
      temp[slot] = '\0';
         /* append slash RCS slash v */
      strcat(temp, "/RCS/v");

      slot++;
      j = strlen(temp);
         /* copy remainder of source name
            (basic file name) to the end
            of temp */
      for(i=slot; source_name[i]!='\0'; i++){
         temp[j] = source_name[i];
         j++;
      }

      temp[j] = '\0';
         /* put temp into rcs name */
      strcpy(rcs_name, temp);
   }


}  /* ends create_rcs_file_name */



/*
   rev_number(char *string)

   This function pulls an integer revision number
   out of a string.
*/

rev_number(char *string)
{
   char  *new_string;
   int   doit = 0,
         i, j, result;

   i = 0;
   while(doit == 0){
      j = string[i];
      doit = isdigit(j);
      i++;
   }
   i--;

   for(j=0; string[i] != '\0'; i++, j++)
         new_string[j] = string[i];
   result = atoi(new_string);
   return(result);

}  /* ends rev_number */



/*
   replace_slash(char string[])

   This function searches through string and replaces
   every slash with a backslash.  We need this because
   you cannot have the slash character in any DOS
   system calls.  DOS interprets that as an option.
*/

replace_slash(char string[])
{
   int  slash      = 47,
        back_slash = 92;
   int  i, j;

   j = strlen(string);
   for(i=0; i<j; i++){
      if(string[i] == slash) string[i] = back_slash;
   }

}  /* ends replace_slash */




/*
   go_to_correct_rev(FILE *rcs_file, int rev)

   Search down through the rcs file until you reach
   the source code for the desired revision.
*/

go_to_correct_rev(FILE *rcs_file, int rev)
{
   char *result, string[80];
   int  found_it = 0,
        new_rev,
        reading = 1,
        still_reading = 1;

/*
         Read in the rcs file until you hit FIRST_LINE.
         Check to see if the rev after FIRST_LINE
         is the one you want.  If it is not, keep
         reading until the next FIRST_LINE or end of file.
         If you found the rev you want, then read down
         through the header lines until you hit the
         DELIMETER.  Then quit the function.
*/

   while(reading){ /* read file */
      result = fgets(string, 80, rcs_file);
      if( strncmp(string, FIRST_LINE, 5) == 0){
            /* if hit FIRST_LINE then get rev # */
         result = fgets(string, 80, rcs_file);
         new_rev = atoi(string);
         if(rev == new_rev){
                   /* if rev # matches then read
                      down to the DELIMETER
                      and quit there */
            while(still_reading){
               result = fgets(string, 80, rcs_file);
               if( strncmp(string, DELIMETER, 5) == 0){
                  still_reading = 0;
                  reading       = 0;
                  found_it      = 1;
               }  /* ends if DELIMETER */
               if(result == '\0') still_reading = 0;
            }  /* ends while still_reading */
         }  /* ends if rev == new_rev */
      }  /* ends if FIRST_LINE */
      if(result == '\0') reading = 0;
   } /* ends while reading */

   if(found_it == 0){
      printf("\n\ncheckout.c> Did not find the"
             " desired revision\n");
      fclose(rcs_file);
      exit(-5);
   }
}  /* ends go_to_correct_rev */





/*
   copy_rcs_to_source(FILE *rcs_file, FILE *source_file)

   This function copies the lines from the rcs file to the
   source file.
*/

copy_rcs_to_source(FILE *rcs_file, FILE *source_file)
{
   char string[80];
   int  reading = 1;

   while(reading){
      fgets(string, 80, rcs_file);
      if( strncmp(string, DELIMETER, 5) == 0)
         reading = 0;
      else
         fputs(string, source_file);
   }  /* ends while reading */
}  /* ends copy_rcs_to_source */

/*
   copy_latest_rcs_to_source(FILE *rcs_file, FILE *source_file)

   This function copies the most recent revision from the
   rcs file to the source file.
*/

copy_latest_rcs_to_source(FILE *rcs_file, FILE *source_file)
{
   char string[80];
   int  reading = 1;

/*
          Read down until you hit the first DELIMETER.
          Then copy lines from the rcs file to the
          source file until you hit the DELIMETER again.
*/

   while(reading){
      fgets(string, 80, rcs_file);
         if( strncmp(string, DELIMETER, 5) == 0){
            while(reading){
               fgets(string, 80, rcs_file);
                  if(strncmp(string, DELIMETER, 5) == 0)
                     reading = 0;
                  else
                     fputs(string, source_file);
            } /* ends while reading */
         }  /* ends if DELIMETER */
   }  /* ends while reading */
}  /* ends copy_latest_rcs_to_source */




/*
   get_header_lines(FILE *the_file, int version)

   Have the user enter the header lines and write
   each line to the rcs file.

   Write out the FIRST_LINE
       version
       today's date
       user's header lines
       DELIMETER
*/

get_header_lines(FILE *the_file, int version)
{
   char   string[80];
   int    entering = 1;
   time_t ltime;

   time(&ltime);

   fputs(FIRST_LINE, the_file);
   sprintf(string, "%d\n", version);
   fputs(string, the_file);
   sprintf(string, "%s", ctime(&ltime));
   fputs(string, the_file);

   printf("\n\nEnter your header lines");
   printf("\nStop the header lines by entering");
   printf("\na . on a line by itself.");
   printf("\n");
   while(entering){
      printf(">>");
      fgets(string, 80, stdin);
      if(string[0] != '.')
       fputs(string, the_file);
      else
       entering = 0;
   } /* ends while entering */

   fputs(DELIMETER, the_file);

}  /* ends get_header_lines */

