/* parse.h -
 * API for the "driver" program of the Tiger compiler,
 * enabling testing and debugging.
 * Run with ./parse <Tiger-source-code-file>
 * to see the result of parsing (success or failure),
 * the type of the program's output (if any),
 * and a visual representation fo the IR translation.
 * Run with ./parse <Tiger-source-code-file> -p
 * (the -p flag at the end) to see a printout of the
 * abstract syntax tree above the program's output type.
 * Author: Amittai Aviram - aviram@bc.edu.
 */

#include "util.h"

A_Exp parse(string fname);

