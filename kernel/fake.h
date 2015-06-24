#ifndef FAKE_H
#define FAKE_H

void clock_settings(unsigned long *quartz, unsigned long *ticks);
unsigned long current_clock();

unsigned long cons_read(char *string, unsigned long length);
void cons_echo(int on);
int sys_info();

#endif
