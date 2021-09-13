/*
 *  Specific functionlity for watchtowers and watchtower regions (faces)
 */

#ifndef TOWER_H
#define TOWER_H

#include "shape.h"

typedef struct Watchtower {
    char *id;        // Watchtower ID
    char *postcode;  // Postcode
    int pop;         // Population Served
    char *contact;   // Watchtower Point of Contact Name
    coord_t coord;   // x, y

    long region;     // face
} tower_t;

typedef struct TowerRegion {
    long id;
    edge_t *edge;
    list_t *towers;
    long long pop;
} face_t;

void freeTower(void *);
void freeRegion(void *);

void fPrintTower(FILE *, tower_t);
void printTower(FILE *, tower_t);
void pyPrintTower(tower_t);

void readTowers();
void printRegion(FILE *, face_t);
long findContainingFace(list_t *, coord_t);

#endif
