/*
 * print_ir.c -
 * Implementation of abstractions to print a visual
 * representation of the intermediate representation (IR)
 * of a given Tiger program.
 * See print_ir.h for more information.
 * Author: Amittai Aviram - aviram@bc.edu.
 */

#include <stdio.h>

#include "print_ir.h"

#define OFFSET 2

const char * P_stm_names[] = {
    "assign_stm",
    "pcall_stm",
    "seq_stm",
    "if_stm",
    "if_else_stm",
    "while_stm",
    "for_stm",
    "break_stm",
    "exp_stm"
};

const char * P_exp_names[] = {
    "num_exp",
    "string_exp",
    "mem_exp",
    "var_exp",
    "field_exp",
    "subscript_exp",
    "record_exp",
    "array_exp",
    "arith_op_exp",
    "div_op_exp",
    "rel_op_exp",
    "if_exp",
    "if_else_exp",
    "fcall_exp",
    "seq_exp"
};

const char * P_op_names[] = {
    "plus_op",
    "minus_op",
    "times_op",
    "divide_op",
    "eq_op",
    "neq_op",
    "lt_op",
    "le_op",
    "gt_op",
    "ge_op",
    "and_op",
    "or_op"
};

void P_print_exp_list(TR_ExpList exp_list, int offset);
void P_print_stm(TR_Stm stm, int offset);

void indent(int offset) {
    for (int i = 0; i < offset; ++i) {
        putchar(' ');
    }
}

void P_print_name(S_Symbol name, int offset) {
    indent(offset);
    printf("%s\n", S_name(name));
}

void P_print_exp(TR_Exp exp, int offset) {
    indent(offset); 
    printf("%s - reg: %d - size: %d\n",
            P_exp_names[exp->kind], exp->reg, exp->size);
    switch (exp->kind) {
        case TR_NUM_EXP:
            {
                indent(offset + OFFSET);
                printf("value: %d\n", exp->u.num);
                break;
            }
        case TR_STRING_EXP:
            {
                indent(offset + OFFSET);
                printf("%d: %s\n",
                        exp->u.str.label, exp->u.str.str);
                break;

            }
        case TR_MEM_EXP:
            {
                indent(offset + OFFSET);
                printf(
                        "%s - nesting: %d - offset: %d\n",
                        S_name(exp->u.mem.name),
                        exp->u.mem.nesting_level,
                        exp->u.mem.offset
                      );
                break;
            }
        case TR_VAR_EXP:
            {
                P_print_exp(exp->u.var, offset + OFFSET);
                break;
            }
        case TR_FIELD_EXP:
            {
                P_print_exp(exp->u.field.var, offset + OFFSET);
                P_print_name(exp->u.field.field_name, offset + OFFSET);
                indent(offset + OFFSET);
                printf("Offset: %d\n", exp->u.field.field_offset);
                break;
            }
        case TR_SUBSCRIPT_EXP:
            {
                P_print_exp(exp->u.subscript.var, offset + OFFSET);
                P_print_exp(exp->u.subscript.index, offset + OFFSET);
                break;
            }
        case TR_RECORD_EXP:
            {
                P_print_exp_list(exp->u.record, offset + OFFSET);
                break;
            }
        case TR_ARRAY_EXP:
            {
                indent(offset + OFFSET);
                printf("Initializer:\n");
                P_print_exp(exp->u.array, offset + 2 * OFFSET);
                break;
            }
        case TR_ARITH_OP_EXP:
            {
                indent(offset + OFFSET);
                printf("%s\n", P_op_names[exp->u.arith.op]);
                P_print_exp(exp->u.arith.left, offset + OFFSET);
                P_print_exp(exp->u.arith.right, offset + OFFSET);
                break;
            }
        case TR_DIV_OP_EXP:
            {
                P_print_exp(exp->u.arith.left, offset + OFFSET);
                P_print_exp(exp->u.arith.right, offset + OFFSET);
                break;
            }
        case TR_REL_OP_EXP:
            {
                indent(offset + OFFSET);
                printf("%s\n", P_op_names[exp->u.arith.op]);
                P_print_exp(exp->u.arith.left, offset + OFFSET);
                P_print_exp(exp->u.arith.right, offset + OFFSET);
                break;
            }
        case TR_FCALL_EXP:
            {
                P_print_name(exp->u.fcall.name, offset + OFFSET);
                P_print_exp_list(exp->u.fcall.args, offset + 2 * OFFSET);
                break;
            }
        case TR_SEQ_EXP:
            {
                TR_StmList stms = exp->u.seq;
                P_print_stm(stms->head, offset + OFFSET);
                for (TR_StmList stms = exp->u.seq; stms; stms = stms->tail) {
                    P_print_stm(stms->head, offset + OFFSET);
                }
                break;
            }
        default:
            break;
    }
}

