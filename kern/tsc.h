/* See COPYRIGHT for copyright information. */

#ifndef JOS_KERN_TSC_H
#define JOS_KERN_TSC_H
#ifndef JOS_KERNEL
# error "This is a JOS kernel header; user programs should not #include it"
#endif

void tsc_calibrate(void);
void timer_start(void);
void timer_stop(void);

#endif	// !JOS_KERN_TSC_H
