#pragma once

#include "symbol.h"
#include "types.h"

typedef struct F_Frame_ *F_Frame;
typedef struct F_Var_ *F_Var;
typedef struct TR_VarList_ *TR_VarList;

struct F_Frame_ {
    int nesting_level;
    TR_VarList parameters;
    TR_VarList variables;
    int end;
};

struct F_Var_ {
    S_Symbol name;
    T_Type type;
};

F_Frame make_F_Frame(int nesting_level);
F_Var make_F_Var(S_Symbol name, T_Type type);
void F_add_param(F_Frame frame, F_Var param);
void F_add_var(F_Frame frame, F_Var var);
void F_print_frame(F_Frame frame);