void P_print_exp_list(TR_ExpList exp_list, int offset) {
    for (TR_ExpList exps = exp_list; exps; exps = exps->tail) {
        P_print_exp(exps->head, offset);
    }
}

void P_print_label(string name, TR_Label label, int offset) {
    indent(offset);
    printf("%s - %d:\n", name, label);
}

void P_print_stm(TR_Stm stm, int offset) {
    indent(offset); 
    printf("%s\n", P_stm_names[stm->kind]);
    switch (stm->kind) {
        case TR_ASSIGN_STM:
            {
                P_print_exp(stm->u.assign.value, offset + OFFSET);
                P_print_exp(stm->u.assign.var, offset + OFFSET);
                break;
            }
        case TR_PCALL_STM:
            {
                P_print_name(stm->u.pcall.name, offset + OFFSET);
                P_print_exp_list(stm->u.pcall.args, offset + 2 * OFFSET);
                break;
            }
        case TR_SEQ_STM:
            {
                TR_StmList stms = stm->u.seq;
                P_print_stm(stms->head, offset + OFFSET);
                for (TR_StmList stms = stm->u.seq; stms; stms = stms->tail) {
                    P_print_stm(stms->head, offset + OFFSET);
                }
                break;
            }
        case TR_IF_STM:
            {
                P_print_exp(stm->u.if_.test, offset + OFFSET);
                indent(offset + OFFSET);
                puts("True:");
                P_print_stm(stm->u.if_.true_branch, offset + 2 * OFFSET);
                indent(offset + OFFSET);
                printf("Skip: %d\n", stm->u.if_.false_label);
                break;
            }
        case TR_IF_ELSE_STM:
            {
                P_print_exp(stm->u.if_.test, offset + OFFSET);
                indent(offset + OFFSET);
                puts("True:");
                P_print_stm(stm->u.if_else.true_branch, offset + 2 * OFFSET);
                indent(offset + OFFSET);
                printf("False - %d:\n", stm->u.if_else.false_label);
                P_print_stm(stm->u.if_else.false_branch, offset + 2 * OFFSET);
                indent(offset + OFFSET);
                printf("Join: %d\n", stm->u.if_else.join_label);
                break;
            }
        case TR_WHILE_STM:
            {
                indent(offset + OFFSET);
                printf("Test - %d:\n", stm->u.while_.test_label);
                P_print_exp(stm->u.while_.test, offset + 2 * OFFSET);
                P_print_stm(stm->u.while_.body, offset + OFFSET);
                indent(offset + OFFSET);
                printf("Skip: %d\n", stm->u.while_.skip_label);
                break;
            }
        case TR_FOR_STM:
            {
                P_print_exp(stm->u.for_.var, offset + OFFSET);
                P_print_exp(stm->u.for_.lo, offset + OFFSET);
                P_print_exp(stm->u.for_.hi, offset + OFFSET);
                indent(offset + OFFSET);
                printf("Test: %d\n", stm->u.for_.test_label);
                P_print_stm(stm->u.for_.body, offset + OFFSET);
                indent(offset + OFFSET);
                printf("Skip: %d\n", stm->u.for_.skip_label);
                break;
            }
        case TR_BREAK_STM:
            {
                break;
            }
        case TR_EXP_STM:
            P_print_exp(stm->u.exp, offset + OFFSET);
            break;
        default:
            return;
    }
}

void P_print_function_body(TR_Function func) {
    if (!func->body) {
        return;
    }
    for (TR_StmList stms = func->body; stms; stms = stms->tail) {
        P_print_stm(stms->head, OFFSET);
    }
}

void P_print_ir(TR_Function func) {
    TR_print_function(func);
    indent(OFFSET);
    puts("Code:");
    P_print_function_body(func);
    putchar('\n');
    for (TR_FunctionList flist = func->children; flist; flist = flist->tail) {
        P_print_ir(flist->head);
    }
}

