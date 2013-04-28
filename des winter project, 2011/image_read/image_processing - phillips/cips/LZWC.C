
/*****************************************************
 *
 * File: lzwc.c
 * Author: Dwayne Phillips
 * Compiler: Microsoft C 6.0
 * Functions: append_to_end
 *            build_string
 *            compress
 *            find_string
 *            insert_in_front
 *            insert_into_table
 *            is_present
 *            last
 *            output_the_code
 *            set_character_to_string
 *            short_output
 *            write_code
 *
 * Purpose: Compression routines for LZW data compression and
 *          decompression program.
 *
 * Date: October 1991
 *
******************************************************/


#include "d:\cips\lzwinc.h"


/*****************************************************
 *
 * compress(..
 *
 * This is the main routine for the compression
 * process.
 *
 * A.  Read input from file.
 *
 * B.  If this is the first character read
 *     in the file, set the string w to the
 *     character k.
 *
 * C.  If this is the end of the input file and the
 *     end of the input buffer, code the last
 *     character and quit.
 *
 * D.  If this is the end of the input buffer, but
 *     not the end of the file, continue compressing.
 *
 * E.  If wk is in the string table, then set
 *     w = wk and go back to read the next character
 *     k.
 *
 * F.  Else, wk is not in the string table, so
 *     output the code of string w,
 *     put wk in the string table,
 *     set the string w = the character k,
 *     and go back and read the next character k.
 *
******************************************************/


compress(string_table, in_file_desc, out_file_desc)
   int    in_file_desc, out_file_desc;
   struct item string_table[];
{

   char   in_buffer[LENGTH1], k, w[100];

   int    bytes_read,
          bytes_written,
          coding,
          first_pass,
          i,
          in_counter,
          j,
          out_counter,
          still_reading;

   short  out_buffer[LENGTH2];

   for(i=0; i<LENGTH2; i++)
      out_buffer[i] = 0;

   in_counter    = 0;
   out_counter   = 0;
   first_pass    = 1;
   still_reading = 1;

   while(still_reading){
/* A. */
      bytes_read = read(in_file_desc, in_buffer, LENGTH1);

      LZWTEST( printf("\nLZW: TEST: read %d bytes", bytes_read);)

      in_counter  = 0;
      if(bytes_read < LENGTH1)
         still_reading = 0;

/* B. */
         if(first_pass == 1){
            set_character_to_string(w, in_buffer[in_counter]);
            in_counter++;
            first_pass = 0;
         }

         coding = 1;
         while(coding){

/* C. */
            if( (in_counter >= bytes_read)   &&
                (still_reading == 0)){
               output_the_code(w, string_table, out_buffer,
                              &out_counter, out_file_desc);
               bytes_written = short_output(out_buffer,
                                         out_file_desc,
                                         &out_counter);
               LZWTEST( printf("\nLZW: TEST: wrote %d bytes",
                         bytes_written); )
               coding = 0;
            }

/* D. */
            else{
               if( (in_counter >= bytes_read)   &&
                   (still_reading == 1)) {
                  coding     = 0;
                  in_counter = 0;
               }
               else{ /* else not end of in buffer */
                  k = in_buffer[in_counter];
                  in_counter++;

                  printf(" .");

/* E. */
                  if(is_present(w, k, string_table))
                     append_to_end(w, k);
/* F. */
                  else{
                     output_the_code(w, string_table, out_buffer,
                                    &out_counter, out_file_desc);
                     insert_into_table(w, k, string_table);
                     set_character_to_string(w, k);
                  }  /* ends else wk is not in string table */
               }  /* ends else not reached end of in_buffer */
            }  /* ends else end of in_buffer but not
                  end of file */
         }  /* ends while_coding */
   }  /* ends while still_reading */

}  /* ends compress */


/*****************************************************
 *
 * set_character_to_string(..
 *
 * Take a character k and use it to create a
 * 1 character long string w.
 *
*****************************************************/

set_character_to_string(w, k)
   char w[], k;
{
   w[0] = k;
   w[1] = '\0';
}



/*****************************************************
 *
 * output_the_code(..
 *
 * A.  Find the string w in the string_table.
 *
 * B.  Write the code (number) representing
 *     w to the output buffer.
 *
*****************************************************/
output_the_code(w, string_table, out_buffer,
                out_counter, out_file_desc)
   char    w[];
   int     *out_counter, out_file_desc;
   short   out_buffer[];
   struct  item string_table[];
{
   short n;
/* A. */
   find_string(w, string_table, &n);
/* B. */
   write_code(n, out_buffer, out_counter, out_file_desc);
}


/*****************************************************
 *
 * find_string(...
 *
 * Search for the string w in the string table.
 *
 * A.  Set x = last character of string w.
 *
 * B.  Search through the string table.
 *
 * C.  If x = character in string table
 *     then build up that string w2 and compare it
 *     to w.  If they match you found w in the
 *     table.
 *
 * D.  If x != character in string table
 *     then keep looking.
 *
*****************************************************/

