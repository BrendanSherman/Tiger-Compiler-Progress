/*
 * symbol.h -
 * The Symbol module, supporting symbols and symbol tables.
 * A symbol here is a unique representation of a string identifier.
 * Supports key-value storage and lookup based upon the symbol
 * abstraction.
 * The Environment module (env.h) is built as a layer above
 * this one.
 * Author: Andrew Appel.
 * Revised and reformatted by Amittai Aviram - aviram@bc.edu.
 */

#pragma once

#include "util.h"

typedef struct S_Symbol_ * S_Symbol;

/* Make a unique symbol from a given string.  
 *  Different calls to S_Symbol("foo") will yield the same S_symbol
 *  value, even if the "foo" strings are at different locations. */
S_Symbol make_S_Symbol(string);

/* Extract the underlying string from a symbol */
string S_name(S_Symbol);

/* S_table is a mapping from S_symbol->any, where "any" is represented
 *     here by void*  */
typedef struct TAB_Table_ * S_Table;

/* Make a new table */
S_Table S_empty();

/* Enter a binding "sym->value" into "t", shadowing but not deleting
 *    any previous binding of "sym". */
void S_enter(S_Table t, S_Symbol sym, void * value);

/* Look up the most recent binding of "sym" in "t", or return NULL
 *    if sym is unbound. */
void * S_look(S_Table t, S_Symbol sym);

/* Start a new "scope" in "t".  Scopes are nested. */
void S_begin_scope(S_Table t);

/* Remove any bindings entered since the current scope began,
   and end the current scope. */
void S_end_scope(S_Table t);

