/* Simple linker script for JOS user-level programs.
   See the GNU ld 'info' manual ("info ld") to learn the syntax. */

OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(_start)

SECTIONS
{
	/* Load programs at this address: "." means the current address */
	/* Note, UASAN shadow is loaded at 0xC000000 in GNUMakefile, keep them in sync */
	. = 0x800020;

	.text : {
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

	/* Adjust the address for the data segment to the next page */
	. = ALIGN(0x1000);

	.data : {
		__data_start = .;
		*(.data .data.rel.local .got .got.plt)
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

	/* Place debugging symbols so that they can be found by
	 * the kernel debugger.
	 * Specifically, the four words at 0x200000 mark the beginning of
	 * the stabs, the end of the stabs, the beginning of the stabs
	 * string table, and the end of the stabs string table, respectively.
	 */

	/* FIXME: lld cannot handle this, aligning to a page boundary. */

	. += 0x1000;
	. = ALIGN(0x1000);

	.stab_info /* 0x200000 */ : {
		LONG(__STAB_BEGIN__);
		LONG(__STAB_END__);
		LONG(__STABSTR_BEGIN__);
		LONG(__STABSTR_END__);
	}

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
	}

	/DISCARD/ : {
		*(.eh_frame .note.GNU-stack .comment)
	}
}
