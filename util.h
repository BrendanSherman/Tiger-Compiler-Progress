/*
 * util.h -
 * Commonly used utility functions.
 * Author: Andrew Appel.
 * Revised and reformatted by Amittai Aviram - aviram@bc.edu.
 */

#pragma once

#include <assert.h>
#include <stdbool.h>

typedef char * string;

typedef struct UBoolList_ * UBoolList;

struct UBoolList_ {
    bool head;
    UBoolList tail;
};

void * malloc_checked(int);
string make_String(const char * const);
UBoolList make_UBoolList(bool head, UBoolList tail);

