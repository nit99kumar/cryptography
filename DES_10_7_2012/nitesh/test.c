#include <stdio.h>
#include <stdlib.h>

typedef struct name1
{
	char *n1;
}
name1;

typedef struct name2
{
	char *n2;
}
name2;

int main()
{
	name1 *a;
	name2 *b;
	
	a->n1="nitesh";
	b->n2=(char *) malloc ( sizeof(a->n1) );
	b->n2=a->n1;
	printf("%s", b->n2);

	return 0;
}
