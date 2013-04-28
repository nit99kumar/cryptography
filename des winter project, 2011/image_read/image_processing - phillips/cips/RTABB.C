

/*
   rtabb.c

   This program removes the TAB characters from a text file,
   substitutes the desired amount of spaces for each TAB,
   and writes the result back to the input file. 
   
   usage:  rtabb input_file output_file spaces

   If you do not enter spaces, 
   then rtabb uses 5 spaces for each TAB.

   Dwayne Phillips
   31 August 1991
   You may use this code for non-commercial purposes

*/



#include <stdio.h>

main(argc, argv)
  int argc;
  char *argv[];
{
  char  string[80];
  FILE *input_file;
  FILE *output_file;
  int spaces;

  if(argc < 2){
    printf("\nrtabb: no file specified");
    printf("\nrtabb: correct usage is:");
    printf("\n\trtabb input_file spaces\n");
    exit (1);
  }


  if((input_file = fopen (argv[1], "rt")) == NULL){
    printf("\nrtabb: error opening file %s\n", argv[1]);
    exit(1);
   }

  if((output_file = fopen ("((((", "wt")) == NULL){
    printf("\nrtabb: error opening file ((((\n");
    exit(1);
  }

  if (argc < 3)  
     spaces = 5;
  else
     spaces = string_to_number(argv[2]);

  if(spaces <= 0){
        printf("\nrtabb: error - spaces must be a positive number\n");
        exit(1);
  }

  remove_tab_file(input_file, output_file, spaces); 

  fclose(input_file);
  fclose(output_file);

  unlink(argv[1]);
  sprintf(string, "copy (((( %s", argv[1]);
  system(string);
  unlink("((((");

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
