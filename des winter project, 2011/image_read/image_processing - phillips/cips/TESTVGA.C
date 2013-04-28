#include "d:\cips\cips.h"




main()
{
   int i, j, k;
   char response[80];


   printf("\n\nHit return to start testVGA");
   read_string(response);

   set_64_colors();


   for(i=0; i<200; i++)
      for(j=0; j<200; j++)
	 my_set_pixel(i, j, 12 + j/5);



/**********

   for(i=0; i<400; i++){
      for(j=0; j<20; j++){
	 my_set_pixel(i,j,0);
      }
   }

   for(i=0; i<400; i++){
      for(j=20; j<40; j++){
	 my_set_pixel(i,j,1);
      }
   }

   for(i=0; i<400; i++){
      for(j=40; j<60; j++){
	 my_set_pixel(i,j,4);
      }
   }

   for(i=0; i<400; i++){
      for(j=60; j<80; j++){
	 my_set_pixel(i,j,5);
      }
   }

   for(i=0; i<400; i++){
      for(j=80; j<100; j++){
	 my_set_pixel(i,j,8);
      }
   }

   for(i=0; i<400; i++){
      for(j=100; j<120; j++){
	 my_set_pixel(i,j,2);
      }
   }

   for(i=0; i<400; i++){
      for(j=120; j<140; j++){
	 my_set_pixel(i,j,6);
      }
   }

   for(i=0; i<400; i++){
      for(j=140; j<160; j++){
	 my_set_pixel(i,j, 9);
      }
   }

   for(i=0; i<400; i++){
      for(j=160; j<180; j++){
	 my_set_pixel(i,j, 3);
      }
   }

   for(i=0; i<400; i++){
      for(j=180; j<200; j++){
	 my_set_pixel(i,j, 12);
      }
   }

   for(i=0; i<400; i++){
      for(j=200; j<220; j++){
	 my_set_pixel(i,j,13);
      }
   }

   for(i=0; i<400; i++){
      for(j=220; j<240; j++){
	 my_set_pixel(i,j, 7);
      }
   }

   for(i=0; i<400; i++){
      for(j=240; j<260; j++){
	 my_set_pixel(i,j,10);
      }
   }

   for(i=0; i<400; i++){
      for(j=260; j<280; j++){
	 my_set_pixel(i,j,11);
      }
   }

   for(i=0; i<400; i++){
      for(j=280; j<300; j++){
	 my_set_pixel(i,j,14);
      }
   }

   for(i=0; i<400; i++){
      for(j=300; j<320; j++){
	 my_set_pixel(i,j,15);
      }
   }
*************/


   read_string(response);
   clear_text_screen();





}  /* ends main */



lookup(color)
   int color;
{
   int table[16] =
	 {0, 1, 4, 5, 8, 2, 6, 9, 3, 12, 13, 7, 10, 11, 14, 15};
   return(table[color]);
}  /* ends lookup */






   /**********************************
   *
   *   Modes for the SigmaVGA Legend
   *               (hex)
   *   10 - 640x350x64?
   *   12 - 640x480x16
   *   29 - 800x600x16
   *   30 - 800x600x256
   *   38 - 1024x768x256
   *
   ***********************************/

my_set_video_mode()
{

   union REGS regs;

   regs.h.al = 0x29; /* mode */
   regs.h.ah = 0x00;
   int86(0x10, &regs, &regs);


}  /* ends my_set_video_mode */





my_set_pixel(x, y, color)
   unsigned int x, y, color;
{

union REGS regs;
char  r[80];
int i;

   regs.h.ah = 0x0c;
   regs.x.dx = y;
   regs.x.cx = x;
   regs.h.al = color;
   regs.h.bh = 0x00;

   int86(0x10, &regs, &regs);

}  /* ends my_set_pixel */




my_set_colors()
{
   _asm{
      mov ax,0013h ; 800x600x16
      int 10h

      mov ah,10h
      mov al,1bh
      mov bx,0h
      mov cx,100h
      int 10h
   }
}  /* ends my_set_colors */








