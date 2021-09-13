/*
 *  Constructs for basic geometric types such as coordinates, vectors 
 *  and complex polygons using a DCEL data structure
 */

#include<assert.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"shape.h"
#include"utils.h"

// Creates vector from 2 points
vec_t getVec(coord_t A, coord_t B) {
    return (vec_t) {.dx = B.x - A.x,
                    .dy = B.y - A.y};
}

// Dot product
double dot(vec_t u, vec_t v) {
    return u.dx * v.dx + u.dy * v.dy;
} 

coord_t mid(edge_t edge) {
    return mid_c(edge.start, edge.end);
}

coord_t mid_c(coord_t coord1, coord_t coord2) {
    return (coord_t) {.x = (coord1.x + coord2.x) / 2, 
                      .y = (coord1.y + coord2.y) / 2};
}

void freeEdge(void *ptr) {
    edge_t *edge = (edge_t *) ptr;
    
    free(edge->pair);
    free(edge);
}

void printEdge(edge_t e) {
    char prev[100], pair[100], next[100];

    if (e.prev != NULL) {
        sprintf(prev, "%p | %s%ld(%ld)",
                (void *) e.prev,
                e.prev->parity ? "A" : "B",
                e.prev->id, e.prev->face);
    } else {
        strcpy(prev, "(null)");
    }

    if (e.pair != NULL) {
        sprintf(pair, "%p | %s%ld(%ld)",
                (void *) e.pair,
                e.pair->parity ? "A" : "B",
                e.pair->id, e.pair->face);
    } else {
        strcpy(pair, "(null)");
    }

    if (e.next != NULL) {
        sprintf(next, "%p | %s%ld(%ld)",
                (void *) e.next,
                e.next->parity ? "A" : "B",
                e.next->id, e.next->face);
    } else {
        strcpy(next, "(null)");
    }

    printf("\n======<edge_t object at %p>======\n"
           "  edge:     %s%ld(%ld)\n"
           "  start:    (%lf, %lf)\n"
           "  end:      (%lf, %lf)\n"
           "  pair:     %s\n"
           "  prev:     %s\n"
           "  next:     %s\n",
           (void *) &e, e.parity ? "A" : "B", e.id, e.face,
           e.start.x, e.start.y, e.end.x, e.end.y,
           pair, prev, next
           );
}

void pyPrintEdge(edge_t edge) {
    printf("@E%ld %ld %lf %lf %lf %lf\n", edge.id, edge.face,
    edge.start.x, edge.start.y, edge.end.x, edge.end.y);
}

// Returns true if two edges are on the same interior face
bool sameFace(const edge_t *a, const edge_t *b) {
    return (a->face == b->face) && (a->face != -1);
}

// Finds the pair (u, v) in {a, a'} x {b, b'}
// where A' denotes pair of A
// such that u.face == v.face
// This modifies the values of a, b to the matching pair
// This pairing has to be unique!
void findMatchingEdges(edge_t **a, edge_t **b) {
    edge_t *a1 = *a, *a2 = (*a)->pair,
           *b1 = *b, *b2 = (*b)->pair;

    // ASSERT that there is only 1
    assert(sameFace(a1, b1) + sameFace(a1, b2) +
           sameFace(a2, b1) + sameFace(a2, b2) == 1);

    if (sameFace(a1, b1)) {*a = a1, *b = b1;}
    if (sameFace(a1, b2)) {*a = a1, *b = b2;}
    if (sameFace(a2, b1)) {*a = a2, *b = b1;}
    if (sameFace(a2, b2)) {*a = a2, *b = b2;}
}

/* The idea here is, given some half edge AB and a point X,
 * we consider the vectors u = AB and v = AX.
 * We can rotate u 90 degrees clockwise and obtain u'
 * and now all we need is to find the sign of ||proj_u'(v)||
 * which is the same sign as <u', v> (inner/dot product)
 */
int onHalfPlane(edge_t edge, coord_t coord) {
    vec_t u = getVec(edge.start, edge.end),
          v = getVec(edge.start, coord);

    // 90deg cw rotation
    vec_t uPerp = {.dx = u.dy,
                   .dy = -u.dx};
    
    double dp = dot(uPerp, v);

    // 1 = yes, 0 = incident, -1 = opposite
    return dp > 0 ? 1 : dp == 0 ? 0 : -1;
}

edge_t * readPolygon(FILE *f, list_t *list, int *id) {
    coord_t first, cur, prev;

    edge_t *cur_cw = NULL, 
           *cur_ccw = NULL;
    edge_t *first_cw, *first_ccw, 
           *prev_cw,  *prev_ccw;
    
    double x, y;
    bool endLoop = false, 
         firstLoop = true;

    fscanf(f, "%lf %lf", &x, &y);
    first.x = x, first.y = y;
    cur.x = x, cur.y = y;
    
    while (!endLoop) {
        prev = cur;
        prev_cw = cur_cw;
        prev_ccw = cur_ccw;

        // Invariant here: prev and cur edges/vertices equal

        if (fscanf(f, "%lf %lf", &x, &y) == 2) {
            cur.x = x, cur.y = y;
        } else {  // Cycle back to start
            cur = first;
            endLoop = true;
        }
        cur_cw = safeMalloc(sizeof(edge_t));
        cur_ccw = safeMalloc(sizeof(edge_t));

        // Initialise edges
        *cur_cw = (edge_t) {.start = prev,
                            .end = cur,
                            .id = *id,
                            .face = 0,
                            .parity = true,
                            .next = NULL,
                            .prev = prev_cw,
                            .pair = cur_ccw};
        *cur_ccw = (edge_t) {.start = cur,
                             .end = prev,
                             .id = (*id)++,  // increment for next edge
                             .face = -1, 
                             .parity = false,
                             .next = prev_ccw,
                             .prev = NULL,
                             .pair = cur_cw};

        if (firstLoop) {
            firstLoop = false;
            first_cw = cur_cw;
            first_ccw = cur_ccw;
        }

        // Append only cw direction to our dyn list
        appendList(list, cur_cw);

        if (prev_cw != NULL) prev_cw->next = cur_cw;
        if (prev_ccw != NULL) prev_ccw->prev = cur_ccw;

        // Invariant: prev is prvious of cur
    }

    // Now need to link first and last edges together
    // cur is last edge
    first_cw->prev = cur_cw; cur_cw->next = first_cw;
    first_ccw->next = cur_ccw; cur_ccw->prev = first_ccw;

    return first_cw;
}

