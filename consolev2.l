%{
	#define YYDEBUG 1
	#include "consolev2.tab.h"
%}

identifier  [a-zA-Z_][a-zA-Z0-9_]*
integer     [0-9]+
float       ([0-9]*\.[0-9]+|[0-9]+\.)
string      \"(\\.|[^"])*\"

%%

<INITIAL>{

"/*"    { comment(); }
"//".*  { /* ignore line comment */ }

{identifier} {
    yylval.name = strdup(yytext);
    return TK_IDENTIFIER;
}

{integer} {
    yylval.intval = strtol(yytext, NULL, 10);
    return TK_INTEGER;
}

{float} {
    yylval.fltval = strtof(yytext, NULL);
    return TK_FLOAT;
}

{string} {
    yylval.strval = strdup(yytext);
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

"home"              { return TK_HOME; }
"origine"           { return TK_HOME; }

"clear"             { return TK_CLEAR; }
"efface"            { return TK_CLEAR; }

"reset"             { return TK_RESET; }

"echo"              { return TK_ECHO; }

"load"              { return TK_LOAD; }
"charge"            { return TK_LOAD; }

"(endif|finsi|fsi)"                 { return TK_ENDIF; }
"(endfor|finpour|fpour)"            { return TK_ENDFOR; }
"(endwhile|fintantque|ftantque)"    { return TK_ENDWHILE; }
 
"(if|si)"           { return TK_IF; }
"(then|alors)"      { return TK_THEN; }
"(else|sinon)"      { return TK_ELSE; }
"(while|tantque)"   { return TK_WHILE; }
"(for|pour)"        { return TK_FOR; }
"(from|de)"         { return TK_FROM; }
"(to|jusqua)"       { return TK_TO; }
"(do|faire)"        { return TK_DO; }

"(&&|and|et)"   { return TK_AND; }
"(\|\||or|ou)"  { return TK_OR; }
"(not|pas)"     { return TK_NOT; }

"cos"   { return TK_COS; }
"sin"   { return TK_SIN; }
"tan"   { return TK_TAN; }
"abs"   { return TK_ABS; }
"sqrt"  { return TK_SQRT; }
"log"   { return TK_LOG; }
"log10" { return TK_LOG10; }
"exp"   { return TK_EXP; }

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

.   { /* ignore junk characters */ }
}

%%

static void comment(void)
{
    int c;

    while ((c = input()) != 0)
        if (c == '*')
        {
            while ((c = input()) == '*')
                ;

            if (c == '/')
                return;

            if (c == 0)
                break;
        }
    yyerror("unterminated comment");
}
