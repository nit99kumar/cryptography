
       /*****************************************************
       *
       *       file d:\lsu\lzw.c
       *
       *       Functions: This file contains
       *
       *       Purpose:
       *          This is the beginning of lzw coding.
       *
       *       External Calls:
       *
       *       Modifications:
       *          1 December 1990 - started
       *
       ******************************************************/


#include "d:\cips\lzw.h"



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

   struct table_item string_table[TABLE_SIZE];

/* 
 * usage: lzw [-d] input output
 * where lzw original_file compressed_file
 * where lzw -d compressed_file full_size_file
 * 
 */

	printf("\nargc=%d", argc);

   if(argc < 3   ||   argc > 4){
		printf("\n\nusage: lzw [-d] input output\n\n");
      exit(1);
   }

       /*
          If there are four arguments then you are decompressing
          the compressed input file to a full size output file.
       */

   if(argc >= 4){
      strcpy(output_file_name, argv[2]);
      strcpy(input_file_name, argv[1]);
      in_file_desc = my_open(input_file_name);

      in_file_desc = my_open(input_file_name);
      if((displacement = lseek(in_file_desc, 0L, 2)) < 2){
         printf("\nLZW> ERROR - input file is empty");
         exit(0);
      }

      displacement = lseek(in_file_desc, 0L, 0);
      out_file_desc = my_open(output_file_name);
      initialize(string_table);
      perform_decompression(string_table, in_file_desc, out_file_desc);
      close(in_file_desc);
      close(out_file_desc);
   }

   else{   /* else you compress the full size input file and write
              out a compressed output file */

      strcpy(input_file_name, argv[1]);
      strcpy(output_file_name, argv[2]);

      in_file_desc = my_open(input_file_name);
      if((displacement = lseek(in_file_desc, 0L, 2)) < 2){
         printf("\nLZW> ERROR - input file is empty");
         exit(0);
      }

      displacement = lseek(in_file_desc, 0L, 0);
      out_file_desc = my_open(output_file_name);
      initialize(string_table);

/*****************
string_table[256].num = 97;
string_table[256].character = 'b';
string_table[257].num = 98;
string_table[257].character = 'a';

string_table[258].num = 256;
string_table[258].character = 'c';

for(s=256; s<259; s++)
 printf("\nTEST> table[%d].num=%d  .character=%c",
  s, string_table[s].num, string_table[s].character);

strcpy(r, "ab");
insert_into_table(r, 'c', string_table);
for(s=256; s<259; s++)
 printf("\nTEST> table[%d].num=%d  .character=%c",
  s, string_table[s].num, string_table[s].character);
read_string(r);
*****************/

      perform_compression(string_table, in_file_desc, out_file_desc);
read_string(r);
print_string_table(string_table);
      close(in_file_desc);
      close(out_file_desc);
   }  /* ends else compress input file to output file */


}  /* ends main  */




/* 0.0
        Set the first 256 items in the string table
        to the char number.

        Set the remaining items to the null char.
*/

initialize(string_table)
   struct table_item string_table[];
{
   int i;
   for(i=0; i<256; i++){
      string_table[i].num = 0;
      string_table[i].character = i;
   }

   for(i=256; i<TABLE_SIZE; i++){
      string_table[i].num = 0;
      string_table[i].character = '\0';
   }
}  /* ends initialize */



/* debug routine */
print_string_table(string_table)
   struct table_item string_table[];
{
   int s, k;
   printf("\n");
   s=256;
   k=1;
   while(k){
      if(string_table[s].num !=0 ){
         if((string_table[s].character > 33)  &&
            (string_table[s].character < 127))
            printf("\nTEST> table[%d]  %3d %c",
                    s, string_table[s].num, string_table[s].character);
         else
            printf("\nTEST> table[%d]  %3d %2d(d)",
                    s, string_table[s].num, string_table[s].character);
         s++;
      }
      else
         k = 0;
   }
}  /* ends print_string_table */




