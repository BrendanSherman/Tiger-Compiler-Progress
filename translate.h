#pragma once

#include "absyn.h"
#include "frame.h"
#include "symbol.h"

#define TR_RET_REG 0
#define TR_ESC_REG 15
#define TR_ESC_REG2 14

typedef struct TR_TransExp_ * TR_TransExp;
typedef struct TR_Function_ * TR_Function;
typedef struct TR_FunctionList_ * TR_FunctionList;
typedef struct TR_Stm_ * TR_Stm;
typedef struct TR_StmList_ * TR_StmList;
typedef struct TR_Exp_ * TR_Exp;
typedef struct TR_ExpList_ * TR_ExpList;
typedef struct TR_LabelList_ * TR_LabelList;
typedef struct TR_VarList_ *TR_VarList;
typedef int TR_Label;
typedef int TR_Temp;

struct TR_TransExp_ {
    enum { TR_FUNCTION, TR_STM, TR_EXP } kind;
    union {
        TR_Function function;
        TR_Stm stm;
        TR_Exp exp;
    } u;
};

struct TR_Function_ {
    S_Symbol name;
    F_Frame frame;
    TR_Function parent;
    TR_FunctionList children;
    TR_StmList body;
};

struct TR_FunctionList_ {
    TR_Function head;
    TR_FunctionList tail;
};

struct TR_VarList_ {
    F_Var head;
    TR_VarList tail;
};

struct TR_Stm_ {
    enum {
        TR_ASSIGN_STM,
        TR_PCALL_STM,
        TR_SEQ_STM,
        TR_IF_STM,
        TR_IF_ELSE_STM,
        TR_WHILE_STM,
        TR_FOR_STM,
        TR_BREAK_STM,
        TR_EXP_STM
    } kind;
    union {
        struct { TR_Exp value; TR_Exp var; } assign;
        struct { S_Symbol name; TR_ExpList args; } pcall;
        TR_StmList seq;
        struct {
            TR_Exp test;
            TR_Label false_label;
            TR_Stm true_branch;
        } if_;
        struct {
            TR_Exp test;
            TR_Label false_label;
            TR_Stm true_branch;
            TR_Stm false_branch;
            TR_Label join_label;
        } if_else;
        struct {
            TR_Label test_label;
            TR_Exp test;
            TR_Label skip_label;
            TR_Stm body;
        } while_;
        struct {
            TR_Exp var;
            TR_Exp lo;
            TR_Exp hi;
            TR_Label test_label;
            TR_Label skip_label;
            TR_Stm body;
        } for_;
        TR_Label break_;
        TR_Exp exp;
    } u;
};

struct TR_StmList_ {
    TR_Stm head;
    TR_StmList tail;
};

struct TR_Exp_ {
    int size;
    TR_Temp reg;
    enum {
        TR_NUM_EXP,
        TR_STRING_EXP,
        TR_MEM_EXP,
        TR_VAR_EXP,
        TR_FIELD_EXP,
        TR_SUBSCRIPT_EXP,
        TR_RECORD_EXP,
        TR_ARRAY_EXP,
        TR_ARITH_OP_EXP,
        TR_DIV_OP_EXP,
        TR_REL_OP_EXP,
        TR_IF_EXP,
        TR_IF_ELSE_EXP,
        TR_FCALL_EXP,
        TR_SEQ_EXP
    } kind;
    union {
        int num;
        struct { string str; TR_Label label; } str;
        struct { S_Symbol name; int nesting_level; int offset; } mem;
        TR_Exp var;
        struct { TR_Exp var; S_Symbol field_name; int field_offset; } field;
        struct { TR_Exp var; TR_Exp index; } subscript;
        struct { TR_Exp left; TR_Exp right; A_Oper op; } arith;
        struct { TR_Exp left; TR_Exp right; } div;
        struct { TR_Exp left; TR_Exp right; A_Oper op; } rel;
        struct {
            TR_Exp test;
            TR_Label false_label;
            TR_Exp true_branch;
        } if_;
        struct {
            TR_Exp test;
            TR_Label false_label;
            TR_Exp true_branch;
            TR_Exp false_branch;
            TR_Label join_label;
        } if_else;
        struct { S_Symbol name; TR_ExpList args; } fcall;
        TR_Exp array;
        TR_ExpList  record;
        TR_StmList seq;
    } u;
};

