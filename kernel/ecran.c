#include <inttypes.h>
#include <cpu.h>
#include <string.h>
#include <stdio.h>
#include "const.h"


enum {NOIR, BLEU, VERT, CYAN, ROUGE, MAGENTA, MARRON, GRIS, GRIS_FONCE, BLEU_CLAIR,
            VERT_CLAIR, CYAN_CLAIR, ROUGE_CLAIR, MAGENTA_CLAIR, JAUNE, BLANC };


uint8_t cf = NOIR; // COULEUR FOND
uint8_t ct = BLANC; // couleur texte

uint8_t cursor_col = 0;
uint8_t cursor_lig = 0;

/*uint16_t * ptr_mem(uint32_t lig, uint32_t col) 
{
    uint32_t ptr = MEM_VIDEO;
    ptr += 2*(lig*NB_COL + col);
    return (uint16_t *) ptr;    
}*/
uint16_t * ptr_mem(uint32_t lig, uint32_t col);

/*void ecrit_car(uint32_t lig, uint32_t col, char c) 
{
    uint16_t * ptr = ptr_mem(lig, col);
    // little endian : chars de contrôle aux grands indices
    *ptr = c | (cf << 12) | (ct << 8);
}*/
void ecrit_car(uint32_t lig, uint32_t col, char c);

/*void place_curseur(uint32_t lig, uint32_t col)
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
}*/
void place_curseur(uint32_t lig, uint32_t col);

void efface_ecran()
{
    for(uint8_t i = 0; i < NB_LINE; i++) {
        for(uint8_t j = 0; j < NB_COL; j++) {
            ecrit_car(i, j, ' ');
        }
    }
    place_curseur(0,0);
}

void efface_ligne(uint32_t lig) {
    for(uint8_t j = 0; j < NB_COL; j++) {
        ecrit_car(lig, j, ' ');
    }
}

void step_cursor() {
    cursor_col ++;
    if(cursor_col == 80) {
        cursor_col = 0;
        cursor_lig ++;
        if (cursor_lig == 25) {
            cursor_lig = 0;
        }
    }
    place_curseur(cursor_lig, cursor_col);
}

void traite_car(char c)
{
    if(c < 32) {
        switch(c) {
            // TODO caractères de contrôle
            case 8:
                break;
            case 9:
                break;
            case 10:
                break;
            case 12:
                break;
            case 13:
                break;
            default:
                break;
}
    } else {
        ecrit_car(cursor_lig, cursor_col, c);
        step_cursor();
    }
}

void defilement()
{
    void * mem = (void *) MEM_VIDEO;
    void * newmem = (void *) (MEM_VIDEO + NB_COL * 2);
    uint32_t size = NB_COL * NB_LINE - NB_COL;
    memmove(mem, newmem, size*2);
    efface_ligne(NB_LINE -1);
}

void console_putbytes(char *chaine, int32_t taille)
{
    for(int i = 0; i < taille; i++) {
        traite_car(chaine[i]);
    }
}

void affiche_haut_gauche(char * s) 
{
    uint32_t len = strlen(s);
    for(uint32_t i = 0; i < len; i++) {
        ecrit_car(0, i, s[i]);
    }
}
