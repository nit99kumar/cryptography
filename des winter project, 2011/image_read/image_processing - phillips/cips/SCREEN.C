
#include <graph.h>

main(int argc, char *argv[])
{
   if(argc == 1){
      _setvideomode(_TEXTC80);      /* MSC 6.0 statements */
      _setbkcolor(7);
      _settextcolor(0);
      _clearscreen(_GCLEARSCREEN);
      exit(1);
   }

   if(argc == 2){
      printf("\nusage: screen [background color] [text colot]\n");
      printf("\nWhere the colors are:");
      printf("\n0-black 1-blue 2-green 3-cyan 4-red 5-magenta 6-brown");
      printf("\n7-white 8-dark gray 9-light blue 10-light green");
      printf("\n11-light cyan 12-light red 13-light magenta");
      printf("\n14-yellow 15-bright white");
      exit(2);
   }

   if(argc == 3){
      _setvideomode(_TEXTC80);      /* MSC 6.0 statements */
      _setbkcolor(atoi(argv[1]));
      _settextcolor(atoi(argv[2]));
      _clearscreen(_GCLEARSCREEN);
      exit(1);
      
   }
}
