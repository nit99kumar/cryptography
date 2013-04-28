

/*PAGE Includes and Definitions

  A. Include file definitions
  B. Define constants
  C. Define the possible page breaks
	  You modify the program to include new page break
     types at that point.  The PAGES array will hold
     up to PAGE_TYPE types of markers.  If this is not
     enough, increase the value of the PAGE_TYPE 
     constant.

*/

/* A. */
#include <time.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* B. */
#define BLANK      ' '
#define EOL       '\n'
#define LENGTH      80
#define LPP         66
#define MAX_STRING 255
#define NEWLINE   '\n'
#define PAGE_TYPE   10
#define TAB_SPACE    5
#define TITLES      50


/* C. */
char PAGES[PAGE_TYPE][LENGTH] = {
		"CPAGE",    /* FORTRAN page marker                 */
		"#PAGE",    /* C Script (UNIX) page marker         */
		"/*PAGE",   /* C page marker                       */
						/* add new types of page markers here  */
	  };

/*PAGE main */

main(int argc, char *argv[])
{
   char paged_file_name[LENGTH],
	     string[LENGTH],
        string2[LENGTH],
        time_string[LENGTH],
        titles[TITLES][LENGTH],
		  toc_file_name[LENGTH];

   FILE  *input_file;
   FILE  *output_file;
   FILE  *paged_file;
   FILE  *toc_file;

   int  i,
        line_counter,
        result,
        titles_pages[TITLES];

   struct tm *time_of_day;

   time_t seconds;

   for(i=0; i<TITLES; i++) titles[i][0] = '\0';
   time(&seconds);
   time_of_day = localtime(&seconds);
   sprintf(time_string, "%d-%d-%d\0",
           time_of_day->tm_mon+1,
           time_of_day->tm_mday,
           time_of_day->tm_year);

/*PAGE Interpret Command Line and Open Files

  A. If less than 2 or more than 4 arguments, quit
  B. If 3 arguments, ensure the input and output file
        names are different.
  C. Open the input file - if unsucessful, quit
  D. Open the paged_file (a temporary file)

*/

/* A. */
   if(argc < 2  ||  argc > 3){
      printf("\ntoc: usage: toc input_file [output_file]");
      exit(1);
   }

/* B. */
   if(argc == 3){
      if((result = strcmp(argv[1], argv[2])) == 0){
          printf("\ntoc: input and output files must be different\n");
          exit(2);
      }
   }

/* C. */
   if((input_file = fopen(argv[1], "rt")) == NULL){
      printf("\ntoc: Error opening input file %s\n", argv[1]);
      exit(3);
   }

/* D. */
	get_unique_file_name(paged_file_name);
   if((paged_file = fopen(paged_file_name, "wt")) == NULL){
      printf("\ntoc: Error opening paged file\n");
      fclose(input_file);
      exit(4);
   }


/*PAGE Paginate, Write TOC, Concatinate

  A. Paginate the input file.  This reads the input file,
        paginates it, writes it to a temporary file, and
        builds the table of contents.
  B. Close the files.
  C. Write the table of contents to a temporary file.
  D. Combine the table of contents and the paginated file
        into the output file.
	  E. Open the temporary files.
     F. Copy the lines from the temporary files to the
          output file.
	  G. Close all files.
	  H. Delete the temporary files.
*/

/* A. */
   paginate_file(input_file, paged_file,
                 argv[1], titles, titles_pages);
printf("\n\npaginated file");

/* B. */
   fclose(input_file);
   fclose(paged_file);

/* C. */
	get_unique_file_name(toc_file_name);
   write_table_of_contents(toc_file_name, argv[1], titles,
                           titles_pages, time_string);
printf("\n\nwrote TOC file");

/* D. */
   if(argc == 3){
      if((output_file = fopen(argv[2], "wt")) == NULL){
         printf("\ntoc: Error opening output file\n");
         exit(1);
      }
   }  /* ends of if argc == 3 */
   else
      output_file = stdout;

/* E. */
   if((toc_file = fopen(toc_file_name, "rt")) == NULL){
      printf("\ntoc: Error opening table of contents file\n");
      exit(1);
   }
   if((paged_file = fopen(paged_file_name, "rt")) == NULL){
      printf("\ntoc: Error opening paged file\n");
      exit(1);
   }
printf("\n\nopened all files for final output");

/* F. */
   while(fgets(string, MAX_STRING, toc_file)){
      fputs(string, output_file);
   }
   while(fgets(string, MAX_STRING, paged_file)){
      fputs(string, output_file);
   }
printf("\n\ncopied all output");

/* G. */
   if(argc == 3)
      fclose(output_file);
   fclose(paged_file);
   fclose(toc_file);

/* H. */
	if(remove(paged_file_name) == -1)
		printf("\ntoc: ERROR could not delete temp file %s",
				  paged_file_name);
	if(remove(toc_file_name) == -1)
		printf("\ntoc: ERROR could not delete temp file %s",
				  toc_file_name);


}  /* end of main program */

