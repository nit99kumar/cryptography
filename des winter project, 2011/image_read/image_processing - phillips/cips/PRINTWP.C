
#include <stdio.h>

#define NEWPAGE 0x0C

main(argc, argv)
   int argc;
   char *argv[];
{
   int c, print = 1, reading = 1;
   FILE *input_file, *output_file;

  if(argc < 3  ||  argc > 3){
     printf("\nusage: printwp wp-file-name odd-or-even\n");
	 exit(1);
  }

  if((input_file = fopen (argv[1], "rb")) == NULL){
    printf("\nprintwp: error opening file %s\n", argv[1]);
    exit(1);
  }

  if((output_file = fopen ("prn", "wb")) == NULL){
    printf("\nprintwp: error opening printer \n");
    exit(1);
  }

     /* if even is specified, skip the first page */
  if(argv[2][0] == 'e' || argv[2][0] == 'E') toggle(&print);

  while(reading){
     c = fgetc(input_file);
	 if(print)        fputc(c, output_file);
	 if(c == NEWPAGE) toggle(&print);
	 if(c == -1)      reading = 0;
  } /* ends while reading */

  fclose(input_file);
  fclose(output_file);

}  /* the end */

toggle(int *a)
{
   if(*a == 1) 
      *a = 0;
   else        
      *a = 1;
}