/*******************************************************

            C O M P R E S S I O N    C O D E

*******************************************************/



/* 2.0
*/

perform_compression(string_table, in_file_desc, out_file_desc)
   int    in_file_desc, out_file_desc;
   struct table_item string_table[];
{

   char   in_buffer[IB_LENGTH], k, w[100];

   int    bytes_read,
          bytes_written,
          coding,
          first_pass,
          i,
          in_counter,
          j,
          out_counter,
          still_reading;

   short  out_buffer[OB_LENGTH];

   for(i=0; i<OB_LENGTH; i++)
      out_buffer[i] = 0;

   in_counter    = 0;
   out_counter   = 0;
   first_pass    = 1;
   still_reading = 1;

   while(still_reading){

      bytes_read = my_read(in_file_desc, in_buffer, IB_LENGTH);
printf("\n2.0> read %d bytes", bytes_read);
      in_counter  = 0;
      if(bytes_read < IB_LENGTH)
         still_reading = 0;

         if(first_pass == 1){
            create_string(w, in_buffer[in_counter]);
            in_counter++;
            first_pass = 0;
         }

         coding = 1;
         while(coding){

                /* end of in_buffer and end of file
                   so code last string and quit */
            if( (in_counter >= bytes_read)   &&
                (still_reading == 0)){
               output_code_of(w, string_table, out_buffer,
                              &out_counter, out_file_desc);
               bytes_written = write_short_buffer(out_buffer,
                                         out_file_desc,
                                         &out_counter);
printf("\n2.0> wrote %d bytes", bytes_written);
               coding = 0;
            }

                /* end of in_buffer but not end of file */
            else{
               if( (in_counter >= bytes_read)   &&
                   (still_reading == 1)) {
                  coding     = 0;
                  in_counter = 0;
               }
               else{ /* else not end of in_buffer */
                  k = in_buffer[in_counter];
                  in_counter++;

                  if(is_in_string_table(w, k, string_table))
                     append_to_w(w, k);
                  else{
                     output_code_of(w, string_table, out_buffer,
                                    &out_counter, out_file_desc);
                        insert_into_table(w, k, string_table);
                        create_string(w, k);
                  }  /* ends else wk is not in string table */
               }  /* ends else not reached end of in_buffer */
            }  /* ends else end of in_buffer but not
                  end of file */
         }  /* ends while_coding */
   }  /* ends while still_reading */

}  /* ends perform_compression */




/* 2.1
         Take a character k and use it to create a
         1 character long string w.
*/

create_string(w, k)
   char w[], k;
{
   w[0] = k;
   w[1] = '\0';
}




/* 2.2
         This function finds the string w in the string_table
         and then writes w's num to the output buffer.
         If the output buffer is full, then write it out
         to the output file.

         2.2.1 - find string w in the string_table
         2.2.2 - ouput the num
*/

output_code_of(w, string_table, out_buffer,
               out_counter, out_file_desc)
   char    w[];
   int     *out_counter, out_file_desc;
   short   out_buffer[];
   struct  table_item string_table[];
{
   short n;

   find_string_w_in_table(w, string_table, &n);
   write_out_code(n, out_buffer, out_counter, out_file_desc);

}  /* ends output_code_of */





/* 2.2.1
          This function searches through the string_table
          to find the string w.  It returns n which is the
          num of the string w in the string_table.

          This is very similar to the function
          is_in_string_table.  This function searches
          for the string w and returns where it is in the
          table.  is_in_string_table searches for the
          string w appended by the character k and returns
          a 1 if found and a 0 if not found.
*/

find_string_w_in_table(w, string_table, n)
   char    w[];
   short   *n;
   struct  table_item string_table[];
{
   char w2[100], x;
   int  i, j, k, searching;
   w2[0]     = '\0';
   i         = TABLE_SIZE - 1;
   x = last_character_of(w);
   searching = 1;

