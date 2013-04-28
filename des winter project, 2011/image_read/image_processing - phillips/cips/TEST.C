
#include "cips.h"

struct stacks{
   short         x;
   short         y;
   struct stacks *next;
};

struct stacks *stack;


main(argc, argv)
   int argc;
   char *argv[];
{
   short x, y;

   stack = NULL;
   stack->next = NULL;
printf("\nstack is now%x",stack);

   push(3, 4);
   push(4, 6);
   push(5, 6);
   push(6, 6);
   push(7, 6);
   push(8, 6);
   push(9, 6);
   push(0, 6);
printf("\nafter push stack is %x stack next is %x",stack, stack->next);
show_stack();

   if(is_not_empty(stack)){
      pop(&x, &y);
      printf("\n\tpopped %d %d ", x, y);
   }
   else
      printf("\nelse stack is empty");

   if(is_not_empty(stack)){
      pop(&x, &y);
      printf("\n\tpopped %d %d ", x, y);
   }
   else
      printf("\nelse stack is empty");

   if(is_not_empty(stack)){
      pop(&x, &y);
      printf("\n\tpopped %d %d ", x, y);
   }
   else
      printf("\nelse stack is empty");

   if(is_not_empty(stack)){
      pop(&x, &y);
      printf("\n\tpopped %d %d ", x, y);
   }
   else
      printf("\nelse stack is empty");

   if(is_not_empty(stack)){
      pop(&x, &y);
      printf("\n\tpopped %d %d ", x, y);
   }
   else
      printf("\nelse stack is empty");

   if(is_not_empty(stack)){
      pop(&x, &y);
      printf("\n\tpopped %d %d ", x, y);
   }
   else
      printf("\nelse stack is empty");

   if(is_not_empty(stack)){
      pop(&x, &y);
      printf("\n\tpopped %d %d ", x, y);
   }
   else
      printf("\nelse stack is empty");

   if(is_not_empty(stack)){
      pop(&x, &y);
      printf("\n\tpopped %d %d ", x, y);
   }
   else
      printf("\nelse stack is empty");

   if(is_not_empty(stack)){
      pop(&x, &y);
      printf("\n\tpopped %d %d ", x, y);
   }
   else
      printf("\nelse stack is empty");

   if(is_not_empty(stack)){
      pop(&x, &y);
      printf("\n\tpopped %d %d ", x, y);
   }
   else
      printf("\nelse stack is empty");

   if(is_not_empty(stack)){
      pop(&x, &y);
      printf("\n\tpopped %d %d ", x, y);
   }
   else
      printf("\nelse stack is empty");

   if(is_not_empty(stack)){
      pop(&x, &y);
      printf("\n\tpopped %d %d ", x, y);
   }
   else
      printf("\nelse stack is empty");



}




show_stack()
{
char r[80];
   struct stacks *temp;
   temp = stack;
   while(temp != NULL){
      printf("\n\t\t\t\t%d %d %x %x",temp->x,temp->y, temp, temp->next);
      temp = temp->next;
   }
}




int is_not_empty(pointer)
   struct stacks *pointer;
{
   int result = 0;
   if(pointer != NULL)
      result = 1;
   return(result);

}  /* ends is_empty */






push(x, y)
   short  x, y;
{
   char r[80];
   struct stacks *new_one;

   new_one = (struct stacks *)
             calloc(1, sizeof(struct stacks ));
   new_one->next = stack;
   new_one->x    = x;
   new_one->y    = y;
   stack         = new_one;
}  /* ends push */






pop(x, y)
   short  *x, *y;
{
   struct stacks *temp;

   temp    = stack;
   *x      = stack->x;
   *y      = stack->y;
   stack   = stack->next;
   free(temp);
}  /* ends pop */



