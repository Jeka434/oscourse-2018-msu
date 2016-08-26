/* See COPYRIGHT for copyright information. */

#ifndef JOS_KERN_KCLOCK_H
#define JOS_KERN_KCLOCK_H
#ifndef JOS_KERNEL
# error "This is a JOS kernel header; user programs should not #include it"
#endif

#define	IO_RTC_CMND		0x070		/* RTC control port */
#define	IO_RTC_DATA		0x071		/* RTC data port */

#define RTC_AREG	0x0A
#define RTC_BREG	0x0B
#define RTC_CREG	0x0C
#define RTC_DREG	0x0D

#define RTC_PIE		0x40
#define RTC_AIE		0x20
#define RTC_UIE		0x10

void rtc_init(void);
uint8_t rtc_check_status(void);

#endif	// !JOS_KERN_KCLOCK_H
