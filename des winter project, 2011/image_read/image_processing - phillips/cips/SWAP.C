#include <stdio.h>
#define LENGTH 80

main(int argc, char *argv[])
{
   char c1[LENGTH],
        c2[LENGTH],
        rep[LENGTH],
        string[LENGTH],
        string2[LENGTH];

   FILE  *input_file;
   FILE  *output_file;

   if(argc < 3 || argc > 3){
      printf("\nusage: swap in-file out-file");
      return(1);
   }

   if((input_file = fopen(argv[1], "rt")) == NULL){
      printf("\ntoc: Error opening input file %s\n", argv[1]);
      exit(3);
   }

   if((output_file = fopen(argv[2], "wt")) == NULL){
      printf("\ntoc: Error opening output file %s\n", argv[2]);
      exit(3);
   }

   while(fgets(string, LENGTH, input_file)){
      sscanf(string, "%s %s", c1, c2);
      sprintf(string2, "\n%-15s %s", c2, c1);
      fputs(string2, output_file);
   }

   fclose(input_file);
   fclose(output_file);

}  /* the end */

