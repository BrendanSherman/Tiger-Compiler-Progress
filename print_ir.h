/*
 * print_ir.h -
 * Facility to print a visual
 * representation of the intermediate representation (IR)
 * of a given Tiger program based on its parsed AST.
 * The IR scheme is set forth in the Translate module
 * (translate.h and translate.c).
 * Author: Amittai Aviram - aviram@bc.edu
 */

#pragma once

#include "translate.h"

void P_print_ir(TR_Function main_);
void P_print_function_body(TR_Function func);