   while(searching){

      if(x == string_table[i].character){
         insert_into_w(w2, x);
         build_string(w2, string_table[i].num, string_table);
         if(strcmp(w2, w) == 0){
            searching = 0;
            *n = i;
         }  /* ends if w2 == w */
         else{
            i--;
            w2[0] = '\0';
         }
      }  /* ends if found a match for x */

      else{ /* else keep searching for a match for x */
         i--;
         if(i < 0){
            printf("\n\n2.2.1> ERROR - did not find string in table\n");
            searching = 0;
            *n = 0;
         }  /* ends if i < 0 */
      }  /* ends else keep searching */
   }  /* ends while searching */

}  /* ends find_string_w_in_table */




/* 2.2.1.1
             This function returns the last non-null character
             in a string w.
*/

last_character_of(w)
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
}  /* ends last_character_of */






/* 2.2.2
           This function outputs the number code of a
           string.  It writes it to the out_buffer.
           If the out_buffer is full, it writes the
           buffer to the output file.
*/

write_out_code(n, out_buffer, out_counter, out_file_desc)
   int     *out_counter, out_file_desc;
   short   n, out_buffer[];
{
   int bytes_written, i;

   if(*out_counter >= OB_LENGTH){
      bytes_written = write_short_buffer(out_buffer,
                                         out_file_desc,
                                         out_counter);
printf("\n2.2.2> wrote %d bytes", bytes_written);
      *out_counter = 0;
      for(i=0; i<OB_LENGTH; i++)
         out_buffer[i] = 0;
   }  /* ends if out_buffer is full */

   out_buffer[*out_counter] = n;
   *out_counter = *out_counter + 1;
printf("\n2.2.2> outputed %3d - out counter = %3d", n, *out_counter);

}  /* ends write_out_code */



/* 2.2.2.1
          This function writes the out_buffer (an array
          of type short) to the output file.
*/

write_short_buffer(out_buffer, out_file_desc, out_counter)
   int    *out_counter, out_file_desc;
   short  out_buffer[];
{
        int     i, written;

        char    buffer[(sizeof(short))*OB_LENGTH],
                *charptr;

        charptr = (char *)out_buffer;
        for(i=0; i<((sizeof(short))*OB_LENGTH); i++)
           buffer[i] = *charptr++;

printf("\n\t2.2.2.1> counter=%d  desired size to output is %d",
 *out_counter, (*out_counter)*sizeof(short));
        written = my_write(out_file_desc, buffer,
                           (*out_counter)*sizeof(short));
        return(written);
}  /* ends write_short_buffer */





/* 2.3
         This function searches the string table to see
         if the string w with the character k appended
         to it is present.
         Look through the string table to see if k is the
         string_table[i].character.  If it is, see if the
         string_table[i].num points to a string that matches
         w.  If it does not, then keep searching until
         you reach string table entry 255 (the end of strings
         in the table all entries prior to 256 are single
         characters).

         find k
         build string w2
         if w == w2 then return result = 1
         if not, find k again
         if you cannot find k where w2 == w
            then return result = 0
*/

is_in_string_table(w, k, string_table)
   char   w[], k;
   struct table_item string_table[];
{
   int  i, result, searching;
   char w2[100];

   w2[0]     = '\0';
   result    = 0;
   i         = TABLE_SIZE - 1;
   searching = 1;

   while(searching){

      if(k == string_table[i].character){
         build_string(w2, string_table[i].num, string_table);
         if(strcmp(w, w2) == 0){
            result    = 1;
            searching = 0;
         }  /* ends if w2 == w */
         else{
            i--;
            w2[0] = '\0';
         }  /* ends else w != w2 */
      }  /* ends if found a match for k */

      else{ /* else keep searching for a match for k */
         i--;
         if(i < 0){
            /*printf("\n2.3> Did not find string %s character %c in table",
                   w, k);*/
/*print_string_table(string_table);*/
            searching = 0;
         }  /* ends if i < 0 */
      }  /* ends else keep searching for a match for k */

   }  /* ends while searching */

