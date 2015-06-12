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

void defilement()
{
    void * mem = (void *) MEM_VIDEO;
    void * newmem = (void *) (MEM_VIDEO + NB_COL * 2);
    uint32_t size = NB_COL * NB_LINE - NB_COL;
    memmove(mem, newmem, size*2);
    efface_ligne(NB_LINE -1);
}

void traite_car(char c)
{
    if(c < 32) {
        switch(c) {
            case 8:
	      if(cursor_col!=0){
		place_curseur(cursor_lig, cursor_col-1);
	      }
              break;
            case 9:
	      place_curseur(cursor_lig, (cursor_col!=NB_COL-1)?((cursor_col/8)*8+8):NB_COL-1 );               
	      break;
            case 10:
	      if(cursor_lig+1==NB_LINE){
		defilement();
		place_curseur(NB_LINE-1, 0);
	      }
	      else 
		place_curseur(cursor_lig+1, 0);
              break;
            case 12:
	        efface_ecran();
                break;
            case 13:
	      place_curseur(cursor_lig, 0);
                break;
            default:
                break;
}
    } else {
        ecrit_car(cursor_lig, cursor_col, c);
        step_cursor();
    }
}

void console_putbytes(char *chaine, int32_t taille)
{
    for(int i = 0; i < taille; i++) {
        traite_car(chaine[i]);
    }
    if(cursor_lig==24 && cursor_col == 79){
      defilement();
    }
}

void affiche_haut_gauche(char * s) 
{
    uint32_t len = strlen(s);
    for(uint32_t i = 0; i < len; i++) {
        ecrit_car(0, i, s[i]);
    }
}
