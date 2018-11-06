/* See COPYRIGHT for copyright information. */

#include <inc/mmu.h>
#include <inc/memlayout.h>
#include <inc/trap.h>

#include <kern/picirq.h>

#ifdef CONFIG_KSPACE
.comm intr_ret_eip, 4
.comm intr_ebp_reg, 4
.comm intr_esp_reg, 4
.comm intr_cs, 4
.comm intr_eflags, 4
#endif

.text

.globl clock_thdlr
.type clock_thdlr, @function;
.align 2;
#ifdef CONFIG_KSPACE
clock_thdlr:
	popl intr_ret_eip
	popl intr_cs
	popl intr_eflags
	movl %ebp, intr_ebp_reg
	movl %esp, intr_esp_reg
	movl $0x0,%ebp
	movl $(bootstacktop),%esp
	pushl $GD_KD
	pushl intr_esp_reg
	pushl intr_eflags
	pushl intr_cs
	pushl intr_ret_eip
	pushl $0
	pushl $(IRQ_OFFSET + IRQ_CLOCK)
	pushl %ds
	pushl %es

	pushl %eax
	pushl %ecx
	pushl %edx
	pushl %ebx
	pushl intr_esp_reg
	pushl intr_ebp_reg
	pushl %esi
	pushl %edi

	pushl %esp  /* trap(%esp) */
	call trap
	jmp .
#else
clock_thdlr:
    jmp .
#endif