find_string(w, string_table, n)
   char    w[];
   short   *n;
   struct  item string_table[];
{
   char w2[100], x;
   int  i, j, k, searching;

   w2[0]     = '\0';
   i         = TABLE - 1;

/* A. */
   x = last(w);
   searching = 1;

/* B. */
   while(searching){

/* C. */
      if(x == string_table[i].code_char){
         insert_in_front(w2, x);
         build_string(w2, string_table[i].code_num, string_table);
         if(strcmp(w2, w) == 0){
            searching = 0;
            *n = i;
         }  /* ends if w2 == w */
         else{
            i--;
            w2[0] = '\0';
         }
      }

/* D. */
      else{
         i--;
         if(i < 0){
            printf("\n\nLZW: ERROR - did not find ",
                   "string in table\n");
            searching = 0;
            *n = 0;
         }
      }

   }

}  /* ends find_string */




/*****************************************************
 *
 * last(...
 *
 * Return the last character of the string w.
 *
*****************************************************/
last(w)
   char w[];
{
   int i, searching;
   i = 0;
   searching = 1;
   while(searching){
      if(w[i] == '\0')
         searching = 0;
      else
         i++;
   }
   return(w[i-1]);
}


/*****************************************************
 *
 * write_code(...
 *
 * Write the number code of the string.
 *
 * A.  If the output buffer is full, then write
 *     it to the output file.
 *
 * B.  If the output buffer is not full, then
 *     put the number in the output buffer.
 *
*****************************************************/

write_code(n, out_buffer, out_counter, out_file_desc)
   int     *out_counter, out_file_desc;
   short   n, out_buffer[];
{
   int bytes_written, i;

/* A. */
   if(*out_counter >= LENGTH2){
      bytes_written = short_output(out_buffer,
                                         out_file_desc,
                                         out_counter);
      LZWTEST( printf("\nLZW: TEST: wrote %d bytes", bytes_written);)
      *out_counter = 0;
      for(i=0; i<LENGTH2; i++)
         out_buffer[i] = 0;
   }
/* B. */
   out_buffer[*out_counter] = n;
   *out_counter = *out_counter + 1;
   LZWTEST( printf("\nLZW: TEST: outputed %3d - out counter = %3d",
            n, *out_counter); )

}




/*****************************************************
 *
 * short_output(...
 *
 * Take an array of shorts, condense each 16
 * bit short down to 12 bits and pack these
 * into a char array for storage to disk.
 *
 * bit zero is the rightmost bit 0000 0001
 *
 * To set a bit you OR it with a 1.  e.g.
 * to set bit 3 you OR with 0000 0100
 *
 * To clear a bit you AND it with a 0.
 * e.g. to clear bit 3 - AND with 1111 1011
 *
 * A.  Based on the ith short and the condensing
 *     by 3/4th's, calculate the byte in the
 *     char buffer and the bit in that byte.
 * 
 * B.  Loop over the first 8 bits of the 16 bit short.
 * 
 * C.  Test the bit.
 * 
 * D.  If it is 1, set the corresponding bit in the
 *     byte in the output buffer.
 * 
 * E.  If it is 0, clear the corresponding bit in the
 *     byte in the output buffer.
 * 
 * F.  Repeat steps C., D., and E. for the next 4
 *     bits of the 16 bit short.
 * 
 * G.  Write the buffer to disk.
 * 
*****************************************************/


short_output(out_buffer, out_file_desc, out_counter)
   int    *out_counter, out_file_desc;
   short  out_buffer[];
{
   int     bit_in_byte, byte_in_buffer,
           i, ii, j, written;

   char    buff3[LENGTH3],
           mask,
           temp_byte,
           temp_char;

   union   sb{
      short n;
      char  c[sizeof(short)];
   }  u;

/* A. */
   for(i=0; i<*out_counter; i++){
      ii = i * 2;
      byte_in_buffer = (ii * 3)/4;
      bit_in_byte    = (ii * 3) % 4;
      if(bit_in_byte == 2) bit_in_byte = 4;
      u.n = out_buffer[i];

/* B. */
      for(j=0; j<8; j++){
         temp_char = u.c[0];
         temp_char = temp_char >> j;
/* C. */
         if((temp_char & 0x1) != 0){ /* 1 bit */
/* D. */
            mask = 0x01;
            mask = mask << bit_in_byte;
            temp_byte = buff3[byte_in_buffer];
            temp_byte = temp_byte | mask;
            buff3[byte_in_buffer] = temp_byte;
         } /* ends set bit */
/* E. */
         else{ /* 0 bit */
            mask = 0x01;
            mask = mask << bit_in_byte;
            mask = ~mask;
            temp_byte = buff3[byte_in_buffer];
            temp_byte = temp_byte & mask;
            buff3[byte_in_buffer] = temp_byte;
         }  /* ends clear bit */

         bit_in_byte++;
         if(bit_in_byte > 7){
             bit_in_byte = bit_in_byte % 8;
             byte_in_buffer++;
         }
      }  /* ends j loop over 8 */

/* F. */
      for(j=0; j<4; j++){
         temp_char = u.c[1];
         temp_char = temp_char >> j;
         if((temp_char & 0x1) != 0){
            mask = 0x01;
            mask = mask << bit_in_byte;
            temp_byte = buff3[byte_in_buffer];
            temp_byte = temp_byte | mask;
            buff3[byte_in_buffer] = temp_byte;
         }

         else{
            mask = 0x01;
            mask = mask << bit_in_byte;
            mask = ~mask;
            temp_byte = buff3[byte_in_buffer];
            temp_byte = temp_byte & mask;
            buff3[byte_in_buffer] = temp_byte;
         }

         bit_in_byte++;
         if(bit_in_byte > 7){
             bit_in_byte = bit_in_byte % 8;
             byte_in_buffer++;
         }
      }
   }  /* ends i loop over output_counter */

/* G. */
   written = write(out_file_desc, buff3, byte_in_buffer);

   LZWTEST( printf("\nLZW: TEST: wrote out %d bytes", 
                    written); )

   return(written);
}