edge_t * generateSplit(list_t *edgeList, edge_t *edgeA, edge_t *edgeB,
                       int *edgeId, int *faceId) {
        coord_t midA = mid(*edgeA),
                midB = mid(*edgeB);

        // find the inner edges first
        findMatchingEdges(&edgeA, &edgeB);

        bool adjAB = (edgeA->next == edgeB),
             adjBA = (edgeA->pair->prev == edgeB->pair);

        // logically, adjAB implies adjBA
        assert(!adjBA || adjAB);  

        // connect midpoints and form new edge
        edge_t *newEdge = safeMalloc(sizeof(edge_t)),
               *newPair = safeMalloc(sizeof(edge_t));
        // newEdge stays with the old face number here
        *newEdge = (edge_t) {.start = midA,
                             .end = midB,
                             .id = *edgeId,
                             .face = edgeA->face,
                             .parity = true,  // arbitrary
                             .next = NULL,
                             .prev = NULL,
                             .pair = newPair};
        // but newPair gets the new face number
        *newPair = (edge_t) {.start = midB,
                             .end = midA,
                             .id = (*edgeId)++,
                             .face = *faceId,
                             .parity = false,  // arbitrary
                             .next = NULL,
                             .prev = NULL,
                             .pair = newEdge};
        // we don't assign prev and next yet

        /* now split inner edges and assign prev + next
         * edgeA -> newA1 + edgeA; edgeA.pair -> edgeA.pair + newA2
         * edgeB -> newB1 + edgeB; edgeB.pair -> edgeB.pair + newB2
         */
        edge_t *newA1 = safeMalloc(sizeof(edge_t)),
               *newA2 = safeMalloc(sizeof(edge_t)),
               *newB1 = safeMalloc(sizeof(edge_t)),
               *newB2 = safeMalloc(sizeof(edge_t));

        /* newA1 starts at midA in same direction of edgeA
         * which is clockwise for newPair, hence it gets 
         * next edge number 
         */ 
        *newA1 = (edge_t) {.start = midA,
                           .end = edgeA->end,
                           .id = *edgeId,
                           .face = *faceId,
                           .parity = true,  // arbitrary
                           .next = adjAB ? newB1 : edgeA->next,
                           .prev = newPair,
                           .pair = newA2};        
        /* newA2 ends at midA in same direction of edgeA.pair
         * this is not affected by the split, and stays in the same face
         * this is the pair of newA1, so edge number is same as newA1 
         */
        *newA2 = (edge_t) {.start = edgeA->end,  
                           .end = midA,
                           .id = (*edgeId)++,  // increment as we have finished this pair 
                           .face = edgeA->pair->face, 
                           .parity = false,  // arbitrary
                           .next = edgeA->pair,
                           .prev = adjBA ? newB2 : edgeA->pair->prev,
                           .pair = newA1};
        
        /* newB1 ends at midB in same direction of edgeB
         * which is clockwise for newPair, hence it gets 
         * next edge number
         */
        *newB1 = (edge_t) {.start = edgeB->start,
                           .end = midB,
                           .id = *edgeId, 
                           .face = *faceId,  // finished with new face
                           .parity = true,  // arbitrary
                           .next = newPair,
                           .prev = adjAB ? newA1 : edgeB->prev,
                           .pair = newB2};
        /* newB2 starts at midB in same direction of edgeB.pair
         * this is not affected by the split, and stays in the same face        
         * this is the pair of newB1, so edge number is same as newB1 
         */
        *newB2 = (edge_t) {.start = midB,
                           .end = edgeB->start,
                           .id = (*edgeId)++,  // increment as we have finished this pair 
                           .face = edgeB->pair->face,  // same face as edgeB->pair
                           .parity = false,  // arbitrary
                           .next = adjBA ? newA2 : edgeB->pair->next,
                           .prev = edgeB->pair,
                           .pair = newB1};
        // At this point, all 6 new half-edges have been created
    
        // Before we lose reference of the original edges' 
        // prev and next, update these pointers first
        edgeA->next->prev = newA1, edgeA->pair->prev->next = newA2;
        edgeB->prev->next = newB1, edgeB->pair->next->prev = newB2;

        // Now, we update the 4 original half-edges
        // We only need to change start/end coords and next/prev
        edgeA->end = midA, edgeA->pair->start = midA;
        edgeB->start = midB, edgeB->pair->end = midB;

        edgeA->next = newEdge, edgeB->prev = newEdge;
        edgeA->pair->prev = newA2;
        edgeB->pair->next = newB2;

        // Finally, we link the new edges' next/prev
        newEdge->next = edgeB, newEdge->prev = edgeA;
        newPair->next = newA1, newPair->prev = newB1;

        // Append everything ionto edgeList
        appendList(edgeList, newEdge);
        appendList(edgeList, newA1);
        appendList(edgeList, newB1);

        // update other edges of new face
        for (edge_t *cur = newA1->next; cur != newB1; cur = cur->next) {
            cur->face = *faceId;
        }
        (*faceId)++;

        // return edge in new face
        return newPair; 
}