/*PAGE paginate_file function

  paginate_file(FILE *input_file, FILE *output_file,
                char *name_of_input_file, char titles[TITLES][LENGTH],
                int titles_pages[])

  This function breaks the input file into pages and inserts
  spaces at the left edge of the page.  This makes it easier
  to punch holes in the page for a 3 ring binder.

  A. Declare variables.
  B. Read the date and time and put them into the time_string.
  C. Print the header for the first page.
  D. Loop through the input file.
        E. Look for the PAGE comments.
        F. If this is the first PAGE, record this in the table
              of contents but do not make an extra page.
        G. If you hit PAGE, then make a new page and
              and entry into the table of contents.
        H. Tab over the input line and write it to the output file.
        I. If the page is full, make a new page.
  J. Fill up the end of the last page with blank lines.

*/

paginate_file(FILE *input_file, FILE *output_file,
              char *name_of_input_file, char titles[TITLES][LENGTH],
              int titles_pages[])
{
/* A. */
   char   string[MAX_STRING], time_string[LENGTH];
   int    i,
          first_page   = 1,
          line_counter = 0,
          page_counter = 1,
          t_count      = 0;
   struct tm *time_of_day;
   time_t seconds;

/* B. */
   time(&seconds);
   time_of_day = localtime(&seconds);
   sprintf(time_string, "%d-%d-%d\0",
           time_of_day->tm_mon+1,
           time_of_day->tm_mday,
           time_of_day->tm_year);

/* C. */
   write_3_line_header(output_file, name_of_input_file,
                       time_string, &page_counter);
   line_counter = line_counter + 3;
   blank_line(output_file);
   ++line_counter;

/* D. */
   while(fgets(string, MAX_STRING, input_file)){

/* E. */
		if(new_page_marker(string) == 1){

/* F. */
         if(first_page == 1){
            first_page = 0;
            strcpy(titles[t_count], string);
            titles_pages[t_count] = 1;
            t_count++;
         }  /* ends of if first_page == 1 */
         else{

/* G. */
            strcpy(titles[t_count], string);
            titles_pages[t_count] = page_counter + 1;
            t_count++;
            make_new_page(input_file, output_file,
                          name_of_input_file, time_string,
                          &page_counter, &line_counter);
         } /* ends of else first_page = 0 */
      }  /* end of if page */

/* H. */
      pad_string(string, TAB_SPACE);
      fputs(string, output_file);
      ++line_counter;

/* I. */
      if(line_counter >= LPP-10)
         make_new_page(input_file, output_file,
                       name_of_input_file, time_string,
                       &page_counter, &line_counter);

   }  /* end of while fgets */

/* J. */
   for(i=line_counter; i<=LPP; i++)
      blank_line(output_file);

} /* end of paginate_file function */


/*PAGE pad_string function

  pad_string(char string[], int num_blanks)

  This function inserts spaces into the beginning (left side)
  of a string.

  A. Declare variables.
  B. Fill a temporary string with num_blanks spaces.
  C. Concatenate input string onto temporary string.
  D. Copy temporary string back to input string.

*/

pad_string(char string[], int num_blanks)
{
/* A. */
   int   count, index;
   char  temp[LENGTH];

/* B. */
   for(count = 0; count < num_blanks; count++)
      temp[count] = ' ';
   temp[count] = '\0';
/* C. */
   strcat(temp, string);
/* D. */
   strcpy(string, temp);

}  /* end of pad_string function */



