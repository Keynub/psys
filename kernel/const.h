#ifndef PL_CONST
#define PL_CONST

#define MEM_VIDEO 0x000B8000
#define DEFAULT_CONTROL 0x0000FFFF
#define NB_LINE 25
#define NB_COL 80
#define LOW_CURSOR_POS_CMD 0x0F
#define HIGH_CURSOR_POS_CMD 0x0E
#define CURSOR_CMD_PORT 0x3D4
#define CURSOR_DATA_PORT 0x3D5

#define TABLE_VECT_INT_32 0x1000
#define CONST_VECT_INT 0x8E00

#define CLOCK_CMD_PORT 0x43
#define CLOCK_DATA_PORT 0x40
#define SET_FREQUENCY_CMD 0x34

#define QUARTZ 0x1234DD
#define CLOCKFREQ 100
#define SCHEDFREQ 50

#define MASK_DATA_PORT 0x21

#define NAME_SIZE 20
#define STACK_SIZE 512
#define MAX_NB_PROCESS 4
#endif
