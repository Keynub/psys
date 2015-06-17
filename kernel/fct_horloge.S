#include "const.h"
    .text
    .globl traitant_IT_32
traitant_IT_32:
    // sauvegarde des registres importants
    push %eax
    push %ecx
    push %edx

    // acquittement de l'IRQ
    movw $0x20, %dx
    movb $0x20, %al
    outb %al, %dx

    // appel du traitant en C
    call tic_PIT

    // restauration des registres importants
    pop %edx
    pop %ecx
    pop %eax

    // fin du traitant
    iret


/*
void regler_frequence_horloge()
{
    uint16_t freq = QUARTZ / CLOCKFREQ;
    uint8_t freq_low = freq % 256;
    uint8_t freq_hi = freq >> 8;
    
   outb(SET_FREQUENCY_CMD, CLOCK_CMD_PORT);
   outb(freq_low, CLOCK_DATA_PORT);
   outb(freq_hi, CLOCK_DATA_PORT);
}
*/
    .globl regler_frequence_horloge
regler_frequence_horloge:
    enter $0, $0
    movl $QUARTZ, %edx
    movw %dx, %ax
    shr $16, %edx
    /* QUARTZ in dx:ax */
    movw $CLOCKFREQ, %cx
    divw %cx
    /* freq_low in %al
        freq_hi in %ah */
    movb %al, %cl

    movw $CLOCK_CMD_PORT, %dx
    movb $SET_FREQUENCY_CMD, %al
    outb %al, %dx

    movb $CLOCK_DATA_PORT, %dx
    movb %cl, %al
    outb %al, %dx

    movb %ah, %al
    outb %al, %dx

    leave
    ret


/*
void demasque_IRQ()
{
    uint8_t mask;
    mask = inb(MASK_DATA_PORT);
    mask = mask & 0xFE;
    outb(mask, MASK_DATA_PORT);
}
*/
    .globl demasque_IRQ
demasque_IRQ:
    enter $0, $0
    movw $MASK_DATA_PORT, %dx
    inb %dx, %al
    andb $0xFE, %al
    outb %al, %dx
    
    leave
    ret

