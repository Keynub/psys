#include "fake.h"
#include "ecran.h"


unsigned long cons_read(char *string, unsigned long length){string = string; length = length;return 0;}
void cons_echo(int on){on = on;}
int sys_info(){return 0;}
