#include "stdio.h"
#include "sync_queue.h"
#include "cpu.h"



int station1(){
    int i = 0;
    int j;
    printf("station 1 init\n");
    while(true) {
		for (int k = 0; k < 50000000; k++);
        i = ((i+7) *4) % 11; // rng
        printf("station 1 sending %d to station 2\n", i);
        psend(0, i);
		for (int k = 0; k < 50000000; k++);
        preceive(2, &j);
        if(i == j) {
            printf("Received correct value from station 3 !\n");
        }
        printf("station 1 receiving %d from station 3\n", j);
    }
    return 0;
}

int station2(){
    int j;
    printf("station 2 init\n");
    while(true) {
		for (int i = 0; i < 50000000; i++);
        preceive(0, &j);
        printf("station 2 receiving %d from station 1\n", j);
		for (int i = 0; i < 50000000; i++);
        printf("transmitting to station 3\n");
        psend(1, j);
    }
        return 0;
}

int station3(){
    int j;
    printf("station 3 init\n");
    while(true) {
		for (int i = 0; i < 50000000; i++);
        preceive(1, &j);
        printf("station 3 receiving %d from station 2\n", j);
		for (int i = 0; i < 50000000; i++);
        printf("transmitting to station 1\n");
        psend(2, j);
    }
        return 0;
}

void repartiteur() {
    // initialise 3 files et lance 3 stations
    int a, b, c;
    a = pcreate(10);
    b = pcreate(10);
    c = pcreate(10);
    cree_processus("station3", 1,  &station3);
    cree_processus("station2", 1,  &station2);
    cree_processus("station1", 1,  &station1);
    printf("ca c'est fait\n");
    while(1) {
		sti();
		for (int i = 0; i < 50000000; i++);
		cli();
    }
}