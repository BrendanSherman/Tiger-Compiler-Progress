/*
 * errormsg.h -
 * Facilities for reporting errors.
 * Author: Andrew Appel.
 * Revised, simplified, and reformatted
 * by Amittai Aviram - aviram@bc.edu.
 */

#pragma once

#include <stdbool.h>

#include "util.h"

typedef struct E_Pos_ {
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} E_Pos;

extern bool EM_any_errors;

void EM_error(E_Pos, string, ...);
void EM_reset(string file_name);
