
#include <stdio.h>
#define Y 300
print_huge(a)
   short a[Y][Y];
{
   int i;

   printf("\n\nthis is huge part 2");
	for(i=Y-10; i<Y; i++)
	   printf("\na[%d][%d]=%d", i, i, a[i][i]);
}
