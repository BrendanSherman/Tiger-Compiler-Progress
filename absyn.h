/*
 * absyn.h -
 * The AST module, representing the abstract syntax
 * of Tiger programs.
 * See Andrew Appel, _Modern Compiler Implementation in C_,
 * Chapter 4.
 * All types and functions declared in this module begin with "A_."
 * Linked list types end with "..List."
 * Author: Andrew Appel.
 * Revised and reformatted by Amittai Aviram - aviram@bc.edu.
 */

#pragma once

#include "errormsg.h"
#include "symbol.h"

/* Type Definitions */

typedef E_Pos A_Pos;
typedef struct A_Var_ * A_Var;
typedef struct A_Exp_ * A_Exp;
typedef struct A_Dec_ * A_Dec;
typedef struct A_Type_ * A_Type;
typedef struct A_DecList_ * A_DecList;
typedef struct A_ExpList_ * A_ExpList;
typedef struct A_Field_ * A_Field;
typedef struct A_FieldList_ * A_FieldList;
typedef struct A_FunDec_ * A_FunDec;
typedef struct A_FunDecList_ * A_FunDecList;
typedef struct A_TypeDec_ * A_TypeDec;
typedef struct A_TypeDecList_ * A_TypeDecList;
typedef struct A_EField_ * A_EField;
typedef struct A_EFieldList_ * A_EFieldList;

typedef enum {
    A_PLUS_OP,
    A_MINUS_OP,
    A_TIMES_OP,
    A_DIVIDE_OP,
    A_EQ_OP,
    A_NEQ_OP,
    A_LT_OP,
    A_LE_OP,
    A_GT_OP,
    A_GE_OP,
    A_AND_OP,
    A_OR_OP
} A_Oper;

struct A_Var_ {
    enum {A_SIMPLE_VAR, A_FIELD_VAR, A_SUBSCRIPT_VAR} kind;
    A_Pos pos;
    union {
        S_Symbol simple;
        struct {A_Var var; S_Symbol sym;} field;
        struct {A_Var var; A_Exp exp;} subscript;
    } u;
};

struct A_Exp_ {
    enum {
        A_VAR_EXP,
        A_NIL_EXP,
        A_INT_EXP,
        A_STRING_EXP,
        A_CALL_EXP,
        A_OP_EXP,
        A_RECORD_EXP,
        A_SEQ_EXP,
        A_ASSIGN_EXP,
        A_IF_EXP,
        A_WHILE_EXP,
        A_FOR_EXP,
        A_BREAK_EXP,
        A_LET_EXP,
        A_ARRAY_EXP}
    kind;
    A_Pos pos;
    union {
        A_Var var;
        /* nil; - needs only the pos */
        int intt;
        string stringg;
        struct {S_Symbol func; A_ExpList args;} call;
        struct {A_Oper oper; A_Exp left; A_Exp right;} op;
        struct {S_Symbol type; A_EFieldList fields;} record;
        A_ExpList seq;
        struct {A_Var var; A_Exp exp;} assign;
        struct {A_Exp test; A_Exp then; A_Exp elsee;} iff; /* elsee is optional */
        struct {A_Exp test; A_Exp body;} whilee;
        struct {S_Symbol var; A_Exp lo; A_Exp hi; A_Exp body; bool escape;} forr;
        /* breakk; - need only the pos */
        struct {A_DecList decs; A_Exp body;} let;
        struct {S_Symbol type; A_Exp size; A_Exp init;} array;
    } u;
};

struct A_Dec_ {
    enum {A_TYPE_DEC_GROUP, A_VAR_DEC, A_FUNCTION_DEC_GROUP} kind;
    A_Pos pos;
    union {
        A_TypeDecList type;
        struct {S_Symbol var; S_Symbol type; A_Exp init; bool escape;} var;
        /* escape may change after the initial declaration */
        A_FunDecList function;
    } u;
};

struct A_Type_ {
    enum {A_NAME_TYPE, A_RECORD_TYPE, A_ARRAY_TYPE} kind;
    A_Pos pos;
    union {
        S_Symbol name;
        A_FieldList record;
        S_Symbol array;
    } u;
};

/* Linked lists and nodes of lists */

