/*

   Travel Accounting Program

   Dwayne Phillips
   November 1992

   Command Line:
      travel [return]

   Purpose:
      This program performs the same function as the
      travel accounting system used by the secretaries
      on the Wang Alliance system prior to January 1993.

      It takes input concerning a person's trip, stores
      that to a file for later use, calculates the
      money due or due back, and prints an accounting.

      It's major advantages are: (1) no more Wang, and
      (2) it saves the accounting data to a disk for
      later retrieval, editing, and printing.  The Wang
      program did not save the accounting to disk so you
      could not edit it.

      This program will run on the UNIX workstations and
      (with a minor change) on an IBM PC or clone.  It is
      written entirely in C and is basic terminal and file
      input and output.

   Files:
      The program uses two files: (1) the data file,
      and (2) a print file.

      Data File
         The data file contains the accounting data
         entered on a traveller.  This data contains
         information about the traveller, advances, and
         expenses.

         Each travel accounting has certain data items associated
         with it.  These include:
      
            Name - name of the person traveling (1)
            ORN - ORN for this travel accounting (1)
            Cash advance amount (1)
            GTR or Ticket Advance Amount - (1,many)
            Transportation Costs (1,many)
               Mode
               Carrier
               Travel Class
               Amount
            Itenerary (1,many)
               Depart Date
               Leaving Location
               Depart Time
               Depart Via
               Arrive Date
               Arrive Location
               Arrive Time
            Daily Expense (1,many)
               Max Lodging
               MIE
               Actual Lodging
               POV Miles
               POV Description
               Rental Car
               Rental Car Description
               Taxi
               Taxi Description
               Parking
               Parking Description
               Official Phone   
               Official Phone   Description
               Other Expenses
               Other Description
            Rental Car (1)
               Personal Mileage
               Total Mileage
               Total Gas

         I store these items in a data file in the 
         following order:
               name - L chars
               ORN  - L chars
               cash_advance - long
               GTR - struct GTR_struct
               ts  - struct trans_struct
               its - struct itinerary_struct
               ds  - struct daily_struct
               rs  - struct rental_struct
            

      Print File
         This program prints the results in a nice looking
         form just like the Wang program did.  Since the
         workstations are on a network with many different
         printers, the program "prints" to a temporary
         print file, and then uses the "system" command
         to print that file to a network printer.  The
         use enters the name of the desired netowrk
         printer.

   Linked Lists:
      Most of the information about the traveller is
      kept in linked lists of structures.  This is 
      because you do not know how many pieces of data
      the traveller will have when the program starts.
      Therefore, you use a linked list and allocate
      new nodes in the list as you need them.
         
      I always create at least one blank struct in
      the linked list.  That way, you can write
      something to the data file and read something
      back.

   Money:
      All money in this progam is kept in penneys
      using a long variable.  There are routines that
      transform this penney value to a float value for
      keyboard input and display and print output.

   POV:
      The POV money is kept in a long, but it is in
      penneys times ten.  This will accomodate fractions
      of cents.  For example, POV=265 for 26.5 cents a
      mile.  The POV calculations take this into account
      and divide by 10 at the appropriate time.

   Possibile Changes:
      There are three global variables you will need
      to change in the future.  These are (1) MIE 
      rates and (2) POV_RATE.  These change from
      time to time with inflation and you must update
      them.  They are in the define area before the
      main program.

   UNIX and DOS Versions:
      This program is very basic C and will run on
      UNIX and DOS platforms (and probably others too).
      You must set the values of the UNIX_SYSTEM and
      DOS_SYSTEM global variables before compiling.
      Just set them to 1 and 0.  These are in the 
      define area fbefore the main program.

   Revision History:
      Version 1 - February 1993
      Version 1.01 - 9 February 1993 
         Made a small change to get_money so it would
         not lose a penney.
      Version 1.1 - 12 February 1993
         Added the rental car structure.  Previous versions
         did not do this because the secrataries did
         not need it.  They changed their minds, so 
         I added it.
      Version 1.2 - 22 February 1993
         Made a change in the print_daily subroutine.
         If your actual lodging exceeds the max allowed
         lodging, then you must use the max lodging
         in calculations.
      Version 1.3 - 20 April 1993
         Made several changes:
         (1) Added 3 more MIE values.  Earlier versions
             only used MIE1 and MIE2.  This version uses
             MIE1 through MIE5.  This is because our
             organization changed the meals and incidental
             expense rates for the cities.  So, I
             added a couple of lines of code to the 
             MIE loop in the routines 
             edit_daily_expenses and get_daily_expenses.
             DP - 4-15-93
         (2) The program now displays the name of the
             date file right after it closes that file.
             This will help the user know where the data
             is stored.
         (3) The DOS_SYSTEM version of the program gets
             the data file name directly from the user
             for new cases.  When the user enters a file
             name, the program checks for the existance
             of that file.  If it already exists, the program
             asks for another file name.
         (4) Changed several edit functions so you can
             skip to the end of a long linked list.
             The changes were in edit_trans_cost
             edit_intinerary edit_daily_expenses
      Version 1.4 - 8 September 1993
         I changed all occurences of SAD to CAC to reflect
         our new division name.

*/
/*****************************************************/
/*****************************************************/
/*PAGE Includes and data structures */

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#define L            100
#define LPP           66
#define TAB            5
#define END_OF_LIST 0x00

   /* Here are the money values that
      change with inflation.  Recall, the 
      POV_RATE is in cents times ten.  The
      MIE rates are in cents. 

      Changed from 3 MIE rates to 5 MIE
      rates - DP - 4/15/93 */
#define MIE1           0
#define MIE2        2600
#define MIE3        3000
#define MIE4        3400
#define MIE5        3800
#define POV_RATE     250

   /* Here are the globals you set before
      compiling for UNIX or DOS. */
#define UNIX_SYSTEM    1
#define DOS_SYSTEM     0



   /* Here are the structures.  Most of these are
      for linked lists that the program creates
      dynamically as it goes along. */

   /* The Month-Day-Year Structure */
struct MDY_struct{
   short month;
   short day;
   short year;
};

   /* The Transportation Cost Structure */
struct trans_struct{
   char   mode[L];
   char   carrier[L];
   char   travel_class[L];
   long   amount;
   struct trans_struct *next;
};

   /* The Itenerary Structure */
struct itinerary_struct{
   struct MDY_struct depart_date;
   char   leave_loc[L];
   long   depart_time;
   char   depart_via[L];
   struct MDY_struct arrive_date;
   char   arrive_loc[L];
   long   arrive_time;
   struct itinerary_struct *next;
};

   /* The Daily Expense Structure */
struct daily_struct{
   struct MDY_struct today;
   long   max_lodging;
   long   mie;
   long   actual_lodging;
   long   pov_miles;
   char   pov_desc[L];
   long   rental_car;
   char   rental_car_desc[L];
   long   taxi;
   char   taxi_desc[L];
   long   parking;
   char   parking_desc[L];
   long   phone;
   char   phone_desc[L];
   long   other;
   char   other_desc[L];
   struct daily_struct *next;
};

   /* The GTR Advance Structure */
struct GTR_struct{
   long   GTR;
   struct GTR_struct *next;
};

   /* The Rental Car Structure */
struct rental_struct{
   long personal_mileage;
   long total_mileage;
   long total_gas;
};


   /* Here are the functions I needed to define. */
float  dollars_of();
long   calculate_mie();
long   get_money();
long   pennies_of();
struct GTR_struct        * get_GTRs();
struct trans_struct      * get_transportation_cost();
struct itinerary_struct  * get_itinerarys();
struct daily_struct      * get_daily_expenses();
struct rental_struct     * get_rental_expenses();
struct GTR_struct        * retrieve_GTRs();
struct trans_struct      * retrieve_transportation_cost();
struct itinerary_struct  * retrieve_itinerarys();
struct daily_struct      * retrieve_daily_expenses();
struct rental_struct     * retrieve_rental_expenses();
struct itinerary_struct  * traverse_itinerary();
/*****************************************************/
/*****************************************************/
/*PAGE main */

