/* semant.h -
 * Definitions for the Semantic module,
 * which encapsulates the semantic analysis
 * of a Tiger program, consisting of
 * - Type checking throughout the AST and
 * - Translation of the AST into IR.
 * Author: Andrew Appel.
 * Revised, extended, and reformatted by Amittai Aviram - aviram@bc.edu.
 */

#pragma once

#include "absyn.h"
#include "symbol.h"
#include "translate.h"
#include "types.h"
#include "util.h"

typedef struct SEM_ExpType_ * SEM_ExpType;

struct SEM_ExpType_ {
    TR_TransExp exp;
    T_Type type;
};

SEM_ExpType make_ExpType(TR_TransExp exp, T_Type type);

SEM_ExpType SEM_trans_var(S_Table venv, S_Table tenv, TR_Function func, A_Var var);
SEM_ExpType SEM_trans_exp(S_Table venv, S_Table tenv, TR_Function func, A_Exp exp);
void SEM_trans_dec(S_Table venv, S_Table tenv, TR_Function func, A_Dec dec);
T_Type SEM_trans_type(S_Table tenv, A_Type type);
SEM_ExpType SEM_trans_prog(A_Exp prog);