struct A_Field_ {S_Symbol name; S_Symbol type; A_Pos pos; bool escape;};
struct A_FieldList_ {A_Field head; A_FieldList tail;};
struct A_ExpList_ {A_Exp head; A_ExpList tail;};

struct A_FunDec_ {
    A_Pos pos;
    S_Symbol name;
    A_FieldList params; 
    S_Symbol result;
    A_Exp body;
};

struct A_FunDecList_ {A_FunDec head; A_FunDecList tail;};
struct A_DecList_ {A_Dec head; A_DecList tail;};
struct A_TypeDec_ {S_Symbol name; A_Type type;};
struct A_TypeDecList_ {A_TypeDec head; A_TypeDecList tail;};
struct A_EField_ {S_Symbol name; A_Exp exp;};
struct A_EFieldList_ {A_EField head; A_EFieldList tail;};


/* Function Prototypes */
A_Var make_A_SimpleVar(A_Pos pos, S_Symbol sym);
A_Var make_A_FieldVar(A_Pos pos, A_Var var, S_Symbol sym);
A_Var make_A_SubscriptVar(A_Pos pos, A_Var var, A_Exp exp);
A_Exp make_A_VarExp(A_Pos pos, A_Var var);
A_Exp make_A_NilExp(A_Pos pos);
A_Exp make_A_IntExp(A_Pos pos, int i);
A_Exp make_A_StringExp(A_Pos pos, string s);
A_Exp make_A_CallExp(A_Pos pos, S_Symbol func, A_ExpList args);
A_Exp make_A_OpExp(A_Pos pos, A_Oper oper, A_Exp left, A_Exp right);
A_Exp make_A_RecordExp(A_Pos pos, S_Symbol type, A_EFieldList fields);
A_Exp make_A_SeqExp(A_Pos pos, A_ExpList seq);
A_Exp make_A_AssignExp(A_Pos pos, A_Var var, A_Exp exp);
A_Exp make_A_IfExp(A_Pos pos, A_Exp test, A_Exp then, A_Exp elsee);
A_Exp make_A_WhileExp(A_Pos pos, A_Exp test, A_Exp body);
A_Exp make_A_ForExp(A_Pos pos, S_Symbol var, A_Exp lo, A_Exp hi, A_Exp body);
A_Exp make_A_BreakExp(A_Pos pos);
A_Exp make_A_LetExp(A_Pos pos, A_DecList decs, A_Exp body);
A_Exp make_A_ArrayExp(A_Pos pos, S_Symbol type, A_Exp size, A_Exp init);
A_EField make_A_EField(S_Symbol name, A_Exp exp);
A_EFieldList make_A_EFieldList(A_EField head, A_EFieldList tail);
A_ExpList make_A_ExpList(A_Exp head, A_ExpList tail);
A_Dec make_A_TypeDecGroup(A_Pos pos, A_TypeDecList type);
A_Dec make_A_VarDec(A_Pos pos, S_Symbol var, S_Symbol type, A_Exp init);
A_Dec make_A_FunctionDecGroup(A_Pos pos, A_FunDecList function);
A_Type make_A_NameType(A_Pos pos, S_Symbol name);
A_Type make_A_RecordType(A_Pos pos, A_FieldList record);
A_Type make_A_ArrayType(A_Pos pos, S_Symbol array);
A_Field make_A_Field(A_Pos pos, S_Symbol name, S_Symbol type);
A_FieldList make_A_FieldList(A_Field head, A_FieldList tail);
A_FunDec make_A_FunDec(A_Pos pos, S_Symbol name, A_FieldList params, S_Symbol result,
        A_Exp body);
A_FunDecList make_A_FunDecList(A_FunDec head, A_FunDecList tail);
A_TypeDec make_A_TypeDec(S_Symbol name, A_Type type);
A_TypeDecList make_A_TypeDecList(A_TypeDec head, A_TypeDecList tail);
A_DecList make_A_DecList(A_Dec head, A_DecList tail);


// Added array prefix at end for now, we can fix order later 
typedef struct A_ArrayPrefix_ * A_ArrayPrefix;

struct A_ArrayPrefix_ {
    A_Pos pos;
    S_Symbol name;
    A_Exp index;
};

A_ArrayPrefix make_A_ArrayPrefix(A_Pos pos, S_Symbol name, A_Exp index);