struct TR_ExpList_ {
    TR_Exp head;
    TR_ExpList tail;
};

struct TR_LabelList_ {
    TR_Label head; 
    TR_LabelList tail;
};

extern TR_LabelList TR_loop_list;

TR_TransExp make_TR_TransFunction(TR_Function function);
TR_TransExp make_TR_TransStm(TR_Stm stm);
TR_TransExp make_TR_TransExp(TR_Exp exp);

TR_Function make_TR_Function(S_Symbol name, F_Frame frame);
TR_FunctionList make_TR_FunctionList(TR_Function head, TR_FunctionList tail);
void TR_append_function(TR_Function parent, TR_Function child);
void TR_add_param(TR_Function func, S_Symbol name, T_Type type);
void TR_add_var(TR_Function func, S_Symbol name, T_Type type);
void TR_add_stm_to_function(TR_Function func, TR_Stm stm);
void TR_add_stm_list_to_function(TR_Function func, TR_StmList stms);

TR_Stm make_TR_AssignStm(TR_Exp value, TR_Exp var);
TR_Stm make_TR_PCallStm(S_Symbol name, TR_ExpList args);
TR_Stm make_TR_SeqStm(TR_StmList stms);
TR_Stm make_TR_IfStm(TR_Exp test, TR_Stm true_branch);
TR_Stm make_TR_IfElseStm(TR_Exp test, TR_Stm true_branch, TR_Stm false_branch);
TR_Stm make_TR_WhileStm(TR_Exp test, TR_Stm body);
TR_Stm make_TR_ForStm(TR_Exp var, TR_Exp lo, TR_Exp hi, TR_Stm body);
TR_Stm make_TR_BreakStm(TR_Label skip_label);
TR_Stm make_TR_ExpStm(TR_Exp exp);

TR_StmList make_TR_StmList(TR_Stm head, TR_StmList tail);
TR_StmList TR_add_stm(TR_StmList list, TR_Stm stm);



TR_Exp make_TR_NumExp(int num);
TR_Exp make_TR_StringExp(string lit);
TR_Exp make_TR_MemExp(S_Table venv, S_Symbol sym);
TR_Exp make_TR_VarExp(TR_Exp var);
TR_Exp make_TR_FieldExp(TR_Exp var, S_Symbol field_name, int field_size, int field_offset);
TR_Exp make_TR_SubscriptExp(TR_Exp var, int element_size, TR_Exp index);
TR_Exp make_TR_RecordExp(int size, TR_ExpList init);
TR_Exp make_TR_ArrayExp(int size, TR_Exp inits);
TR_Exp make_TR_ArithOpExp(TR_Exp left, TR_Exp right, A_Oper op);
TR_Exp make_TR_DivOpExp(TR_Exp left, TR_Exp right);
TR_Exp make_TR_RelOpExp(TR_Exp left, TR_Exp right, A_Oper op);
TR_Exp make_TR_IfExp(TR_Exp test, TR_Exp true_branch);
TR_Exp make_TR_IfElseExp(TR_Exp test, TR_Exp true_branch, TR_Exp false_branch);
TR_Exp make_TR_FCallExp(S_Table venv, S_Symbol name, TR_ExpList args);
TR_Exp make_TR_SeqExp(TR_StmList stms);

TR_Stm TR_convert_seq_exp_to_stm(TR_Exp seq);
TR_Exp TR_convert_seq_stm_to_exp(TR_Stm seq, int size);

TR_ExpList make_TR_ExpList(TR_Exp head, TR_ExpList tail);
TR_ExpList TR_add_exp(TR_ExpList list, TR_Exp exp);

TR_LabelList make_TR_LabelList(TR_Label label);
void TR_push_loop(TR_Label label);
void TR_pop_loop();

TR_Label TR_new_label();
TR_Temp TR_new_temp();

void TR_print_function(TR_Function func);