main(argc, argv)
   int  argc;
   char *argv[];
{
   int   choice, i, not_finished = 1, lines=0;
   long  cash_advance; /* the cash advance           */ 
   short days=0;

   char  data_file_name[L],
         date_string[L],
         name[L],
         ORN[L],
         print[L],
         print_file_name[L],
         printer_name[L],
         r[L];

      /* Declare the two files */
   FILE *data_file, *print_file;
       
      /* Declare the pointers for the
         linked lists. */
   struct GTR_struct       *GTR;
   struct trans_struct     *ts;
   struct itinerary_struct *its;
   struct daily_struct     *ds;
   struct rental_struct    *rs;
   struct MDY_struct       first_day, last_day;


   get_date(date_string);

   GTR = END_OF_LIST;
   ts  = END_OF_LIST;
   its = END_OF_LIST;
   ds  = END_OF_LIST;
   rs  = END_OF_LIST;


      /* Start the basic loop which continues
         until the user says quit. */
   while(not_finished){
      show_main_menu();
      gets(r);
      choice = atoi(r);

       switch(choice){
          case 0:
             printf("\nTHE END\n\n");
             not_finished = 0;
             break;


                  /* Process new travel accounting */
          case 1:
             printf("\ncase 1");

                /* Get the input from the user */

                /* get traveller's name and ORN */
             get_name(name);
             get_orn(ORN);
 
             get_data_file_name(name, date_string, data_file_name);
             data_file = fopen(data_file_name, "w");
             if(data_file == '\0'){
                printf("\nERROR: Could not create data file %s",
                          data_file_name);
                exit(-1);
             }  /* ends opening data file */
 
                /* get the Cash Advance */
             printf("\n\tCash Advance Amount? ");
             cash_advance = get_money();
       
                /* get the GTR or Ticket Advance(s) */
             GTR = get_GTRs();
 
                /* get the transportation cost structure(s) */
             ts = get_transportation_cost();
 
                /* get the itinerary structure(s) */
             its = get_itinerarys();
             get_first_last_dates(its, &first_day, &last_day);
         
                /* get the daily expenses */
             ds = get_daily_expenses(&first_day, &last_day, &days);

                /* get the rental car expenses if any */
             rs = get_rental_expenses();
 
                /* save the data to the data file */
             fwrite(name, (L*sizeof(char)), 1, data_file);
             fwrite(ORN,  (L*sizeof(char)), 1, data_file);
             fwrite(&cash_advance, sizeof(long), 1, data_file);
             save_GTRs(GTR, data_file);
             save_transportation_cost(ts, data_file);
             save_itinerarys(its, data_file);
             save_daily_expenses(ds, data_file);
             save_rental_expenses(rs, data_file);
             fclose(data_file);
             printf("\n\nWrote your data to file: %s",
                    data_file_name);

             printf("\nDo you want to print? (y or n)");
             gets(print);

             if(print[0] == 'y' || print[0] == 'Y'){
                tmpnam(print_file_name);
                print_file = fopen(print_file_name, "wt");
                if(print_file == '\0'){
                   printf("\nERROR Could not open print file %s",
                           print_file_name);
                   exit(2);
                }  /* ends if could not open print file */

                get_first_last_dates(its, &first_day, &last_day);
                print_accounting(date_string, name, ORN, 
                                 days, &lines, print_file,
                                 &first_day, &last_day,
                                 cash_advance, GTR, ts,
                                 its, ds, rs);
                lines = 0;
                fclose(print_file);
                if(UNIX_SYSTEM){
                   printf("\nEnter the name of the printer:");
                   printf(" (e.g. GE02_SPARC, ps1, ps, etc.)");
                   printf("\n     __________\b\b\b\b\b\b\b\b\b\b");
                   gets(printer_name);
                   sprintf(r, "lpr -P%s %s", 
                      printer_name, print_file_name);
                }
                if(DOS_SYSTEM)
                   sprintf(r, "type %s > prn", print_file_name);
                system(r);
                unlink(print_file_name);
             }  /* ends if yes print */
             break;



                  /* Process old travel accounting */
          case 2:
             printf("\ncase 2");
             printf("\nEnter the data file name:");
             printf("______________________________");
             for(i=0; i<30; i++) printf("\b");
             gets(data_file_name);

             data_file = fopen(data_file_name, "r");
             if(data_file == '\0'){
                printf("\nERROR: Could not open data file %s",
                          data_file_name);
                exit(-1);
             }  /* ends opening data file */

             fread(name, (L*sizeof(char)), 1, data_file);
             fread(ORN,  (L*sizeof(char)), 1, data_file);
             fread(&cash_advance, sizeof(long), 1, data_file);
             GTR   = retrieve_GTRs(data_file);
             ts    = retrieve_transportation_cost(data_file);
             its   = retrieve_itinerarys(data_file);
             ds    = retrieve_daily_expenses(data_file, &days);
             rs    = retrieve_rental_expenses(data_file);
             fclose(data_file);

             printf("\nDo you want to edit ");
             printf("this accounting? (y or n)");
             gets(r);
             if(r[0] == 'y' || r[0] == 'Y'){
                review_accounting(name, ORN, &cash_advance,
                                  GTR, ts, its, ds, rs);
             }  /* ends edit accounting */




             printf("\nDo you want to print? (y or n)");
             gets(print);

             if(print[0] == 'y' || print[0] == 'Y'){

                tmpnam(print_file_name);
                print_file = fopen(print_file_name, "wt");
                if(print_file == '\0'){
                   printf("\nERROR Could not open print file %s",
                           print_file_name);
                   exit(2);
                }  /* ends if could not open print file */

                get_first_last_dates(its, &first_day, &last_day);
                print_accounting(date_string, name, ORN, 
                                 days, &lines, print_file,
                                 &first_day, &last_day,
                                 cash_advance, GTR, ts,
                                 its, ds, rs);
                lines = 0;
                fclose(print_file);
                if(UNIX_SYSTEM){
                   printf("\nEnter the name of the printer:");
                   printf(" (e.g. GE02_SPARC, ps1, ps, etc.)");
                   printf("\n     __________\b\b\b\b\b\b\b\b\b\b");
                   gets(printer_name);
                   sprintf(r, "lpr -P%s %s", 
                      printer_name, print_file_name);
                }
                if(DOS_SYSTEM)
                   sprintf(r, "type %s > prn", print_file_name);
                system(r);
                unlink(print_file_name);
             }  /* ends if yes print */




             printf("\nDo you want to save ");
             printf("this edited accounting? (y or n)");
             gets(r);
             if(r[0] == 'y' || r[0] == 'Y'){
 
                get_data_file_name(name, date_string, data_file_name);
                data_file = fopen(data_file_name, "w");
                if(data_file == '\0'){
                   printf("\nERROR: Could not create data file %s",
                             data_file_name);
                   exit(-1);
                }  /* ends opening data file */

                   /* save the data to the data file */
                fwrite(name, (L*sizeof(char)), 1, data_file);
                fwrite(ORN,  (L*sizeof(char)), 1, data_file);
                fwrite(&cash_advance, sizeof(long), 1, data_file);
                save_GTRs(GTR, data_file);
                save_transportation_cost(ts, data_file);
                save_itinerarys(its, data_file);
                save_daily_expenses(ds, data_file);
                fclose(data_file);
                printf("\n\nWrote your data to file: %s",
                       data_file_name);
             }  /* ends if save edited accounting */

             break;

          default:
             printf("\n\nSorry, cannot understand your ");
             printf("choice, try again.");
             break;
       }  /* ends switch choice */
   }  /* ends while not_finished */



      /* free up the memory you allocated */
   free(GTR);
   free(ts);
   free(its);
   free(ds);



}  /* ends main */
/*****************************************************/
/*****************************************************/
/*****************************************************/
/*PAGE review_accounting

   This function is the main routine for the
   review process.  It shows menus, interprets
   the responses and calls other functions to make
   changes as desired.
*/

review_accounting(name, ORN, cash_advance,
                  GTR, ts, its, ds, rs)
   char    name[], ORN[];
   long    *cash_advance;
   struct  daily_struct      *ds;
   struct  GTR_struct       *GTR;
   struct  itinerary_struct *its;
   struct  trans_struct     *ts;
   struct  rental_struct    *rs; 
{
   char r[L];
   int  choice, not_finished = 1;

   while(not_finished){
      show_review_menu();
      gets(r);
      choice = atoi(r);
      switch(choice){

         case 0:
            not_finished = 0;
            break;

         case 1: /* name and ORN */
            edit_name_orn(name, ORN);
            break;

         case 2: /* cash advance */
            edit_cash_advance(cash_advance); 
            break;

         case 3: /* GTR */
            edit_GTRs(GTR);
            break;

         case 4: /* Transportation cost */
            edit_trans_cost(ts);
            break;

         case 5: /* Itenerary */
            edit_itinerary(its);
            break;

         case 6: /* Daily Expenses */
            edit_daily_expenses(ds);
            break;

         case 7: /* Rental Car Use */
            edit_rental_expenses(rs);
            break;

         default:
            printf("\n\nSorry, cannot understand your");
            printf(" choice, try again.");
            break;
      }  /* ends switch choice */
   }  /* ends while not_finished */
}  /* ends review_accounting */
/*****************************************************/
/*****************************************************/
/*PAGE edit_name_orn

   This function allows the user to edit the
   traveller's name and ORN.
*/

edit_name_orn(name, ORN)
   char name[], ORN[];
{
   char r[L];
   int  choice, not_finished = 1;

   while(not_finished){
      printf("\n");
      printf("\n\t1. Name is %s", name);
      printf("\n\t2. ORN is %s", ORN);
      printf("\n\n\tEnter number to make a change, 0 to quit");
      printf("\n\t__\b\b");
      gets(r);
      choice = atoi(r);
      switch(choice){

         case 0:
            not_finished = 0;
            break;

         case 1:
            get_name(name);
            break;

         case 2:
            get_orn(ORN);
            break;

         default:
            printf("\n\nSorry, cannot understand your");
            printf(" choice, try again.");
            break;
      }  /* ends switch choice */
   }  /* ends not_finished */
}  /* ends edit_name_ORN */
/*****************************************************/
/*****************************************************/
/*PAGE edit_cash_advance

   This function allows the user to edit the
   cash advance.
*/

edit_cash_advance(cash_advance)
   long *cash_advance;
{
   char r[L];
   int  choice, not_finished = 1;
   long temp;

   temp = *cash_advance;

   while(not_finished){
      printf("\n");
      printf("\n\t1. Cash advance is $%8.2f", dollars_of(temp));
      printf("\n\n\tEnter number to make a change, 0 to quit");
      printf("\n\t__\b\b");
      gets(r);
      choice = atoi(r);
      switch(choice){

         case 0:
            not_finished = 0;
            break;

         case 1:
            printf("\n\tCash Advance Amount? ");
            temp = get_money();
            break;

         default:
            printf("\n\nSorry, cannot understand your");
            printf(" choice, try again.");
            break;
      }  /* ends switch choice */
   }  /* ends while not_finished */

   *cash_advance = temp;

}  /* ends edit_cash_advance */
/*****************************************************/
/*****************************************************/
/*PAGE edit_GTRs

   This function allows the user to edit the
   GTRs.
*/

edit_GTRs(GTR)
   struct GTR_struct *GTR;
{
   char   r[L];
   int    choice, not_finished = 1;
   long   temp;
   struct GTR_struct *g;

   g = GTR;
   while(not_finished){
      printf("\n");
      printf("\n\t1. GTR is $%8.2f", 
             dollars_of(g->GTR));
      printf("\n\n\tEnter 1 to change this GTR");
      printf("\n\tEnter 2 to go on to next GTR");
      printf("\n\t__\b\b");
      gets(r);
      choice = atoi(r);

      if(choice == 1){
         printf("\nEnter new GTR:________\b\b\b\b\b\b\b\b");
         temp   = get_money();
         g->GTR = temp;
      }  /* ends if choice == 1 */

      if(choice == 2){
         if(g->next == END_OF_LIST){
            printf("\nNo more advances to edit");
            not_finished = 0;
         }  /* ends if END_OF_LIST */
         else
            g = g->next;
      }  /* ends if choice == 2 */
   }  /* ends while not_finished */
}  /* ends edit_GTRs */
/*****************************************************/
/*****************************************************/
/*PAGE edit_trans_cost

   This function allows the user to edit the
   transportation costs.
*/

edit_trans_cost(ts)
   struct trans_struct *ts;
{
   char   r[L];
   int    choice, j, not_finished = 1;
   struct trans_struct *t;

   t = ts;
   while(not_finished){
      printf("\n");
      printf("\n\t1. Mode: %s", t->mode);
      printf("\n\t2. Carrier: %s", t->carrier);
      printf("\n\t3. Class: %s", t->travel_class);
      printf("\n\t4. Amount: $%8.2f", 
              dollars_of(t->amount));
      printf("\n\t5. Go on to the next transportation");
      printf("\n\t6. Quit editing transportation costs");
      printf("\n");
      printf("\n\tEnter the number to change:__\b\b");
      gets(r);
      choice = atoi(r);
      switch(choice){

         case 1:
            printf("\n\tMode:______________________________");
            printf("(enter 0 to quit)");
            for(j=0; j<47; j++) printf("\b");
            gets(t->mode);
            break;

         case 2:
            printf("\n\tCarrier:______________________________");
            for(j=0; j<30; j++) printf("\b");
            gets(t->carrier);
            break;

         case 3:
            printf("\n\tTravel Class:______________________________");
            for(j=0; j<30; j++) printf("\b");
            gets(t->travel_class);
            break;

         case 4:
            printf("\n\tAmount:");
            t->amount = get_money();
            break;

         case 5:
            if(t->next == END_OF_LIST){
               printf("\n\nNo more to edit");
               not_finished = 0;
            }  /* ends if END_OF_LIST */
            else
               t = t->next;
            break;

         case 6:
            not_finished = 0;
            break;

         default:
            printf("\n\nSorry, cannot understand your");
            printf(" choice, try again.");
            break;

      }  /* ends switch choice */
   }  /* ends while not_finished */

}  /* ends edit_trans_cost */
/*****************************************************/
/*****************************************************/
/*PAGE edit_itinerary

   This function allows the user to edit the
   itinerary.
*/

edit_itinerary(its)
   struct itinerary_struct *its;
{
   char   r[L], s[L];
   int    choice, i, not_finished = 1;
   struct itinerary_struct *t;
   struct MDY_struct       dummy;

