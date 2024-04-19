/*
 * table.c - Functions to manipulate generic tables.
 * Copyright (c) 1997 Andrew W. Appel.
 * Reformatted by Amittai Aviram - aviram@bc.edu.
 */

#include <stdio.h>
#include "table.h"
#include "util.h"

#define TABSIZE 127

typedef struct Binder_ * Binder;

struct Binder_ {
    void * key;
    void * value;
    Binder next;
    void * prevtop;
};

struct TAB_Table_ {
    Binder table[TABSIZE];
    void * top;
};


static Binder make_Binder(void * key, void * value, Binder next, void * prevtop) {
    Binder b = malloc_checked(sizeof(*b));
    b->key = key;
    b->value = value;
    b->next = next;
    b->prevtop = prevtop; 
    return b;
}

TAB_Table TAB_empty() { 
    TAB_Table t = malloc_checked(sizeof(*t));
    t->top = NULL;
    for (int i = 0; i < TABSIZE; i++) {
        t->table[i] = NULL;
    }
    return t;
}

/* The cast from pointer to integer in the expression
 *   ((unsigned)key) % TABSIZE
 * may lead to a warning message.  However, the code is safe,
 * and will still operate correctly.  This line is just hashing
 * a pointer value into an integer value, and no matter how the
 * conversion is done, as long as it is done consistently, a
 * reasonable and repeatable index into the table will result.
 */

void TAB_enter(TAB_Table t, void * key, void * value) {
    int index;
    assert(t && key);
    index = ((unsigned long)key) % TABSIZE;
    t->table[index] = make_Binder(key, value, t->table[index], t->top);
    t->top = key;
}

void * TAB_look(TAB_Table t, void * key) {
    int index;
    Binder b;
    assert(t && key);
    index = ((unsigned long)key) % TABSIZE;
    for(b = t->table[index]; b; b = b->next) {
        if (b->key == key) {
            return b->value;
        }
    }
    return NULL;
}

void * TAB_pop(TAB_Table t) {
    void * k;
    Binder b;
    int index;
    assert (t);
    k = t->top;
    assert (k);
    index = ((unsigned long)k) % TABSIZE;
    b = t->table[index];
    assert(b);
    t->table[index] = b->next;
    t->top = b->prevtop;
    return b->key;
}

void TAB_dump(TAB_Table t, void (*show)(void * key, void * value)) {
    void * k = t->top;
    int index = ((unsigned long)k) % TABSIZE;
    Binder b = t->table[index];
    if (b == NULL) {
        return;
    }
    t->table[index] = b->next;
    t->top = b->prevtop;
    show(b->key, b->value);
    TAB_dump(t, show);
    assert(t->top == b->prevtop && t->table[index] == b->next);
    t->top = k;
    t->table[index] = b;
}
