#include <inc/lib.h>

#ifndef PTE_COW
#define PTE_COW 0x800
#endif    // PTE_COW
#ifndef PTE_SHARE
#define PTE_SHARE 0x400
#endif    // PTE_SHARE

void
memlayout(void)
{
	pte_t *pgtab;
	int i, j;
	int total_p = 0;
	int total_u = 0;
	int total_w = 0;
	int total_cow = 0;

	cprintf("EID: %d, PEID: %d\n", thisenv->env_id, thisenv->env_parent_id);
	cprintf("pgdir: %p, uvpd: %p, uvpt: %p\n",
			thisenv->env_pgdir, uvpd, uvpt);

	for (i = 0; i < NPDENTRIES; i++) {
		if ((uvpd[i] & PTE_P) == 0)
			continue;
		if (i >= PDX(KERNBASE))
			break;
		pgtab = (pte_t *)uvpt + i * NPTENTRIES;
		for (j = 0; j < NPTENTRIES; j++) {
			if (pgtab[j] == 0)
				continue;
			cprintf("[%p] %p -> %08x: %c %c %c |%s%s\n",
					pgtab + j, PGADDR(i, j, 0), pgtab[j],
					(pgtab[j] & PTE_P) ? total_p++, 'P' : '-',
					(pgtab[j] & PTE_U) ? total_u++, 'U' : '-',
					(pgtab[j] & PTE_W) ? total_w++, 'W' : '-',
					(pgtab[j] & PTE_COW) ? total_cow++, " COW" : "",
					(pgtab[j] & PTE_SHARE) ? " SHARE" : "");
		}
	}

	cprintf("Memory usage summary:\n");
	cprintf("  PTE_P: %d\n", total_p);
	cprintf("  PTE_U: %d\n", total_u);
	cprintf("  PTE_W: %d\n", total_w);
	cprintf("  PTE_COW: %d\n", total_cow);
}

void
umain(int argc, char *argv[])
{
	envid_t ceid;
	int pipefd[2];
	int res;

	memlayout();

	res = pipe(pipefd);
	if (res < 0)
		panic("pipe() failed\n");
	ceid = fork();
	if (ceid < 0)
		panic("fork() failed\n");

	if (ceid == 0) {
		// Child environment
		int i;
		cprintf("\n");
		for (i = 0; i < 102400; i++)
			sys_yield();
		cprintf("==== Child\n");
		memlayout();
		return;
	}

	cprintf("==== Parent\n");
	memlayout();

}
