/*
 * prabsyn.c -
 * Prints abstract syntax data structures.
 * Most functions handle an instance
 * of an abstract syntax rule.
 * Author: Andrew Appel.
 * Reformatted by Amittai Aviram - aviram@bc.edu.
 */

#include <stdio.h>
#include "absyn.h"  /* abstract syntax data structures */
#include "prabsyn.h" /* function prototype */
#include "symbol.h" /* symbol table data structures */
#include "util.h"

/* local function prototypes */
static void pr_var(FILE * out, A_Var v, int d);
static void pr_dec(FILE * out, A_Dec v, int d);
static void pr_type(FILE * out, A_Type v, int d);
static void pr_field(FILE * out, A_Field v, int d);
static void pr_fieldList(FILE * out, A_FieldList v, int d);
static void pr_expList(FILE * out, A_ExpList v, int d);
static void pr_funDec(FILE * out, A_FunDec v, int d);
static void pr_funDecList(FILE * out, A_FunDecList v, int d);
static void pr_decList(FILE * out, A_DecList v, int d);
static void pr_typeDec(FILE * out, A_TypeDec v, int d);
static void pr_typeDecList(FILE * out, A_TypeDecList v, int d);
static void pr_efield(FILE * out, A_EField v, int d);
static void pr_efieldList(FILE * out, A_EFieldList v, int d);

static void indent(FILE * out, int d) {
    for (int i = 0; i <= d; i++) fprintf(out, " ");
}

/* Print A_Var types. Indent d spaces. */
static void pr_var(FILE * out, A_Var v, int d) {
    indent(out, d);
    switch (v->kind) {
        case A_SIMPLE_VAR:
            fprintf(out, "simpleVar(%s)", S_name(v->u.simple)); 
            break;
        case A_FIELD_VAR:
            fprintf(out, "%s\n", "fieldVar(");
            pr_var(out, v->u.field.var, d + 1);
            fprintf(out, ",\n"); 
            indent(out, d + 1);
            fprintf(out, "%s)", S_name(v->u.field.sym));
            break;
        case A_SUBSCRIPT_VAR:
            fprintf(out, "%s\n", "subscriptVar(");
            pr_var(out, v->u.subscript.var, d + 1);
            fprintf(out, "%s\n", ","); 
            pr_exp(out, v->u.subscript.exp, d + 1);
            fprintf(out, ")");
            break;
        default:
            assert(0); 
    } 
}

char * str_oper[] = {
    "PLUS", "MINUS", "TIMES", "DIVIDE", 
    "EQUAL", "NOTEQUAL", "LESSTHAN", "LESSEQ", "GREATER", "GREATEREQ",
    "AND", "OR"
};

void pr_oper(FILE * out, A_Oper d) {
    fprintf(out, "%s", str_oper[d]);
}

