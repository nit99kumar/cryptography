
#include <stdio.h>

#define YES 1
#define NO  0

main(argc, argv)
   int argc;
   char *argv[];
{
   int c, nl, nw, nc, ns, inword;

   FILE *input_file;

  if(argc < 2  ||  argc > 2){
     printf("\nusage: wc file\n");
	 exit(1);
  }

  if((input_file = fopen (argv[1], "r")) == NULL){
    printf("\nwc: error opening file %s\n", argv[1]);
    exit(2);
  }

  inword = NO;
  nl = ns = nw = nc = 0;

  while((c = fgetc(input_file)) != -1){
     ++nc;
	 if(c == '\n')
	    ++nl;
	 if( c == '.' || c == '?' || c == '!')
	    ++ns;
	 if( c == ' ' || c == '\n' || c == '\t')
	    inword = NO;
	 else if (inword == NO){
	    inword = YES;
		++nw;
	 }
  }

  fclose(input_file);

  printf("\n\n%d lines   %d sentences   %d words   %d characters\n", 
          nl, ns, nw, nc);
}  /* the end */
