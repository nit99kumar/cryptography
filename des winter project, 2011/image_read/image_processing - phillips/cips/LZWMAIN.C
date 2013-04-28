

/*****************************************************
 *
 * File: lzwmain.c
 * Author: Dwayne Phillips
 * Compiler: Microsoft C 6.0
 * Functions: main
 *
 * Purpose: Main routine for LZW data compression and
 *          decompression program.
 *
 * Procedure:
 *   usage: lzw [-d] input output
 *   where for compression you enter
 *   lzw normal_file compressed_file
 *   and for decompression you enter
 *   lzw -d compressed_file normal_file
 *
 *   A. Read in the command line arguments.
 *   B. If too many or too few arguments, quit.
 *   C. If argc == 4 and argv[1] == -d, decompress.
 *   D. If argc == 3, compress.
 *
 * Date: October 1991
 *
******************************************************/


#include "d:\cips\lzwinc.h"



main(argc, argv)
   int  argc;
   char *argv[];
{
   char  input_file_name[80],
         output_file_name[80],
         r[80];

   int i,
       in_file_desc,
       j,
       out_file_desc;

   long displacement;

   short s;

   struct item string_table[TABLE];

/* B. */
   if(argc < 3   ||   argc > 4){
      printf("\n\nusage: lzw [-d] input output\n\n");
      exit(1);
   }

/* C. */
   if(argc == 4   &&
      (strcmp(argv[1], "-d")) == 0){
      strcpy(output_file_name, argv[3]);
      strcpy(input_file_name, argv[2]);
      in_file_desc = open(input_file_name,
                          O_RDWR | O_CREAT | O_BINARY,
                          S_IWRITE);
      if((displacement = lseek(in_file_desc, 0L, 2)) < 2){
         printf("\nLZW: ERROR - input file is empty");
         exit(0);
      }

      displacement = lseek(in_file_desc, 0L, 0);
      out_file_desc = open(output_file_name,
                           O_RDWR | O_CREAT | O_BINARY,
                           S_IWRITE);
      setup(string_table);
      printf("\nLZW: Decompressing %s --> %s",
             input_file_name, output_file_name);
      decompress(string_table, in_file_desc, out_file_desc);
      close(in_file_desc);
      close(out_file_desc);
   }

/* D. */
   if(argc == 3){
      strcpy(input_file_name, argv[1]);
      strcpy(output_file_name, argv[2]);

      in_file_desc = open(input_file_name,
                          O_RDWR | O_CREAT | O_BINARY,
                          S_IWRITE);
      if((displacement = lseek(in_file_desc, 0L, 2)) < 2){
         printf("\nLZW: ERROR - input file is empty");
         exit(0);
      }

      displacement = lseek(in_file_desc, 0L, 0);
      out_file_desc = open(output_file_name,
                           O_RDWR | O_CREAT | O_BINARY,
                           S_IWRITE);
      setup(string_table);

      printf("\nLZW: Compressing %s --> %s",
             input_file_name, output_file_name);

      compress(string_table, in_file_desc, out_file_desc);

      LZWTEST( printf("\nLZW: TEST: Hit Enter to continue"); )
      LZWTEST( gets(r); )
      LZWTEST( display_table(string_table); )
      close(in_file_desc);
      close(out_file_desc);
   }

}
