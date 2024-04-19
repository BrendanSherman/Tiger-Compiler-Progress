/*
 * types.h - 
 *
 * Defines the Types module, which abstracts
 * the Tiger type system.
 * All types and functions declared in this module begin with "T_"
 * Linked list types end with "..List"
 * Author: Andrew Appel.
 * Revised and reformatted by Amittai Aviram - aviram@bc.edu.
 */

#pragma once

#include "symbol.h"

#define T_INT_SIZE 4
#define T_POINTER_SIZE 8
#define T_NIL_SIZE 0

typedef struct T_Type_ * T_Type;
typedef struct T_TypeList_ * T_TypeList;
typedef struct T_Field_ * T_Field;
typedef struct T_FieldList_ * T_FieldList;

struct T_Type_ {
    enum {T_RECORD, T_NIL, T_INT, T_STRING, T_ARRAY,
        T_NAME, T_VOID} kind;
    union {
        T_FieldList record;
        T_Type array;
        struct {S_Symbol sym; T_Type type;} name;
    } u;
};

struct T_TypeList_ {
    T_Type head;
    T_TypeList tail;
};

struct T_Field_ {
    S_Symbol name;
    T_Type type;
};

struct T_FieldList_ {
    T_Field head;
    T_FieldList tail;
};

T_Type make_T_Nil();
T_Type make_T_Int();
T_Type make_T_String();
T_Type make_T_Void();
T_Type make_T_Record(T_FieldList fields);
T_Type make_T_Array(T_Type type);
T_Type make_T_Name(S_Symbol sym, T_Type type);
T_TypeList make_T_TypeList(T_Type head, T_TypeList tail);
T_Field make_T_Field(S_Symbol name, T_Type type);
T_FieldList make_T_FieldList(T_Field head, T_FieldList tail);
int T_size(T_Type type);
void T_print_type(T_Type t);
void T_print_type_list(T_TypeList list);
const char * const T_type_name(T_Type type);
