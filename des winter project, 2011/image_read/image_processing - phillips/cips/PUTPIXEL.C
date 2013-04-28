
put_pixel(int ppx, int ppy, int color)
{
   _asm
   {

      mov ax,ppy    ; put x y in position for
      mov bx,ppx    ; address calculation
		    ; from svPutPixel

      mov dx,800
      mul dx	    ; mult y*bytes_per_row
      add bx,ax     ; add in x coordinate
      adc dx,0	    ; es:bx = byte address of pixel
		    ; dx    = bank number
		    ; this is from svPixelAddr2D

      mov ax,0A000H
      mov es,ax     ; load es with EGAseg 0xA000
		    ; this does @@LoadSeg es,EGAseg

		    ;@TsengSeg dl
      push ax
      mov al,dl
      mov ah,al
      shl ah,1
      shl ah,1
      shl ah,1
      or al,ah
      or al,01000000B
		       ;@Port VGAsegsel,al
		       ; set a port to a value
		       ; put value in al
		       ; put port number in dx
      mov dx,03CDH
      out dx,al

      pop ax


      mov ax,color
      mov es:[bx],ax ; set the pixel value




   }  /* ends _asm */

}  /* ends put_pixel */
