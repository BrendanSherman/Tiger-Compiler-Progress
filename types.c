/*
 * types.c - 
 *
 * Implementation of the Tiger type system.
 * See types.h for more information.
 * Author: Andrew Appel.
 * Revised and reformatted by Amittai Aviram - aviram@bc.edu.
 */

#include <stdio.h>
#include "symbol.h"
#include "types.h"
#include "util.h"

static const char * const T_type_names[] = {
   "type_record", "type_nil", "type_int", "type_string", 
   "type_array", "type_name", "type_void"
};

static const char * const T_type_strings[] =  {
    "T_RECORD",
    "T_NIL",
    "T_INT",
    "T_STRING",
    "T_ARRAY",
    "T_NAME",
    "T_VOID"
};

static struct T_Type_ type_nil = { T_NIL };
T_Type make_T_Nil() {
    return &type_nil;
}

static struct T_Type_ type_int = { T_INT };
T_Type make_T_Int() { return &type_int; }

static struct T_Type_ type_string = { T_STRING };
T_Type make_T_String() { return &type_string; }

static struct T_Type_ type_void = { T_VOID };
T_Type make_T_Void() { return &type_void; }

T_Type make_T_Record(T_FieldList fields) {
    T_Type p = malloc_checked(sizeof(*p));
    p->kind = T_RECORD;
    p->u.record = fields;
    return p;
}

T_Type make_T_Array(T_Type type) {
    T_Type p = malloc_checked(sizeof(*p));
    p->kind = T_ARRAY;
    p->u.array = type;
    return p;
}

T_Type make_T_Name(S_Symbol sym, T_Type type) {
    T_Type p = malloc_checked(sizeof(*p));
    p->kind = T_NAME;
    p->u.name.sym = sym;
    p->u.name.type = type;
    return p;
}


T_TypeList make_T_TypeList(T_Type head, T_TypeList tail) {
    T_TypeList p = malloc_checked(sizeof(*p));
    p->head = head;
    p->tail = tail;
    return p;
}

T_Field make_T_Field(S_Symbol name, T_Type type) {
    T_Field p = malloc_checked(sizeof(*p));
    p->name = name;
    p->type = type;
    return p;
}

T_FieldList make_T_FieldList(T_Field head, T_FieldList tail) {
    T_FieldList p = malloc_checked(sizeof(*p));
    p->head = head;
    p->tail = tail;
    return p;
}

int T_size(T_Type type) {
    switch (type->kind) {
        case T_INT:
            return T_INT_SIZE;
        case T_RECORD:
        case T_STRING:
        case T_ARRAY:
            return T_POINTER_SIZE;
        default:
            return T_NIL_SIZE;
    }
}

/* printing functions - used for debugging */
/* This will infinite loop on mutually recursive type_pes */
void T_print_type(T_Type t) {
  if (!t) {
      printf("null");
  } else {
      printf("%s", T_type_strings[t->kind]);
      if (t->kind == T_NAME) {
          printf(", %s", S_name(t->u.name.sym));
      }
      printf("(%d)", T_size(t));
  }
}

void T_print_type_list(T_TypeList list) {
    if (list == NULL) {
        printf("null");
    } else {
        printf("T_List( ");
        T_print_type(list->head);
        printf(", ");
        T_print_type_list(list->tail);
        printf(")");
    }
}

const char * const T_type_name(T_Type type) {
    if (!type) {
        return "(NULL)";
    }
    return T_type_strings[type->kind];
}
