

/**************************************************/
/*PAGE Includes and Definitions

  A. Include file definitions
  B. Define constants
  C. Define the possible page breaks
     You modify the program to include new page break
     types at that point.  The PAGES array will hold
     up to PAGE_TYPE types of markers.  If this is not
     enough, increase the value of the PAGE_TYPE 
     constant.
  D. Declare the function prototypes.

*/

/* A. */
#include <time.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

/* B. */
#define BLANK      ' '
#define LENGTH      80
#define LPP         66
#define MAX_STRING 255
#define PAGE_TYPE   10
#define TAB_SPACE    5
#define FOOT_SPACE  10
#define FOOTER       4
#define DEBUG


/* C. */
char PAGES[PAGE_TYPE][LENGTH] = {
      "CPAGE",    /* FORTRAN page marker           */
      "#PAGE",    /* C Script (UNIX) page marker   */
      "/*PAGE",   /* C page marker                 */
      "//PAGE",   /* C++ page marker               */
                  /* add new types of page         */
                  /* markers here                  */
     };

/* D. */
void paginate_file();
void write_3_line_header();
void blank_line();
int  contains_page_marker();
void pad_string();
void make_new_page();
void strip_title();
void write_table_of_contents();
void start_toc_file();
void write_title_to_toc_file();
void end_toc_page();
/**************************************************/
/*PAGE main */
/*
              toc file
            /           \
           /              ----------
 input file               ---------- output file
           \              ----------
            \          /
              paged file

*/

main(int argc, char *argv[])
{
   char paged_file_name[LENGTH],
        string[LENGTH],
        time_string[LENGTH],
        *temp_name,
        toc_file_name[LENGTH];

   FILE  *input_file,
         *output_file,
         *paged_file,
         *toc_file;

   int   result;

   struct tm *time_of_day;

   time_t seconds;

      /* get the temporary file names */
   temp_name = malloc(LENGTH);
   temp_name = tmpnam(temp_name);
   strcpy(toc_file_name, temp_name);
   temp_name = tmpnam(temp_name);
   strcpy(paged_file_name, temp_name);
   free(temp_name);
 
   time(&seconds);
   time_of_day = localtime(&seconds);
   sprintf(time_string, "%d-%d-%d\0",
           time_of_day->tm_mon+1,
           time_of_day->tm_mday,
           time_of_day->tm_year);
/**************************************************/
/*PAGE Interpret Command Line and Open Files

  A. If less than 2 or more than 4 arguments, quit
  B. If 3 arguments, ensure the input and output file
        names are different.
  C. Open the input file - if unsucessful, quit
  D. Open the paged_file (a temporary file)

*/

/* A. */
   if(argc < 2  ||  argc > 3){
      printf(
      "\ntoc: usage: toc input_file [output_file]");
      exit(1);
   }

/* B. */
   if(argc == 3){
      if((result = strcmp(argv[1], argv[2])) == 0){
          printf(
"\ntoc: input and output files must be different\n");
          exit(2);
      }
   }

/* C. */
   if((input_file = fopen(argv[1], "rt")) == NULL){
      printf(
    "\ntoc: Error opening input file %s\n", argv[1]);
      exit(3);
   }

/* D. */
#ifdef DEBUG
printf("\nthe paged file name is %s", paged_file_name);
#endif
   if((
   paged_file = fopen(paged_file_name, "wt")) == NULL){
      printf("\ntoc: Error opening paged file\n");
      fclose(input_file);
      exit(4);
   }

   if((toc_file = fopen(toc_file_name, "wt")) == NULL){
      printf("\ntoc: Error opening toc file\n");
      fclose(input_file);
      fclose(paged_file);
      exit(5);
   }
/**************************************************/
/*PAGE Paginate, Write TOC, Concatinate

  A. Paginate the input file.  This reads the input 
        file, paginates it, writes it to a temporary 
        file, and builds the table of contents.
  B. Close the files.
  C. Combine the table of contents and the paginated 
        file into the output file.
     D. Open the temporary files.
     E. Copy the lines from the temporary files to the
          output file.
     F. Close all files.
     G. Delete the temporary files.
*/

/* A. */
   paginate_file(input_file, paged_file, toc_file,
                 argv[1]);
   printf("\n\npaginated file");

/* B. */
   fclose(input_file);
   fclose(paged_file);
   fclose(toc_file);

/* C. */
   if(argc == 3){
      if((output_file = fopen(argv[2], "wt")) == NULL){
         printf("\ntoc: Error opening output file\n");
         exit(1);
      }
   }  /* ends of if argc == 3 */
   else
      output_file = stdout;

/* D. */
   if((toc_file = fopen(toc_file_name, "rt")) == NULL){
      printf(
      "\ntoc: Error opening table of contents file\n");
      exit(1);
   }

   if((
   paged_file = fopen(paged_file_name, "rt")) == NULL){
      printf("\ntoc: Error opening paged file\n");
      exit(1);
   }
   printf("\n\nopened all files for final output");

/* E. */
   while(fgets(string, MAX_STRING, toc_file)){
      fputs(string, output_file);
   }
   while(fgets(string, MAX_STRING, paged_file)){
      fputs(string, output_file);
   }
   printf("\n\ncopied all output");

/* F. */
   if(argc == 3)
      fclose(output_file);
   fclose(paged_file);
   fclose(toc_file);

/* G. */
   if(remove(paged_file_name) == -1)
    printf(
    "\ntoc: ERROR could not delete temp paged file %s",
              paged_file_name);
   if(remove(toc_file_name) == -1)
    printf(
    "\ntoc: ERROR could not delete temp toc file %s",
              toc_file_name);
   return(1);
}  /* end of main program */
/**************************************************/
/*PAGE paginate_file function

  paginate_file(...

  This function breaks the input file into pages 
  and inserts spaces at the left edge of the page.
  This makes it easier to punch holes in the page 
  for a 3 ring binder.

  A. Declare variables.
  B. Read the date and time and put them into 
     the time_string.
  C. Print the header for the first page.
  D. Loop through the input file.
        E. Look for the PAGE comments.
        F. If this is the first PAGE, record this in 
	      the table of contents but do not make 
	      an extra page.
        G. If you hit PAGE, then make a new page and
              and entry into the table of contents.
        H. Tab over the input line and write it to 
	      the output file.
        I. If the page is full, make a new page.
  J. Fill up the end of the last page with blank lines.

*/