   return(result);

}  /* ends is_in_string_table */



/* 2.3.1
           This function takes the number from the string
           table and uses it to build a string w.  It
           calls itself as it moves up through the table
           inserting new characters into the front of
           the string.
*/

build_string(w, number, string_table)
   char   w[];
   short  number;
   struct table_item string_table[];
{
   if(string_table[number].character != '\0'){
      insert_into_w(w, string_table[number].character);
      if(string_table[number].num != 0)
         build_string(w, string_table[number].num, string_table);
   }
}  /* ends build_string */





/* 2.3.1.1
            This function inserts the character x into the
            front of string w.
*/

insert_into_w(w, x)
   char w[], x;
{
   char temp;
   int  i, j, not_finished;

      /* first find the end of the string w */
   i = 0;
   not_finished = 1;
   while(not_finished){
      if(w[i] == '\0')
         not_finished = 0;
      else
         i++;
   }  /* ends while not_finished */
   w[i+1] = '\0';
   for(j=i; j>0; j--)
      w[j] = w[j-1];
   w[0] = x;

}  /* ends insert_into_w */






/* 2.4
        This function appends the character k onto the
        end of the string w.
*/

append_to_w(w, k)
   char w[], k;
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
   w[i] = k;
   w[i+1] = '\0';

}  /* ends append_to_w */



/* 2.5
        This function inserts the string w and character
        k into the string table.

        Search the table from bottom up until you find
        the first open place.
        Set the character of that place to k.
        set the num of that place to the value returned
        by find_string_w_in_table.
*/

insert_into_table(w, k, string_table)
   char   w[], k;
   struct table_item string_table[];
{
   int   i, j, searching;
   short s;

   i         = TABLE_SIZE - 1;
   searching = 1;

   while(searching){
      if(string_table[i].character != '\0')
         searching = 0;
      else{
         i--;
         if(i < 0)
            printf("\n2.5> Table full - cannot insert");
      }
   }  /* ends while searching */

   find_string_w_in_table(w, string_table, &s);

   string_table[i+1].num       = s;
   string_table[i+1].character = k;
/*printf("\n2.5> inserted string %s and char %c into table", w,k);*/
/*print_string_table(string_table);*/
}  /* ends is_in_string_table */




/*******************************************************

          D E C O M P R E S S I O N    C O D E

*******************************************************/



/* 3.0
*/

perform_decompression(string_table, in_file_desc, out_file_desc)
   int    in_file_desc, out_file_desc;
   struct table_item string_table[];
{

   char   c,
          final_char,
          k,
          out_buffer[IB_LENGTH],
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
          in_buffer[OB_LENGTH],
          incode,
          oldcode,
          s;

   for(i=0; i<OB_LENGTH; i++)
      in_buffer[i] = 0;
   for(i=0; i<IB_LENGTH; i++)
      out_buffer[i] = '\0';

   stack_pointer = -1;
   in_counter    = 0;
   out_counter   = 0;
   still_reading = 1;

