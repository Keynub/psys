#ifndef PL_ECRAN
#define PL_ECRAN

#include <inttypes.h>

uint16_t * ptr_mem(uint32_t lig, uint32_t col);

void ecrit_car(uint32_t lig, uint32_t col, char c);

void place_curseur(uint32_t lig, uint32_t col);

void efface_ecran();

void traite_car(char c);

void affiche_haut_gauche(char * s);

void defilement();

void console_putbytes(char *chaine, int32_t taille);

int cons_write(const char *str, long size);

#endif
