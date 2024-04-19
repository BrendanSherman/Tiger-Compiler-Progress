/*
 * absyn.c - Abstract syntax functions. Most functions create
 * an instance of an abstract syntax rule.
 */

#include <stdbool.h>
// abstract syntax data structures
#include "absyn.h"
#include "util.h"
// symbol table data structures
#include "symbol.h" 

A_Var make_A_SimpleVar(A_Pos pos, S_Symbol sym) {
    A_Var p = malloc_checked(sizeof(*p));
    p->kind = A_SIMPLE_VAR;
    p->pos = pos;
    p->u.simple = sym;
    return p;
}

A_Var make_A_FieldVar(A_Pos pos, A_Var var, S_Symbol sym) {
    A_Var p = malloc_checked(sizeof(*p));
    p->kind = A_FIELD_VAR;
    p->pos = pos;
    p->u.field.var = var;
    p->u.field.sym = sym;
    return p;
}

A_Var make_A_SubscriptVar(A_Pos pos, A_Var var, A_Exp exp) {
    A_Var p = malloc_checked(sizeof(*p));
    p->kind = A_SUBSCRIPT_VAR;
    p->pos = pos;
    p->u.subscript.var = var;
    p->u.subscript.exp = exp;
    return p;
}

A_Exp make_A_VarExp(A_Pos pos, A_Var var) {
    A_Exp p = malloc_checked(sizeof(*p));
    p->kind = A_VAR_EXP;
    p->pos = pos;
    p->u.var = var;
    return p;
}

A_Exp make_A_NilExp(A_Pos pos) {
    A_Exp p = malloc_checked(sizeof(*p));
    p->kind = A_NIL_EXP;
    p->pos = pos;
    return p;
}

A_Exp make_A_IntExp(A_Pos pos, int i) {
    A_Exp p = malloc_checked(sizeof(*p));
    p->kind = A_INT_EXP;
    p->pos = pos;
    p->u.intt = i;
    return p;
}

A_Exp make_A_StringExp(A_Pos pos, string s) {
    A_Exp p = malloc_checked(sizeof(*p));
    p->kind = A_STRING_EXP;
    p->pos = pos;
    p->u.stringg = s;
    return p;
}

A_Exp make_A_CallExp(A_Pos pos, S_Symbol func, A_ExpList args) {
    A_Exp p = malloc_checked(sizeof(*p));
    p->kind = A_CALL_EXP;
    p->pos = pos;
    p->u.call.func = func;
    p->u.call.args = args;
    return p;
}

A_Exp make_A_OpExp(A_Pos pos, A_Oper oper, A_Exp left, A_Exp right) {
    A_Exp p = malloc_checked(sizeof(*p));
    p->kind = A_OP_EXP;
    p->pos = pos;
    p->u.op.oper = oper;
    p->u.op.left = left;
    p->u.op.right = right;
    return p;
}

A_Exp make_A_RecordExp(A_Pos pos, S_Symbol type, A_EFieldList fields) {
    A_Exp p = malloc_checked(sizeof(*p));
    p->kind = A_RECORD_EXP;
    p->pos = pos;
    p->u.record.type = type;
    p->u.record.fields = fields;
    return p;
}

A_Exp make_A_SeqExp(A_Pos pos, A_ExpList seq) {
    A_Exp p = malloc_checked(sizeof(*p));
    p->kind = A_SEQ_EXP;
    p->pos = pos;
    p->u.seq = seq;
    return p;
}

A_Exp make_A_AssignExp(A_Pos pos, A_Var var, A_Exp exp) {
    A_Exp p = malloc_checked(sizeof(*p));
    p->kind = A_ASSIGN_EXP;
    p->pos = pos;
    p->u.assign.var = var;
    p->u.assign.exp = exp;
    return p;
}

A_Exp make_A_IfExp(A_Pos pos, A_Exp test, A_Exp then, A_Exp elsee) {
    A_Exp p = malloc_checked(sizeof(*p));
    p->kind = A_IF_EXP;
    p->pos = pos;
    p->u.iff.test = test;
    p->u.iff.then = then;
    p->u.iff.elsee = elsee;
    return p;
}

A_Exp make_A_WhileExp(A_Pos pos, A_Exp test, A_Exp body) {
    A_Exp p = malloc_checked(sizeof(*p));
    p->kind = A_WHILE_EXP;
    p->pos = pos;
    p->u.whilee.test = test;
    p->u.whilee.body = body;
    return p;
}

A_Exp make_A_ForExp(A_Pos pos, S_Symbol var, A_Exp lo, A_Exp hi, A_Exp body) {
    A_Exp p = malloc_checked(sizeof(*p));
    p->kind = A_FOR_EXP;
    p->pos = pos;
    p->u.forr.var = var;
    p->u.forr.lo = lo;
    p->u.forr.hi = hi;
    p->u.forr.body = body;
    p->u.forr.escape = true;
    return p;
}

A_Exp make_A_BreakExp(A_Pos pos) {
    A_Exp p = malloc_checked(sizeof(*p));
    p->kind = A_BREAK_EXP;
    p->pos = pos;
    return p;
}