   t = its;
   while(not_finished){
      printf("\n");
      printf("\n\t1. Departure: %2d-%2d-%2d",
              t->depart_date.month,
              t->depart_date.day,
              t->depart_date.year);
      printf("\n\t2. Leaving: %s", t->leave_loc);
      printf("\n\t3. Time: %ld", t->depart_time);
      printf("\n\t4. Via: %s", t->depart_via);
      printf("\n\t5. Arrival: %2d-%2d-%2d",
              t->arrive_date.month,
              t->arrive_date.day,
              t->arrive_date.year);
      printf("\n\t6. Arriving: %s", t->arrive_loc);
      printf("\n\t7. Time: %ld", t->arrive_time);
      printf("\n\t8. Go on to the next itinerary.");
      printf("\n\t9. Quit editing itinerary");
      printf("\n");
      printf("\n\tEnter the number to change:__\b\b");
      gets(r);
      choice = atoi(r);
      switch(choice){

         case 1:
            printf("\nEnter the new departure date");
            get_mdy(&dummy);
            t->depart_date.month = dummy.month;
            t->depart_date.day   = dummy.day  ;
            t->depart_date.year  = dummy.year ;
            break;

         case 2:
            printf("\nLeaving:______________________________");
            for(i=0; i<30; i++) printf("\b");
            gets(t->leave_loc);
            break;

         case 3:
            printf("\nAt:____ (24 hour time - no colon)");
            for(i=0; i<30; i++) printf("\b");
            gets(s);
            t->depart_time = atoi(s);
            break;

         case 4:
            printf("\nVia:______________________________");
            for(i=0; i<30; i++) printf("\b");
            gets(t->depart_via);
            break;

         case 5:
            printf("\nARRIVAL");
            get_mdy(&dummy);
            t->arrive_date.month = dummy.month;
            t->arrive_date.day   = dummy.day;
            t->arrive_date.year  = dummy.year;
            break;

         case 6:
            printf("\nArriving:______________________________");
            for(i=0; i<30; i++) printf("\b");
            gets(t->arrive_loc);
            break;

         case 7:
            printf("\nAt:____ (24 hour time - no colon)");
            for(i=0; i<30; i++) printf("\b");
            gets(s);
            t->arrive_time = atoi(s);
            break;

         case 8:
            if(t->next == END_OF_LIST){
               printf("\nNo more itinerarys");
               not_finished = 0;
            }  /* ends if END_OF_LIST */
            else
               t = t->next;
            break;

         case 9:
            not_finished = 0;
            break;

         default:
            printf("\n\nSorry, cannot understand your");
            printf(" choice, try again.");
            break;

      }  /* ends switch choice */
   }  /* ends while not_finished */
}  /* ends edit_itinerary */
/*****************************************************/
/*****************************************************/
/*PAGE edit_daily_expenses

   This function allows the user to edit the
   daily expenses.
*/

edit_daily_expenses(ds)
   struct daily_struct *ds;
{
   char   r[L], s[L];
   int    choice, j, not_done, not_finished = 1;
   struct daily_struct *t;
   struct MDY_struct    dummy;

   t = ds;
   while(not_finished){
      printf("\n");
      printf("\n\t1.  Today is: %2d-%2d-%2d",
              t->today.month,
              t->today.day,
              t->today.year);
      printf("\n\t2.  Maximum Lodging: $%8.2f",
              dollars_of(t->max_lodging));
      printf("\n\t3.  M&IE: $%8.2f", 
              dollars_of(t->mie));
      printf("\n\t4.  Actual Lodging: $%8.2f",
              dollars_of(t->actual_lodging));
      printf("\n\t5.  POV Miles: %ld", t->pov_miles);
      printf("\n\t6.  POV Description: %s", 
              t->pov_desc);
      printf("\n\t7.  Rental Car Cost: $%8.2f",
              dollars_of(t->rental_car));
      printf("\n\t8.  Rental Car Description: %s",
              t->rental_car_desc);
      printf("\n\t9.  Taxi Cost: $%8.2f",
              dollars_of(t->taxi));
      printf("\n\t10. Taxi Description: %s ",
              t->taxi_desc);
      printf("\n\t11. Parking Cost: $%8.2f",
              dollars_of(t->parking));
      printf("\n\t12. Parking Description: %s",
              t->parking_desc);
      printf("\n\t13. Telephone Cost: $%8.2f",
              dollars_of(t->phone));
      printf("\n\t14. Telephone Description: %s",
              t->phone_desc);
      printf("\n\t15. Other Cost: $%8.2f",
              dollars_of(t->other));
      printf("\n\t16. Other Description: %s",
              t->other_desc);
      printf("\n\t17. Go on to the next day's expenses");
      printf("\n\t18. Quit editing daily expenses");
      printf("\n");
      printf("\n\tEnter the number to change:__\b\b");
      gets(r);
      choice = atoi(r);
      switch(choice){

         case 1:
            printf("\nEnter date:");
            get_mdy(&dummy);
            t->today.month = dummy.month;
            t->today.day   = dummy.day;
            t->today.year  = dummy.year;
            break;

         case 2:
            printf("\nMax Lodging at TDY Point:_____\b\b\b\b\b");
            t->max_lodging = get_money();
            break;
      
         case 3:
            not_done = 1;
            while(not_done){
               printf("\nM&IE at TDY Point:_____\b\b\b\b\b");
               t->mie = get_money();
                     /* change - added 3 MIE values here
                        4-15-93 - DP */
               if(t->mie != MIE1  &&
                  t->mie != MIE2  &&
                  t->mie != MIE3  &&
                  t->mie != MIE4  &&
                  t->mie != MIE5){
                  printf("\nMust equal");
                  show_money(MIE1);
                  printf(", ");
                  show_money(MIE2);
                  printf(", ");
                  show_money(MIE3);
                  printf(", ");
                  show_money(MIE4);
                  printf(", ");
                  show_money(MIE5);
               }  /* ends if MIE values */
               else
                  not_done = 0;
            }  /* ends M&IE loop */
            break;

         case 4:
            printf("\nActual Lodging:_____\b\b\b\b\b");
            t->actual_lodging = get_money();
            break;
      
         case 5:
            printf("\nPOV Miles:_____\b\b\b\b\b");
            gets(s);
            t->pov_miles = atoi(s);
            break;

         case 6:
            printf("\nDesc:______________________________");
            for(j=0; j<30; j++) printf("\b");
            gets(t->pov_desc);
            break;

         case 7:
            printf("\nRental Car:______\b\b\b\b\b\b");
            t->rental_car = get_money();
            break;

         case 8:
            printf("\nDesc:______________________________");
            for(j=0; j<30; j++) printf("\b");
            gets(t->rental_car_desc);
            break;

         case 9:
            printf("\nTaxi:______\b\b\b\b\b\b");
            t->taxi = get_money();
            break;

         case 10:
            printf("\nDesc:______________________________");
            for(j=0; j<30; j++) printf("\b");
            gets(t->taxi_desc);
            break;

         case 11:
            printf("\nParking:______\b\b\b\b\b\b");
            t->parking = get_money();
            break;
      
         case 12:
            printf("\nDesc:______________________________");
            for(j=0; j<30; j++) printf("\b");
            gets(t->parking_desc);
            break;
      
         case 13:
            printf("\nOfficial Phone:______\b\b\b\b\b\b");
            t->phone = get_money();
            break;
      
         case 14:
            printf("\nDesc:______________________________");
            for(j=0; j<30; j++) printf("\b");
            gets(t->phone_desc);
            break;

         case 15:
            printf("\nOther:______\b\b\b\b\b\b");
            t->other = get_money();
            break;

         case 16:
            printf("\nDesc:______________________________");
            for(j=0; j<30; j++) printf("\b");
            gets(t->other_desc);
            break;

         case 17:
            if(t->next == END_OF_LIST){
               printf("\nNo more daily expenses");
               not_finished = 0;
            }  /* ends if END_OF_LIST */
            else
               t = t->next;
            break;

         case 18:
            not_finished = 0;
            break;

         default:
            printf("\n\nSorry, cannot understand your");
            printf(" choice, try again.");
            break;

      }  /* ends switch choice */
   }  /* ends while not_finished */
}  /* ends edit_daily_expenses */
/*****************************************************/
/*****************************************************/
/*PAGE edit_rental_expenses

   This function allows the user to edit the
   rental car personal use information.
*/

edit_rental_expenses(rs)
   struct rental_struct *rs;
{
   char r[L], s[L];
   int  choice, not_finished = 1;
   long temp;

   while(not_finished){
      printf("\n\t1. Personal mileage is %ld", 
             rs->personal_mileage);
      printf("\n\t2. Total mileage is %ld",
             rs->total_mileage);
      printf("\n\t3. Total gas is $%8.2f",
             dollars_of(rs->total_gas));
      printf("\n\n\tEnter number to make a change, 0 to quit");
      printf("\n\t__\b\b");
      gets(r);
      choice = atoi(r);
      switch(choice){

         case 0:
            not_finished = 0;
            break;

         case 1:
            printf("\nPersonal Mileage:________\b\b\b\b\b\b\b\b");
            gets(s);
            rs->personal_mileage = atoi(s);
            break;

         case 2:
            printf("\nTotal Rental Car Mileage:");
            printf("________\b\b\b\b\b\b\b\b");
            gets(s);
            rs->total_mileage = atoi(s);
            break;

         case 3:
            printf("\nTotal Gas:$________\b\b\b\b\b\b\b\b");
            temp = get_money();
            rs->total_gas = temp;
            break;

         default:
            printf("\n\nSorry, cannot understand your ");
            printf("choice, try again.");
            break;
      }  /* ends switch choice */
   }  /* ends while not_finished */
}  /* ends edit_rental_expenses */
/*****************************************************/
/*****************************************************/
/*PAGE show_main_menu

   This function shows the main menu on
   the screen.
*/

show_main_menu()
{
   printf("\n");
   printf("\n\tTravel Accounting System - CAC - ");
   printf("Version 1.3 - April 1993");
   printf("\n");
   printf("\n1. Process new accounting");
   printf("\n2. Review or print old accounting");
   printf("\n");
   printf("\n   Enter 0 to quit");
   printf("\n\n");
}  /* ends show_main_menu */
/*****************************************************/
/*PAGE show_review_menu

   This function shows the review menu on
   the screen.
*/

show_review_menu()
{
   printf("\n");
   printf("\nDo you want to review or change:");
   printf("\n\t1. Traveller's name and ORN");
   printf("\n\t2. Cash advance");
   printf("\n\t3. GTR or Ticket advance amount");
   printf("\n\t4. Transportation costs ");
   printf("\n\t5. Itinerary");
   printf("\n\t6. Daily expenses");
   printf("\n\t7. Rental Car Personal Use");
   printf("\n\t\tEnter 0 to quit");
   printf("\n\n__\b\b");
}  /* ends show_review_menu */
/*****************************************************/
/*****************************************************/
/*PAGE get_mdy

   Interact with the user to get the 
   month-day-year structure.
*/

get_mdy(a)
   struct MDY_struct *a;
{
   char r[L];

   printf("\nEnter month number:");
   gets(r);
   a->month = atoi(r);
   if(a->month != -1){
      printf("Enter day number:");
      gets(r);
      a->day = atoi(r); 
      printf("Enter year number (two digit):");
      gets(r);
      a->year = atoi(r);
   }
}  /* ends get_mdy */
/*****************************************************/
/*****************************************************/
/*PAGE get_name

   Interact with the user to get the 
   name of the traveller.
*/