/* Print A_Var types. Indent d spaces. */
void pr_exp(FILE * out, A_Exp v, int d) {
    indent(out, d);
    switch (v->kind) {
        case A_VAR_EXP:
            fprintf(out, "varExp(\n"); pr_var(out, v->u.var, d + 1); 
            fprintf(out, ")");
            break;
        case A_NIL_EXP:
            fprintf(out, "nilExp()");
            break;
        case A_INT_EXP:
            fprintf(out, "intExp(%d)", v->u.intt);
            break;
        case A_STRING_EXP:
            fprintf(out, "stringExp(%s)", v->u.stringg);
            break;
        case A_CALL_EXP:
            fprintf(out, "callExp(%s,\n", S_name(v->u.call.func));
            pr_expList(out, v->u.call.args, d + 1);
            fprintf(out, ")");
            break;
        case A_OP_EXP:
            fprintf(out, "opExp(\n");
            indent(out, d + 1); pr_oper(out, v->u.op.oper);
            fprintf(out, ",\n"); 
            pr_exp(out, v->u.op.left, d + 1);
            fprintf(out, ",\n"); 
            pr_exp(out, v->u.op.right, d + 1);
            fprintf(out, ")");
            break;
        case A_RECORD_EXP:
            fprintf(out, "recordExp(%s,\n", S_name(v->u.record.type)); 
            pr_efieldList(out, v->u.record.fields, d + 1);
            fprintf(out, ")");
            break;
        case A_SEQ_EXP:
            fprintf(out, "seqExp(\n");
            pr_expList(out, v->u.seq, d + 1);
            fprintf(out, ")");
            break;
        case A_ASSIGN_EXP:
            fprintf(out, "assignExp(\n");
            pr_var(out, v->u.assign.var, d + 1);
            fprintf(out, ",\n");
            pr_exp(out, v->u.assign.exp, d + 1);
            fprintf(out, ")");
            break;
        case A_IF_EXP:
            fprintf(out, "iffExp(\n");
            pr_exp(out, v->u.iff.test, d + 1);
            fprintf(out, ",\n");
            pr_exp(out, v->u.iff.then, d + 1);
            if (v->u.iff.elsee) { /* else is optional */
                fprintf(out, ",\n");
                pr_exp(out, v->u.iff.elsee, d + 1);
            }
            fprintf(out, ")");
            break;
        case A_WHILE_EXP:
            fprintf(out, "whileExp(\n");
            pr_exp(out, v->u.whilee.test, d + 1);
            fprintf(out, ",\n");
            pr_exp(out, v->u.whilee.body, d + 1);
            fprintf(out, ")\n");
            break;
        case A_FOR_EXP:
            fprintf(out, "forExp(%s,\n", S_name(v->u.forr.var)); 
            pr_exp(out, v->u.forr.lo, d + 1);
            fprintf(out, ",\n");
            pr_exp(out, v->u.forr.hi, d + 1);
            fprintf(out, "%s\n", ",");
            pr_exp(out, v->u.forr.body, d + 1);
            fprintf(out, ",\n");
            indent(out, d + 1);
            fprintf(out, "%s", v->u.forr.escape ? "TRUE)" : "FALSE)");
            break;
        case A_BREAK_EXP:
            fprintf(out, "breakExp()");
            break;
        case A_LET_EXP:
            fprintf(out, "letExp(\n");
            pr_decList(out, v->u.let.decs, d + 1);
            fprintf(out, ",\n");
            pr_exp(out, v->u.let.body, d + 1);
            fprintf(out, ")");
            break;
        case A_ARRAY_EXP:
            fprintf(out, "arrayExp(%s,\n", S_name(v->u.array.type));
            pr_exp(out, v->u.array.size, d + 1);
            fprintf(out, ",\n");
            pr_exp(out, v->u.array.init, d + 1);
            fprintf(out, ")");
            break;
        default:
            assert(0); 
    } 
}

static void pr_dec(FILE * out, A_Dec v, int d) {
    indent(out, d);
    switch (v->kind) {
        case A_FUNCTION_DEC_GROUP:
            fprintf(out, "functionDecGroup(\n"); 
            pr_funDecList(out, v->u.function, d + 1);
            fprintf(out, ")");
            break;
        case A_VAR_DEC:
            fprintf(out, "varDec(%s,\n", S_name(v->u.var.var));
            if (v->u.var.type) {
                indent(out, d + 1);
                fprintf(out, "%s,\n", S_name(v->u.var.type)); 
            }
            pr_exp(out, v->u.var.init, d + 1);
            fprintf(out, ",\n");
            indent(out, d + 1);
            fprintf(out, "%s", v->u.var.escape ? "TRUE)" : "FALSE)");
            break;
        case A_TYPE_DEC_GROUP:
            fprintf(out, "typeDec(\n"); 
            pr_typeDecList(out, v->u.type, d + 1);
            fprintf(out, ")");
            break;
        default:
            assert(0); 
    } 
}

