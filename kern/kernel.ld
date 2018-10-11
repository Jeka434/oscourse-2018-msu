/* Simple linker script for the JOS kernel.
   See the GNU ld 'info' manual ("info ld") to learn the syntax. */

OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(_start)

SECTIONS
{
	/* Link the kernel at this address: "." means the current address */
	/* Note, KASAN shadow is loaded at 0xFA000000 in GNUMakefile, keep them in sync */
	. = 0xE0100000;

	/* AT(...) gives the load address of this section, which tells
	   the boot loader where to load the kernel in physical memory */
	.text : AT(0x100000) {
		__text_start = .;
		*(.text .stub .text.* .gnu.linkonce.t.*)
		. = ALIGN(4);
		__text_end = .;
	}

	PROVIDE(etext = .);	/* Define the 'etext' symbol to this value */

	.rodata : {
		__rodata_start = .;
		*(.rodata .rodata.* .gnu.linkonce.r.* .data.rel.ro.local)
		. = ALIGN(4);
		__rodata_end = .;
	}

	/* Include debugging information in kernel memory */
	.stab : {
		PROVIDE(__STAB_BEGIN__ = .);
		*(.stab);
		PROVIDE(__STAB_END__ = .);
		BYTE(0)		/* Force the linker to allocate space
				   for this section */
	}

	.stabstr : {
		PROVIDE(__STABSTR_BEGIN__ = .);
		*(.stabstr);
		PROVIDE(__STABSTR_END__ = .);
		BYTE(0)		/* Force the linker to allocate space
				   for this section */

		/* Ensure page-aligned segment size */
		. = ALIGN(0x1000);
	}

	/* Adjust the address for the data segment to the next page */
	. = ALIGN(0x1000);

	/* The data segment */
	.data : {
		__data_start = .;
		*(.data .data.rel .data.rel.local .got .got.plt)
		. = ALIGN(4);
		__data_end = .;
	}

	.ctors :
	{
		__ctors_start = .;
		KEEP(*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)))
		KEEP(*(.init_array .ctors))
		__ctors_end = .;
		. = ALIGN(4);
	}

	.dtors :
	{
		__dtors_start = .;
		KEEP(*(SORT_BY_INIT_PRIORITY(.fini_array.*) SORT_BY_INIT_PRIORITY(.dtors.*)))
		KEEP(*(.fini_array .dtors))
		__dtors_end = .;
		. = ALIGN(4);
	}

	PROVIDE(edata = .);

	.bss : {
		__bss_start = .;
		*(.bss)
		*(COMMON)
		/* Ensure page-aligned segment size */
		. = ALIGN(0x1000);
		__bss_end = .;
	}

	PROVIDE(end = .);

	/DISCARD/ : {
		*(.eh_frame .note.GNU-stack)
	}
}