get_name(a)
   char a[];
{
   int i;
   printf("\n\t\tName:______________________________");
   for(i=0; i<30; i++) printf("\b");
   gets(a);
}  /* ends get_name */
/*****************************************************/
/*****************************************************/
/*PAGE get_data_file_name

        Use the name of the traveller and the
        date to create a file name for the
        data file.

        If you are using a DOS system, the user
        will enter the file name by hand.
*/

get_data_file_name(name, date_string, file_name)
   char date_string[], file_name[], name[];
{
   char   s[L], s2[L];
   FILE   *fp;
   int    i, j, k, looking;

   s2[0] = '\0';

   if(UNIX_SYSTEM){
        /* find first non-blank character in the name */
      i=0;
      while(name[i] == ' ')
         i++;

         /* copy non-blank characters from name */
      j=0;
      while(name[i] != '\0' &&
            name[i] != '\n'){
         if(name[i] == ' ')
            file_name[j] = '.';
         else
            file_name[j] = name[i];
         j++;
         i++;
      }
   
         /* remove any extra periods */
      while(file_name[j-1] == '.')
         j--;
      file_name[j++] = '.';
      file_name[j]   = '\0';
   
      strcat(file_name, date_string);
   
      looking      = 1;
      k            = 0; 
      strcpy(s2, file_name);
   
      while(looking){
         fp = fopen(s2, "rt");
         if(fp == '\0')
            looking = 0;
         else{
            fclose(fp);
            sprintf(s, ".%d", ++k);
            strcpy(s2, file_name);
            strcat(s2, s);
         }  /* ends else tried another name */
      }  /* ends while looking */

      strcpy(file_name, s2);
   }  /* ends if UNIX_SYSTEM */

   if(DOS_SYSTEM){

      looking = 1;
      while(looking){
         printf("\n\nEnter the name of the file you");
         printf("\nwant to use as the data file: ");
         gets(s2);
         fp = fopen(s2, "r");
         if(fp == '\0')
            looking = 0;
         else{
            fclose(fp);
            printf("\nFile %s already exists", s2);
            printf("\nTry another file name");
         }  /* ends else tried another name */
      }  /* ends while looking */

      strcpy(file_name, s2);

   }  /* ends if DOS_SYSTEM */

}  /* ends get_data_file_name */
/*****************************************************/
/*****************************************************/
/*PAGE get_orn

   Interact with the user to get the 
   ORN.
*/

get_orn(a)
   char a[];
{
   int i;
   printf("\n\t\tORN:______________________________");
   for(i=0; i<30; i++) printf("\b");
   gets(a);
}  /* ends get_orn */
/*****************************************************/
/*****************************************************/
/*PAGE get_money

   This function interacts with the user
   to get a money value.  It gets it as
   a float and returns it to the caller
   as a long.
*/

long get_money()
{
   char  r[L];
   float f;
   long  l;

   gets(r);
   f = atof(r);
   f = f + 0.005;
   l = pennies_of(f);
   return(l);
}  /* ends get_money */
/*****************************************************/
/*****************************************************/
/*PAGE get_number

   This functions prompts the user with
   the string given in the input, reads 
   an integer from the keyboard, and 
   returns that integer to the calling
   function.
*/

get_number(s)
   char s[];
{
   char r[80];
   int  result;

   printf("\n\t%s: ", s);
   gets(r);
   result = atoi(r);
   return(result);
}  /* ends get_number */
/*****************************************************/
/*****************************************************/
/*PAGE get_transportation_cost

   This function gets the
   transportation cost structures.

*/

struct trans_struct * get_transportation_cost()
{
   char s[L];
   int first=1, j, not_done=1;
   struct trans_struct *current, *new1, *result;

   while(not_done){

      printf("\n\tMode:______________________________");
      printf("(enter 0 to quit)");
      for(j=0; j<47; j++) printf("\b");
      gets(s);

      if(s[0] == '0'){
         not_done = 0;
            /* if the user enters 0 on the first try,
               then create at least one instance of
               the struct to pass back. */
         if(first){
            first = 0;
            new1 = (struct trans_struct *) 
                    calloc(1, sizeof(struct trans_struct));
            new1->next = END_OF_LIST;
            result     = new1;
            strcpy(result->mode, s);
         }  /* ends if first */ 
      }  /* ends if s[0] == '0' */

      else{  /* get data */
         new1 = (struct trans_struct *) 
                 calloc(1, sizeof(struct trans_struct));
         new1->next = END_OF_LIST;

         if(first){
            result   = new1;
            current  = new1;
            first    = 0;
         }
         else{
            current->next = new1;
            current       = new1;
         }

         strcpy(current->mode, s);
         printf("\n\tCarrier:______________________________");
         for(j=0; j<30; j++) printf("\b");
         gets(current->carrier);
         printf("\n\tTravel Class:______________________________");
         for(j=0; j<30; j++) printf("\b");
         gets(current->travel_class);
         printf("\n\tAmount:");
         current->amount = get_money();
      }  /* ends else get data */
   }  /* ends while not_done */
   
   return(result);

}  /* ends get_transportation_cost */
/*****************************************************/
/*****************************************************/
/*PAGE get_GTRs

   This function gets the GTR
   advances.
*/

struct GTR_struct * get_GTRs()
{
   int  first=1, not_done=1;
   long a;
   struct GTR_struct *current, *new1, *result;

   while(not_done){
      printf("\nGTR or Ticket Advance Amount:");
      a = get_money();

      if(a == 0){
         not_done = 0;
            /* if the user enters 0 on the first try,
               then create at least one instance of
               the struct to pass back. */
         if(first){
            first = 0;
            new1 = (struct GTR_struct *) 
                    calloc(1, sizeof(struct GTR_struct));
            new1->next = END_OF_LIST;
            new1->GTR  = 0;
            result     = new1;
         }  /* ends if first == 1 */
      }  /* ends if a == 0 */

      else{     /* get data */
         new1 = (struct GTR_struct *) 
                 calloc(1, sizeof(struct GTR_struct));
         new1->next = END_OF_LIST;

         if(first){
            result   = new1;
            current  = new1;
            first    = 0;
         }
         else{
            current->next = new1;
            current       = new1;
         }
         current->GTR = a;        
      }  /* ends else get data */
   }  /* ends while not_done */

   return(result);

}  /* ends get_GTRs */
/*****************************************************/
/*****************************************************/
/*PAGE get_itinerarys 

   This function gets the itinerary
   for the traveler.
*/


struct itinerary_struct * get_itinerarys()
{
   char   s[L];
   int    i, first=1, not_done=1;
   struct MDY_struct dummy;
   struct itinerary_struct *current, *new1, *result;

   while(not_done){
      printf("\n\t\tItinerary:");
      printf("\n\t\t(enter -1 for month to quit)");
      printf("\nDEPARTURE");
      get_mdy(&dummy);

      if(dummy.month == -1){
         not_done = 0;
            /* if the user enters 0 on the first try,
               then create at least one instance of
               the struct to pass back. */
         if(first){
            first = 0;
            new1 = (struct itinerary_struct *)
                    calloc(1, sizeof(struct itinerary_struct));
            new1->next = END_OF_LIST;
            result     = new1;
            result->depart_date.month = -1;
         }  /* ends if first */
      }  /* ends if dummy.month == -1 */

      else{     /* get data */
         new1 = (struct itinerary_struct *)
                 calloc(1, sizeof(struct itinerary_struct));
         new1->next = END_OF_LIST;
           
            /* If first time through, then set the
               result pointer to the new struct. */
         if(first){
            result  = new1;
            current = new1;
            first   = 0;
         }
         else{
            current->next = new1;
            current       = new1;
         }

         current->depart_date.month = dummy.month;
         current->depart_date.day   = dummy.day;
         current->depart_date.year  = dummy.year;

         printf("\nLeaving:______________________________");
         for(i=0; i<30; i++) printf("\b");
         gets(current->leave_loc);

         printf("\nAt:____ (24 hour time - no colon)");
         for(i=0; i<30; i++) printf("\b");
         gets(s);
         current->depart_time = atoi(s);

         printf("\nVia:______________________________");
         for(i=0; i<30; i++) printf("\b");
         gets(current->depart_via);

         printf("\nARRIVAL");
         get_mdy(&dummy);
         current->arrive_date.month = dummy.month;
         current->arrive_date.day   = dummy.day;
         current->arrive_date.year  = dummy.year;

         printf("\nArriving:______________________________");
         for(i=0; i<30; i++) printf("\b");
         gets(current->arrive_loc);

         printf("\nAt:____ (24 hour time - no colon)");
         for(i=0; i<30; i++) printf("\b");
         gets(s);
         current->arrive_time = atoi(s);
      }  /* ends else get the data */
   }  /* ends while not_done */

   return(result);
}  /* ends get_intenerarys */
/*****************************************************/
/*****************************************************/
/*PAGE get_daily_expenses 

   This function runs through the
   days of the trip and gets all
   the expenses and descriptions.
*/

struct daily_struct * get_daily_expenses(first_day, last_day, days)
   short  *days;
   struct MDY_struct *first_day, *last_day;
{
   char   s[L];
   int    j, looping = 1, not_done = 1;
   struct daily_struct *current, *new1, *result;
   struct MDY_struct dummy, next_day;

   new1 = (struct daily_struct *)
           calloc(1, sizeof(struct daily_struct));
   new1->next = END_OF_LIST;
   result     = new1;
   current    = new1;
   *days      = *days + 1;

   current->today.month = first_day->month;
   current->today.day   = first_day->day;
   current->today.year  = first_day->year;

   while(looping){
      dummy.month = current->today.month;
      dummy.day   = current->today.day;
      dummy.year  = current->today.year;
      printf("\nDaily Expense");
      show_mdy(&dummy);

      printf("\nMax Lodging at TDY Point:_____\b\b\b\b\b");
      current->max_lodging = get_money();
      
      not_done = 1;
      while(not_done){
         printf("\nM&IE at TDY Point:_____\b\b\b\b\b");
         current->mie = get_money();
                     /* change - added 3 MIE values here
                        4-15-93 - DP */
         if(current->mie != MIE1  &&
            current->mie != MIE2  &&
            current->mie != MIE3  &&
            current->mie != MIE4  &&
            current->mie != MIE5){
            printf("\nMust equal");
            show_money(MIE1);
            printf(", ");
            show_money(MIE2);
            printf(", ");
            show_money(MIE3);
            printf(", ");
            show_money(MIE4);
            printf(", ");
            show_money(MIE5);
         }  /* ends if MIE values */
         else
            not_done = 0;
      }  /* ends M&IE loop */

      printf("\nActual Lodging:_____\b\b\b\b\b");
      current->actual_lodging = get_money();

      printf("\nPOV Miles:_____\b\b\b\b\b");
      gets(s);
      current->pov_miles = atoi(s);

      printf("\nDesc:______________________________");
      for(j=0; j<30; j++) printf("\b");
      gets(current->pov_desc);

      printf("\nRental Car:______\b\b\b\b\b\b");
      current->rental_car = get_money();

      printf("\nDesc:______________________________");
      for(j=0; j<30; j++) printf("\b");
      gets(current->rental_car_desc);

      printf("\nTaxi:______\b\b\b\b\b\b");
      current->taxi = get_money();

      printf("\nDesc:______________________________");
      for(j=0; j<30; j++) printf("\b");
      gets(current->taxi_desc);

      printf("\nParking:______\b\b\b\b\b\b");
      current->parking = get_money();

      printf("\nDesc:______________________________");
      for(j=0; j<30; j++) printf("\b");
      gets(current->parking_desc);

      printf("\nOfficial Phone:______\b\b\b\b\b\b");
      current->phone = get_money();

      printf("\nDesc:______________________________");
      for(j=0; j<30; j++) printf("\b");
      gets(current->phone_desc);

      printf("\nOther:______\b\b\b\b\b\b");
      current->other = get_money();

      printf("\nDesc:______________________________");
      for(j=0; j<30; j++) printf("\b");
      gets(current->other_desc);

         /* now test for the end */
      if(current->today.month == last_day->month   &&
         current->today.day   == last_day->day     &&
         current->today.year  == last_day->year)
         looping = 0;
      else{
         new1 = (struct daily_struct *)
                 calloc(1, sizeof(struct daily_struct));
         new1->next    = END_OF_LIST;
         current->next = new1;
         current       = new1;
         *days         = *days + 1;
         tomorrow_is(&dummy, &next_day);
         current->today.month = next_day.month;
         current->today.day   = next_day.day;
         current->today.year  = next_day.year;
      }  /* ends else not done looping */

   }  /* ends loop over the days */

