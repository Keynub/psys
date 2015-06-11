#include "const.h"

.text
.globl ptr_mem
.globl ecrit_car
.globl place_curseur

/*
uint16_t * ptr_mem(uint32_t lig, uint32_t col) 
{
    uint32_t ptr = MEM_VIDEO;
    ptr += 2*(lig*NB_COL + col);
    return (uint16_t *) ptr;    
}
*/

ptr_mem :
    enter $0, $0
    movl $NB_COL, %eax
    mull 8(%ebp)
    addl 12(%ebp), %eax
    shll $1, %eax
    addl $MEM_VIDEO, %eax
    leave
    ret


/*
void ecrit_car(uint32_t lig, uint32_t col, char c) 
{
    uint16_t * ptr = ptr_mem(lig, col);
    // little endian : chars de contrôle aux grands indices
    *ptr = c | (cf << 12) | (ct << 8);
}
*/

ecrit_car:
    /* en x86_32, le cadre est pour variables locales seulement : passage de paramètres par push standard 
        ici, pas de variables locales hors registre : taille du cadre de pile = 0*/
    enter $0, $0
    pushl 12(%ebp)
    pushl 8(%ebp)
    call ptr_mem
    addl $8, %esp
    movl %eax, %ecx
    movb 16(%ebp), %al
    movb cf, %ah
    shlb $4, %ah
    orb ct, %ah
    movw %ax, (%ecx)
    leave
    ret

/*
void place_curseur(uint32_t lig, uint32_t col)
{
    cursor_col = col;
    cursor_lig = lig;
    uint16_t pos = lig * NB_COL + col;
    uint8_t pos_low = pos % 256;
    uint8_t pos_hi = pos >> 8;
    
   outb(LOW_CURSOR_POS_CMD, CURSOR_CMD_PORT);
   outb(pos_low, CURSOR_DATA_PORT);
   outb(HIGH_CURSOR_POS_CMD, CURSOR_CMD_PORT);
   outb(pos_hi, CURSOR_DATA_PORT);
}
*/

place_curseur:
    enter $0, $0
    movl 12(%ebp), %ecx
    movl %ecx, cursor_col
    movl 8(%ebp), %ecx
    movl %ecx, cursor_lig
    movl $NB_COL, %eax
    mull 8(%ebp)
    addl 12(%ebp), %eax
    movb %al, %cl // pos_low
    // pos_hi dans %ah

    movb $LOW_CURSOR_POS_CMD, %al
    movw $CURSOR_CMD_PORT, %dx
    outb %al, %dx

    movb %cl, %al
    movw $CURSOR_DATA_PORT, %dx
    outb %al, %dx

    movb $HIGH_CURSOR_POS_CMD, %al
    movw $CURSOR_CMD_PORT, %dx
    outb %al, %dx

    movb %ah, %al
    movw $CURSOR_DATA_PORT, %dx
    outb %al, %dx

    leave
    ret
