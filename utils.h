/*
 *  Utility functions for safe memory allocation
 *  and a python-inspired implementation dynamic arrays (lists)
 */

#ifndef UTIL_H
#define UTIL_H

typedef struct DynamicArray list_t;

// "generic" dynamic array using void pointers
// supporting iteration
struct DynamicArray {
    long curSize, maxSize;
    void **arr;

    // list iteration
    long index;
    void **_next;

    // function pointer for how to free
    void (*freeElem)(void *);
};

void * safeMalloc(size_t);
void * safeRealloc(void *, size_t);
FILE * safeOpen(const char *, const char *);

list_t * initList(void);
void appendList(list_t *, void *);
void * getList(list_t *, long);
void iterList(list_t *, void **);
bool nextList(list_t *);
void freeList(list_t *);

#endif