   return(result);

}  /* ends get_daily_expenses */
/*****************************************************/
/*****************************************************/
/*PAGE get_rental_expenses

   This function gets the personal rental
   car costs from the user.  There is only one
   struct.  This is not a linked list.
*/

struct rental_struct * get_rental_expenses()
{
   char   s[L];
   long   temp;
   struct rental_struct *result;

   result = (struct rental_struct *)
             calloc(1, sizeof(struct rental_struct));

   printf("\n");
   printf("\nYou will be asked to sign a statement");
   printf(" certifying personal");
   printf("\nuse of your vehicle.  If you did not");
   printf("\nhave any personal use of the vehicle,");
   printf("\nanswer 0 to the following three questions.");
   printf("\n");
   printf("\nPersonal Mileage:________\b\b\b\b\b\b\b\b");
   gets(s);
   result->personal_mileage = atoi(s);
   printf("\nTotal Gas:$________\b\b\b\b\b\b\b\b");
   temp = get_money();
   result->total_gas = temp;
   printf("\nTotal Rental Car Mileage:");
   printf("________\b\b\b\b\b\b\b\b");
   gets(s);
   result->total_mileage = atoi(s);
   return(result);

}  /* ends get_rental_expense */
/*****************************************************/
/*****************************************************/
/*PAGE get_first_last_dates

   This function traverses the 
   itinerary linked list and returns
   the first departure date and
   the last arrival date.
*/

get_first_last_dates(its, first_day, last_day)
   struct itinerary_struct *its;
   struct MDY_struct *first_day, *last_day;
{
   int    not_finished = 1;
   struct itinerary_struct *a;

   a = its;
   first_day->month = a->depart_date.month;
   first_day->day   = a->depart_date.day;
   first_day->year  = a->depart_date.year;
   while(not_finished){
      if(a->next == END_OF_LIST)
         not_finished = 0;
      else{
         a = a->next;
      }
   }  /* ends while */

   last_day->month = a->arrive_date.month;
   last_day->day   = a->arrive_date.day;
   last_day->year  = a->arrive_date.year;

}  /* ends get_first_last_dates */
/*****************************************************/
/*****************************************************/
/*PAGE get_date

   This function reads the date and time and
   returns the date in short form in a char array.
*/

get_date(date_string)
   char date_string[];
{
   struct tm *time_of_day;
   long   seconds;

   time(&seconds);
   time_of_day = localtime(&seconds);
   sprintf(date_string, "%d-%d-%d",
      time_of_day->tm_mon+1,
      time_of_day->tm_mday,
      time_of_day->tm_year);
}  /* ends get_date */
/*****************************************************/
/*****************************************************/
/*PAGE save_GTRs

   This function saves the GTR linked list
   to the data file.  You will always save at 
   least one struct to the data file.
*/

save_GTRs(GTR, data_file)
   FILE   *data_file;
   struct GTR_struct *GTR;
{
   struct GTR_struct *current;
   
   current = GTR;
   while(current != END_OF_LIST){
      fwrite(current,
             sizeof(struct GTR_struct), 
             1, data_file);
      current = current->next;
   }  /* ends while */
}  /* ends save_GTRs */
/*****************************************************/
/*****************************************************/
/*PAGE save_transportation_cost


   This function saves the transportation 
   cost linked list
   to the data file.  You will always save at 
   least one struct to the data file.
*/

save_transportation_cost(tc, data_file)
   FILE   *data_file;
   struct trans_struct *tc;
{
   struct  trans_struct *current;
   
   current = tc;
   while(current != END_OF_LIST){
      fwrite(current, 
             sizeof(struct trans_struct), 
             1, data_file);
      current = current->next;
   }  /* ends while */
}  /* ends save_transportation_cost */
/*****************************************************/
/*****************************************************/
/*PAGE save_itinerarys


   This function saves the itinerary 
   linked list to the data file.  
   You will always save at 
   least one struct to the data file.
*/

save_itinerarys(its, data_file)
   FILE   *data_file;
   struct itinerary_struct *its;
{
   struct itinerary_struct *current;
   
   current = its;
   while(current != END_OF_LIST){
      fwrite(current, 
             sizeof(struct itinerary_struct), 
             1, data_file);
      current = current->next;
   }  /* ends while */
}  /* ends save_itinerarys */
/*****************************************************/
/*****************************************************/
/*PAGE save_daily_expenses


   This function saves the daily expense 
   linked list to the data file.  
   You will always save at 
   least one struct to the data file.
*/

save_daily_expenses(ds, data_file)
   FILE   *data_file;
   struct daily_struct *ds;
{
   struct daily_struct *current;
   
   current = ds;
   while(current != END_OF_LIST){
      fwrite(current, 
             sizeof(struct daily_struct), 
             1, data_file);
      current = current->next;
   }  /* ends while */
}  /* ends save_daily_expenses */
/*****************************************************/
/*****************************************************/
/*PAGE save_rental_expenses

   This function saves the rental car expenses
   to the data file.  There is only one
   struct.  This is not a linked list.
*/

save_rental_expenses(rs, data_file)
   FILE   *data_file;
   struct rental_struct *rs;
{
   fwrite(rs,
          sizeof(struct rental_struct),
          1, data_file);

}  /* ends save_rental_expenses */
/*****************************************************/
/*****************************************************/
/*PAGE retrieve_GTRs

   This function reads the GTR linked list from
   disk.  You will always read at least one struct
   from disk.  It returns the pointer to the 
   front of the list.
*/

struct GTR_struct * retrieve_GTRs(data_file)
   FILE *data_file;
{
   int    first=1, not_done=1;
   struct GTR_struct *current, *new1, *result;

   while(not_done){
      new1 = (struct GTR_struct *) 
              calloc(1, sizeof(struct GTR_struct));
      new1->next = END_OF_LIST;

      if(first){
         first   = 0;
         result  = new1;
         current = new1;
      }
      else{
         current->next = new1;
         current       = new1;
      }

      fread(current, 
            sizeof(struct GTR_struct),
            1, data_file);
      if(current->next == END_OF_LIST)
         not_done = 0;
   }  /* ends while not_done */

   return(result);
}  /* ends retrieve_GTRs */
/*****************************************************/
/*****************************************************/
/*PAGE retrieve_transportation_cost

   This function reads the transportation cost 
   linked list from disk.  You will 
   always read at least one struct
   from disk.  It returns the pointer to the 
   front of the list.
*/

struct trans_struct * retrieve_transportation_cost(data_file)
   FILE *data_file;
{
   int    first=1, not_done=1;
   struct trans_struct *current, *new1, *result;

   while(not_done){
      new1 = (struct trans_struct *) 
              calloc(1, sizeof(struct trans_struct));
      new1->next = END_OF_LIST;

      if(first){
         first   = 0;
         result  = new1;
         current = new1;
      }
      else{
         current->next = new1;
         current       = new1;
      }

      fread(current, 
            sizeof(struct trans_struct),
            1, data_file);
      if(current->next == END_OF_LIST)
         not_done = 0;
   }  /* ends while not_done */

   return(result);
}  /* ends retrieve_transportation_cost */
/*****************************************************/
/*****************************************************/
/*PAGE retrieve_itinerarys

   This function reads the itinerary
   linked list from disk.  You will 
   always read at least one struct
   from disk.  It returns the pointer to the 
   front of the list.
*/

struct itinerary_struct * retrieve_itinerarys(data_file)
   FILE *data_file;
{
   int    first=1, not_done=1;
   struct itinerary_struct *current, *new1, *result;

   while(not_done){
      new1 = (struct itinerary_struct *) 
              calloc(1, sizeof(struct itinerary_struct));
      new1->next = END_OF_LIST;

      if(first){
         first   = 0;
         result  = new1;
         current = new1;
      }
      else{
         current->next = new1;
         current       = new1;
      }

      fread(current, 
            sizeof(struct itinerary_struct),
            1, data_file);
      if(current->next == END_OF_LIST)
         not_done = 0;
   }  /* ends while not_done */

   return(result);
}  /* ends retrieve_itinerarys */
/*****************************************************/
/*****************************************************/
/*PAGE retrieve_daily_expenses

   This function reads the daily expense
   linked list from disk.  You will 
   always read at least one struct
   from disk.  It returns the pointer to the 
   front of the list.
*/

struct daily_struct * retrieve_daily_expenses(data_file, days)
   FILE  *data_file;
   short *days;
{
   int    first=1, not_done=1;
   struct daily_struct *current, *new1, *result;

   while(not_done){
      new1 = (struct daily_struct *) 
              calloc(1, sizeof(struct daily_struct));
      new1->next = END_OF_LIST;

      if(first){
         first   = 0;
         result  = new1;
         current = new1;
      }
      else{
         current->next = new1;
         current       = new1;
      }

      fread(current, 
            sizeof(struct daily_struct),
            1, data_file);
      *days = *days + 1;
      if(current->next == END_OF_LIST)
         not_done = 0;
   }  /* ends while not_done */

   return(result);
}  /* ends retrieve_daily_expenses */
/*****************************************************/
/*****************************************************/
/*PAGE retrieve_rental_expenses

   This function reads the rental car expenses
   from disk.  There is only one struct on the
   disk.  This is not a linked list.
*/

struct rental_struct * retrieve_rental_expenses(data_file)
   FILE  *data_file;
{
   struct rental_struct *result;

   result = (struct rental_struct *)
             calloc(1, sizeof(struct rental_struct));
   fread(result, sizeof(struct rental_struct),
         1, data_file);
   return(result);
}  /* ends retrieve_rental_expenses */
/*****************************************************/
/*****************************************************/
/*PAGE print_accounting

   This function is the main printing routine.  It
   calls several other routines to print the complete
   final travel accounting.
*/

print_accounting(date_string, name, ORN, days, lines,
                 output_file, first_day, last_day,
                 cash_advance, GTR, ts, its, ds, rs)
   char   date_string[],
          name[],
          ORN[];
   FILE   *output_file;
   int    *lines;
   long   cash_advance;
   short  days;
   struct daily_struct     *ds;
   struct GTR_struct       *GTR;
   struct itinerary_struct *its;
   struct MDY_struct       *first_day, *last_day;
   struct trans_struct     *ts;
   struct rental_struct    *rs;
{
   long personal_use     = 0,
        total_advance    = 0,
        total_daily_cost = 0,
        total_expenses   = 0,
        total_trans_cost = 0;

