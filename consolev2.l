/*
 * Copyright 2015 Mathias Leyendecker / University of Strasbourg
 *
 * This file is part of MTurtle.
 * MTurtle is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MTurtle is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MTurtle.  If not, see <http://www.gnu.org/licenses/>.
 *
 * ====================================================================
 *
 * MTurtle Console
 * Command line interpreter for Turtle graphics
 */
 
%{
	#define YYDEBUG 1
	#include "consolev2.tab.h"
	#include "consolev2_common.h"
    
    YY_BUFFER_STATE parsebuf;
    
    YY_BUFFER_STATE filebuf[10];
    int filebufindex = 0;
%}

identifier  [a-zA-Z_][a-zA-Z0-9_]*
integer     [0-9]+
float       ([0-9]*\.[0-9]+|[0-9]+\.)
string      \"(\\.|[^"])*\"

%option extra-type="struct exec_env*"
%x COMMENT

%%

<INITIAL>{

"/*"    { BEGIN(COMMENT); }
"//".*  { /* ignore line comment */ }

{integer} {
    yylval.intval = strtol(yytext, NULL, 10);
    return TK_INTEGER;
}

{float} {
    yylval.fltval = strtof(yytext, NULL);
    return TK_FLOAT;
}

{string} {
    int lg = strlen(yytext);
    yylval.strval = malloc(lg);
    strncpy(yylval.strval, yytext + 1, lg - 2);
    yylval.strval[lg - 2] = '\0';
    
    return TK_STRING;
}

"exit"              { return TK_EXIT; }
"quitte"            { return TK_EXIT; }

"help"              { return TK_HELP; }
"aide"              { return TK_HELP; }

"forward"           { return TK_FORWARD; }
"fwd"               { return TK_FORWARD; }
"avance"            { return TK_FORWARD; }

"backward"          { return TK_BACKWARD; }
"back"              { return TK_BACKWARD; }
"recule"            { return TK_BACKWARD; }

"left"              { return TK_LEFT; }
"gauche"            { return TK_LEFT; }

"right"             { return TK_RIGHT; }
"droite"            { return TK_RIGHT; }

"pendown"           { return TK_PENDOWN; }
"down"              { return TK_PENDOWN; }
"baissecrayon"      { return TK_PENDOWN; }

"penup"             { return TK_PENUP; }
"up"                { return TK_PENUP; }
"levecrayon"        { return TK_PENUP; }

"centeredcircle"    { return TK_CENTEREDCIRCLE; }
"centcirc"          { return TK_CENTEREDCIRCLE; }
"cerclecentre"      { return TK_CENTEREDCIRCLE; }

"circle"            { return TK_CIRCLE; }
"circ"              { return TK_CIRCLE; }
"cercle"            { return TK_CIRCLE; }

"write"             { return TK_WRITE; }
"ecrire"            { return TK_WRITE; }
"ecrit"             { return TK_WRITE; }

"hideturtle"        { return TK_HIDETURTLE; }
"cachetortue"       { return TK_HIDETURTLE; }

"showturtle"        { return TK_SHOWTURTLE; }
"montretortue"      { return TK_SHOWTURTLE; }

"home"              { return TK_HOME; }
"origine"           { return TK_HOME; }

"clear"             { return TK_CLEAR; }
"efface"            { return TK_CLEAR; }

"reset"             { return TK_RESET; }

"echo"              { return TK_ECHO; }

(load|charge) {
    return TK_LOAD;
}

(endif|finsi|fsi)                   { return TK_ENDIF; }
(endfor|finpour|fpour)              { return TK_ENDFOR; }
(endwhile|fintantque|ftantque)      { return TK_ENDWHILE; }
(endrepeat|finrepeter|frepeter)     { return TK_ENDREPEAT; }
(endfunc|finfonc|ffonc)             { return TK_ENDFUNC; }
 
(if|si)             { return TK_IF; }
(then|alors)        { return TK_THEN; }
(else|sinon)        { return TK_ELSE; }
(while|tantque)     { return TK_WHILE; }
(for|pour)          { return TK_FOR; }
(from|de)           { return TK_FROM; }
(to|jusqua)         { return TK_TO; }
(do|faire)          { return TK_DO; }
(repeat|repeter)    { return TK_REPEAT; }
(times|fois)        { return TK_TIMES; }

(func|fonc)                 { return TK_FUNC; }
(return|renvoyer|renvoie)   { return TK_RETURN; }

(&&|and|et)   { return TK_AND; }
(\|\||or|ou)  { return TK_OR; }
(not|pas)     { return TK_NOT; }

(color|colour|couleur)  { return TK_COLOR; }
(red|rouge)             { return TK_CL_RED; }
(green|vert)            { return TK_CL_GREEN; }
(blue|bleu)             { return TK_CL_BLUE; }
(yellow|jaune)          { return TK_CL_YELLOW; }
(teal|cyan)             { return TK_CL_TEAL; }
(magenta)               { return TK_CL_MAGENTA; }
(orange)                { return TK_CL_ORANGE; }
(black|noir)            { return TK_CL_BLACK; }
(white|blanc)           { return TK_CL_WHITE; }
(grey|gray|gris)        { return TK_CL_GREY; }
(silver|argent)         { return TK_CL_SILVER; }

"cos"   { return TK_COS; }
"sin"   { return TK_SIN; }
"tan"   { return TK_TAN; }
"abs"   { return TK_ABS; }
"sqrt"  { return TK_SQRT; }
"log"   { return TK_LOG; }
"log10" { return TK_LOG10; }
"exp"   { return TK_EXP; }
"rmdr"  { return TK_RMDR; }
"max"   { return TK_MAX; }
"min"   { return TK_MIN; }
"ceil"  { return TK_CEIL; }
"floor" { return TK_FLOOR; }

{identifier} {
    yylval.name = strdup(yytext);
    return TK_IDENTIFIER;
}

"=="    { return TK_EQ; }
"!="    { return TK_NEQ; }
">="    { return TK_GEQ; }
"<="    { return TK_LEQ; }
"<-"    { return TK_ASSIGN; }

[ \t]   { /* ignore whitespace */ }
\n      { return TK_NEWLINE; }



"!" { return '!'; }
"<" { return '<'; }
">" { return '>'; }
"+" { return '+'; }
"-" { return '-'; }
"*" { return '*'; }
"/" { return '/'; }
"%" { return '%'; }
"(" { return '('; }
")" { return ')'; }
"," { return ','; }
";" { return TK_NEWLINE; }

<<EOF>> {
    yyterminate();
    /*return TK_EOF;*/
}

"\0" {
    yyterminate();
}

.   { /* ignore junk characters */
    yyerror(NULL, "invalid character");
    fprintf(stderr, "Invalid character found: %s\n", yytext);
    yyterminate();
}
}

