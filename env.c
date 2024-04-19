/*
 * env.c - Implementation of symbol tables.
 * See header env.h for details.
 *  Author: Andrew Appel.
 *  Revised, reformatted, and extended
 *  by Amittai Aviram - aviram@bc.edu.
 */
#include <stdlib.h>

#include "env.h"
#include "symbol.h"
#include "table.h"
#include "types.h"
#include "util.h"

E_EnvEntry make_E_VarEntry(T_Type type, int nesting_level, int offset) {
    E_EnvEntry env_entry = malloc_checked(sizeof(*env_entry));
    env_entry->kind = E_VAR_ENTRY;
    env_entry->u.var.type = type;
    env_entry->u.var.nesting_level = nesting_level;
    env_entry->u.var.offset = offset;
    return env_entry;
}

E_EnvEntry make_E_FunEntry(T_TypeList formals, T_Type result) {
    E_EnvEntry env_entry = malloc_checked(sizeof(*env_entry));
    env_entry->kind = E_FUN_ENTRY;
    env_entry->u.fun.formals = formals;
    env_entry->u.fun.result = result;
    return env_entry;
}

S_Table E_base_tenv() {
    S_Table type_table = S_empty();
    S_enter(type_table, make_S_Symbol("int"), make_T_Int());
    S_enter(type_table, make_S_Symbol("string"), make_T_String());
    return type_table;
}

S_Table E_base_venv() {
    S_Table value_table = S_empty();
    S_enter(
            value_table,
            make_S_Symbol("print"),
            make_E_FunEntry(make_T_TypeList(make_T_String(), NULL), make_T_Void())
           );
    S_enter(
            value_table,
            make_S_Symbol("flush"),
            make_E_FunEntry(NULL, make_T_Void())
           );
    S_enter(
            value_table,
            make_S_Symbol("getchar"),
            make_E_FunEntry(NULL, make_T_String())
           );
    S_enter(
            value_table,
            make_S_Symbol("ord"),
            make_E_FunEntry(make_T_TypeList(make_T_String(), NULL), make_T_Int())
           );
    S_enter(
            value_table,
            make_S_Symbol("chr"),
            make_E_FunEntry(make_T_TypeList(make_T_Int(), NULL), make_T_String())
           );
    S_enter(
            value_table,
            make_S_Symbol("size"),
            make_E_FunEntry(make_T_TypeList(make_T_String(), NULL), make_T_Int())
           );
    S_enter(
            value_table,
            make_S_Symbol("substring"),
            make_E_FunEntry(
                make_T_TypeList(
                    make_T_String(),
                    make_T_TypeList(
                        make_T_Int(),
                        make_T_TypeList(
                            make_T_Int(),
                            NULL
                            )
                        )
                    ),
                make_T_Int()
                )
           );
    S_enter(
            value_table,
            make_S_Symbol("concat"),
            make_E_FunEntry(
                make_T_TypeList(
                    make_T_String(),
                    make_T_TypeList(
                        make_T_String(),
                        NULL
                        )
                    ),
                make_T_String()
                )
           );
    S_enter(
            value_table,
            make_S_Symbol("not"),
            make_E_FunEntry(make_T_TypeList(make_T_Int(), NULL), make_T_Int())
           );
    S_enter(
            value_table,
            make_S_Symbol("exit"),
            make_E_FunEntry(make_T_TypeList(make_T_Int(), NULL), make_T_Void())
           );

    return value_table;
}