   blank_line(output_file);
   *lines = *lines + 1;
   print_header(output_file);
   *lines = *lines + 1;

   blank_line(output_file);
   *lines = *lines + 1;
   blank_line(output_file);
   *lines = *lines + 1;

   print_title(ORN, name, date_string, first_day, 
               last_day, output_file);
   *lines = *lines + 4;
 
   print_advances(output_file, lines, 
                  cash_advance, GTR,
                  &total_advance);

   print_equal(5, 70, output_file);
   increment(lines, output_file);

   print_trans_cost(output_file, lines, ts,
                    &total_trans_cost);

   print_equal(5, 70, output_file);
   increment(lines, output_file);

   print_itinerary(output_file, lines, its);

   blank_line(output_file);
   increment(lines, output_file);
   print_equal(5, 70, output_file);
   increment(lines, output_file);

   print_daily(output_file, lines, 
               ds, &total_daily_cost,
               first_day, last_day,
               its);

   blank_line(output_file);
   increment(lines, output_file);
   print_equal(5, 70, output_file);
   increment(lines, output_file);

   print_rental_car(rs, output_file, lines, 
                    name, &personal_use);

   blank_line(output_file);
   increment(lines, output_file);
   print_equal(5, 70, output_file);
   increment(lines, output_file);

   total_expenses = total_trans_cost + total_daily_cost;
   total_expenses = total_expenses - personal_use;

   print_bottom_line(output_file, lines, 
                     total_advance, 
                     total_expenses,
                     total_trans_cost,
                     total_daily_cost);

   blank_line(output_file);
   increment(lines, output_file);
   print_dash(5, 70, output_file);
   increment(lines, output_file);

   print_certification(output_file, lines, name);

   pad_final_page(output_file, lines);

}  /* ends print_accounting */
/*****************************************************/
/*****************************************************/
/*PAGE print_title

   This function prints the title on the
   first page of the accounting report.
*/

print_title(ORN, name, date_string, first_day, 
            last_day, output_file)
   char   date_string[], name[], ORN[];
   FILE   *output_file;
   struct MDY_struct *first_day, *last_day;
{
   char s[L];

   sprintf(s, "ORN %s\n", ORN);
   insert_spaces(s, 40);
   fputs(s, output_file);

   sprintf(s,
     "Travel Summary for %s      Submitted %s\n",
                name, date_string);
   insert_spaces(s, 10);
   fputs(s, output_file);

   blank_line(output_file);

   sprintf(s,
     "Beginning %d-%d-%d, and Ending %d-%d-%d\n",
     first_day->month, first_day->day, first_day->year,
     last_day->month, last_day->day, last_day->year);      
   insert_spaces(s, 5);
   fputs(s, output_file);

}  /* ends print_title */
/*****************************************************/
/*****************************************************/
/*PAGE print_advances

   This function prints the cash, GTR, and total
   advances.

*/

print_advances(output_file, lines, 
               cash_advance, GTR,
               total)
   FILE   *output_file;
   int    *lines;
   long   cash_advance, *total;
   struct GTR_struct *GTR;
{
   char   s[L];
   long   t=0;
   struct GTR_struct *g;
   
   blank_line(output_file);
   increment(lines, output_file);

   sprintf(s, 
      "CASH ADVANCE               $%8.2f\n",
       dollars_of(cash_advance));
   insert_spaces(s, 25);
   fputs(s, output_file);
   increment(lines, output_file);

   g      = GTR;
   *total = *total + g->GTR;
   while(g->next != END_OF_LIST){
      g      = g->next;
      *total = *total + g->GTR;
   }  /* ends while */

   t      = *total;
   sprintf(s, 
      "GTR and TICKET ADVANCES    $%8.2f\n",
      dollars_of(t));
   insert_spaces(s, 25);
   fputs(s, output_file);
   increment(lines, output_file);

   *total = *total + cash_advance;
   t      = *total;
   sprintf(s, 
      "TOTAL ADVANCE                      $%8.2f\n",
      dollars_of(t));
   insert_spaces(s, 35);
   fputs(s, output_file);
   increment(lines, output_file);

}  /* ends print_advances */
/*****************************************************/
/*****************************************************/
/*PAGE print_trans_cost

   This function prints the transportation
   cost.

*/

print_trans_cost(output_file, lines, 
                 ts, total)
   FILE   *output_file;
   int    *lines;
   long   *total;
   struct trans_struct *ts;
{
   char s1[L], s2[L], s3[L];
   long t = 0;
   struct trans_struct *tt;

   tt = ts;
   t  = t + tt->amount;

   while(tt->next != END_OF_LIST){
      tt      = tt->next;
      t = t + tt->amount;
   }  /* ends while */

   *total = t;

   sprintf(s1, "Transportation Costs\n");
   insert_spaces(s1, 20);
   fputs(s1, output_file);
   increment(lines, output_file);
   blank_line(output_file);
   increment(lines, output_file);

   sprintf(s1, "Mode        Carrier             Class\n");
   insert_spaces(s1, 10);
   fputs(s1, output_file);
   increment(lines, output_file);
   
   print_dash(8, 60, output_file);
   increment(lines, output_file);

  tt = ts;
  while(tt != END_OF_LIST){
      sprintf(s1, "%s", tt->mode);
      append_spaces(s1, 12);
      insert_spaces(s1, 8);
      sprintf(s2, "%s", tt->carrier);
      append_spaces(s2, 20);
      sprintf(s3, "%s", tt->travel_class);
      append_spaces(s3, 10);

      strcat(s1, s2);
      strcat(s1, s3);

      sprintf(s3, "$%8.2f\n", 
              dollars_of(tt->amount));
      strcat(s1, s3);

      fputs(s1, output_file);
      increment(lines, output_file);
      tt = tt->next;
   }  /* ends while */

   blank_line(output_file);
   increment(lines, output_file);

   sprintf(s1, "$%8.2f\n", dollars_of(t));
   insert_spaces(s1, 70);
   fputs(s1, output_file);
   increment(lines, output_file);

}  /* ends print_trans_cost */
/*****************************************************/
/*****************************************************/
/*PAGE print_itinerary

   This function prints the itinerary.

*/

print_itinerary(output_file, lines, its)
   FILE   *output_file;
   int    *lines;
   struct itinerary_struct *its;
{
   char   s1[L], s2[L];
   struct itinerary_struct *ii;

   sprintf(s1, "Itinerary\n");
   insert_spaces(s1, 30);
   fputs(s1, output_file);
   increment(lines, output_file);

   blank_line(output_file);
   increment(lines, output_file);

   ii = its;
   while(ii != END_OF_LIST){

      sprintf(s1, "Date   %2d-%2d-%2d Leaving  %s",
         ii->depart_date.month,
         ii->depart_date.day,
         ii->depart_date.year,
         ii->leave_loc);
      append_spaces(s1, 40);
      insert_spaces(s1, 5);
      sprintf(s2, "  At %4ld Via %s\n",
         ii->depart_time, ii->depart_via);
      strcat(s1, s2);
      fputs(s1, output_file);
      increment(lines, output_file); 

      sprintf(s1, "       %2d-%2d-%2d Arriving %s",
         ii->arrive_date.month,
         ii->arrive_date.day,
         ii->arrive_date.year,
         ii->arrive_loc);
      append_spaces(s1, 40);
      insert_spaces(s1, 5);
      sprintf(s2, "  At %4ld\n",
         ii->arrive_time);
      strcat(s1, s2);
      fputs(s1, output_file);
      increment(lines, output_file); 

      print_dash(8, 60, output_file);
      increment(lines, output_file);

      ii = ii->next;

   }  /* ends while */

}  /* ends print_itinerary */
/*****************************************************/
/*****************************************************/
/*PAGE print_daily

   This function prints out the daily expenses
   and lodging.  It also calculates the
   expenses etc.
*/

print_daily(output_file, lines, ds, total,
               first_day, last_day,
               its)
   FILE   *output_file;
   int    *lines;
   long   *total;
   struct daily_struct     *ds;
   struct itinerary_struct *its;
   struct MDY_struct       *first_day, *last_day;
{
   char   s1[L], s2[L], s3[L], statement[L];
   long   allowed_mie  = 0,
          today        = 0, 
          pov_cost     = 0;
   struct daily_struct     *dd;
   struct itinerary_struct *ii;

   sprintf(s1, "Daily Expenses - Lodgings +\n");
   insert_spaces(s1, 20);
   fputs(s1, output_file);
   increment(lines, output_file);   

   sprintf(s1, "Date\n");
   insert_spaces(s1, 5);
   fputs(s1, output_file);
   increment(lines, output_file);   

   dd = ds;
   ii = its;

