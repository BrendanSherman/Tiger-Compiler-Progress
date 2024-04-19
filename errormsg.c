/*
 * errormsg.c - functions used in all phases of the compiler to give
 *              error messages about the Tiger program.
 * Author: Andrew Appel.
 * Revised, simplified, and reformatted
 * by Amittai Aviram - aviram@bc.edu.
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "errormsg.h"
#include "util.h"

bool any_errors = false;
static string file_name = "";
extern FILE * yyin;
extern int yylineno;
extern int colnum;

void EM_error(E_Pos pos, string message, ...) {
    va_list ap;
    any_errors = true;
    if (file_name) {
        fprintf(stderr,"%s:", file_name);
    }
    fprintf(stderr, "%d.%d: ", pos.first_line, pos.first_column);
    va_start(ap, message);
    vfprintf(stderr, message, ap);
    va_end(ap);
    fprintf(stderr, "\n");
}

void EM_reset(string fname) {
    any_errors = false;
    yylineno = 1;
    colnum = 1;
    file_name = fname;
    yyin = fopen(fname, "r");
    if (!yyin) {
        perror("Cannot open input file");
        exit(EXIT_FAILURE);
    }
}

