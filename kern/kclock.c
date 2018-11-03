/* See COPYRIGHT for copyright information. */

#include <inc/x86.h>
#include <kern/kclock.h>

void
rtc_init(void)
{
	nmi_disable();
	// LAB 4: your code here
	uint8_t A, B;
	outb(IO_RTC_CMND, RTC_BREG);
	B = inb(IO_RTC_DATA);
	B |= RTC_PIE;
	outb(IO_RTC_DATA, B);
	outb(IO_RTC_CMND, RTC_AREG);
	A = inb(IO_RTC_DATA);
	A |= 0xF;
	outb(IO_RTC_DATA, A);
	nmi_enable();
}

uint8_t
rtc_check_status(void)
{
	uint8_t status;
	// LAB 4: your code here
	outb(IO_RTC_CMND, RTC_CREG);
	status = inb(IO_RTC_DATA);
	return status;
}

