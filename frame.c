#include "frame.h"
#include "translate.h"
#include <stdio.h>
#include <stdlib.h>

F_Frame make_F_Frame(int nesting_lvl) {
    F_Frame frame = malloc_checked(sizeof(*frame));
    frame->nesting_level = nesting_lvl;
    frame->parameters = NULL;
    frame->variables = NULL;
    frame->end = 0;
    return frame;
}

F_Var make_F_Var(S_Symbol name, T_Type type) {
    F_Var var = malloc_checked(sizeof(*var));
    var->name = name;
    var->type = type;
    return var;
}

void F_add_param(F_Frame frame, F_Var param) {
    // Add new param to head of existing params list
    TR_VarList list = malloc_checked(sizeof(*list));
    list->head = param;
    list->tail = frame->parameters;
    frame->parameters = list;
    // Use T_size function to update end variable
    frame->end += T_size(param->type);
}

void F_add_var(F_Frame frame, F_Var var) {
    TR_VarList list = malloc_checked(sizeof(*list));
    list->head = var;
    list->tail = frame->variables;
    frame->variables = list;
    // Use T_size function to update end variable
    frame->end += T_size(var->type);
}

void F_print_frame(F_Frame frame) {
    printf("\t\tNesting Level: %d\n", frame->nesting_level);
    if(frame->parameters) {
        printf("\t\tCurrent Parameters:\n");
        // Print name and type for each parameter
        for (TR_VarList params = frame->parameters; params; params = params->tail) {
            if (params->head) {
                F_Var v = params->head;
                printf("\t\t\t%s : ", S_name(params->head->name)); 
                T_print_type(v->type); // Print type of the variable
                printf("\n");
            }
        }
    }
    if(frame->variables) {
        printf("\t\tLocal Variables: \n");
        // Print name and type for each variable
        for (TR_VarList vars = frame->variables; vars; vars = vars->tail) {
            if (vars->head) {
                F_Var v = vars->head;
                printf("\t\t\t%s : ", S_name(vars->head->name)); 
                T_print_type(v->type); // Print type of the variable
                printf("\n");
            }
        }
    }
    
}