   while(dd != END_OF_LIST){

      today    = 0;
      pov_cost = 0;

      allowed_mie = 0;
      sprintf(s1, "%2d-%2d-%2d M&IE $%8.2f",
         dd->today.month, dd->today.day,
         dd->today.year, dollars_of(dd->mie));
      insert_spaces(s1, 5);

      allowed_mie = calculate_mie(dd, ii, 
                                  first_day, 
                                  last_day, 
                                  statement);
      sprintf(s2, "          %s M&IE ALLOWED", statement);
      strcat(s1, s2);
      append_spaces(s1, 59);
      sprintf(s3, "$%8.2f\n", 
         dollars_of(allowed_mie));
      strcat(s1, s3);
      fputs(s1, output_file);
      increment(lines, output_file);   

      today  = today + allowed_mie;
      *total = *total + allowed_mie;

      blank_line(output_file);
      increment(lines, output_file);   

      sprintf(s1, "Maximum Lodging $%8.2f\n",
         dollars_of(dd->max_lodging));
      insert_spaces(s1, 14);
      fputs(s1, output_file);
      increment(lines, output_file);   


      if(dd->actual_lodging > 0){
               /* change 2-22-93 - you must check
                  to ensure actual lodging does not
                  exceed max allowed lodging. */
         if(dd->actual_lodging <= dd->max_lodging){
            today  = today  + dd->actual_lodging;
            *total = *total + dd->actual_lodging;
         }
         else{
            today  = today  + dd->max_lodging;
            *total = *total + dd->max_lodging;
         }

         sprintf(s1, "Actual Lodging  $%8.2f",
            dollars_of(dd->actual_lodging));
         append_spaces(s1, 45);
         insert_spaces(s1, 14);

         if(dd->actual_lodging <= dd->max_lodging)
            sprintf(s2, "$%8.2f\n", 
               dollars_of(dd->actual_lodging));
         else
            sprintf(s2, "$%8.2f\n", 
               dollars_of(dd->max_lodging));
         strcat(s1, s2);
         fputs(s1, output_file);
         increment(lines, output_file);   
      }  /* ends if lodging > 0 */


      if(dd->pov_miles > 0){
         pov_cost = dd->pov_miles * POV_RATE /10;
         today  = today  + pov_cost;
         *total = *total + pov_cost;

         sprintf(s1, "POV - %ld miles at 0.%d per miles",
            dd->pov_miles, POV_RATE);
         append_spaces(s1, 45);
         insert_spaces(s1, 14);
         sprintf(s2, "$%8.2f\n", dollars_of(pov_cost));
         strcat(s1, s2);
         fputs(s1, output_file);
         increment(lines, output_file);   

         sprintf(s1, "Description: %s\n", dd->pov_desc);
         insert_spaces(s1, 18);
         fputs(s1, output_file);
         increment(lines, output_file);   
      }  /* ends if pov_miles > 0 */


      if(dd->rental_car > 0){
         today  = today  + dd->rental_car;
         *total = *total + dd->rental_car;

         sprintf(s1, "Rental Car");
         append_spaces(s1, 45);
         insert_spaces(s1, 14);
         sprintf(s2, "$%8.2f\n", 
            dollars_of(dd->rental_car));
         strcat(s1, s2);
         fputs(s1, output_file);
         increment(lines, output_file);   
      
         sprintf(s1, "Description: %s\n", 
            dd->rental_car_desc);
         insert_spaces(s1, 18);
         fputs(s1, output_file);
         increment(lines, output_file);   
      }  /* ends if rental_car > 0 */


      if(dd->taxi > 0){
         today  = today  + dd->taxi;
         *total = *total + dd->taxi;

         sprintf(s1, "Taxi");
         append_spaces(s1, 45);
         insert_spaces(s1, 14);
         sprintf(s2, "$%8.2f\n", dollars_of(dd->taxi));
         strcat(s1, s2);
         fputs(s1, output_file);
         increment(lines, output_file);   
      
         sprintf(s1, "Description: %s\n", 
            dd->taxi_desc);
         insert_spaces(s1, 18);
         fputs(s1, output_file);
         increment(lines, output_file);   
      }  /* ends if taxi > 0 */


      if(dd->parking > 0){
         today  = today  + dd->parking;
         *total = *total + dd->parking;

         sprintf(s1, "Parking");
         append_spaces(s1, 45);
         insert_spaces(s1, 14);
         sprintf(s2, "$%8.2f\n", dollars_of(dd->parking));
         strcat(s1, s2);
         fputs(s1, output_file);
         increment(lines, output_file);   
      
         sprintf(s1, "Description: %s\n", 
            dd->parking_desc);
         insert_spaces(s1, 18);
         fputs(s1, output_file);
         increment(lines, output_file);   
      }  /* ends if parking > 0 */


      if(dd->phone > 0){
         today  = today  + dd->phone;
         *total = *total + dd->phone;

         sprintf(s1, "Official Phone");
         append_spaces(s1, 45);
         insert_spaces(s1, 14);
         sprintf(s2, "$%8.2f\n", dollars_of(dd->phone));
         strcat(s1, s2);
         fputs(s1, output_file);
         increment(lines, output_file);   
      
         sprintf(s1, "Description: %s\n", 
            dd->phone_desc);
         insert_spaces(s1, 18);
         fputs(s1, output_file);
         increment(lines, output_file);   
      }  /* ends if phone > 0 */



      if(dd->other > 0){
         today  = today  + dd->other;
         *total = *total + dd->other;

         sprintf(s1, "Other");
         append_spaces(s1, 45);
         insert_spaces(s1, 14);
         sprintf(s2, "$%8.2f\n", dollars_of(dd->other));
         strcat(s1, s2);
         fputs(s1, output_file);
         increment(lines, output_file);   
      
         sprintf(s1, "Description: %s\n", 
            dd->other_desc);
         insert_spaces(s1, 18);
         fputs(s1, output_file);
         increment(lines, output_file);   
      }  /* ends if phone > 0 */

      sprintf(s1, "$%8.2f\n", dollars_of(today));
      insert_spaces(s1, 70);
      fputs(s1, output_file);
      increment(lines, output_file);

      print_dash(8, 60, output_file);
      increment(lines, output_file);

      dd = dd->next;
      ii = its;

   }  /* ends while */

}  /* ends print_daily */
/*****************************************************/
/*****************************************************/
/*PAGE print_rental_car

   This function prints the rental car statement.

*/

print_rental_car(rs, output_file, 
                 lines, name, personal_use)
   char   name[];
   FILE   *output_file;
   int    *lines;
   long   *personal_use;
   struct rental_struct *rs;
{
   char s1[L], s2[L];
  

   blank_line(output_file);
   increment(lines, output_file);

   sprintf(s1,
      "I hereby verify that, except as noted below,");
   sprintf(s2," no personal use was\n");
   strcat(s1, s2);
   insert_spaces(s1, 8);
   fputs(s1, output_file);
   increment(lines, output_file);

   sprintf(s1,
      "made of the rental vehicle.  *NOTE*");
   sprintf(s2,"  A detailed justification for\n");
   strcat(s1, s2);
   insert_spaces(s1, 8);
   fputs(s1, output_file);
   increment(lines, output_file);

   sprintf(s1,
      "rental of a vehicle other than Class");
   sprintf(s2," A Subcompact required.\n");
   strcat(s1, s2);
   insert_spaces(s1, 8);
   fputs(s1, output_file);
   increment(lines, output_file);

   blank_line(output_file);
   increment(lines, output_file);

   print_dash(50, 20, output_file);
   increment(lines, output_file);

   sprintf(s1, "%s\n", name);
   insert_spaces(s1, 50);
   fputs(s1, output_file);
   increment(lines, output_file);

   blank_line(output_file);
   increment(lines, output_file);

   sprintf(s1, "TOTAL GAS        $%8.2f\n", 
           dollars_of(rs->total_gas));
   insert_spaces(s1, 8);
   fputs(s1, output_file);
   increment(lines, output_file);

   sprintf(s1, "PERSONAL MILES     %ld\n",
           rs->personal_mileage);
   insert_spaces(s1, 8);
   fputs(s1, output_file);
   increment(lines, output_file);

   sprintf(s1, "TOTAL MILES        %ld\n",
           rs->total_mileage);
   insert_spaces(s1, 8);
   fputs(s1, output_file);
   increment(lines, output_file);

   blank_line(output_file);
   increment(lines, output_file);

   if(rs->total_mileage == 0) 
      *personal_use = 0;
   else
      *personal_use = (rs->total_gas * rs->personal_mileage)
                      /rs->total_mileage;

   sprintf(s1, "PERSONAL USE OF RENTAL CAR");
   append_spaces(s1, 55);
   insert_spaces(s1, 8);
   sprintf(s2, "     ($%8.2f)\n", 
           dollars_of(*personal_use));
   strcat(s1, s2);
   fputs(s1, output_file);
   increment(lines, output_file);

   blank_line(output_file);
   increment(lines, output_file);

}  /* end print_rental_car */
/*****************************************************/
/*****************************************************/
/*PAGE print_bottom_line

   This function prints the bottom line, i.e.
   the amount due the traveler or due back.
*/

print_bottom_line(output_file, lines, 
                  total_advance, 
                  total_expenses,
                  total_trans_cost, 
                  total_daily_cost)
   FILE   *output_file;
   int    *lines;
   long   total_advance, total_expenses, 
          total_trans_cost, total_daily_cost;
{
   char   s[L], s2[L];
   long   total;

   total = total_advance - total_expenses;

   sprintf(s, "TOTAL EXPENSES     $%8.2f\n",
           dollars_of(total_expenses));
   insert_spaces(s, 50);
   fputs(s, output_file);
   increment(lines, output_file);

   blank_line(output_file);
   increment(lines, output_file);

   print_dash(50, 30, output_file);
   increment(lines, output_file);

   if(total < 0){
      total = total * (-1);
      sprintf(s, "Due Traveler       $%8.2f\n",
              dollars_of(total)); 
      insert_spaces(s, 50);
      fputs(s, output_file);
      increment(lines, output_file);
      total = total * (-1);
   }
   else{
      sprintf(s, "Traveler to Owe     $%8.2f\n",
              dollars_of(total)); 
      insert_spaces(s, 50);
      fputs(s, output_file);
      increment(lines, output_file);
   }
   
   print_dash(50, 30, output_file);
   increment(lines, output_file);

   blank_line(output_file);
   increment(lines, output_file);

   blank_line(output_file);
   increment(lines, output_file);

   sprintf(s, "TRANSPORTATION PAID BY TRAVELER");
   append_spaces(s, 50);
   insert_spaces(s, 8);
   sprintf(s2, "$%8.2f\n", dollars_of(total_trans_cost));
   strcat(s, s2);
   fputs(s, output_file);
   increment(lines, output_file);

   sprintf(s, "PER DIEM AND OTHER EXPENSES");
   append_spaces(s, 50);
   insert_spaces(s, 8);
   sprintf(s2, "$%8.2f\n", dollars_of(total_daily_cost));
   strcat(s, s2);
   fputs(s, output_file);
   increment(lines, output_file);

   sprintf(s, "TOTAL EXPENSES CLAIMED");
   insert_spaces(s, 31);
   append_spaces(s, 50);
   sprintf(s2, "     $%8.2f\n", dollars_of(total_expenses));
   strcat(s, s2);
   fputs(s, output_file);
   increment(lines, output_file);

   sprintf(s, "LESS ADVANCE OF FUNDS RECEIVED");
   append_spaces(s, 50);
   insert_spaces(s, 8);
   sprintf(s2, "$%8.2f\n", dollars_of(total_advance));
   strcat(s, s2);
   fputs(s, output_file);
   increment(lines, output_file);

   sprintf(s, "BALANCE DUE TRAVELER");
   append_spaces(s, 50);
   insert_spaces(s, 8);
   if(total < 0){
      total = total * (-1);
      sprintf(s2, "$%8.2f\n", dollars_of(total));
      total = total * (-1);
   }
   else
      sprintf(s2, "$    0.00\n");
   strcat(s, s2);
   fputs(s, output_file);
   increment(lines, output_file);

   sprintf(s, "EXCESS ADVANCE TO BE REFUNDED");
   append_spaces(s, 50);
   insert_spaces(s, 8);
   if(total >= 0){
      sprintf(s2, "$%8.2f\n", dollars_of(total));
   }
   else
      sprintf(s2, "$    0.00\n");
   strcat(s, s2);
   fputs(s, output_file);
   increment(lines, output_file);


}  /* ends print_bottom_line */
/*****************************************************/
/*****************************************************/
/*PAGE print_certification

   This function prints the final certification.

*/

print_certification(output_file, lines, name)
   char   name[];
   FILE   *output_file;
   int    *lines;
{
   char s1[L], s2[L];
  

   blank_line(output_file);
   increment(lines, output_file);

   sprintf(s1,
      "I certify that this voucher and any");
   sprintf(s2," attachments are correct.\n");
   strcat(s1, s2);
   insert_spaces(s1, 8);
   fputs(s1, output_file);
   increment(lines, output_file);

   sprintf(s1,
      "Quarters and meals were not furnished");
   sprintf(s2," by the Government.  No\n");
   strcat(s1, s2);
   insert_spaces(s1, 8);
   fputs(s1, output_file);
   increment(lines, output_file);

   sprintf(s1,
      "leave was taken except as noted above");
   sprintf(s2," and no payment of credit\n");
   strcat(s1, s2);
   insert_spaces(s1, 8);
   fputs(s1, output_file);
   increment(lines, output_file);

   sprintf(s1, "has been received.");
   insert_spaces(s1, 8);
   fputs(s1, output_file);
   increment(lines, output_file);

   blank_line(output_file);
   increment(lines, output_file);

   blank_line(output_file);
   increment(lines, output_file);

   sprintf(s1, 
   "__________                    ____________________\n");
   insert_spaces(s1, 8);
   fputs(s1, output_file);
   increment(lines, output_file);

   sprintf(s1, 
      "DATE                          %s\n",
      name);
   insert_spaces(s1, 8);
   fputs(s1, output_file);
   increment(lines, output_file);



}  /* ends print_certification */   
/*****************************************************/
/*****************************************************/
/*PAGE print_dash

   This function prints dashes to the 
   output file.  It prints n leading spaces
   and m dashes.
*/

