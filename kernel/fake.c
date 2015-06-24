#include "fake.h"
#include "ecran.h"

void clock_settings(unsigned long *quartz, unsigned long *ticks){quartz = ticks;}
unsigned long current_clock(){return 0;}
void wait_clock(unsigned long clock){clock = clock;}

unsigned long cons_read(char *string, unsigned long length){string = string; length = length;return 0;}
void cons_echo(int on){on = on;}
int sys_info(){return 0;}
