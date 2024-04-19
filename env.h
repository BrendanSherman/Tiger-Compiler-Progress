/*
 * env.h - Support for symbol tables.
 * There are two symbol tables:
 *  - The type environment (tenv),
 *      mapping names of types to type objects
 *      as defined in the Types module.
 *  - The value environment (venv),
 *      mapping names of objects to metadata
 *      packaged into EnvEntry objects,
 *      including type.
 *  Author: Andrew Appel.
 *  Revised, reformatted, and extended
 *  by Amittai Aviram - aviram@bc.edu.
 */

#include "types.h"

typedef struct E_EnvEntry_ * E_EnvEntry;

struct E_EnvEntry_ {
    enum { E_VAR_ENTRY, E_FUN_ENTRY } kind;
    union {
        struct { T_Type type; int nesting_level; int offset; } var;
        struct { T_TypeList formals; T_Type result; } fun;
    } u;
};

E_EnvEntry make_E_VarEntry(T_Type type, int nesting_level, int offset);
E_EnvEntry make_E_FunEntry(T_TypeList formals, T_Type result);
S_Table E_base_tenv();
S_Table E_base_venv();