void paginate_file(FILE *input_file, 
                   FILE *output_file,
                   FILE *toc_file,
                   char *name_of_input_file)
{
/* A. */
   char   string[MAX_STRING], time_string[LENGTH];
   int    i,
          first_page       = 1,
          line_counter     = 0,
          page_counter     = 1,
          toc_line_counter = 0;
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
      if(contains_page_marker(string)){

/* F. */
         if(first_page == 1){
            first_page = 0;
            start_toc_file(toc_file, 
                           name_of_input_file,
                           time_string,
                           string,
                           &toc_line_counter,
                           1);
         }  /* ends of if first_page == 1 */
         else{

/* G. */
            make_new_page(input_file, output_file,
                          name_of_input_file, 
			  time_string,
                          &page_counter, 
			  &line_counter);
            write_title_to_toc_file(toc_file,
                                    name_of_input_file,
                                    time_string,
                                    string,
                                    &toc_line_counter,
                                    page_counter);
                                    
         } /* ends of else first_page = 0 */
      }  /* end of if page */

/* H. */
      pad_string(string, TAB_SPACE);
      fputs(string, output_file);
      ++line_counter;

/* I. */
      if(line_counter >= LPP-FOOT_SPACE)
         make_new_page(input_file, output_file,
                       name_of_input_file, time_string,
                       &page_counter, &line_counter);

   }  /* end of while fgets */

/* J. */
   for(i=line_counter; i<=LPP; i++)
      blank_line(output_file);
   end_toc_page(toc_file, &toc_line_counter);

} /* end of paginate_file function */
/**************************************************/
/*PAGE pad_string function

  pad_string(...

  This function inserts spaces into the beginning 
  (left side) of a string.

  A. Declare variables.
  B. Fill a temporary string with num_blanks spaces.
  C. Concatenate input string onto temporary string.
  D. Copy temporary string back to input string.

*/