/*PAGE make_new_page function

  make_new_page(FILE *input_file, FILE *output_file,
                char *name_of_input_file, char *time_string,
                int *page_counter, int *line_counter)

  This function fills the remainder of a page with blank lines
  and prints a header at the top of the next page.

  A. Declare variables.
  B. Loop until the page runs out of lines.
  C. If you reach the end of a page, then
        D. Print 4 blank lines, jump to the next page,
           and print a page header.

*/

make_new_page(FILE *input_file, FILE *output_file,
              char *name_of_input_file, char *time_string,
              int *page_counter, int *line_counter)
{
/* A. */
   char string[MAX_STRING];
   int  not_finished = 1;

/* B. */
   while(not_finished){

/* C. */
      if(*line_counter >= LPP - 10){

/* D. */
         not_finished = 0;
         blank_line(output_file);
         blank_line(output_file);
         blank_line(output_file);
         blank_line(output_file);
         *line_counter = 0;
         *page_counter = *page_counter + 1;
         blank_line(output_file);
         *line_counter = *line_counter + 1;
         write_3_line_header(output_file, name_of_input_file,
                             time_string, page_counter);
         *line_counter = *line_counter + 3;
         blank_line(output_file);
         *line_counter = *line_counter + 1;
      }  /* end of if line counter >= LPP - 10 */
      fputs("     \n", output_file);
      *line_counter = *line_counter + 1;
   }  /* end of while not_finished */
}  /* end of make_new_page function */


/*PAGE strip_title function

  strip_title(char title[], char string[])

  This function takes an array (title) which has the PAGE
  comment followed by a title and returns an array (string)
  with only the title remaining.  It strips out
  the PAGE comment and leaves the title.
  For example, title = CPAGE This is a title
  the result,  string = This is a title

  A. Declare variables.
  B. Strip the first 6 characters out of the title array
        and put the result into the string array.
  C. Remove any special comment characters which may be
        at the end of the string array.

*/

strip_title(char title[], char string[])
{
/* A. */
   int i, n;

/* B. */
   for(i=6; i<80; i++){
      string[i-6] = title[i];
      if(title[i] == '\0')
         i = 81;
   }  /* end of loop over i */

/* C. */
   i = strlen(string);
   i--;

   if(i > 0){
      n = 1;
      while(n){
         if(string[i] == '\n'  ||
            string[i] == '/'   ||
            string[i] == '*'){
           string[i] = '\0';
           i--;
         }  /* end of if string[i] */
         else
            n = 0;
      }  /* end of while n */
   }  /* end of if i > 0 */
}  /* end of strip_title function */



/*PAGE write_3_line_header function

  write_3_line_header(FILE *file, char *file_name,
                      char *time_string, int *page_number)

  This function writes a three line header to the top of a new
  page.  The header contains the file name, date, and page
  number.

  A. Declare variables.
  B. Fill strings with blanks.
  C. Fill strings with information and send to
        the output file.

*/

write_3_line_header(FILE *file, char *file_name,
                    char *time_string, int *page_number)
{
/* A. */
   char b1[LENGTH], b2[LENGTH], b3[LENGTH], string[LENGTH];
   int  i;

/* B. */
   for(i=0; i<LENGTH; i++){
      b1[i]     = ' ';
      b2[i]     = ' ';
      b3[i]     = ' ';
      string[i] = ' ';
   } /* end of loop over i */

   b1[LENGTH-15] = '\0';
   b2[LENGTH-15] = '\0';
   b3[LENGTH-15] = '\0';

/* C. */
   sprintf(string, "%s\n", file_name);
   strcat(b1, string);
   fputs(b1, file);

   sprintf(string, "%s\n", time_string);
   strcat(b2, string);
   fputs(b2, file);

   sprintf(string, "Page %d\n", *page_number);
   strcat(b3, string);
   fputs(b3, file);

}  /* end of write_3_line_header function */


/*PAGE blank_line function
 *
 * blank_line(FILE *file)
 *
 * This function sends a blank line to a file.
 *
 * A.  Send a string of blanks to a file.
 *
*/

blank_line(FILE *file)
{
   fputs("          \n", file);
} /* end of blank_line function */


