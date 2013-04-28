
/*****************************************************
 *
 * File: lzwdc.c
 * Author: Dwayne Phillips
 * Compiler: Microsoft C 6.0
 * Functions: code_of
 *            decompress
 *            is_not_present
 *            next_code
 *            next_symbol
 *            pop
 *            push
 *            put_in_table
 *            short_input
 *            write_character_k
 *
 * Purpose: Decompression routines for LZW data
 *          compression and decompression program.
 *
 * Date: October 1991
 *
******************************************************/


#include "d:\cips\lzwinc.h"


/*****************************************************
 *
 * decompress(..
 *
 * This is the main routine for the decompression
 * process.
 *
 * A.  Clear the buffers and set counters.
 *
 * B.  Read from the compressed file.
 *
 * C.  code = oldcode = first input code
 *
 * D.  Decode code and set character k.
 *
 * E.  Write out character k and set
 *     final character = k
 *
 * F.  NEXT CODE section of algorithm.
 *
 * G.  Input buffer and the compressed file
 *     are both empty so output the last
 *     character and quit.
 *
 * H.  Input buffer is empty, but the compressed
 *     file is not, so read more from the
 *     compressed file.
 *
 * I.  Process compressed data.  Set
 *     code   = next input code
 *     incode = code
 *
 * J.  If the code is not in the string table, then
 *     output the final character,
 *     code   = oldcode,
 *     incode = new code of the oldcode and
 *              the final character.
 *
 * K.  next symbol looks at the code.  If it is a
 *     number + symbol, then it strips off the characters
 *     and puts them on the stack until the string
 *     is gone.  Then the code of code is a single
 *     character.
 *
 * L.  If the code of code is a single character, then
 *     output the character k and set final character = k.
 *
 * M.  While there are characters on the stack, pop the
 *     top character off of the stack and output it.
 *
 * N.  Put the oldcode (number) and k (character) into
 *     the string table, and set oldcode = incode.
 *     Go back for more input.
 *
******************************************************/



decompress(string_table, in_file_desc, out_file_desc)
   int    in_file_desc, out_file_desc;
   struct item string_table[];
{

   char   c,
          final_char,
          k,
          out_buffer[LENGTH1],
          special_char,
          stack[100],
          w[100];

   int    bytes_read,
          bytes_written,
          decoding,
          i,
          in_counter,
          j,
          out_counter,
          special_case,
          stack_pointer,
          still_reading;

   short  code,
          in_buffer[LENGTH2],
          incode,
          oldcode,
          s;

/* A. */
   for(i=0; i<LENGTH2; i++)
      in_buffer[i] = 0;
   for(i=0; i<LENGTH1; i++)
      out_buffer[i] = '\0';

   stack_pointer = -1;
   in_counter    = 0;
   out_counter   = 0;
   still_reading = 1;

/* B. */
   while(still_reading){

      bytes_read = short_input(in_buffer, in_file_desc);
      if(bytes_read < LENGTH2*2)
         still_reading = 0;
      LZWTEST( printf("\nLZW: TEST: read %d bytes", bytes_read);)

/* C. */
      code    = in_buffer[in_counter];
      oldcode = code;
      in_counter++;

/* D. */
      code_of(code, &s, &k, string_table);

/* E. */
      write_character_k(k, out_buffer,
                       &out_counter,
                       out_file_desc);
      final_char = k;

/* F. - NEXT CODE */
      decoding = 1;
      while(decoding){

/* G. */
         if( (in_counter >= bytes_read/2)   &&
             (still_reading == 0)){
            bytes_written = write(out_file_desc, out_buffer,
                                  out_counter);
            LZWTEST( printf("\nLZW: TEST: wrote"); )
            LZWTEST( printf(" %d bytes", bytes_written); )
            decoding = 0;
         }

/* H. */
         else{
            if( (in_counter >= bytes_read/2)   &&
                (still_reading == 1)){
               in_counter = 0;
               decoding   = 0;
            }

/* I. */
            else{
               code   = in_buffer[in_counter];
               incode = code;
               in_counter++;

               printf(" .");

/* J. */
               if(is_not_present(code, string_table)){
                  /* ??? write_character_k(final_char, out_buffer,
                                   &out_counter, out_file_desc);*/
                  push(final_char, stack, &stack_pointer);
                  code   = oldcode;
                  incode = next_code(oldcode,
                                       final_char,
                                       string_table);
               }

/* K. */
               next_symbol(&code, string_table,
                           stack, &stack_pointer);

               code_of(code, &s, &k, string_table);

/* L. */
               if(s == 0){
                  write_character_k(k, out_buffer, &out_counter,
                                   out_file_desc);
                  final_char = k;

/* M. */
                  while(stack_pointer > -1){
                     pop(&c, stack, &stack_pointer);
                     write_character_k(c, out_buffer,
                                      &out_counter,
                                      out_file_desc);
                  }

/* N. */
                  put_in_table(oldcode, k, string_table);

                  oldcode = incode;
               }  /* ends else code of CODE is a single character */

            }  /* ends else not end of in_buffer */

         }  /* ends else end of in_buffer
               but not end of file */

      }  /* ends while decoding */

   }  /* ends while still_reading */

}



