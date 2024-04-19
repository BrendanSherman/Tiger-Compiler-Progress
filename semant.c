/* Authors: Julia Finn - finnju@bc.edu, Brendan Sherman - 
 * semant.c -
 * Implementation of semantic analysis.
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "env.h"
#include "semant.h"

T_Type SEM_trans_type(S_Table tenv, A_Type type);
T_TypeList SEM_make_formal_type_list(S_Table tenv, A_FieldList params);
T_Type SEM_actual_type(S_Table tenv, T_Type type);
bool SEM_types_agree(T_Type t1, T_Type t2);

SEM_ExpType make_SEM_ExpType(TR_TransExp exp, T_Type type) {
    SEM_ExpType exp_type = malloc_checked(sizeof(*exp_type));
    exp_type->exp = exp;
    exp_type->type = type;
    return exp_type;
}

SEM_ExpType SEM_trans_cond_stm(S_Table venv, S_Table tenv, TR_Exp tr_if_exp,
        SEM_ExpType then_exp_type, TR_Function func, A_Exp exp) {
    T_Type cond_type = make_T_Void();
    TR_Stm tr_cond_stm = NULL;
    TR_Stm tr_then_stm = NULL;
    if (then_exp_type->exp->kind == TR_STM) {
        tr_then_stm = then_exp_type->exp->u.stm;
    } else if (then_exp_type->exp->kind == TR_EXP &&
            then_exp_type->exp->u.exp->kind == TR_SEQ_EXP) {
        tr_then_stm = TR_convert_seq_exp_to_stm(then_exp_type->exp->u.exp);
    }
    if (exp->u.iff.elsee) {
        SEM_ExpType else_exp_type = SEM_trans_exp(venv, tenv, func, exp->u.iff.elsee);
        if (!SEM_types_agree(else_exp_type->type, then_exp_type->type)) {
            EM_error(exp->u.iff.then->pos, "types of then and else clauses differ\n");
        }
        cond_type = else_exp_type->type;
        TR_Stm tr_else_stm = NULL;
        if (else_exp_type->exp) {
            if (else_exp_type->exp->kind == TR_STM) {
                tr_else_stm = else_exp_type->exp->u.stm;
            } else if (else_exp_type->exp->kind == TR_EXP) {
                tr_else_stm = TR_convert_seq_exp_to_stm(else_exp_type->exp->u.exp);
            } else {
                EM_error(exp->u.iff.elsee->pos, "else clause is neither an expression nor a statement\n");
            }
            tr_cond_stm = make_TR_IfElseStm(tr_if_exp, tr_then_stm, tr_else_stm);
        }
    } else {
        if (then_exp_type->type->kind != T_VOID) {
            EM_error(exp->u.iff.then->pos, "then-clause must have no value");
        }
        tr_cond_stm = make_TR_IfStm(tr_if_exp, tr_then_stm);
    }
    TR_TransExp tr = make_TR_TransStm(tr_cond_stm);
    return make_SEM_ExpType(tr, cond_type);
}

SEM_ExpType SEM_trans_cond_exp(S_Table venv, S_Table tenv, TR_Exp tr_if_exp,
        SEM_ExpType then_exp_type, TR_Function func, A_Exp exp) {
    T_Type cond_type = make_T_Void();
    TR_Exp tr_cond_exp = NULL;
    TR_Exp tr_then_exp = then_exp_type->exp->u.exp;
    if (exp->u.iff.elsee) {
        SEM_ExpType else_exp_type = SEM_trans_exp(venv, tenv, func, exp->u.iff.elsee);
        if (!SEM_types_agree(else_exp_type->type, then_exp_type->type)) {
            EM_error(exp->u.iff.then->pos, "types of then and else clauses differ\n");
        }
        cond_type = else_exp_type->type;
        TR_Exp tr_else_exp = NULL;
        if (else_exp_type->exp) {
            if (else_exp_type->exp->kind == TR_EXP) {
                tr_else_exp = else_exp_type->exp->u.exp;
            } else if (else_exp_type->exp->kind == TR_EXP) {
                tr_else_exp = TR_convert_seq_stm_to_exp(else_exp_type->exp->u.stm,
                        T_size(else_exp_type->type));
            } else {
                EM_error(exp->u.iff.elsee->pos, "else clause is neither an expression nor a statement");
            }
            tr_cond_exp = make_TR_IfElseExp(tr_if_exp, tr_then_exp, tr_else_exp);
        }
    } else {
        if (then_exp_type->type->kind != T_VOID) {
            EM_error(exp->u.iff.then->pos, "then clause must have no value\n");
        }
        tr_cond_exp = make_TR_IfExp(tr_if_exp, tr_then_exp);
    }
    TR_TransExp tr = make_TR_TransExp(tr_cond_exp);
    return make_SEM_ExpType(tr, cond_type);
}

SEM_ExpType SEM_trans_condition(S_Table venv, S_Table tenv, TR_Function func, A_Exp exp) {
    TR_Exp tr_if_exp = NULL;
    SEM_ExpType if_exp_type = SEM_trans_exp(venv, tenv, func, exp->u.iff.test);
    if (!if_exp_type || if_exp_type->type->kind != T_INT) {
        EM_error(exp->u.iff.test->pos, "test expression must evaluate to an integer");
    }
    if (if_exp_type->exp && if_exp_type->exp->kind == TR_EXP) {
        tr_if_exp = if_exp_type->exp->u.exp;
    }
    SEM_ExpType then_exp_type = SEM_trans_exp(venv, tenv, func, exp->u.iff.then);
    if (then_exp_type->exp) {
        if (then_exp_type->type->kind == T_VOID) {
            return SEM_trans_cond_stm(venv, tenv, tr_if_exp, then_exp_type, func, exp);
        } else {
            return SEM_trans_cond_exp(venv, tenv, tr_if_exp, then_exp_type, func, exp);
        }
    }
    EM_error(exp->u.iff.then->pos, "unrecognizable then-clause in conditional expression");
    return make_SEM_ExpType(NULL, make_T_Void());
}

void SEM_add_code_to_function(SEM_ExpType body_exp_type, TR_Function func) {
    if (body_exp_type->exp) {
        TR_Stm tr_body_stm = NULL;
        if (body_exp_type->exp->kind == TR_STM) {
            tr_body_stm = body_exp_type->exp->u.stm;
        } else if (body_exp_type->exp->kind == TR_EXP) {
            if (body_exp_type->exp->u.exp->kind == TR_SEQ_EXP) {
                tr_body_stm = TR_convert_seq_exp_to_stm(body_exp_type->exp->u.exp);
            } else {
                tr_body_stm = make_TR_ExpStm(body_exp_type->exp->u.exp);
            }
        }
        TR_add_stm_to_function(func, tr_body_stm);
    }
}

SEM_ExpType SEM_trans_var(S_Table venv, S_Table tenv, TR_Function func, A_Var var) {
    switch(var->kind) {
        case A_SIMPLE_VAR:
            {
                E_EnvEntry entry = S_look(venv, var->u.simple);
                if (entry && entry->kind == E_VAR_ENTRY) {
                    return make_SEM_ExpType(make_TR_TransExp(make_TR_MemExp(venv, var->u.simple)), SEM_actual_type(tenv, entry->u.var.type));
                } else {
                    EM_error(var->pos, "undefined variable %s\n", S_name(var->u.simple));
                    return make_SEM_ExpType(NULL, make_T_Int());
                }
            }
        case A_FIELD_VAR:
            {
                SEM_ExpType var_exp_type = SEM_trans_var(venv, tenv, func, var->u.field.var);
                if (!var_exp_type && var_exp_type->type->kind != T_RECORD) {
                    EM_error(var->pos, "field used in something not a record");
                    return make_SEM_ExpType(NULL, make_T_Int());
                }
                T_FieldList fields = var_exp_type->type->u.record;
                int field_offset = 0;
                for (; fields; fields = fields->tail) {
                    if (fields->head->name == var->u.field.sym) {
                        assert(var_exp_type->exp && var_exp_type->exp->kind == TR_EXP);
                        return make_SEM_ExpType(make_TR_TransExp(make_TR_FieldExp(var_exp_type->exp->u.exp, var->u.field.sym, T_size(fields->head->type), field_offset)), fields->head->type);
                    }
                    field_offset += T_size(fields->head->type);
                }
                EM_error(var->pos, "field not found for given record typek\n");
                return make_SEM_ExpType(NULL, make_T_Int());
            }
        case A_SUBSCRIPT_VAR:
            {
                SEM_ExpType var_exp_type = SEM_trans_var(venv, tenv, func, var->u.subscript.var);
                if (!var_exp_type || var_exp_type->type->kind != T_ARRAY) {
                    EM_error(var->pos, "subscript applied to something not an array\n");
                    return make_SEM_ExpType(NULL, make_T_Int());
                }
                SEM_ExpType index_exp_type = SEM_trans_exp(venv, tenv, func, var->u.subscript.exp);
                if (SEM_actual_type(tenv, index_exp_type->type)->kind != T_INT) {
                    EM_error(var->u.subscript.exp->pos, "non-integer index expression\n");
                }
                assert(var_exp_type->exp && var_exp_type->exp->kind == TR_EXP);
                assert(index_exp_type->exp && index_exp_type->exp->kind == TR_EXP);
                return make_SEM_ExpType(make_TR_TransExp(make_TR_SubscriptExp(var_exp_type->exp->u.exp, T_size(var_exp_type->type), index_exp_type->exp->u.exp)), var_exp_type->type->u.array); 
            }
    }
    return NULL;
}

SEM_ExpType SEM_trans_exp(S_Table venv, S_Table tenv, TR_Function func, A_Exp exp) {
    switch (exp->kind) {
        case A_VAR_EXP:
            {
                SEM_ExpType var_exp_type = SEM_trans_var(venv, tenv, func, exp->u.var);
                if (!var_exp_type) {
                    EM_error(exp->pos, "failed to check variable\n");
                    return make_SEM_ExpType(NULL, make_T_Int());
                }
                TR_TransExp tr = make_TR_TransExp(make_TR_VarExp(var_exp_type->exp->u.exp));
                return make_SEM_ExpType(tr, var_exp_type->type);
            }
        case A_NIL_EXP:
            {
                TR_TransExp tr = make_TR_TransExp(make_TR_NumExp(0));
                return make_SEM_ExpType(tr, make_T_Nil());
            }
        case A_INT_EXP:
            {
                TR_TransExp tr = make_TR_TransExp(make_TR_NumExp(exp->u.intt));
                return make_SEM_ExpType(tr, make_T_Int());
            }
        case A_STRING_EXP:
            {
                TR_Exp tr_str_exp = make_TR_StringExp(exp->u.stringg);
                TR_TransExp tr = make_TR_TransExp(tr_str_exp);
                return make_SEM_ExpType(tr, make_T_String());
            }
        case A_CALL_EXP:
            {
                E_EnvEntry function_entry = S_look(venv, exp->u.call.func);
                if (!function_entry || function_entry->kind != E_FUN_ENTRY) {
                    EM_error(exp->pos, "undefined function %s", S_name(exp->u.call.func));
                    return make_SEM_ExpType(NULL, make_T_Int());
                }
                A_ExpList args;
                T_TypeList formals;
                TR_ExpList tr_args = NULL;
                for (
                        args = exp->u.call.args, formals = function_entry->u.fun.formals;
                        args && formals;
                        args = args->tail, formals = formals->tail
                    ) {
                    SEM_ExpType arg_exp_type = SEM_trans_exp(venv, tenv, func, args->head);
                    if (arg_exp_type->type->kind != formals->head->kind) {
                        EM_error(args->head->pos, "argument does not have expected type\n");
                    }
                    tr_args = TR_add_exp(tr_args, arg_exp_type->exp->u.exp);
                }
                if (formals) {
                    EM_error(args->head->pos, "too few arguments for function %s\n",
                            S_name(exp->u.call.func));
                } else if (args) {
                    EM_error(args->head->pos, "too many arguments for function %s\n",
                            S_name(exp->u.call.func));
                }
                TR_TransExp tr;
                if (function_entry->u.fun.result && function_entry->u.fun.result->kind != T_VOID) {
                    tr = make_TR_TransExp(make_TR_FCallExp(venv, exp->u.call.func, tr_args));
                } else {
                    tr = make_TR_TransStm(make_TR_PCallStm(exp->u.call.func, tr_args));
                }
                return make_SEM_ExpType(tr, function_entry->u.fun.result);
            }
        case A_RECORD_EXP:
            {
                T_Type record_type = S_look(tenv, exp->u.record.type);
                if (!record_type || record_type->kind != T_RECORD) {
                    EM_error(exp->pos, "not a record type: %s", S_name(exp->u.record.type));
                    return make_SEM_ExpType(NULL, make_T_Record(NULL));
                }
                T_FieldList fields;
                A_EFieldList efields;
                int size = 0;
                TR_ExpList tr_fields = NULL;
                for (
                        efields = exp->u.record.fields, fields = record_type->u.record;
                        efields && fields;
                        efields = efields->tail, fields = fields->tail
                        ) {
                    if (efields->head->name != fields->head->name) {
                        EM_error(exp->pos, "unexpected field name: %s -- expecting %s\n",
                                S_name(efields->head->name), S_name(fields->head->name));
                    }
                    SEM_ExpType efield_exp_type = SEM_trans_exp(venv, tenv, func, efields->head->exp);
                    if (!SEM_types_agree(efield_exp_type->type, fields->head->type)) {
                        EM_error(exp->pos, "unexpected type for field %s\n",
                                S_name(efields->head->name));
                    }
                    tr_fields = TR_add_exp(tr_fields, efield_exp_type->exp->u.exp);
                    size += T_size(efield_exp_type->type);
                }
                if (fields) {
                    EM_error(exp->pos, "too few fields — missing field %s\n", fields->head->name);
                } else if (efields) {
                    EM_error(exp->pos, "too many fields — unexpected field %s\n", efields->head->name);
                }
                TR_TransExp tr = make_TR_TransExp(make_TR_RecordExp(size, tr_fields));
                return make_SEM_ExpType(tr, record_type);
            }
        case A_ARRAY_EXP:
            {
                T_Type array_type = SEM_actual_type(tenv, S_look(tenv, exp->u.array.type));
                if (!array_type || array_type->kind != T_ARRAY) {
                    EM_error(exp->pos, "%s does not name an array type\n", S_name(exp->u.array.type));
                    array_type = make_T_Array(make_T_Int());
                }
                T_Type element_type = SEM_actual_type(tenv, array_type->u.array);
                SEM_ExpType init_exp_type = SEM_trans_exp(venv, tenv, func, exp->u.array.init);
                if (!init_exp_type || init_exp_type->type->kind != element_type->kind) {
                    EM_error(exp->u.array.init->pos, "initializer's type does not match array's declared type\n");
                    return make_SEM_ExpType(NULL, array_type);
                }
                TR_Exp tr_init_exp = NULL;
                if (init_exp_type->exp) {
                    tr_init_exp = init_exp_type->exp->u.exp;
                } else {
                    EM_error(exp->u.array.init->pos, "unrecognized array initializer\n");
                    tr_init_exp = make_TR_NumExp(0);
                }
                int size = 0;
                SEM_ExpType size_exp_type = SEM_trans_exp(venv, tenv, func, exp->u.array.size);
                if (!size_exp_type || size_exp_type->type->kind != T_INT) {
                    EM_error(exp->u.array.size->pos, "array size expression does not evaluate to an integer\n");
                }
                if (!size_exp_type->exp || size_exp_type->exp->kind != TR_EXP) {
                    EM_error(exp->u.array.size->pos, "unrecognizable array size");
                } else {
                    size = size_exp_type->exp->u.exp->u.num * T_size(element_type);
                }
                TR_TransExp tr = make_TR_TransExp(make_TR_ArrayExp(size, tr_init_exp));
                return make_SEM_ExpType(tr, array_type);
            }
        case A_SEQ_EXP:
            {
                TR_StmList stms = NULL;
                T_Type seq_type = make_T_Void();
                if (exp->u.seq) {
                    A_ExpList el;
                    for (el = exp->u.seq; el->tail; el = el->tail) {
                        SEM_ExpType exp_type = SEM_trans_exp(venv, tenv, func, el->head);
                        if (exp_type && exp_type->exp && exp_type->exp->kind == TR_STM) {
                            stms = TR_add_stm(stms, exp_type->exp->u.stm);
                        }
                    }
                    SEM_ExpType last_exp_type = SEM_trans_exp(venv, tenv, func, el->head);
                    if (last_exp_type) {
                        if (last_exp_type->exp) {
                            if (last_exp_type->exp->kind == TR_EXP) {
                                stms = TR_add_stm(stms, make_TR_ExpStm(last_exp_type->exp->u.exp));
                            } else if (last_exp_type->exp->kind == TR_STM) {
                                stms = TR_add_stm(stms, last_exp_type->exp->u.stm);
                            }
                        }
                        seq_type = last_exp_type->type;
                    }
                }
                TR_TransExp tr = make_TR_TransExp(make_TR_SeqExp(stms));
                return make_SEM_ExpType(tr, seq_type);
            }
        case A_ASSIGN_EXP:
            {
                SEM_ExpType var_exp_type = SEM_trans_var(venv, tenv, func, exp->u.assign.var);
                SEM_ExpType val_exp_type = SEM_trans_exp(venv, tenv, func, exp->u.assign.exp);
                TR_TransExp tr;
		if (!var_exp_type || !var_exp_type->type) {
                    EM_error(exp->u.assign.var->pos,
                            "cannot resolve type of variable\n");
                }
                if (!val_exp_type || !val_exp_type->type) {
                    EM_error(exp->u.assign.exp->pos,
                            "cannot resolve type of value assigned to variable\n");
                }
                if (!SEM_types_agree(var_exp_type->type, val_exp_type->type)) {
                    EM_error(exp->pos, "assignment variable and expression types differ\n");
                }
                if (val_exp_type && val_exp_type->exp && val_exp_type->exp->kind == TR_EXP
                        && var_exp_type && var_exp_type->exp && var_exp_type->exp->kind == TR_EXP) {
                    TR_Exp tr_value = val_exp_type->exp->u.exp;
                    TR_Exp tr_var = var_exp_type->exp->u.exp;
                    TR_Stm tr_assign = make_TR_AssignStm(tr_value, tr_var);
                    tr = make_TR_TransStm(tr_assign);
                }
                return make_SEM_ExpType(tr, make_T_Void());
            }
        case A_IF_EXP:
            {
                return SEM_trans_condition(venv, tenv, func, exp);
            }
        case A_WHILE_EXP:
            {
                SEM_ExpType test_exp_type = SEM_trans_exp(venv, tenv, func, exp->u.whilee.test);
                if (!test_exp_type || test_exp_type->type->kind != T_INT) {
                    EM_error(exp->u.whilee.test->pos, "test expression must evaluate to an integer\n");
                }
                TR_Exp tr_test_exp = NULL;
                if (test_exp_type->exp && test_exp_type->exp->kind == TR_EXP) {
                	tr_test_exp = test_exp_type->exp->u.exp;
		}
                TR_Stm tr_while_stm = make_TR_WhileStm(tr_test_exp, NULL);
                TR_push_loop(tr_while_stm->u.while_.skip_label);
                SEM_ExpType body_exp_type = SEM_trans_exp(venv, tenv, func, exp->u.whilee.body);
                TR_pop_loop();
                if (!body_exp_type || body_exp_type->type->kind != T_VOID) {
                    EM_error(exp->u.whilee.body->pos, "while-loop body must have no value");
                }
                TR_Stm tr_body_stm = NULL;
                if (body_exp_type->exp) {
                    if (body_exp_type->exp->kind == TR_STM) {
                    	tr_body_stm = body_exp_type->exp->u.stm;
                    } else if (body_exp_type->exp->kind == TR_EXP &&
                            body_exp_type->exp->u.exp->kind == TR_SEQ_EXP) {
                        tr_body_stm = TR_convert_seq_exp_to_stm(body_exp_type->exp->u.exp);
                    }
                }
                tr_while_stm->u.while_.body = tr_body_stm;
                TR_TransExp tr = make_TR_TransStm(tr_while_stm);
                return make_SEM_ExpType(tr, make_T_Void());
            }
        case A_FOR_EXP:
            {
                SEM_ExpType lo_exp_type = SEM_trans_exp(venv, tenv, func, exp->u.forr.lo);
                SEM_ExpType hi_exp_type = SEM_trans_exp(venv, tenv, func, exp->u.forr.hi);
                if (!lo_exp_type || lo_exp_type->type->kind != T_INT) {
                    EM_error(exp->u.forr.lo->pos, "lower bound expression must evaluate to an integer\n");
                }
                TR_Exp tr_lo_exp = NULL;
                if (lo_exp_type->exp && lo_exp_type->exp->kind == TR_EXP) {
                    tr_lo_exp = lo_exp_type->exp->u.exp;
                }
                if (!hi_exp_type || hi_exp_type->type->kind != T_INT) {
                    EM_error(exp->u.forr.hi->pos, "upper bound expression must evaluate to an integer\n");
                }
                TR_Exp tr_hi_exp = NULL;
                if (hi_exp_type->exp && hi_exp_type->exp->kind == TR_EXP) {
                    tr_hi_exp = hi_exp_type->exp->u.exp;
                }
                S_begin_scope(venv);
                F_add_var(func->frame, make_F_Var(exp->u.forr.var, make_T_Int()));
                S_enter(venv, exp->u.forr.var,
                        make_E_VarEntry(lo_exp_type->type, func->frame->nesting_level, func->frame->end));
                TR_Exp tr_mem_exp = make_TR_MemExp(venv, exp->u.forr.var);
                TR_Exp tr_var_exp = make_TR_VarExp(tr_mem_exp);
                TR_Stm tr_for_stm = make_TR_ForStm(tr_var_exp, tr_lo_exp, tr_hi_exp, NULL);
                TR_push_loop(tr_for_stm->u.for_.skip_label);
                SEM_ExpType body_exp_type = SEM_trans_exp(venv, tenv, func, exp->u.forr.body);
                TR_pop_loop();
                if (!body_exp_type || body_exp_type->type->kind != T_VOID) {
                    EM_error(exp->u.forr.body->pos, "for-loop body must have no value");
                }
                TR_Stm tr_body_stm = NULL;
                if (body_exp_type->exp) {
                    if (body_exp_type->exp->kind == TR_STM) {
                        tr_body_stm = body_exp_type->exp->u.stm;
                    } else if (body_exp_type->exp->kind == TR_EXP &&
                            body_exp_type->exp->u.exp->kind == TR_SEQ_EXP) {
                        tr_body_stm = TR_convert_seq_exp_to_stm(body_exp_type->exp->u.exp);
                    } else {
                        EM_error(exp->u.forr.body->pos, "for-loop body is misread as a function.");
                    }
                }
                S_end_scope(venv);
                tr_for_stm->u.for_.body = tr_body_stm;
                TR_TransExp tr = make_TR_TransStm(tr_for_stm);
                return make_SEM_ExpType(tr, make_T_Void());
            }
        case A_BREAK_EXP:
            {
                if (!TR_loop_list) {
                    EM_error(exp->pos, "break statement outside of a loop\n");
                }
                TR_Stm tr_break_stm = make_TR_BreakStm(TR_loop_list->head);
                TR_TransExp tr = make_TR_TransStm(tr_break_stm);
                return make_SEM_ExpType(tr, make_T_Void());
            }
        case A_OP_EXP:
            {
                SEM_ExpType left = SEM_trans_exp(venv, tenv, func, exp->u.op.left);
                if (!left || !left->type) {
                    EM_error(exp->u.op.left->pos, "cannot find type for left operand\n");
                    return make_SEM_ExpType(NULL, make_T_Int());
                }
                SEM_ExpType right = SEM_trans_exp(venv, tenv, func, exp->u.op.right);
                if (!right || !right->type) {
                    EM_error(exp->u.op.right->pos, "cannot find type for right operand");
                    return make_SEM_ExpType(NULL, make_T_Int());
                }
                if (exp->u.op.oper == A_EQ_OP || exp->u.op.oper == A_NEQ_OP) {
                    if (!SEM_types_agree(left->type, right->type)) {
                        EM_error(exp->pos, "comparison operand types differ\n");
                    }
                } else {
                    if (left->type->kind != T_INT) {
                        EM_error(exp->u.op.left->pos, "integer required in binary operation");
                    } 
                    if (right->type->kind != T_INT) {
                        EM_error(exp->u.op.right->pos, "integer required in binary operation");
                    } 
                }
                TR_Exp tr_op_exp = NULL;
                switch (exp->u.op.oper) {
                    case A_PLUS_OP:
                    case A_MINUS_OP:
                    case A_TIMES_OP:
                        tr_op_exp = make_TR_ArithOpExp(left->exp->u.exp, right->exp->u.exp, exp->u.op.oper);
                        break;
                    case A_DIVIDE_OP:
                        make_TR_DivOpExp(left->exp->u.exp, right->exp->u.exp);
                        break;
                    default:
                        make_TR_RelOpExp(left->exp->u.exp, right->exp->u.exp, exp->u.op.oper);
                }
                TR_TransExp tr = make_TR_TransExp(tr_op_exp);
                return make_SEM_ExpType(tr, make_T_Int());
            }
        case A_LET_EXP:
            {
                S_begin_scope(venv);
                S_begin_scope(tenv);
                for (A_DecList d = exp->u.let.decs; d; d = d->tail) {
                    SEM_trans_dec(venv, tenv, func, d->head);
                }
                SEM_ExpType body_exp_type = SEM_trans_exp(venv, tenv, func, exp->u.let.body);
                if (!body_exp_type) {
                    EM_error(exp->pos, "cannot establish type of in-clause");
                    return make_SEM_ExpType(NULL, make_T_Void());
                }
                S_end_scope(tenv);
                S_end_scope(venv);
                return body_exp_type;
            }
        default:
            EM_error(exp->pos, "unrecognized expression");
            return make_SEM_ExpType(NULL, make_T_Void());
    }
}

void SEM_trans_dec(S_Table venv, S_Table tenv, TR_Function func, A_Dec dec) {
    switch (dec->kind) {
        case A_VAR_DEC:
            {
                SEM_ExpType init_exp_type = SEM_trans_exp(venv, tenv, func, dec->u.var.init);
                if (!init_exp_type) {
                    EM_error(dec->pos, "variable declaration lacks an initializer\n");
                    init_exp_type = make_SEM_ExpType(make_TR_TransExp(make_TR_NumExp(0)), make_T_Int());
                }
                if (!init_exp_type->type) {
                    EM_error(dec->u.var.init->pos, "cannot establish type for initializing expression");
                    init_exp_type->type = make_T_Int();
                }
                T_Type init_type = init_exp_type->type;
                if (dec->u.var.type) {
                    T_Type declared_type = S_look(tenv, dec->u.var.type);
                    if (!SEM_types_agree(declared_type, init_type)) {
                        EM_error(dec->pos, "declared type does not match that of initializer");
                        init_type = declared_type;
                    } 
                } else if (init_type->kind == T_NIL) {
                    EM_error(dec->u.var.init->pos, "nil cannot initialize a non-record variable");
                    init_type = make_T_Int();
                }
                TR_add_var(func, dec->u.var.var, init_type);
                S_enter(venv, dec->u.var.var,
                        make_E_VarEntry(init_type, func->frame->nesting_level, func->frame->end)); 
                TR_Exp tr_var_exp = make_TR_MemExp(venv, dec->u.var.var); 
                // Leave this: assert(init_exp_type->exp);
                if (!init_exp_type->exp) {
                    EM_error(dec->u.var.init->pos, "missing initializer expression");
                    exit(EXIT_FAILURE);
                }
                if (init_exp_type->exp->kind != TR_EXP) {
                    EM_error(dec->u.var.init->pos, "unrecognizable variable initializer");
                }
                assert(init_exp_type->exp->kind == TR_EXP);
                TR_Exp tr_init_exp = init_exp_type->exp->u.exp;
                TR_Stm tr_assign_stm = make_TR_AssignStm(tr_init_exp, tr_var_exp);
                TR_add_stm_to_function(func, tr_assign_stm);
                break;
            }
        case A_TYPE_DEC_GROUP:
            {
                for (A_TypeDecList tdl = dec->u.type; tdl; tdl = tdl->tail) {
                    T_Type entered_type = SEM_trans_type(tenv, tdl->head->type);
                    S_enter(tenv, tdl->head->name, entered_type);
                }
                for (A_TypeDecList tdl = dec->u.type; tdl; tdl = tdl->tail) {
                    T_Type stored_type = S_look(tenv, tdl->head->name);
                    if (stored_type->kind == T_RECORD) {
                        T_FieldList fields = stored_type->u.record;
                        for (; fields; fields = fields->tail) {
                            if (!fields->head->type) {
                                EM_error(dec->pos, "incomplete type: field %s", S_name(fields->head->name));
                            } else if (fields->head->type->kind == T_NAME) {
                                T_Type field_type = S_look(tenv, fields->head->type->u.name.sym);
                                if (field_type) {
                                    fields->head->type = field_type;
                                } else {
                                    EM_error(dec->pos, "cannot resolve type for field %s", S_name(fields->head->name));
                                }
                            }
                        }
                    } else if (stored_type->kind == T_NAME && !stored_type->u.name.type) {
                        EM_error(dec->pos, "invalid type definition cycle");
                        return;
                    }
                }
                break;
            }
        case A_FUNCTION_DEC_GROUP:
            {
                for (A_FunDecList fdl = dec->u.function; fdl; fdl = fdl->tail) {
                    A_FunDec fd = fdl->head;
                    T_Type result_type;
                    if (fd->result) {
                        result_type = S_look(tenv, fd->result);
                    } else {
                        result_type = make_T_Void();
                    }
                    T_TypeList formal_types = SEM_make_formal_type_list(tenv, fd->params);
                    S_enter(venv, fd->name, make_E_FunEntry(formal_types, result_type));
                }
                for (A_FunDecList fdl = dec->u.function; fdl; fdl = fdl->tail) {
                    A_FunDec fd = fdl->head;
                    TR_Function new_function = make_TR_Function(fd->name, make_F_Frame(func->frame->nesting_level + 1));
                    TR_append_function(func, new_function);
                    E_EnvEntry function_entry = S_look(venv, fd->name);
                    S_begin_scope(venv);
                    A_FieldList fields;
                    T_TypeList formals;
                    for (
                            fields = fd->params, formals = function_entry->u.fun.formals;
                            fields;
                            fields = fields->tail, formals = formals->tail
                        ) {
                        TR_add_param(new_function, fields->head->name, formals->head);
                        S_enter(venv, fields->head->name,
                                make_E_VarEntry(formals->head, func->frame->nesting_level, func->frame->end));
                    }
                    SEM_ExpType body_exp_type = SEM_trans_exp(venv, tenv, new_function, fd->body);
                    S_end_scope(venv);
                    if (!body_exp_type || !SEM_types_agree(function_entry->u.fun.result, body_exp_type->type)) {
                        EM_error(fd->pos, "function body does not return a value of the given type");
                    }
                    if (fd->body) {
                        SEM_add_code_to_function(body_exp_type, new_function);
                    }
                }
                break;
            }
        default:
            EM_error(dec->pos, "unrecognized declaration");
            break;
    }
}

SEM_ExpType SEM_trans_prog(A_Exp prog) {
    S_Table venv = E_base_venv();
    S_Table tenv = E_base_tenv();
    F_Frame main_frame = make_F_Frame(0); 
    TR_Function main_ = make_TR_Function(make_S_Symbol("main"), main_frame);
    SEM_ExpType exp_type = SEM_trans_exp(venv, tenv, main_, prog);
    if (exp_type->exp) {
        SEM_add_code_to_function(exp_type, main_);
    }
    exp_type->exp = make_TR_TransFunction(main_);
    return exp_type;
}

T_TypeList SEM_make_formal_type_list(S_Table tenv, A_FieldList params) {
    if (!params) {
        return NULL;
    }
    T_TypeList type_list = make_T_TypeList(S_look(tenv, params->head->type), NULL);
    params = params->tail;
    for (T_TypeList current = type_list; params; params = params->tail) {
        current->tail = make_T_TypeList(S_look(tenv, params->head->type), NULL);
        current = current->tail;
    }
    return type_list;
}

T_Type SEM_trans_type(S_Table tenv, A_Type type) {
    if (!type) {
        return NULL;
    }
    switch (type->kind) {
        case A_NAME_TYPE:
            {
                T_Type stored_type = S_look(tenv, type->u.name);
                return make_T_Name(type->u.name, stored_type);
            }
        case A_RECORD_TYPE:
            {
                T_FieldList t_fields = NULL;
                T_FieldList t_current_field = NULL;
                A_FieldList a_fields = type->u.record;
                if (a_fields) {
                    for (; a_fields; a_fields = a_fields->tail) {
                        T_Type t_field_type = S_look(tenv, a_fields->head->type);
                        if (!t_field_type) {
                            t_field_type = make_T_Name(a_fields->head->type, NULL);
                        }
                        T_Field t_field = make_T_Field(a_fields->head->name, t_field_type);
                        T_FieldList new_t_field_node = make_T_FieldList(t_field, NULL);
                        if (t_current_field) {
                            t_current_field->tail = new_t_field_node;
                            t_current_field = t_current_field->tail;
                        } else {
                            t_fields = new_t_field_node;
                            t_current_field = t_fields;
                        }
                    }
                }
                return make_T_Record(t_fields);
            }
        case A_ARRAY_TYPE:
            {
                return make_T_Array(S_look(tenv, type->u.array));
            }

    }
    return NULL;
}

T_Type SEM_actual_type(S_Table tenv, T_Type type) {
    if (!type) {
        return NULL;
    }
    while (type->kind == T_NAME) {
        type = type->u.name.type;
    }
    return type;
}

bool SEM_types_agree(T_Type t1, T_Type t2) {
    if (!t1 || !t2) {
        return false;
    }
    if (t1->kind == T_RECORD || t2->kind == T_RECORD) {
        return (t1 == t2) ||
            (t1->kind == T_NIL) ||
            (t2->kind == T_NIL);
    } else {
        return t1 == t2;
    }
}
