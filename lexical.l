/*** Definition section ***/

%{
/* C code to be copied verbatim */
#include <stdio.h>
%}

/* This tells flex to read only one input file */
%option noyywrap

DIGIT [0-9]

%%
    /*** Rules section ***/

/* keywords */    
void|string|this|while|break|ReadInteger|int|class|extends|if|New|ReadLine|double|interface|implements|else|NewArray|bool|null|for|return|Print {}

/* identifier (falta que solo acepte 31 caracteres */
[a-zA-z][a-zA-z0-9_]* {}

/* parentesis */
"("|")" {printf("%s",yytext==" }

"["|"]" {}

"{"|"}" {}

/* integer literals */
DIGIT+|0[xX][0-9a-eA-E]+ {}

/* double literals */
DIGIT+.DIGIT*[eE][+-]?DIGIT+

/* string literal */
\"[^\"\n]\"

/* boolean literals */
true|false {}

/* operators */
\+|-|\*|/|\%|<|<=|>|>=|=|==|!=|&&|\|\||!|;|,|\. {}

    

    /* [0-9]+ matches a string of one or more digits */
[0-9]+  {
            /* yytext is a string containing the matched text. */
            printf("Saw an integer: %s\n", yytext);
        }

.|\n    {   /* Ignore all other characters. */   }

%%
/*** C Code section ***/

int main(void)
{
    /* Call the lexer, then quit. */
    yylex();
    return 0;
}
