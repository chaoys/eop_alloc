#if 1
#include "eop_alloc.h"
#else
#include <stdlib.h>
#define eop_malloc malloc
#define eop_free free
#define eop_calloc calloc
#endif

int main(void)
{
	int *a = eop_malloc(sizeof(*a));
	if (a) {
		*a = 8;
		*(a+1) = 16;
		eop_free(a);
	}
	return 0;
}