/*PAGE write_table_of_contents function

  write_table_of_contents(char *file_name,
                          char *input_file_name,
                          char titles[TITLES][LENGTH],
                          int  titles_pages[TITLES],
                          char *time_string)

  This function writes the table of contents information to a
  file.  The table of contents information text is in the
  char array titles and the page numbers are in the int array
  titles_pages.

  A. Declare variables.
  B. Open the table of contents file.
  C. Write the table of contents title to the file.
  D. Loop through the table of contents and write
        each entry to the file.
  E. Fill the remainder of the table of contents page with
        blank lines.
  F. Close the file.

*/

write_table_of_contents(char *file_name,
                        char *input_file_name,
                        char titles[TITLES][LENGTH],
                        int  titles_pages[TITLES],
                        char *time_string)
{
/* A. */
   char string[LENGTH], string2[LENGTH];
   FILE *file;
   int  i, line_counter = 0;

printf("\nWTOC> start");
/* B. */
   if((file = fopen(file_name, "wt")) == NULL){
      printf("\ntoc: Error opening table of contents file\n");
      exit(1);
   }  /* end of if */
printf(".");

/* C. */
   blank_line(file);
   line_counter++;
   blank_line(file);
   line_counter++;
   blank_line(file);
   line_counter++;
   fputs("                               Table of Contents\n", file);
   line_counter++;
   fputs("                                    for file\n", file);
   line_counter++;
   sprintf(string,"                                    %s\n",
           input_file_name);
   fputs(string, file);
   line_counter++;
   blank_line(file);
   line_counter++;
   sprintf(string,"                                    %s\n",
           time_string);
   fputs(string, file);
   line_counter++;
   blank_line(file);
   line_counter++;
   fputs("        Page - Contents\n", file);
   line_counter++;
printf(".");

/* D. */
   for(i=0; i<TITLES; i++){
      if(titles[i][0] != '\0'){
         strip_title(titles[i], string2);
         sprintf(string, "        %4d - %s\n",
                 titles_pages[i], string2);
         fputs(string, file);
         line_counter++;
      } /* end if titles */
   } /* end loop over i */
printf(".");

/* E. */
   for(i=line_counter; i<LPP-4; i++)
      blank_line(file);
printf(".");

/* F. */
   fclose(file);
printf(".");

printf("\nWTOC> the end");
} /* end of write_table_of_contents function */




/*PAGE replace_slash function

   replace_slash(char string[])

   This function takes a string and replaces each
   slash character with a back slash character.

*/

replace_slash(char string[])
{
   int  slash      = 47,
	     back_slash = 92,
		  i,
		  j;

   j = strlen(string);
   for(i=0; i<j; i++){
      if(string[i] == slash) string[i] = back_slash;
   }
}  /* end  of replace_slash function */



/*PAGE get_unique_file_name function

   get_unique_file_name(char *file_name)

   This function creates a unique file name and
   places it in the character string parameter.

   Loop until you have found the unique name.
   Use the random number generator and convert this to
      a string.
   See if a file exists having that name by trying to
      open it.

*/

get_unique_file_name(char *file_name)
{
	FILE *a_file;
	int still_looking = 1;

	while(still_looking){
		sprintf(file_name, "%d", rand());
      if((a_file = fopen(file_name, "rt")) == NULL)
			still_looking = 0;
		else
			fclose(a_file);
	}  /* end of while still_looking */
}  /* end of get_unique_file_name function */


/*PAGE new_page_marker function

   new_page_marker(char *string)

   This function compares the input string char array with
   the PAGE strings in the PAGES array defined at the top
   of this program.  If the string matches any of the
   PAGE strings in the PAGES array, it returns a 1,
   else it returns a 0.

*/


new_page_marker(char *string)
{
	int i            = 0, 
	    not_finished = 1, 
		 result       = 0;

	for(i=0; i<PAGE_TYPE && PAGES[i][0] != '\0'; i++){
      if( strncmp(PAGES[i], string, 5) == 0){
			result = 1;
			i      = PAGE_TYPE;
		}
	}
/***************
	while(not_finished){
      if( strncmp(PAGES[i], string, 5) == 0){
			not_finished = 0;
			result       = 1;
		}
		else{
			i++;
			if(i >= PAGE_TYPE)      not_finished = 0;
			if(PAGES[i][0] == '\0') not_finished = 0;
		}
	} **********/ /* ends of while not_finished */
	return(result);
}  /* end of new_page_marker function */
