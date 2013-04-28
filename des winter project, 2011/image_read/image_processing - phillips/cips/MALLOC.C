

#include "d:\cips\cips.h"


/*
   let's try to malloc some structs
*/


#define GND '\0'

struct trans_struct
{
   char employee[30];
   char date[8];
   char number[5];
   char dept[8];
};

struct trans_type
{
   struct trans_struct data;
   struct trans_type *pointer;
};


main()
{
   char response[30];
   int i, j, k;
   struct trans_struct transaction;
   struct trans_type *head, *p;
   struct trans_type *list();


   head = GND;
   while(response[0] != 'x'){
       printf("\nEnter a name (enter x to quit)\n\t-->");
       read_string(response);
       if(response[0] != 'x'){
          strcpy(transaction.employee, response);
          head = list(head, &transaction);
          listlist(head);
       }
   }  /* ends while response */

   while(head != GND){
      printf("\n\tname is --%s--", head->data.employee);
         head = head->pointer;
      }

}  /* ends main */



struct trans_type *list(p, t)
    struct trans_type *p;
    struct trans_struct *t;
{
    struct trans_type *pp;

        /* if p points to GND then the list does not
         * contain any information.  Therefore, create
         * an element of the list and put it between
         * p and GND
         */
    if(p == GND){
        p = (struct trans_type *) malloc(sizeof(struct trans_type));
        strcpy(p->data.employee, t->employee);
        p->pointer = GND;
    }  /* ends if pointer == GND */

        /* else there are elements in the list and
         * insert the new element at the proper location.
         */
    else{
        if(strcmp(p->data.employee, t->employee) < 0){
            p->pointer = list(p->pointer, t);
        }
        else{
           pp = (struct trans_type *) malloc(sizeof(struct trans_type));
           strcpy(pp->data.employee, t->employee);
           pp->pointer = p;
           p = pp;
        }

    }  /* ends else */

    return(p);
}  /* ends list */








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



listlist(h)
    struct trans_type *h;
{
    struct trans_type *x;

    x = h;
    while(x != GND){
       printf("\n\tname is --%s--", x->data.employee);
       x = x->pointer;
    }
}
