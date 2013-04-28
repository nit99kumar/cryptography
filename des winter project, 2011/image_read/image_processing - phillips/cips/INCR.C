

#include <stdlib.h>
#include <stdio.h>


main(argc, argv)
   int  argc;
   char *argv[];
{
   char *e, env[80], string[80];

   printf("\nThe environment variable is %s", argv[1]);
   e = getenv(argv[1]);
	printf("\nThe value of the variable is %s", e);

   sprintf(string, " %s=new+value", argv[1]);
	printf("\nCommand string is --%s--", string);
	putenv(string);

   return("new+value");
}  /* ends main */
