
#include "d:\cips\cips.h"


main()
{
	 int file_desc, i, j;
	 char buffer[100];

	 for(i=0; i<100; i++)
			buffer[i] = ' ';

	j = 1;
	while(j == 1){

   file_desc = open("a:\dummy",
		    O_CREAT | O_RDWR | O_BINARY,
		    S_IREAD | S_IWRITE);
   printf("\nCAT> file desc=%d", file_desc);

	 for(i=0; i<10; i++){
			my_write(file_desc, buffer, 100);
	 }

	 printf("\n\tfinished writing");

	close(file_desc);

	 }

}




read_string(string)
        char *string;
{
        int     eof,
                letter,
                no_error;

        eof = -1;
        no_error = 0;

        while((letter = getchar()) != '\n' &&
               letter !=  eof)
           *string++ = letter;

        *string = '\0';

        return((letter == eof) ? eof : no_error);

}       /* ends read_string */



clear_buffer(string)
   char string[];
{
   int i;
   for(i=0; i<MAX_NAME_LENGTH; i++)
      string[i] = ' ';
}



long_clear_buffer(string)
   char string[];
{
   int i;
   for(i=0; i<300; i++)
      string[i] = ' ';
}

       /*****************************************************
       *
       *       file c:\lsu\mrw.c
       *
       *       Functions: This file contains
       *           my_read
       *           my_write
       *
       *       Purpose: These two functions call the Turbo C
       *           functions _read and _write. All software
       *           will use my_read and my_write so that if
       *           the software is ported to another system
       *           that uses read and write changing the
       *           two functions in this file will take care
       *           of the move.
       *
       *       External Call:
       *           read
       *           _write
       *
       *       Modifications:
       *           10 June 1987 - created
       *
       *
       ************************************************************/


my_read(file_descriptor, buffer, number_of_bytes)
   int  file_descriptor, number_of_bytes;
   char *buffer;
{
   int bytes_read;
   int read();
   bytes_read = read(file_descriptor, buffer, number_of_bytes);
   return(bytes_read);
}


my_write(file_descriptor, buffer, number_of_bytes)
   int  file_descriptor, number_of_bytes;
   char *buffer;
{
   int bytes_written;
   int write();
   bytes_written = write(file_descriptor, buffer, number_of_bytes);
   return(bytes_written);
}


           /******************************************************
           *
           *       file d:\cips\mof.c
           *
           *       Functions: This file contains
           *          my_open
           *
           *       Purpose: This function opens a file. Borland's
           *          Turbo C opens files a little different from
           *          the standard UNIX C. Instead of using this
           *          different method in a number of various files,
           *          the method is placed in this one file. If the
           *          programs are moved to another system, all changes
           *          will be located in this one place.
           *
           *       External Calls:
           *          none
           *
           *       Modifications:
           *          18 June 1987 - created
           *
           ****************************************************/



my_open(file_name)
   char file_name[];
{
   int file_descriptor;

   file_descriptor = open(file_name, O_RDWR | O_CREAT | O_BINARY,
                          S_IWRITE);

   return(file_descriptor);


}  /* ends my_open  */
