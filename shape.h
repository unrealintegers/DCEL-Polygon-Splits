/*
 *  Constructs for basic geometric types such as coordinates, vectors 
 *  and complex polygons using a DCEL data structure
 */

#ifndef SHAPE_H
#define SHAPE_H

#include<stdbool.h>

#include"utils.h"

typedef struct Coordinate {
    double x, y;
} coord_t;

typedef struct Vector {
    double dx, dy;
} vec_t;

typedef struct HalfEdge edge_t;

struct HalfEdge {
    coord_t start, end;
    long id;
    long face;
    bool parity;  // simply something to distinguish pairs, 
    edge_t *pair;
    edge_t *next;
    edge_t *prev;
};

vec_t getVec(coord_t, coord_t);
double dot(vec_t, vec_t);

coord_t mid(edge_t);
coord_t mid_c(coord_t, coord_t);

void freeEdge(void *);
void printEdge(edge_t);
void pyPrintEdge(edge_t);

bool sameFace(const edge_t *, const edge_t *);
void findMatchingEdges(edge_t **, edge_t **);

int onHalfPlane(edge_t, coord_t);

edge_t * readPolygon(FILE *, list_t *, int *);
edge_t * generateSplit(list_t *, edge_t *, edge_t *, int *, int *);

#endif
