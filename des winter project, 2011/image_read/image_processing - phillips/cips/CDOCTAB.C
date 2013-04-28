

 /*
 *	 cdoctab.c
 *
 *       Dwayne Phillips
 *       6 August 1986
 *
 *
 *	 Created the cdoctab program to improve the output of
 *	 CDOC. This inserts 5 blanks on most lines.
 */


#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <dos.h>

#define TAB_SPACE    5    /* number of spaces to insert in a line */
#define MAX_STRING 255
#define BLANK      ' '
#define EOL        '\n'
#define NEWLINE    '\n'


main (argc, argv)
  int argc;           
  char *argv[];       
 {
  FILE *input_file;   
  FILE *output_file;  
  FILE *fopen ();

  if (argc < 2)       
   {
    printf ("tab: no file specified\n");
    exit (1);
   }

  /* see if the files can be opened successfully and, if so, tab
   * the lines contained in the input file and print them either
   * to stdout or an output file. if either file cannot be opened
   * print an error message and exit to the operating system.
   */

  if ((input_file = fopen (argv[1], "rt")) == NULL){
    printf ("cdoctab: error opening file %s\n", argv[1]);
    exit (1);
   }

  if (argc < 3)           /* tab file_name */
    output_file = stdout;

  else if (strings_are_equal (argv[1], argv[2]))
   {
    printf ("cdoctab: input and output files must differ\n");
    exit (1);
   }

  else if ((output_file = fopen (argv[2], "wt")) == NULL)
   {
    printf ("cdoctab: error opening file %s\n", argv[2]);
    exit (1);
   }

  tab_file (input_file, output_file, argv[1]);

  fclose (input_file);

  if (argc == 3)         /* if an output file was opened, close it */
    fclose (output_file);
 }

 tab_file (input_file, output_file, name_of_input_file)
   FILE *input_file;
   FILE *output_file;
   char *name_of_input_file;
  {
   char string [MAX_STRING];  /* buffer for the string read
                                 from the input file */
   int  i, line_counter, page_counter;

   while (fgets (string, MAX_STRING, input_file))
     {

       pad_string (string, TAB_SPACE);  /*****************************
                                        * This is the number of
                                        * spaces inserted at the left
                                        * side of the page.
                                        ******************************/
       fwriteln (output_file, string);

     }    /* ends while fgets           */

}    /* ends tab_file  */



 pad_string (string, num_blanks)
   char string[];
   int num_blanks;
  {
   int index;
   int count;

   char temp[MAX_STRING];


   if( (string[0] != 'C')   &&
       (string[0] != '*')   &&
       (string[0] != '-')   &&
       (string[0] != '_')){
          for (count = 0; count < num_blanks; count++)
             temp[count] = BLANK;

          temp[count] = NULL;
   }
   else
      temp[0] = NULL;

   append_string (string, temp);

   copy_string (temp, string);
  }

 append_string (string1, string2)
   char string1[];
   char string2[];
  {
   int index1;
   int index2;

   for (index2 = 0; string2[index2] != NULL; index2++)
     ;

   for (index1 = 0;(string2[index2] = string1[index1])!= NULL; ++index1)
       ++index2;   /* also increment string2 */
   }


 strings_are_equal (string1, string2)
   char string1[];
   char string2[];
  {
   int index;

   for (index = 0; string1[index] == string2[index]; index++)
     if (string1[index] == NULL)
        return (1);
     
   return (0); 
  }


 copy_string (string1, string2)
   char string1[];  /* string to be copied */
   char string2[];
  {
   int index;
   for (index = 0; (string2[index] = string1[index]) != NULL; index++)
       ;
  }


 fwriteln (file_pointer, string)
  char *string;
  FILE *file_pointer;  /* requires stdio.h be #included */
 {

  while (*string != EOL && *string != NULL)
    putc (*string++, file_pointer);

  putc (NEWLINE, file_pointer);

 }
