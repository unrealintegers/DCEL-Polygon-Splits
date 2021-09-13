/*
 *  Program for sorting watchtowers into regions defined by 
 *  polygon midpoint splits.
 *  
 *  By: Zefeng Li (1044965)
 *  
 *  Run with:
 *      make voronoi1
 *      ./voronoi1 <data> <polygon> <output> < <splits>
 */

#include<assert.h>
#include<stdbool.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"tower.h"

#define BUFFERSIZE 1000 + 1

void generateSplits(list_t *, list_t *, int *, int *);

int main(int argc, char **argv) {
    
    if (argc != 4) {
        printf("Wrong number of arguments!\n");
        exit(EXIT_FAILURE);
    }

    FILE *f;

    // id of upcoming edge/face
    // faceId = -1 means outer face
    int edgeId = 0, faceId = 1;
    tower_t *tower; edge_t *edge; face_t *face;
    list_t *towerList = initList(),
           *edgeList = initList(),  // ONLY cw edges included
           *faceList = initList();
    
    towerList->freeElem = freeTower;
    edgeList->freeElem = freeEdge;
    faceList->freeElem = freeRegion;
    
    // First file: watchtowers.csv
    f = safeOpen(argv[1], "r");
    readTowers(f, towerList);
    fclose(f);

    // Second file: polygon data
    f = safeOpen(argv[2], "r");

    edge = readPolygon(f, edgeList, &edgeId);
    face = safeMalloc(sizeof(face_t));

    *face = (face_t) {.id = edge->id,
                      .edge = edge,
                      .towers = initList(),
                      .pop = 0};
    face->towers->freeElem = NULL;
    appendList(faceList, face);

    fclose(f);

    // stdin: splits

    generateSplits(edgeList, faceList, &edgeId, &faceId);

    // Watchtower membership

    iterList(towerList, (void **) &tower);
    while (nextList(towerList)) {
        long faceId = findContainingFace(faceList, tower->coord);

        if (faceId >= 0) {
            face_t *face = getList(faceList, faceId);
            tower->region = faceId;
            appendList(face->towers, tower);
            face->pop += tower->pop;
        }
    }

    // this is for python visualisation

    iterList(towerList, (void **) &tower);
    while (nextList(towerList)) {
        pyPrintTower(*tower);
    }
    iterList(edgeList, (void **) &edge);
    while (nextList(edgeList)) {
        pyPrintEdge(*edge);
        pyPrintEdge(*(edge->pair));
    }

    f = safeOpen(argv[3], "w");
    iterList(faceList, (void **) &face);
    while (nextList(faceList)) {
        printRegion(f, *face);
    }

    iterList(faceList, (void **) &face);
    while (nextList(faceList)) {
        fprintf(f, "Face %ld population served: %lld\n", face->id, face->pop);
    }
    fclose(f);

    freeList(towerList);
    freeList(edgeList);
    freeList(faceList);

    return 0;
}

void generateSplits(list_t *edgeList, list_t *faceList, int *edgeId, int *faceId) {
    int edgeIdA, edgeIdB;

    char buffer[BUFFERSIZE];

    while(fgets(buffer, BUFFERSIZE, stdin) != NULL) {
        if (sscanf(buffer, " %d %d", &edgeIdA, &edgeIdB) != 2) break;

        // find corresponding edges
        edge_t *edgeA = getList(edgeList, edgeIdA),
               *edgeB = getList(edgeList, edgeIdB);

        edge_t *startEdge = generateSplit(edgeList, edgeA, edgeB, edgeId, faceId);

        face_t *face = safeMalloc(sizeof(face_t));
        *face = (face_t) {.id = startEdge->face,
                          .edge = startEdge,
                          .towers = initList(),
                          .pop = 0};
        face->towers->freeElem = NULL;
        appendList(faceList, face);

        // update edge pointer
        face = getList(faceList, startEdge->pair->face);
        face->edge = startEdge->pair;
    }
}
