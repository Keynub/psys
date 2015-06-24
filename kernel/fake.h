#ifndef FAKE_H
#define FAKE_H

int cons_write(const char *str, long size);
unsigned long cons_read(char *string, unsigned long length);
void cons_echo(int on);
void exit(int retval);
int sys_info();

#endif
