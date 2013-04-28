

/*
	tab program

   usage:  tab input_file #_of_spaces [output_file]

   replace tab by any number of spaces
   if no output file given, send to screen
*/



#include <stdio.h>

main(argc, argv)
  int argc;
  char *argv[];
{
  FILE *input_file;
  FILE *output_file;
  int spaces;

  if(argc < 2   ||   argc > 4){
    printf("\ntab: correct usage is:"
           " tab input_file #_of_spaces [output_file]\n");
    exit (1);
  }


  if((input_file = fopen (argv[1], "rt")) == NULL){
    printf("\ntab: error opening file %s\n", argv[1]);
    exit(1);
   }

  if(argc < 4)
     output_file = stdout;
  else
    if(strcmp(argv[1], argv[3]) == 0){
      printf("\ntab: input and output files must different\n");
      exit(1);
    }

  	 else if((output_file = fopen (argv[3], "wt")) == NULL){
    	      printf("\ntab: error opening file %s\n", argv[3]);
    		   exit(1);
  	 }

  spaces = atoi(argv[2]);

  remove_tab_file(input_file, output_file, spaces);
  fclose(input_file);
  if(argc == 4)
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

         outs[k] = '\n';
			outs[k+1] = '\0';
         fputs(outs, output_file);
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