void pad_string(char string[], int num_blanks)
{
/* A. */
   int   count;
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
/**************************************************/
/*PAGE make_new_page function

  make_new_page(...

  This function fills the remainder of a page with 
  blank lines and prints a header at the top of 
  the next page.

  A. Declare variables.
  B. Loop until the page runs out of lines.
  C. If you reach the end of a page, then
        D. Print FOOTER blank lines, jump to the 
           next page, and print a page header.

*/

void make_new_page(FILE *input_file, 
                   FILE *output_file,
                   char *name_of_input_file, 
                   char *time_string,
                   int *page_counter, 
                   int *line_counter)
{
/* A. */
   int  i, not_finished = 1;

/* B. */
   while(not_finished){

/* C. */
      if(*line_counter >= LPP - FOOT_SPACE){

/* D. */
         not_finished = 0;
         for(i=0; i<FOOTER; i++) 
            blank_line(output_file);
         *line_counter = 0;
         *page_counter = *page_counter + 1;
         blank_line(output_file);
         *line_counter = *line_counter + 1;
         write_3_line_header(output_file, 
                             name_of_input_file,
                             time_string, 
                             page_counter);
         *line_counter = *line_counter + 3;
         blank_line(output_file);
         *line_counter = *line_counter + 1;
      }  /* end of if line counter >= LPP - 10 */
      fputs("     \n", output_file);
      *line_counter = *line_counter + 1;
   }  /* end of while not_finished */
}  /* end of make_new_page function */
/**************************************************/
/*PAGE strip_title function

  strip_title(...

  This function takes an array (title) which has 
  the PAGE comment followed by a title and returns 
  an array (string) with only the title remaining.
  It strips out the PAGE comment and leaves the title.
  For example, 
     title = CPAGE This is a title
  the result,  
     string = This is a title

  A. Declare variables.
  B. Strip the first word out of the title array
        and put the result into the string array.
  C. Remove any special comment characters which may be
        at the end of the string array.

*/

void strip_title(char title[], char string[])
{
/* A. */
   int  i, n, second_word, space = ' ';
   char *first_space;

/* B. */
   first_space = strchr(title, space);
   second_word = first_space - title + 1;

   for(i=second_word; i<LENGTH; i++){
      string[i-second_word] = title[i];
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
/**************************************************/
/*PAGE write_3_line_header function

  write_3_line_header(...

  This function writes a three line header to the 
  top of a new page.  The header contains the file 
  name, date, and page number.

  A. Declare variables.
  B. Fill strings with blanks.
  C. Fill strings with information and send to
        the output file.

*/

void write_3_line_header(FILE *file, 
                         char *file_name,
                         char *time_string, 
                         int *page_number)
{
/* A. */
   char b1[LENGTH], b2[LENGTH], 
        b3[LENGTH], string[LENGTH];
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
/**************************************************/
/*PAGE blank_line function
 
  blank_line(...
  
  This function sends a blank line to a file.
*/

void blank_line(FILE *file)
{
   fputs("          \n", file);
} /* end of blank_line function */
/**************************************************/
/*PAGE contains_page_marker function

   contains_page_marker(...

   This function compares the input string char 
   array with the PAGE strings in the PAGES array 
   defined at the top of this program.  If the string 
   matches any of the PAGE strings in the PAGES array,
   it returns a 1, else it returns a 0.

*/

int contains_page_marker(char *string)
{
   int i      = 0, 
       result = 0;

   for(i=0; i<PAGE_TYPE && PAGES[i][0] != '\0'; i++){
      if( strncmp(PAGES[i], string, 5) == 0){
         result = 1;
         i      = PAGE_TYPE;
      }
   }

   return(result);
}  /* end of contains_page_marker function */
/**************************************************/
/*PAGE start_toc_file function

   start_toc_file(...

   This function sends the text that appears at the
   top of every table of contents page to the
   table of contents file.
*/

void start_toc_file(FILE *toc_file, 
                    char *name_of_input_file,
                    char time_string[],
                    char title_line[],
                    int  *toc_line_counter,
                    int  page_number)
{
   char a_string[LENGTH],
        stripped_title[LENGTH];

   blank_line(toc_file);
   *toc_line_counter = *toc_line_counter + 1;
   blank_line(toc_file);
   *toc_line_counter = *toc_line_counter + 1;
   blank_line(toc_file);
   *toc_line_counter = *toc_line_counter + 1;

   fputs(
"                               Table of Contents\n", 
      toc_file);
   *toc_line_counter = *toc_line_counter + 1;
   fputs(
"                                    for file\n", 
      toc_file);
   *toc_line_counter = *toc_line_counter + 1;

   sprintf(a_string,
      "                                    %s\n",
      name_of_input_file);
   fputs(a_string, toc_file);
   *toc_line_counter = *toc_line_counter + 1;
   blank_line(toc_file);
   *toc_line_counter = *toc_line_counter + 1;
   sprintf(a_string,
   "                                    %s\n",
           time_string);
   fputs(a_string, toc_file);
   *toc_line_counter = *toc_line_counter + 1;
   blank_line(toc_file);
   *toc_line_counter = *toc_line_counter + 1;
   fputs("        Page - Contents\n", toc_file);
   *toc_line_counter = *toc_line_counter + 1;

   strip_title(title_line, stripped_title);
   sprintf(a_string, "        %4d - %s\n",
           page_number, stripped_title);
   fputs(a_string, toc_file);
   *toc_line_counter = *toc_line_counter + 1;

}  /* end of start_toc_file function */
/**************************************************/
/*PAGE write_title_to_toc_file function

   write_title_to_toc_file(...

   This function writes one line of table of
   contents information to the table of contents
   file.

*/

void write_title_to_toc_file(FILE *toc_file, 
                    char *name_of_input_file,
                    char time_string[],
                    char title_line[],
                    int  *toc_line_counter,
                    int  page_number)
{
   char a_string[LENGTH],
        stripped_title[LENGTH];
   int  i;

   strip_title(title_line, stripped_title);
   sprintf(a_string, "        %4d - %s\n",
           page_number, stripped_title);
   fputs(a_string, toc_file);
   *toc_line_counter = *toc_line_counter + 1;

   if(*toc_line_counter >= LPP-10){
      *toc_line_counter = 0;
      for(i=0; i<FOOTER; i++) blank_line(toc_file);
      start_toc_file(toc_file, 
                     name_of_input_file,
                     time_string,
                     title_line,
                     toc_line_counter,
                     page_number);
   }

}  /* end of write_title_to_toc_file function */
/**************************************************/
/*PAGE end_toc_page function

   end_toc_page(...
   
   This function finished off the bottom of the
   table of contents page.
*/

void end_toc_page(FILE *toc_file, 
                  int  *toc_line_counter)
{
   int i;

   for(i=*toc_line_counter; i<LPP; i++)
      blank_line(toc_file);
}  /* end of end_toc_page function */