/*****************************************************
 *
 * short_input(...
 *
 * Read the compressed file and put the data into
 * an array of shorts.  The compressed file has 12
 * bits to represent each 16 bit short, so you
 * must unpack these bits.
 * 
 * A.  Read the packed bits from disk.
 * 
 * B.  Loop over the input buffer.
 * 
 * C.  Loop over 12 bits at a time to unpack them
 *     into 16 bits.
 * 
 * D.  Test the bit.
 * 
 * E.  If it is 1, set the corresponding bit in
 *     the short-char union.
 * 
 * F.  If it is 0, clear the corresponding bit in
 *     the short-char union.
 * 
 * G.  Put the unpacked short into the short array.
 *
*****************************************************/

short_input(in_buffer, in_file_desc)
   short in_buffer[];
   int   in_file_desc;
{
   char  buff3[LENGTH3], mask, temp_char;
   int   bit_in_byte, byte_in_buffer, bytes_read, i, j;
   union sb{
      short n;
      char  c[sizeof(short)];
   }  u;

   i              = 0;
   bit_in_byte    = 0;
   byte_in_buffer = 0;

/* A. */
   bytes_read     = read(in_file_desc,
                         buff3, LENGTH3);
   LZWTEST( printf("\nLZW: TEST: read %d bytes",
                    bytes_read); )

/* B. */
   while(byte_in_buffer <= bytes_read){
      u.c[0] = 0x00;
      u.c[1] = 0x00;

/* C. */
      for(j=0; j<12; j++){
         temp_char = buff3[byte_in_buffer];
         temp_char = temp_char >> bit_in_byte;

/* D. */
         if((temp_char & 0x1) != 0){

/* E. */
            mask = 0x01;
            mask = mask << (j%8);
            if(j <= 7)
               u.c[0] = u.c[0] | mask;
            else
               u.c[1] = u.c[1] | mask;
         }

/* F. */
         else{
            mask = 0x01;
            mask = mask << (j%8);
            mask = ~mask;
            if(j <= 7)
               u.c[0] = u.c[0] & mask;
            else
               u.c[1] = u.c[1] & mask;
         }
         bit_in_byte++;
         if(bit_in_byte > 7){
             bit_in_byte = bit_in_byte % 8;
             byte_in_buffer++;
         }
      } /* j loop over 12 */

/* G. */
      in_buffer[i] = u.n;
      i++;
   }  /* ends while */

   return((bytes_read*4)/3);
}



/*****************************************************
 *
 * code_of(...
 *
 * Set the character x and the short s to the values
 * found in table location code.
 *
*****************************************************/

code_of(code, s, x, string_table)
   char   *x;
   short  code, *s;
   struct item string_table[];
{
   *x = string_table[code].code_char;
   *s = string_table[code].code_num;
}  /* ends code_of */




/*****************************************************
 *
 * write_character_k(...
 *
 * A.  If the output buffer is full, write it to the
 *     output or non-compressed file.
 *
 * B.  Write the character k to the output buffer.
 *
*****************************************************/

