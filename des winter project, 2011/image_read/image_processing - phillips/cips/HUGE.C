
#include <stdio.h>
#define X 300

int a[X][X];
int b[X][X];

main()
{
	int i, j;

printf("\nhello start");

	for(i=0; i<X; i++)
	   for(j=0; j<X; j++)
	      a[i][j] = i%100;

	for(i=X-10; i<X; i++)
	   printf("\na[%d][%d]=%d", i, i, a[i][i]);

	print_huge(a);

printf("\nhello end");
}