print_dash(n, m, output_file)
   FILE *output_file;
   int n, m;
{
   int i;
   char s[L];
   for(i=0; i<n; i++) s[i] = ' ';
   for(i=n; i<n+m; i++) s[i] = '-';
   s[n+m] = '\n';
   s[n+m+1] = '\0'; 
   fputs(s, output_file);
}  /* ends print_dash */
/*****************************************************/
/*****************************************************/
/*PAGE print_underscore

   This function prints dashes to the 
   output file.  It prints n leading spaces
   and m underscores.
*/

print_underscore(n, m, output_file)
   FILE *output_file;
   int n, m;
{
   int i;
   char s[L];
   for(i=0; i<n; i++) s[i] = ' ';
   for(i=n; i<n+m; i++) s[i] = '_';
   s[n+m] = '\n';
   s[n+m+1] = '\0'; 
   fputs(s, output_file);
}  /* ends print_underscore */
/*****************************************************/
/*****************************************************/
/*PAGE print_equal

   This function prints dashes to the 
   output file.  It prints n leading spaces
   and m equals.
*/

print_equal(n, m, output_file)
   FILE *output_file;
   int n, m;
{
   int i;
   char s[L];
   for(i=0; i<n; i++) s[i] = ' ';
   for(i=n; i<n+m; i++) s[i] = '=';
   s[n+m] = '\n';
   s[n+m+1] = '\0'; 
   fputs(s, output_file);
}  /* ends print_equal */
/*****************************************************/
/*****************************************************/
/*PAGE print_header

   This function prints the header
   on each page.
*/

print_header(output_file)
   FILE *output_file;
{
   char s[L];
   strcpy(s,
   "                                     SECRET\n");
   fputs(s, output_file);
}  /* ends print_header */
/*****************************************************/
/*****************************************************/
/*PAGE print_footer

   This function prints the footer
   on each page.
*/

print_footer(output_file)
   FILE *output_file;
{
   char s[L];
   strcpy(s,
   "DECL OADR                            SECRET\n");
   fputs(s, output_file);
   strcpy(s,
   "DRV FIN 2-82 BY 0488576 - CAC Travel Accounting - V1.3 Apr 1993\n");
   fputs(s, output_file);
}  /* ends print_footer */
/*****************************************************/
/*****************************************************/
/*PAGE calculate_mie

   This function calculates the allowed M&IE
   for a day in a trip.
*/

long calculate_mie(dd, ii, first_day, last_day, statement)
   char   statement[];
   struct daily_struct      *dd;
   struct itinerary_struct  *ii;
   struct MDY_struct        *first_day, *last_day;
{
   long result = 0;

   if( first_day->month == dd->today.month  &&
       first_day->day   == dd->today.day    &&
       first_day->year  == dd->today.year){

         /* at this point, ii equals the start
            of the itinerary structure linked
            list.  So, use it as is. */

      if(ii->depart_time <  600){
         result = dd->mie;
         strcpy(statement, "FULL ");
      }

      if(ii->depart_time >=  600  &&
         ii->depart_time <  1200){
         result = (((dd->mie)*3)/4);
         strcpy(statement, "3/4 ");
      }

      if(ii->depart_time >= 1200  &&
         ii->depart_time <  1800){
         result = (((dd->mie)*2)/4);
         strcpy(statement, "2/4 ");
      }

      if(ii->depart_time >= 1800  &&
         ii->depart_time <  2400){
         result = (((dd->mie)*1)/4);
         strcpy(statement, "1/4 ");
      }

      return(result);
   }  /* ends if today is the first day of the trip */ 




   if( last_day->month == dd->today.month  &&
       last_day->day   == dd->today.day    &&
       last_day->year  == dd->today.year){

         /* at this point, ii equals the start
            of the itinerary structure linked
            list.  So, travel to the end of
            the linked list and use the last
            day of the itinerary. */

      ii = traverse_itinerary(ii);

      if(ii->arrive_time > 1800){
         result = dd->mie;
         strcpy(statement, "FULL ");
      }

      if(ii->arrive_time >  1200  &&
         ii->arrive_time <= 1800){
         result = (((dd->mie)*3)/4);
         strcpy(statement, "3/4 ");
      }

      if(ii->arrive_time >   600  &&
         ii->arrive_time <= 1200){
         result = (((dd->mie)*2)/4);
         strcpy(statement, "1/2 ");
      }

      if(ii->arrive_time >     0  &&
         ii->arrive_time <=  600){
         result = (((dd->mie)*1)/4);
         strcpy(statement, "1/4 ");
      }
      return(result);
   }  /* ends if today is the last day of the trip */ 

      /* At this point, today is in the middle
         of the trip, so you get full MI&E */
   result = dd->mie;
   strcpy(statement, "FULL ");

   return(result);

}  /* ends calculate_mie */
/*****************************************************/
/*****************************************************/
/*PAGE increment

   This function increments the line counter,
   checks for end of page condition, and makes
   a new page if necessary.
*/

increment(lines, output_file)
   int  *lines;
   FILE *output_file;
{
   *lines = *lines + 1;
   if(*lines >= LPP-4)
      make_new_page(output_file, lines);
}  /* ends increment */
/*****************************************************/
/*****************************************************/
/*PAGE blank_line

   This function prints a blank line to
   the output file.
*/

blank_line(output_file)
   FILE *output_file;
{
   fputs("\n", output_file);
}  /* ends blank_line */
/*****************************************************/
/*****************************************************/
/*PAGE make_new_page

   This function prints the bottom of one page
   and the top of the next.
*/

make_new_page(output_file, lines)
   int  *lines;
   FILE *output_file;
{
   blank_line(output_file);
   blank_line(output_file);
   print_footer(output_file);
   blank_line(output_file);
   print_header(output_file);
   blank_line(output_file);
   blank_line(output_file);
   *lines = 4;
}  /* ends make_new_page */
/*****************************************************/
/*****************************************************/
/*PAGE pad_final_page

   This function pads the final page of the
   report with blank lines and a footer.
*/

pad_final_page(output_file, lines)
   FILE  *output_file;
   int   *lines;
{

   while(*lines < LPP-4){
      blank_line(output_file);
      *lines = *lines + 1;
 
  }
 
  print_footer(output_file);

}  /* ends pad_final_page */
/*****************************************************/
/*****************************************************/
/*PAGE insert_spaces

   This function inserts spaces at the
   beginning of a string.
*/

insert_spaces(s, n)
   char s[];
   int  n;
{
   char ss[L];
   int  i;
   for(i=0; i<n; i++) ss[i] = ' ';
   ss[n] = '\0';
   strcat(ss, s);
   strcpy(s, ss);
}  /* ends insert_spaces */
/*****************************************************/
/*****************************************************/
/*PAGE append_spaces

   This function appends spaces to the end of
   a string to make it a desired length.

*/

append_spaces(s, length)
   int   length;
   char  s[];
{
   int i, j;
   j = strlen(s);
   for(i=j; i<length; i++)
      s[i] = ' ';
   s[i] = '\0';
}  /* ends append_spaces */
/*****************************************************/
/*****************************************************/
/*PAGE show_mdy

   display the month-day-year structure
   to the screen.
*/

show_mdy(a)
   struct MDY_struct *a;
{
   printf("\n%2d/%2d/%2d",
           a->month, a->day, a->year);
}  /* ends show_mdy */
/*****************************************************/
/*****************************************************/
/*PAGE show_money

   This function displays a money value
   to the screen.  It takes in a long,
   converts it to a float and displays it.
*/

show_money(a)
   long a;
{
   float f;
   f = dollars_of(a);
   printf("$%7.2f", f);
}  /* ends show_money */
/*****************************************************/
/*****************************************************/
/*PAGE pennies_of

   Convert money, expressed in dollars and cents
   in a float, to pennies in a long.

*/


long pennies_of(f)
   float f;
{
   long result;
   result = 100*f;
   return(result);
}  /* ends pennies_of */
/*****************************************************/
/*****************************************************/
/*PAGE dollars_of

   Convert money, expressed in pennies in a long,
   to a dollars and cents in a float.
*/

float dollars_of(n)
   long n;
{
   float result;
   result = (float)(n)/100.0;
   return(result);
}  /* ends dollars_of */
/*****************************************************/
/*****************************************************/
/*PAGE traverse_itinerary

   This function traverses the linked
   list of intenerary_structs.  At the end,
   it returns the last node in the list.
*/

struct itinerary_struct * traverse_itinerary(its)
   struct itinerary_struct *its;
{
   struct itinerary_struct *i;
   
   i = its;

   while(i->next != END_OF_LIST)
      i = i->next;

   return(i);

}  /* ends traverse_itinerary */
/*****************************************************/
/*****************************************************/
/*PAGE tomorrow_is
 
   This function takes in a date
   structure and returns the proper
   date for tomorrow.
*/

tomorrow_is(today, tomorrow)
   struct MDY_struct *today, *tomorrow;
{
   short last_day;

   if(today->month == 1  ||
      today->month == 3  ||
      today->month == 5  ||
      today->month == 7  ||
      today->month == 8  ||
      today->month == 10 ||
      today->month == 12){
      if(today->day == 31){
         tomorrow->day   = 1;
         tomorrow->month = (today->month+1)%12;
         tomorrow->year  = today->year;
         if(today->month == 12)
            tomorrow->year = today->year+1;
      }  /* ends if last day of the month */
      else{
         tomorrow->day   = today->day + 1; 
         tomorrow->month = today->month;
         tomorrow->year  = today->year;
      }  /* ends else not the last day of the month */
   }  /* ends the 31 day months */

   if(today->month == 4  ||
      today->month == 6  ||
      today->month == 9  ||
      today->month == 11){
      if(today->day == 30){
         tomorrow->day   = 1;
         tomorrow->month = today->month+1;
         tomorrow->year  = today->year;
      }  /* ends if last day of the month */
      else{
         tomorrow->day   = today->day + 1; 
         tomorrow->month = today->month;
         tomorrow->year  = today->year;
      }  /* ends else not the last day of the month */
   }  /* ends the 30 day months */

   if(today->month == 2){
      last_day = 28;
      if(today->year == 92  ||
         today->year == 96  ||
         today->year ==  0  ||
         today->year ==  4  ||
         today->year ==  8  ||
         today->year == 16  ||
         today->year == 20  ||
         today->year == 24) last_day = 29;
      if(today->day == last_day){
          tomorrow->day   = 1;
          tomorrow->month = today->month+1;
          tomorrow->year  = today->year;
       }  /* ends if last day of the month */
       else{
          tomorrow->day   = today->day + 1; 
          tomorrow->month = today->month;
          tomorrow->year  = today->year;
       }  /* ends else not the last day of the month */
   }  /* ends February */
}  /* ends tomorrow_is */   