put_pixel(ppx, ppy, color)
   int color, ppx, ppy;
{

int RW_Page=0x0ff;
int W_Page=0x0ff;
int R_Page=0x0ff;
int PAGE_SEL_PORT=0x3cd;
long page, offset;

page   = ((long)(ppy)*800L + (long)(ppx))/0x10000;
offset = ((long)(ppy)*800L + (long)(ppx))%0x10000;



/*printf("\ny=%d x=%d color=%d Page=%ld offset=%ld", ppy, ppx, color, page, offset);*/





   _asm{
	     ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	     ;
	     ;	 taken from file
	     ;	 d:\supervga\256col\wpixel.asm
	     ;
	     ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


	PUSH	BP			;Preserve BP
        MOV     BP,SP                   ;Preserve stack pointer

        PUSH    ES                      ;Preserve segment and index registers
        PUSH    DS
        PUSH    DI
        PUSH    SI

        ; Convert x,y pixel address to Page and Offset

	MOV	AX,ppy			;Fetch y coordinate
					;Video_Pitch=SCREEN_PITCH=800
	MUL	cs:800			;multiply by width in bytes
	ADD	AX,ppx			;add x coordinate to compute offset
	ADC	DX,0			;add overflow to upper 16 bits
					;Graph_Seg=0aoooh
	MOV	DS,CS:0a000h		;Put new address in DS:SI
	MOV	DI,AX
        MOV     AL,DL                   ;Copy page number into AL
	JMP	Select_Page		;Select proper page
Ret_Page:


        ; Set pixel to supplied value

	MOV	AL,color		;Fetch color to use
        MOV     [DI],AL                 ;Set the pixel

        ; Clean up and return

        POP     SI                      ;Restore segment and index registers
        POP     DI
        POP     DS
        POP     ES

        MOV     SP,BP                   ;Restore stack pointer
        POP     BP                      ;Restore BP

	JMP	The_End


Select_Page:
	CMP	AL,CS:RW_Page		;Check if already selected
	JNE	SP_Go
	JMP	Ret_Page
SP_Go:
	PUSH	AX
	PUSH	DX
	MOV	DX,PAGE_SEL_PORT	;Fetch address of page select
	AND	AL,7			;Force page number into 0-7
	MOV	CS:RW_Page,AL		;Save most recently selected page
	MOV	CS:R_Page,0FFh
	MOV	CS:W_Page,0FFh
	MOV	AH,AL			;Copy page into AH
	SHL	AH,1			;Shift page number
	SHL	AH,1
	SHL	AH,1
	OR	AL,AH			;Move page number into "write" bits
	OR	AL,40h			;Force bit 6
	OUT	DX,AL			;Write out the new page select
	POP	DX
	POP	AX
	JMP	Ret_Page

The_End:

   }	/* ends _asm */

}  /* ends put_pixel */







/******** This does not work
ppp(long page, long offset, int color)
{

int RW_Page=0x0ff;
int W_Page=0x0ff;
int R_Page=0x0ff;
int PAGE_SEL_PORT=0x3cd;



   _asm{

	MOV	AX,offset
	MOV	DX,page
					;Graph_Seg=0aoooh
	MOV	DS,CS:0a000h		;Put new address in DS:SI
	MOV	DI,AX
        MOV     AL,DL                   ;Copy page number into AL
	JMP	Select_Page		;Select proper page
Ret_Page:


        ; Set pixel to supplied value

	MOV	AL,color		;Fetch color to use
        MOV     [DI],AL                 ;Set the pixel

        ; Clean up and return

        POP     SI                      ;Restore segment and index registers
        POP     DI
        POP     DS
        POP     ES

        MOV     SP,BP                   ;Restore stack pointer
        POP     BP                      ;Restore BP

	JMP	The_End


Select_Page:
	CMP	AL,CS:RW_Page		;Check if already selected
	JNE	SP_Go
	JMP	Ret_Page
SP_Go:
	PUSH	AX
	PUSH	DX
	MOV	DX,PAGE_SEL_PORT	;Fetch address of page select
	AND	AL,7			;Force page number into 0-7
	MOV	CS:RW_Page,AL		;Save most recently selected page
	MOV	CS:R_Page,0FFh
	MOV	CS:W_Page,0FFh
	MOV	AH,AL			;Copy page into AH
	SHL	AH,1			;Shift page number
	SHL	AH,1
	SHL	AH,1
	OR	AL,AH			;Move page number into "write" bits
	OR	AL,40h			;Force bit 6
	OUT	DX,AL			;Write out the new page select
	POP	DX
	POP	AX
	JMP	Ret_Page

The_End:
   }

}
******************/






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







clear_text_screen()
{
   _setvideomode(_TEXTC80);      /* MSC 6.0 statements */
   _setbkcolor(1);
   _settextcolor(7);
   _clearscreen(_GCLEARSCREEN);
}  /* ends clear_text_screen */






