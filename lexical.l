/*** Definition section ***/

%{
/* C code to be copied verbatim */
#include <stdio.h>

bool equals(char* string1, char* string2){
	int i=0;
	while(string1[i]!=0 && string2[i]!=0 && string1[i]==string2[i]){
		i++;
	}
	return (string1[i]==string2[i])
}

%}

/* This tells flex to read only one input file */
%option noyywrap

DIGIT [0-9]
CHAR [a-zA-z0-9_]

%%
    /*** Rules section ***/

/* keywords */    
void|string|this|while|break|ReadInteger|int|class|extends|if|New|ReadLine|double|interface|implements|else|NewArray|bool|null|for|return|Print {}

/* identifier */
[a-zA-z]CHARCHARCHARCHARCHARCHARCHARCHARCHARCHARCHARCHARCHARCHARCHARCHARCHARCHARCHARCHARCHARCHARCHARCHARCHARCHARCHARCHARCHARCHAR {}

/* parentesis */
"("|")" {printf("%s", equals(yytext, ")") ? "CLOSE" : "OPEN"); }

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

   

%%
/*** C Code section ***/

int main(void)
{
    /* Call the lexer, then quit. */
    yylex();
    return 0;
}
