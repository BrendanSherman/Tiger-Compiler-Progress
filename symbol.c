/*
 * symbol.c -
 * Implementation of Symbol module.
 * See symbol.h for more information.
 * Author: Andrew Appel.
 * Reformatted by Amittai Aviram - aviram@bc.edu.
 */

#include <stdio.h>
#include <string.h>
#include "util.h"
#include "symbol.h"
#include "table.h"

#define SIZE 109  /* should be prime */

struct S_Symbol_ {
    string name;
    S_Symbol next;
};

static S_Symbol hashtable[SIZE];
static struct S_Symbol_ marksym = {"<mark>", 0};


static S_Symbol mksymbol(string name, S_Symbol next) {
    S_Symbol s = malloc_checked(sizeof(*s));
    s->name = name;
    s->next = next;
    return s;
}

static unsigned int hash(char *s0) {
    unsigned int h = 0;
    char *s;
    for(s = s0; *s; s++) {
        h = h * 65599 + *s;
    }
    return h;
}
 
static int streq(string a, string b) {
    return !strcmp(a, b);
}

S_Symbol make_S_Symbol(string name) {
    int index = hash(name) % SIZE;
    S_Symbol syms = hashtable[index];
    S_Symbol sym;
    for(sym = syms; sym; sym = sym->next) {
        if (streq(sym->name, name)) {
            return sym;
        }
    }
    sym = mksymbol(name, syms);
    hashtable[index] = sym;
    return sym;
}
 
string S_name(S_Symbol sym) {
    return sym->name;
}

S_Table S_empty() { 
    return TAB_empty();
}

void S_enter(S_Table t, S_Symbol sym, void * value) {
    TAB_enter(t, sym, value);
}

void * S_look(S_Table t, S_Symbol sym) {
    return TAB_look(t, sym);
}

void S_begin_scope(S_Table t) {
    S_enter(t, &marksym, NULL);
}

void S_end_scope(S_Table t) {
    S_Symbol s;
    do {
        s = TAB_pop(t);
    } while (s != &marksym);
}

void S_dump(S_Table t, void (*show)(S_Symbol sym, void * binding)) {
    TAB_dump(t, (void (*)(void *, void *)) show);
}

