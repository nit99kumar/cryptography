
   /********************************************
   *
   *   mn.c
   *   5 October 1995
   *
   *********************************************/

#include <bios.h>
#include <stdio.h>
#include <stdlib.h>
#define ONE_SECOND 3800000

main(int argc, char *argv[])
{
   char response[80];
   int c, not_finished = 1;
   long percent = 100;
   unsigned long i, delay = ONE_SECOND;

   while(not_finished){
      delay = ONE_SECOND;
      printf("\nEnter the percentage of one second"
             "\n(or enter Q to quit)"
             "\nCurrent percent is %ld: ", percent);
      gets(response);
      if(response[0] == 'Q'    ||
         response[0] == 'q')     not_finished = 0;
      else{
         percent = atoi(response);
         delay = delay*percent/100;
         printf("\nPress and hold the right SHIFT key to stop metronome");
         while( !(_bios_keybrd(_KEYBRD_SHIFTSTATUS) & 0001)){
            for(i=0; i<delay; i++){
            }
            printf("\a");
         }  /* ends while bios keyboard */
      }  /* ends else not quit */
   }  /* ends while not finished */
}  /* ends main */