   while(still_reading){

      bytes_read = read_compressed_file(in_buffer, in_file_desc);
      if(bytes_read < OB_LENGTH*2)
         still_reading = 0;
/***********
printf("\n3.0> read %d bytes", bytes_read);
for(i=0; i<bytes_read/2; i++){
 if( (i%15) == 0) printf("\n3.0> ");
 printf("-%3d", in_buffer[i]);
}
************/

      code    = in_buffer[in_counter];
      in_counter++;
      oldcode = code;
      code_of(code, &s, &k, string_table);
      output_character(k, out_buffer, &out_counter, out_file_desc);
      final_char = k; /* step 4 in algorithm */

      decoding = 1;
      while(decoding){

             /* NEXT CODE */

             /* The next few if, else, if's are checking to
                see if the input compressed file is empty */

             /* end of in_buffer and end of file
                so decode last input and quit */
         if( (in_counter >= bytes_read/2)   &&
             (still_reading == 0)){
            bytes_written = my_write(out_file_desc, out_buffer,
                                     out_counter);
printf("\n2.0> wrote %d bytes", bytes_written);
            decoding = 0;
         }  /* ends if end of in_buffer and end of file */

            /* else end of in_buffer but not end of file
               so reset in_counter and go back and read
               more data */
         else{
            if( (in_counter >= bytes_read/2)   &&
                (still_reading == 1)){
               in_counter = 0;
               decoding   = 0;
            }  /* ends if end of in_buffer */

            else{ /* not end of in_buffer
                     we have input to process so let's
                     process it (step 5 in algorithm */

               code   = in_buffer[in_counter];
               in_counter++;
               incode = code;

               if(not_in_string_table(code, string_table)){
                  /* ??? output_character(final_char, out_buffer,
                                   &out_counter, out_file_desc);*/
                  push_onto_stack(final_char, stack, &stack_pointer);
                  code   = oldcode;
                  incode = new_code_of(oldcode, final_char, string_table);
               }  /* ends if not_in_string_table */

                  /* next_symbol looks at the code
                     if it is a num + char then it strips off
                     the chars and puts them on a stack until
                     they are all gone.
                  */

               next_symbol(&code, string_table,
                           stack, &stack_pointer);

                  /* code of CODE is a single character */
               code_of(code, &s, &k, string_table);
               if(s == 0){  /* if code of CODE is a single char */
                  output_character(k, out_buffer, &out_counter,
                                   out_file_desc);
                  final_char = k;

                     /* do while stack not empty */
                  while(stack_pointer > -1){
                     pop_off_of_stack(&c, stack, &stack_pointer);
                     output_character(c, out_buffer, &out_counter,
                                      out_file_desc);
                  }  /* ends while stack not empty */

                  put_num_char_in_string_table(oldcode, k,
                                            string_table);

                  oldcode = incode;
               }  /* ends else code of CODE is a single character */

            }  /* ends else not end of in_buffer */

         }  /* ends else end of in_buffer
               but not end of file */

      }  /* ends while decoding */

   }  /* ends while still_reading */

}  /* ends perform_decompression */



/* 3.1
         This function reads the type short from the
         compressed file.
*/

read_compressed_file(in_buffer, in_file_desc)
   short in_buffer[];
   int   in_file_desc;
{
   int i, bytes_read;
   union {
           char  bytes[OB_LENGTH*2];
           short nums[OB_LENGTH];
         }  buffer;

   for(i=0; i<OB_LENGTH*2; i++)
      buffer.bytes[i] = '\0';

   bytes_read = my_read(in_file_desc, buffer.bytes, OB_LENGTH*2);
   /*printf("\n3.1> Read %d bytes using file header", bytes_read);*/

   for(i=0; i<bytes_read/2; i++){
      in_buffer[i] = buffer.nums[i];
      /*printf("\n3.1> read short [%3d] = %3d", i, buffer.nums[i]);*/
   }
   return(bytes_read);
}




/* 3.2
        This function looks in the string table and sets
        the char x and short s to the values in the
        table at location = code.
        x = string_table[code].character
        s = string_table[code].num
*/

code_of(code, s, x, string_table)
   char   *x;
   short  code, *s;
   struct table_item string_table[];
{
   *x = string_table[code].character;
   *s = string_table[code].num;
}  /* ends code_of */









/* 3.3
        This function outputs the character k.  It puts it
        in the output buffer.  If the output buffer is full
        it writes the buffer to the output file.
*/
output_character(k, out_buffer, out_counter, out_file_desc)
   char k, out_buffer[];
   int  *out_counter, out_file_desc;
{
   int bytes_written;

   if(*out_counter >= IB_LENGTH){
      bytes_written = my_write(out_file_desc, out_buffer, IB_LENGTH);
printf("\n3.3> wrote %d bytes", bytes_written);
      *out_counter = 0;
   }

   out_buffer[*out_counter] = k;
   *out_counter = *out_counter + 1;

}  /* ends output_character */