/*****************************************************
 *
 * is_present(...
 *
 * Is the string w with character k appended to it
 * in the string table?
 *
 * A.  Search the string table from the end for
 *     the character k.
 *
 * B.  If k = character in string table
 *     then build up that string w2 and compare it
 *     to w.  If they match you found w in the
 *     table.
 *
 * C.  If k != character in string table
 *     then keep looking.
 *
*****************************************************/

is_present(w, k, string_table)
   char   w[], k;
   struct item string_table[];
{
   int  i, result, searching;
   char w2[100];

   w2[0]     = '\0';
   result    = 0;
   i         = TABLE - 1;
   searching = 1;

/* A. */
   while(searching){

/* B. */
      if(k == string_table[i].code_char){
         build_string(w2, string_table[i].code_num, string_table);
         if(strcmp(w, w2) == 0){
            result    = 1;
            searching = 0;
         }  /* ends if w2 == w */
         else{
            i--;
            w2[0] = '\0';
         }  /* ends else w != w2 */
      }  /* ends if found a match for k */

/* C. */
      else{
         i--;
         if(i < 0){
            searching = 0;
         }
      }
   }
   return(result);
}




/*****************************************************
 *
 * build_string(...
 *
 * Use the number representing a string and build
 * the string of characters.
 *
 * A.  If there is a character in the string table,
 *     then put that character into the front of
 *     the string you are building.
 *
 * B.  If that string points to another place in the
 *     string table, then recusively call yourself
 *     to continue building the string.
 *
*****************************************************/
build_string(w, number, string_table)
   char   w[];
   short  number;
   struct item string_table[];
{

/* A. */
   if(string_table[number].code_char != '\0'){
      insert_in_front(w, string_table[number].code_char);

/* B. */
      if(string_table[number].code_num != 0)
         build_string(w,
                      string_table[number].code_num,
                      string_table);
   }
}



/*****************************************************
 *
 * insert_in_front(...
 *
 * Insert character x into the front of string w.
 *
 * A.  Find the end of the string w.
 *
 * B.  Shift all of w over 1.
 *
 * C.  Put x at the front of w.
 *
*****************************************************/

insert_in_front(w, x)
   char w[], x;
{
   char temp;
   int  i, j, not_finished;

/* A. */
   i = 0;
   not_finished = 1;
   while(not_finished){
      if(w[i] == '\0')
         not_finished = 0;
      else
         i++;
   }

/* B. */
   w[i+1] = '\0';
   for(j=i; j>0; j--)
      w[j] = w[j-1];

/* C. */
   w[0] = x;
}



/*****************************************************
 *
 * append_to_end(...
 *
 * Append the character k onto the end of string w.
 *
 * A.  Find the end of w.
 *
 * B.  Put k onto the end.
 *
*****************************************************/

append_to_end(w, k)
   char w[], k;
{
   int i, searching;
   i = 0;

/* A. */
   searching = 1;
   while(searching){
      if(w[i] == '\0')
         searching = 0;
      else
         i++;
   }

/* B. */
   w[i] = k;
   w[i+1] = '\0';
}


/*****************************************************
 *
 * insert_into_table
 *
 * Insert the string w and character k into the string
 * table.
 *
 * A.  Find the first open place in the string table.
 *
 * B.  Find the number s in the string table which
 *     represents the string w.
 *
 * C.  In the first open place in the string table
 *     set the character to k and the number to s.
 *
*****************************************************/

insert_into_table(w, k, string_table)
   char   w[], k;
   struct item string_table[];
{
   int   i, j, searching;
   short s;

   i         = TABLE - 1;
   searching = 1;

/* A. */
   while(searching){
      if(string_table[i].code_char != '\0')
         searching = 0;
      else{
         i--;
         if(i < 0)
            printf("\nLZW: ERROR-  Table full ",
                   "- cannot insert");
      }
   }

/* B. */
   find_string(w, string_table, &s);

/* C. */
   string_table[i+1].code_num  = s;
   string_table[i+1].code_char = k;
   LZWTEST( printf("\nLZW: TEST: inserted string %d",s);)
   LZWTEST( printf(" and char %c into table", k); )
}