set_64_colors()
{

 union REGS regs;
 int i;


      /*********************
	 this works for 256 color modes
	 it does not work for 16 color modes
      *********************/

   _asm{

      mov   ax,0013h ; set graphics mode 13 = 320x200x256
      int   10h

      mov ax,0h
      mov dx,3c8h ; select first DAC register

      out dx,al
      inc dx	  ; set DAC data register

      mov al,0h
      out dx,al
      out dx,al
      out dx,al

      mov al,1h
      out dx,al
      out dx,al
      out dx,al

      mov al,2h
      out dx,al
      out dx,al
      out dx,al

      mov al,3h
      out dx,al
      out dx,al
      out dx,al

      mov al, 4h
      out dx,al
      out dx,al
      out dx,al

      mov al, 5h
      out dx,al
      out dx,al
      out dx,al

      mov al, 6h
      out dx,al
      out dx,al
      out dx,al

      mov al, 7h
      out dx,al
      out dx,al
      out dx,al

      mov al, 8h
      out dx,al
      out dx,al
      out dx,al

      mov al, 9h
      out dx,al
      out dx,al
      out dx,al

      mov al, ah
      out dx,al
      out dx,al
      out dx,al

      mov al, bh
      out dx,al
      out dx,al
      out dx,al

      mov al, ch
      out dx,al
      out dx,al
      out dx,al

      mov al, dh
      out dx,al
      out dx,al
      out dx,al

      mov al,0eh
      out dx,al
      out dx,al
      out dx,al

      mov al,0fh
      out dx,al
      out dx,al
      out dx,al


      mov al,10h
      out dx,al
      out dx,al
      out dx,al

      mov al,11h
      out dx,al
      out dx,al
      out dx,al

      mov al,12h
      out dx,al
      out dx,al
      out dx,al

      mov al,13h
      out dx,al
      out dx,al
      out dx,al

      mov al,14h
      out dx,al
      out dx,al
      out dx,al

      mov al,15h
      out dx,al
      out dx,al
      out dx,al


      mov al,16h
      out dx,al
      out dx,al
      out dx,al

      mov al,17h
      out dx,al
      out dx,al
      out dx,al

      mov al,18h
      out dx,al
      out dx,al
      out dx,al

      mov al,19h
      out dx,al
      out dx,al
      out dx,al

      mov al,1ah
      out dx,al
      out dx,al
      out dx,al

      mov al,1bh
      out dx,al
      out dx,al
      out dx,al


      mov al,1ch
      out dx,al
      out dx,al
      out dx,al

      mov al,1dh
      out dx,al
      out dx,al
      out dx,al

      mov al,1eh
      out dx,al
      out dx,al
      out dx,al

      mov al,1fh
      out dx,al
      out dx,al
      out dx,al

      mov al,20h
      out dx,al
      out dx,al
      out dx,al

      mov al,21h
      out dx,al
      out dx,al
      out dx,al


      mov al,22h
      out dx,al
      out dx,al
      out dx,al

      mov al,23h
      out dx,al
      out dx,al
      out dx,al

      mov al,24h
      out dx,al
      out dx,al
      out dx,al

      mov al,25h
      out dx,al
      out dx,al
      out dx,al

      mov al,26h
      out dx,al
      out dx,al
      out dx,al

      mov al,27h
      out dx,al
      out dx,al
      out dx,al


      mov al,28h
      out dx,al
      out dx,al
      out dx,al

      mov al,29h
      out dx,al
      out dx,al
      out dx,al

      mov al,2ah
      out dx,al
      out dx,al
      out dx,al

      mov al,2bh
      out dx,al
      out dx,al
      out dx,al

      mov al,2ch
      out dx,al
      out dx,al
      out dx,al

      mov al,2dh
      out dx,al
      out dx,al
      out dx,al


      mov al,2eh
      out dx,al
      out dx,al
      out dx,al

      mov al,2fh
      out dx,al
      out dx,al
      out dx,al

      mov al,30h
      out dx,al
      out dx,al
      out dx,al

      mov al,31h
      out dx,al
      out dx,al
      out dx,al

      mov al,32h
      out dx,al
      out dx,al
      out dx,al

      mov al,33h
      out dx,al
      out dx,al
      out dx,al

      mov al,34h
      out dx,al
      out dx,al
      out dx,al

      mov al,35h
      out dx,al
      out dx,al
      out dx,al

      mov al,36h
      out dx,al
      out dx,al
      out dx,al

      mov al,37h
      out dx,al
      out dx,al
      out dx,al

      mov al,38h
      out dx,al
      out dx,al
      out dx,al


      mov al,39h
      out dx,al
      out dx,al
      out dx,al

      mov al,3ah
      out dx,al
      out dx,al
      out dx,al

      mov al,3bh
      out dx,al
      out dx,al
      out dx,al

      mov al,3ch
      out dx,al
      out dx,al
      out dx,al

      mov al,3dh
      out dx,al
      out dx,al
      out dx,al

      mov al,3eh
      out dx,al
      out dx,al
      out dx,al

      mov al,3fh
      out dx,al
      out dx,al
      out dx,al


   } /* ends asm */



}  /* ends my_set_colors */