static void pr_type(FILE * out, A_Type v, int d) {
    indent(out, d);
    switch (v->kind) {
        case A_NAME_TYPE:
            fprintf(out, "nameType(%s)", S_name(v->u.name));
            break;
        case A_RECORD_TYPE:
            fprintf(out, "recordType(\n");
            pr_fieldList(out, v->u.record, d + 1);
            fprintf(out, ")");
            break;
        case A_ARRAY_TYPE:
            fprintf(out, "arrayType(%s)", S_name(v->u.array));
            break;
        default:
            assert(0); 
    } 
}

static void pr_field(FILE * out, A_Field v, int d) {
    indent(out, d);
    fprintf(out, "field(%s,\n", S_name(v->name));
    indent(out, d + 1);
    fprintf(out, "%s,\n", S_name(v->type));
    indent(out, d + 1);
    fprintf(out, "%s", v->escape ? "TRUE)" : "FALSE)");
}

static void pr_fieldList(FILE * out, A_FieldList v, int d) {
    indent(out, d);
    if (v) {
        fprintf(out, "fieldList(\n");
        pr_field(out, v->head, d + 1);
        fprintf(out, ",\n");
        pr_fieldList(out, v->tail, d + 1);
        fprintf(out, ")");
    }
    else fprintf(out, "fieldList()");
}

static void pr_expList(FILE * out, A_ExpList v, int d) {
    indent(out, d);
    if (v) {
        fprintf(out, "expList(\n"); 
        pr_exp(out, v->head, d + 1);
        fprintf(out, ",\n");
        pr_expList(out, v->tail, d + 1);
        fprintf(out, ")");
    }
    else fprintf(out, "expList()"); 

}

static void pr_funDec(FILE * out, A_FunDec v, int d) {
    indent(out, d);
    fprintf(out, "funDec(%s,\n", S_name(v->name));
    pr_fieldList(out, v->params, d + 1);
    fprintf(out, ",\n");
    if (v->result) {
        indent(out, d + 1);
        fprintf(out, "%s,\n", S_name(v->result));
    }
    pr_exp(out, v->body, d + 1);
    fprintf(out, ")");
}

static void pr_funDecList(FILE * out, A_FunDecList v, int d) {
    indent(out, d);
    if (v) {
        fprintf(out, "funDecList(\n"); 
        pr_funDec(out, v->head, d + 1);
        fprintf(out, ",\n");
        pr_funDecList(out, v->tail, d + 1);
        fprintf(out, ")");
    }
    else fprintf(out, "funDecList()");
}

static void pr_decList(FILE * out, A_DecList v, int d) {
    indent(out, d);
    if (v) {
        fprintf(out, "decList(\n"); 
        pr_dec(out, v->head, d + 1);
        fprintf(out, ",\n");
        pr_decList(out, v->tail, d + 1);
        fprintf(out, ")");
    }
    else fprintf(out, "decList()"); 

}

static void pr_typeDec(FILE * out, A_TypeDec v, int d) {
    indent(out, d);
    fprintf(out, "typeDec(%s,\n", S_name(v->name)); 
    pr_type(out, v->type, d + 1);
    fprintf(out, ")");
}

static void pr_typeDecList(FILE * out, A_TypeDecList v, int d) {
    indent(out, d);
    if (v) {
        fprintf(out, "typeDecList(\n"); 
        pr_typeDec(out, v->head, d + 1);
        fprintf(out, ",\n");
        pr_typeDecList(out, v->tail, d + 1);
        fprintf(out, ")");
    }
    else fprintf(out, "typeDecList()");
}

static void pr_efield(FILE * out, A_EField v, int d) {
    indent(out, d);
    if (v) {
        fprintf(out, "efield(%s,\n", S_name(v->name));
        pr_exp(out, v->exp, d + 1);
        fprintf(out, ")");
    }
    else fprintf(out, "efield()");
}

static void pr_efieldList(FILE * out, A_EFieldList v, int d) {
    indent(out, d);
    if (v) {
        fprintf(out, "efieldList(\n"); 
        pr_efield(out, v->head, d + 1);
        fprintf(out, ",\n");
        pr_efieldList(out, v->tail, d + 1);
        fprintf(out, ")");
    }
    else fprintf(out, "efieldList()");
}



 
