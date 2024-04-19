/*
 * tiger.lex
 * Lex token specification file for Tiger compiler.
 * Author: Amittai Aviram - aviram@bc.edu.
 */

%{
#include <stdio.h>
#include <string.h>

#include "absyn.h"
#include "errormsg.h"
#include "util.h"
#include "y.tab.h"

#define YY_USER_ACTION {yylloc.first_line = yylineno; \
   yylloc.first_column = colnum; \
   colnum = colnum + yyleng; \
   yylloc.last_column = colnum; \
   yylloc.last_line = yylineno; \
}

int colnum = 1;

string copy_unescaped(string raw_string);
%}

%option nounput noinput

space [ \t\r]
ws {space}+
digit [0-9]
letter [a-zA-Z]
alnum [a-zA-Z0-9_]

%%

\/\*[^*]*\*+([^/*][^*]*\*+)*\/        { continue; }
{ws}              { continue; }
\n             { ++yylineno; colnum = 1; continue; }
array             { return ARRAY; }
if                { return IF; }
then              { return THEN; }
else              { return ELSE; }
while             { return WHILE; }
for               { return FOR; }
to                { return TO; }
do                { return DO; }
let               { return LET; }
in                { return IN; }
end               { return END; }
of                { return OF; }
break             { return BREAK; }
nil               { return NIL; }
function          { return FUNCTION; }
var               { return VAR; }
type              { return TYPE; }
","               { return COMMA; }
":"               { return COLON; }
";"               { return SEMICOLON; } 
"("               { return LPAREN; }
")"               { return RPAREN; }
"["               { return LBRACK; }
"]"               { return RBRACK; }
"{"               { return LBRACE; }
"}"               { return RBRACE; }
"."               { return DOT; }
"+"               { return PLUS; }
"-"               { return MINUS; }
"*"               { return TIMES; }
"/"               { return DIVIDE; }
"="               { return EQ; }
"<>"              { return NEQ; }
"<"               { return LT; }
"<="              { return LE; }
">"               { return GT; }
">="              { return GE; }
"&"               { return AND; }
"|"               { return OR; }
":="              { return ASSIGN; }
{letter}{alnum}*  { yylval.sval = make_String(yytext);  return ID; }
\"[^"]*\"        { yylval.sval = copy_unescaped(yytext); return STRING; }
{digit}+          { yylval.ival = atoi(yytext); return INT; }
.                 { EM_error(yylloc, "illegal token: %s", yytext); }

%%

int yywrap() {
    return 1;
}

string copy_unescaped(string raw_string) {
    string copy = malloc_checked(strlen(raw_string) + 1);
    char * source = raw_string;
    char * dest = copy;
    if (*source == '"') {
        ++source;
    }
    while (*source) {
        if (*source == '\\') {
            ++source;
            switch (*source) {
                case 'n':
                    *dest = '\n';
                    break;
                case 't':
                    *dest = '\t';
                    break;
                case '^':
                    ++source; 
                    *dest = *source - 64;
                    break;
                case '"':
                case '\\':
                    *dest = *source;
                    break;
                case ' ':
                case '\f':
                case '\n':
                case '\r':
                case '\t':
                case '\v':
                    while (*source != '\\') {
                        ++source;
                    }
                    ++source;
                default:
                    if (*source >= '0' && *source <= '7') {
                        char numeral[4];
                        strncpy(numeral, source, 3);
                        numeral[3] = '\0';
                        source += 2;
                        *dest = strtol(numeral, NULL, 8);
                    } else {
                        EM_error(yylloc, "Illegal escape sequence: ", raw_string);
                    }
                    break;
            }
        } else if (*source == '"') {
            *(++dest) = '\0';
            return copy;
        } else {
            *dest = *source;
        }
        ++source;
        ++dest;
    }
    *(++dest) = '\0';
    return copy;
}

