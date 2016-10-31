// buggy program - causes a divide by zero exception

#include <inc/lib.h>

void
umain(int argc, char **argv)
{
	int result;
	__asm__ volatile (
		"xorl %%eax, %%eax;"
		"inc %%eax;"
		"xorl %%edx, %%edx;"
		"divl %%edx;"
		"movl %%eax, %0;"
		:"=r"(result)
		:
		:"%eax", "%edx"
		);

	cprintf("1/0 is %08x!\n", result);
}

