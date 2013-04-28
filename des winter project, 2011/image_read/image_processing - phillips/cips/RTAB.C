

/*
   rtab.c

   This program removes the TAB characters from a text file,
   substitutes the desired amount of spaces for each TAB,
   and writes the result to either the screen or another 
   file.

   usage:  rtab input_file output_file spaces

   If you do not enter an output_file,
   then the output is sent to the screen.

   If you do not enter spaces, 
   then rtab uses 5 spaces for each TAB.

   Dwayne Phillips
   2 May 1991
   You may use this code for non-commercial purposes

*/



#include "d:\c600\include\stdio.h"

main(argc, argv)
  int argc;
  char *argv[];
{
  FILE *input_file;
  FILE *output_file;
  int spaces;

  if(argc < 2){
    printf("\nrtab: no file specified");
    printf("\nrtab: correct usage is:");
    printf("\n\trtab input_file output_file spaces\n");
    exit (1);
  }


  if((input_file = fopen (argv[1], "rt")) == NULL){
    printf("\nrtab: error opening file %s\n", argv[1]);
    exit(1);
   }

  if(argc < 3)
     output_file = stdout;
  else 
    if(strcmp(argv[1], argv[2]) == 0){
      printf("\nrtab: input and output files must different\n");
      exit(1);
    }

  else if((output_file = fopen (argv[2], "wt")) == NULL){
    printf("\nrtab: error opening file %s\n", argv[2]);
    exit(1);
  }

  if (argc < 4)  
     spaces = 5;
  else
     spaces = string_to_number(argv[3]);

  if(spaces <= 0){
        printf("\nrtab: error - spaces must be a positive number\n");
        exit(1);
  }

  remove_tab_file(input_file, output_file, spaces); 
  fclose(input_file);
  if(argc == 3)
    fclose(output_file);

}  /* ends main */



remove_tab_file (input_file, output_file, spaces)
   FILE *input_file;
   FILE *output_file;
   int  spaces;
{
   char outs[300];
   char string[100];
   int  i, j, k;

   while(fgets(string, 100, input_file)){
         k = 0;
         for(i=0; string[i] != '\n' && string[i] != '\0'; i++){
              if(string[i] == 0x09){
                  for(j=0; j<spaces; j++){
                      outs[k] = ' ';
                      k++;
                  }  /* ends loop over j */
              }  /* ends if string[i] = tab */
              else{
                    outs[k] = string[i];
                    k++;
              }  /* ends else */
         }  /* ends loop over i and string[i] */

         outs[k] = '\0';
         fwrite_line(output_file, outs);
         clear_out_buffer(string, 100);
         clear_out_buffer(outs, 300);

     }  /* ends while fgets */

}  /* ends remove_tab_file */



clear_out_buffer(string, n)
        char string[];
        int n;
{
     int i;
     for(i=0; i<n; i++)
            string[i] = ' ';
     string[0] = '\0';
}



string_to_number(string)
   char string[];
{
   int i, sign, result;

   sign   = 1;
   result = 0;
   i      = 0;

   while(string[i] != '\0'){
      result = result*10 + (string[i] - '0');
      i++;
   }

   result = result * sign;
   return(result);
}  /* ends value_of_string */





fwrite_line(file_pointer, string)
    char *string;
    FILE *file_pointer;
{

    while (*string != '\n' && *string != '\0')
      putc (*string++, file_pointer);
    putc ('\n', file_pointer);
}