write_character_k(k, out_buffer, out_counter, out_file_desc)
   char k, out_buffer[];
   int  *out_counter, out_file_desc;
{
   int bytes_written;

/* A. */
   if(*out_counter >= LENGTH1){
      bytes_written = write(out_file_desc,
                            out_buffer,
                            LENGTH1);
      LZWTEST( printf("\nLZW: TEST: wrote %d bytes",
                 bytes_written); )
      *out_counter = 0;
   }

/* B. */
   out_buffer[*out_counter] = k;
   *out_counter = *out_counter + 1;
}



/*****************************************************
 *
 * is_not_present(...
 *
 * Look at string table location "code."  If the
 * character is null, then the code is not in
 * the table so return a 1.
 *
*****************************************************/

is_not_present(code, string_table)
   short  code;
   struct item string_table[];
{
   int i, result;
   result = 0;
   if(string_table[code].code_char == '\0')
      result = 1;
   return(result);
}




/*****************************************************
 *
 * next_code(...
 *
 * Find the place in the table where you would
 * insert the next code.  Do not insert.
 *
 * A.  Search from the end of the string table.  Stop
 *     when you hit the first occupied place.
 *
 * B.  Return the place just greater than the first
 *     occupied place.
 *
*****************************************************/

next_code(oldcode, final_char, string_table)
   char  final_char;
   short oldcode;
   struct item string_table[];
{
   int i, new_code, searching;

/* A. */
   searching = 1;
   i = TABLE - 1;
   while(searching){
      if(string_table[i].code_char != '\0')
         searching = 0;
      else
         i--;
   }

/* B. */
   new_code = i+1;
   LZWTEST( printf("\nLZW: TEST: inserted into table");)
   LZWTEST( printf(" new_code = %d", new_code); )
   return(new_code);
}




/*****************************************************
 *
 * next_symbol(...
 *
 * Strip the characters off a string and push
 * them onto the stack.
 *
 * A.  Get the code of the input code.
 *
 * B.  If the code is a number + character (string)
 *     then push the character onto the stack
 *     and call next_symbol recursively.
 *
 * C.  Else, set code to s and quit.
 *
*****************************************************/

next_symbol(code, string_table, stack, stack_pointer)
   char   stack[], *stack_pointer;
   short  *code;
   struct item string_table[];
{
   short code2, s;
   char  c;

/* A. */
   code2 = *code;
   code_of(code2, &s, &c, string_table);

/* B. */
   if(s != 0){
      push(c, stack, stack_pointer);
      if(s > 255){
           *code = s;
           next_symbol(code, string_table, stack,
                       stack_pointer);
      }

/* C. */
      else
         *code = s;
   }  /* ends if s != 0 */

}




/*****************************************************
 *
 * put_in_table(...
 *
 * Put the number and character into the first
 * open place in the string table.
 *
 * A.  Search from the end of the table until
 *     you find the first occupied place.
 *
 * B.  Put the number and character into the
 *     place next to the occupied place.
 *
*****************************************************/

put_in_table(n, c, string_table)
   char   c;
   short  n;
   struct item string_table[];
{
   int i, searching;

/* A. */
   searching = 1;
   i = TABLE - 1;
   while(searching){
      if(string_table[i].code_char != '\0')
         searching = 0;
      else
         i--;
   }

/* B. */
   string_table[i+1].code_num       = n;
   string_table[i+1].code_char = c;

}



/*****************************************************
 *
 *  The stack in this program is an array of
 *  char.  Initially, stack_pointer = -1.
 *  stack_pointer points to the "top of"
 *  the stack.
 *
 *  To push onto stack just
 *  increment stack_pointer then
 *  set stack[stack_pointer] = x.
 *
 *  To pop off stack just set
 *  x = stack[stack_pointer] and decrement
 *  the stack_pointer.
 *
 * The stack is empty when
 *  stack_pointer == -1
 *
*****************************************************/




/*****************************************************
 *
 * push(...
 *
*****************************************************/

push(x, stack, stack_pointer)
   char stack[], x;
   int  *stack_pointer;
{
   *stack_pointer = *stack_pointer + 1;
   stack[*stack_pointer] = x;
}



/*****************************************************
 *
 * pop(...
 *
*****************************************************/

pop(x, stack, stack_pointer)
   char stack[], *x;
   int  *stack_pointer;
{
   *x = stack[*stack_pointer];
   *stack_pointer = *stack_pointer - 1;
}