/* 3.4
        This function looks in the string table
        at location code to see if it exists as a
        valid code, i.e. if its character is
        non-null.  If the code does not exist
        it returns a 1.  If the code does exist
        it returns a 0.
*/
not_in_string_table(code, string_table)
   short  code;
   struct table_item string_table[];
{
   int i, result;
   result = 0;
   if(string_table[code].character == '\0')
      result = 1;
   return(result);
}  /* ends not_in_string_table */





/* 3.5
       Return the place
       of this new code to incode.
       Do not insert the char and oldcode
       into the table.
*/

new_code_of(oldcode, final_char, string_table)
   char  final_char;
   short oldcode;
   struct table_item string_table[];
{
   int i, new_code, searching;

   searching = 1;
   i = TABLE_SIZE - 1;
   while(searching){
      if(string_table[i].character != '\0')
         searching = 0;
      else
         i--;
   }

   new_code = i+1;

   printf("\n3.5> inserted into table, new code=%d", new_code);
   return(new_code);
}  /* ends new_code_of */




/* 3.6
        This function looks at code in the string
        table to see if it is a single character
        or a num + char.  If it is a num + char
        it pushes the char onto the stack, sets code
        to the num, and calls itself recursively.
*/

next_symbol(code, string_table, stack, stack_pointer)
   char   stack[], *stack_pointer;
   short  *code;
   struct table_item string_table[];
{
   short code2, s;
   char  c;

   code2 = *code;

   code_of(code2, &s, &c, string_table);
   if(s != 0){  /* if code of CODE is a num + char
                   push char onto stack */
      push_onto_stack(c, stack, stack_pointer);
        /* code = code_of code.num
           if s > 255 then it will be
           another num + char so call next_symbol
           recursively */
      if(s > 255){
         /*code_of(s, code, &c, string_table);
           next_symbol(code, string_table, stack, stack_pointer);*/
           *code = s;
           next_symbol(code, string_table, stack, stack_pointer);
      }
      else /* else s is not a num + char so set the
              code to s so next_symbol will not return
              a code of zero by mistake */
         *code = s;
   }  /* ends if s != 0 */

}  /* ends next_symbol */




        /* NOTE: The stack in this program is an array of
                 char.  Initially, stack_pointer = -1.
                 stack_pointer points to the "top of"
                 the stack.

                 To push onto stack just
                 increment stack_pointer then
                 set stack[stack_pointer] = x.

                 To pop off stack just set
                 x = stack[stack_pointer] and decrement
                 the stack_pointer.

                 The stack is empty when
                 stack_pointer == -1
        */

/* 3.7
        This function pushes the character x down onto
        a stack.
*/

push_onto_stack(x, stack, stack_pointer)
   char stack[], x;
   int  *stack_pointer;
{
   *stack_pointer = *stack_pointer + 1;
   stack[*stack_pointer] = x;
}  /* ends push_onto_stack */




/* 3.8
        This function pops an element off the top of the
        stack and sends it back to the calling funtion.
*/

pop_off_of_stack(x, stack, stack_pointer)
   char stack[], *x;
   int  *stack_pointer;
{
   *x = stack[*stack_pointer];
   *stack_pointer = *stack_pointer - 1;
}  /* ends pop_off_of_stack */



/* 3.9
        This function puts the num and char into the
        string table.
*/

put_num_char_in_string_table(n, c, string_table)
   char   c;
   short  n;
   struct table_item string_table[];
{
   int i, searching;

   searching = 1;
   i = TABLE_SIZE - 1;
   while(searching){
      if(string_table[i].character != '\0')
         searching = 0;
      else
         i--;
   }

   string_table[i+1].num       = n;
   string_table[i+1].character = c;

}  /* ends put_num_char_in_string_table */
