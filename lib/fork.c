// implement fork from user space

#include <inc/string.h>
#include <inc/lib.h>

// PTE_COW marks copy-on-write page table entries.
// It is one of the bits explicitly allocated to user processes (PTE_AVAIL).
#define PTE_COW		0x800

//
// Custom page fault handler - if faulting page is copy-on-write,
// map in our own private writable copy.
//
static void
pgfault(struct UTrapframe *utf)
{
	void *addr = (void *) utf->utf_fault_va;addr=addr;
	uint32_t err = utf->utf_err;err=err;
	int err0;

	// Check that the faulting access was (1) a write, and (2) to a
	// copy-on-write page.  If not, panic.
	// Hint:
	//   Use the read-only page table mappings at uvpt
	//   (see <inc/memlayout.h>).

	// LAB 9: Your code here.
	if (!(err & FEC_WR || uvpt[PGNUM(addr)] & PTE_COW)) {
		panic("pgfault addr=%p, err=%d, pte=%x", addr, err, uvpt[PGNUM(addr)]);
	}

	// Allocate a new page, map it at a temporary location (PFTEMP),
	// copy the data from the old page to the new page, then move the new
	// page to the old page's address.
	// Hint:
	//   You should make three system calls.
	//   No need to explicitly delete the old page's mapping.
	//   Make sure you DO NOT use sanitized memcpy/memset routines when using UASAN.

	// LAB 9: Your code here.
	if ((err0 = sys_page_alloc(0, PFTEMP, PTE_W | PTE_U)) < 0) {
		panic("pgfault error %d", err0);
	}
	memcpy(PFTEMP, ROUNDDOWN(addr, PGSIZE), PGSIZE);
	if ((err0 = sys_page_map(0, PFTEMP, 0, ROUNDDOWN(addr, PGSIZE), PTE_U | PTE_W)) < 0) {
		panic("pgfault error %d", err0);
	}
	sys_page_unmap(0, PFTEMP);
}

//
// Map our virtual page pn (address pn*PGSIZE) into the target envid
// at the same virtual address.  If the page is writable or copy-on-write,
// the new mapping must be created copy-on-write, and then our mapping must be
// marked copy-on-write as well.  (Exercise: Why do we need to mark ours
// copy-on-write again if it was already copy-on-write at the beginning of
// this function?)
//
// Returns: 0 on success, < 0 on error.
// It is also OK to panic on error.
//
static int
duppage(envid_t envid, unsigned pn)
{
	// LAB 9: Your code here.
	int err;
	void *addr;

	addr = (void *) (pn * PGSIZE);
	if (uvpt[pn] & PTE_SHARE) {
		err = sys_page_map(0, addr, envid, addr, uvpt[pn] & PTE_SYSCALL);
		return err;
	}
	if (uvpt[pn] & PTE_COW || uvpt[pn] & PTE_W) {
		if ((err = sys_page_map(0, addr, envid, addr, PTE_COW)) < 0) {
			return err;
		}
		return sys_page_map(0, addr, 0, addr, PTE_COW);
	}
	return sys_page_map(0, addr, envid, addr, 0);
}

//
// User-level fork with copy-on-write.
// Set up our page fault handler appropriately.
// Create a child.
// Copy our address space and page fault handler setup to the child.
// Then mark the child as runnable and return.
//
// Returns: child's envid to the parent, 0 to the child, < 0 on error.
// It is also OK to panic on error.
//
// Hint:
//   Use uvpd, uvpt, and duppage.
//   Remember to fix "thisenv" in the child process.
//   Neither user exception stack should ever be marked copy-on-write,
//   so you must allocate a new page for the child's user exception stack.
//
envid_t
fork(void)
{
	// LAB 9: Your code here.
	int err;
	size_t i, j, pn;
	envid_t child;

	set_pgfault_handler(pgfault);
	if (!(child = sys_exofork())) {
		thisenv = &envs[ENVX(sys_getenvid())];
		return 0;
	}
	if (child > 0) {
		for (i = 0; i < PGSIZE / sizeof(pde_t); i++) {
			if (!(uvpd[i] & PTE_P)) {
				continue;
			}
			for (j = 0; j < PGSIZE / sizeof(pte_t); j++) {
				pn = PGNUM(PGADDR(i, j, 0));
				if (pn < PGNUM(UTOP) && pn != PGNUM(UXSTACKTOP - PGSIZE) && uvpt[pn] & PTE_P) {
					if ((err = duppage(child, pn)) < 0) {
						return err;
					}
				}
			}
		}

		if ((err = sys_env_set_pgfault_upcall(child, thisenv->env_pgfault_upcall)) < 0 ||
			(err = sys_page_alloc(child, (void*) UXSTACKTOP - PGSIZE, PTE_W | PTE_U)) < 0 ||
			(err = sys_env_set_status(child, ENV_RUNNABLE)) < 0) {
			return err;
		}
	}
	return child;
/*

// Duplicating shadow addresses is insane. Make sure to skip shadow addresses in COW above.

#ifdef SANITIZE_USER_SHADOW_BASE
	for (addr = SANITIZE_USER_SHADOW_BASE; addr < SANITIZE_USER_SHADOW_BASE +
		SANITIZE_USER_SHADOW_SIZE; addr += PGSIZE)
		if (sys_page_alloc(p, (void *)addr, PTE_P | PTE_U | PTE_W))
			panic("Fork: failed to alloc shadow base page");
	for (addr = SANITIZE_USER_EXTRA_SHADOW_BASE; addr < SANITIZE_USER_EXTRA_SHADOW_BASE +
		SANITIZE_USER_EXTRA_SHADOW_SIZE; addr += PGSIZE)
		if (sys_page_alloc(p, (void *)addr, PTE_P | PTE_U | PTE_W))
			panic("Fork: failed to alloc shadow extra base page");
	for (addr = SANITIZE_USER_FS_SHADOW_BASE; addr < SANITIZE_USER_FS_SHADOW_BASE +
		SANITIZE_USER_FS_SHADOW_SIZE; addr += PGSIZE)
		if (sys_page_alloc(p, (void *)addr, PTE_P | PTE_U | PTE_W))
			panic("Fork: failed to alloc shadow fs base page");
#endif

*/


}

// Challenge!
int
sfork(void)
{
	panic("sfork not implemented");
	return -E_INVAL;
}
