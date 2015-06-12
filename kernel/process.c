#include "process.h"
#include "global.h"
#include "stdio.h"
/*
      la fonction void ordonnance(void) a pour rôle d'implanter la politique d'ordonnancement en choisissant le prochain processus à activer (comme il n'y en a que 2 pour
 *  l'instant, ça ne devrait pas poser de difficulté) et de provoquer le changement de processus en appelant la fonction ctx_sw avec les bons paramètres ;
    l'ordonnanceur a besoin de savoir quel est le processus en cours d'exécution : le plus simple pour cela est de conserver un pointeur vers la structure de processus
 *  sous la forme d'une variable globale actif (de type « pointeur sur une structure de processus » par exemple ou de façon équivalente, le pid du processus actif sous forme d'un entier signé) ;
    n'oubliez pas de changer l'état des processus dans la fonction ordonnance : le processus élu doit prendre l'état ELU et l'autre devenir ACTIVABLE ;
    les fonctions int32_t mon_pid(void) et char *mon_nom(void) renvoient simplement le pid et le nom du processus en cours d'exécution, elles ne posent pas de difficulté d'implantation. 
 */
void ordonnance(){
  
    uint32_t next = index_run^1;
    process_tab[index_run].state = WAITING;
    process_tab[next].state = RUNNING;
    index_run = next;
    ctx_sw(process_tab[index_run].reg, process_tab[next].reg);
}

char* mon_nom(){
    return process_tab[index_run].name;
}

int16_t mon_pid(){
    return process_tab[index_run].pid;
}