<COMMENT>{

"*/"    { BEGIN(INITIAL); }
<<EOF>> { yyerror(NULL, "malformed comment"); yyterminate(); }
\n      { }
.       { }

}

%%

void scan_string(char* s)
{
    parsebuf = yy_scan_string(s);
    yy_switch_to_buffer(parsebuf);
}

void clean_buffer()
{
    yy_delete_buffer(parsebuf);
}

struct ast_node* scan_file(char* name)
{
    if(filebufindex >= 10)
    {
        fprintf(stderr, "*** FATAL: max include depth reached\n");
        exit(EXIT_FAILURE);
    }
    
    FILE* file = fopen(name, "r");
    if(file == NULL)
    {
        char* str = malloc(1024);
        snprintf(str, 1024, "-!- Unable to open %s: %s", name, strerror(errno));
        struct ast_node* ast = ast_make(AST_ECHO, ast_make_string(str), NULL);
        return ast;
    }
    
    fseek(file, 0, SEEK_END);
    long lg = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    errno = 0;
    char* buf = malloc(sizeof(char) * lg);
    if(buf == NULL)
    {
        fprintf(stderr, "*** FATAL: Unable to create buffer of %ld bytes: %s\n", lg, strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    fread(buf, 1, lg, file);
    fclose(file);
    buf[lg] = '\0';
    
    filebuf[filebufindex] = yy_scan_string(buf);
    if(buf != NULL)
    {
        free(buf);
    }
    filebufindex++;
    
    struct ast_node* ast = NULL;
    yyparse(&ast);
    
    yy_delete_buffer(filebuf[filebufindex - 1]);
    filebufindex--;
    
    return ast;
}
