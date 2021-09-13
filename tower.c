/*
 *  Specific functionlity for watchtowers and watchtower regions (faces)
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "shape.h"
#include "tower.h"

// Assumed that each CSV line is no more than 1000 characters long
#define BUFFERSIZE 1000 + 1
#define SEP ","
#define HEADER "Watchtower ID,Postcode,Population Served,Watchtower Point of Contact Name,x,y"

void freeTower(void *ptr) {
    tower_t *tower = (tower_t *) ptr;

    free(tower->id);
    free(tower->postcode);
    free(tower->contact);
    free(tower);
}

void freeRegion(void *ptr) {
    face_t *face = (face_t *) ptr;

    freeList(face->towers);
    free(face);
}

void fPrintTower(FILE *f, tower_t t) {
    fprintf(f, "\n======<tower_t object at %p>======\n"
               "  id:       %s\n"
               "  postcode: %s\n"
               "  pop:      %d\n"
               "  contact:  %s\n"
               "  coords:   (%lf, %lf)\n",
            (void *) &t, t.id, t.postcode, t.pop, 
            t.contact, t.coord.x, t.coord.y);
}

void printTower(FILE *f, tower_t t) {
    fprintf(f, "Watchtower ID: %s, Postcode: %s, "
               "Population Served: %d, "
               "Watchtower Point of Contact Name: %s, "
               "x: %lf, y: %lf\n",
               t.id, t.postcode, t.pop, t.contact, 
               t.coord.x, t.coord.y);
}

void pyPrintTower(tower_t t) {
    printf("@W%ld %lf %lf\n", t.region, t.coord.x, t.coord.y);
}

void readTowers(FILE *f, list_t *towerList) {

    char buffer[BUFFERSIZE];

    fscanf(f, "%[^\n]s", buffer);
    if (strcmp(HEADER, buffer)) {
        printf("Wrong Header!\n");
        exit(EXIT_FAILURE);
    }
    fscanf(f, "\n");

    for(int n = 1; fgets(buffer, BUFFERSIZE, f) != NULL; n++) {
        tower_t *tower = (tower_t *) safeMalloc(sizeof(tower_t));

        // ID 
        char *token = strtok(buffer, SEP);
        tower->id = safeMalloc((strlen(token) + 1) * sizeof(char));
        strcpy(tower->id, token);

        // Postcode
        token = strtok(NULL, SEP);
        tower->postcode = safeMalloc((strlen(token) + 1) * sizeof(char));
        strcpy(tower->postcode, token);

        // Population
        token = strtok(NULL, SEP); 
        sscanf(token, "%d", &(tower->pop));

        // Contact
        token = strtok(NULL, SEP);
        tower->contact = safeMalloc((strlen(token) + 1) * sizeof(char));
        strcpy(tower->contact, token);

        // Coords
        token = strtok(NULL, SEP);
        sscanf(token, "%lf", &(tower->coord.x));
        token = strtok(NULL, SEP);
        sscanf(token, "%lf", &(tower->coord.y));

        tower->region = -1;

        appendList(towerList, tower);
    }
}

void printRegion(FILE *f, face_t face) {
    tower_t *tower = NULL;

    fprintf(f, "%ld\n", face.id);

    iterList(face.towers, (void **) &tower);
    while (nextList(face.towers)) {
        printTower(f, *tower);
    }
}

long findContainingFace(list_t *faceList, coord_t coord) {
    face_t *face;
    iterList(faceList, (void **) &face);
    while (nextList(faceList)) {
        bool contained = true;
        edge_t *curEdge = face->edge;
        
        do {
            // If not on halfplane for some edge of face, it's not on face, so we short circuit
            if (onHalfPlane(*curEdge, coord) <= 0) {
                contained = false;
                break;
            }

            curEdge = curEdge->next;
        } while (curEdge != face->edge);

        if (contained) {
            return face->id;
        }
    }

    // Not found
    return -1;
}