A_Exp make_A_LetExp(A_Pos pos, A_DecList decs, A_Exp body) {
    A_Exp p = malloc_checked(sizeof(*p));
    p->kind = A_LET_EXP;
    p->pos = pos;
    p->u.let.decs = decs;
    p->u.let.body = body;
    return p;
}

A_Exp make_A_ArrayExp(A_Pos pos, S_Symbol type, A_Exp size, A_Exp init) {
    A_Exp p = malloc_checked(sizeof(*p));
    p->kind = A_ARRAY_EXP;
    p->pos = pos;
    p->u.array.type = type;
    p->u.array.size = size;
    p->u.array.init = init;
    return p;
}

A_Dec make_A_FunctionDecGroup(A_Pos pos, A_FunDecList function) {
    A_Dec p = malloc_checked(sizeof(*p));
    p->kind = A_FUNCTION_DEC_GROUP;
    p->pos = pos;
    p->u.function = function;
    return p;
}

A_Dec make_A_VarDec(A_Pos pos, S_Symbol var, S_Symbol type, A_Exp init) {
    A_Dec p = malloc_checked(sizeof(*p));
    p->kind = A_VAR_DEC;
    p->pos = pos;
    p->u.var.var = var;
    p->u.var.type = type;
    p->u.var.init = init;
    p->u.var.escape = true;
    return p;
}

A_Dec make_A_TypeDecGroup(A_Pos pos, A_TypeDecList type) {
    A_Dec p = malloc_checked(sizeof(*p));
    p->kind = A_TYPE_DEC_GROUP;
    p->pos = pos;
    p->u.type = type;
    return p;
}

A_Type make_A_NameType(A_Pos pos, S_Symbol name) {
    A_Type p = malloc_checked(sizeof(*p));
    p->kind = A_NAME_TYPE;
    p->pos = pos;
    p->u.name = name;
    return p;
}

A_Type make_A_RecordType(A_Pos pos, A_FieldList record) {
    A_Type p = malloc_checked(sizeof(*p));
    p->kind = A_RECORD_TYPE;
    p->pos = pos;
    p->u.record = record;
    return p;
}

A_Type make_A_ArrayType(A_Pos pos, S_Symbol array) {
    A_Type p = malloc_checked(sizeof(*p));
    p->kind = A_ARRAY_TYPE;
    p->pos = pos;
    p->u.array = array;
    return p;
}

A_Field make_A_Field(A_Pos pos, S_Symbol name, S_Symbol type) {
    A_Field p = malloc_checked(sizeof(*p));
    p->pos = pos;
    p->name = name;
    p->type = type;
    p->escape = true;
    return p;
}

A_FieldList make_A_FieldList(A_Field head, A_FieldList tail) {
    A_FieldList p = malloc_checked(sizeof(*p));
    p->head = head;
    p->tail = tail;
    return p;
}

A_ExpList make_A_ExpList(A_Exp head, A_ExpList tail) {
    A_ExpList p = malloc_checked(sizeof(*p));
    p->head = head;
    p->tail = tail;
    return p;
}

A_FunDec make_A_FunDec(A_Pos pos, S_Symbol name, A_FieldList params, S_Symbol result, A_Exp body) {
    A_FunDec p = malloc_checked(sizeof(*p));
    p->pos = pos;
    p->name = name;
    p->params = params;
    p->result = result;
    p->body = body;
    return p;
}

A_FunDecList make_A_FunDecList(A_FunDec head, A_FunDecList tail) {
    A_FunDecList p = malloc_checked(sizeof(*p));
    p->head = head;
    p->tail = tail;
    return p;
}

A_DecList make_A_DecList(A_Dec head, A_DecList tail) {
    A_DecList p = malloc_checked(sizeof(*p));
    p->head = head;
    p->tail = tail;
    return p;
}

A_TypeDec make_A_TypeDec(S_Symbol name, A_Type type) {
    A_TypeDec p = malloc_checked(sizeof(*p));
    p->name = name;
    p->type = type;
    return p;
}

A_TypeDecList make_A_TypeDecList(A_TypeDec head, A_TypeDecList tail) {
    A_TypeDecList p = malloc_checked(sizeof(*p));
    p->head = head;
    p->tail = tail;
    return p;
}

A_EField make_A_EField(S_Symbol name, A_Exp exp) {
    A_EField p = malloc_checked(sizeof(*p));
    p->name = name;
    p->exp = exp;
    return p;
}

A_EFieldList make_A_EFieldList(A_EField head, A_EFieldList tail) {
    A_EFieldList p = malloc_checked(sizeof(*p));
    p->head = head;
    p->tail = tail;
    return p;
}

A_ArrayPrefix make_A_ArrayPrefix(A_Pos pos, S_Symbol name, A_Exp index) {
    A_ArrayPrefix prefix = malloc_checked(sizeof(*prefix));
    prefix->pos = pos;
    prefix->name = name;
    prefix->index = index;
    return prefix;
}
