/*
 * util.c -
 * Commonly used utility functions.
 * Author: Andrew Appel.
 * Revised and reformatted by Amittai Aviram - aviram@bc.edu.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

void * malloc_checked(int len) {
    void * p = malloc(len);
    if (!p) {
        perror("Memory allocation failure");
        exit(1);
    }
    return p;
}

string make_String(const char * const s) {
    string p = malloc_checked(strlen(s) + 1);
    strcpy(p, s);
    return p;
}

UBoolList make_UBoolList(bool head, UBoolList tail) {
    UBoolList list = malloc_checked(sizeof(struct UBoolList_));
    list->head = head;
    list->tail = tail;
    return list;
}
