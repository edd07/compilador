/*** Definition section ***/

%{
/* C code to be copied verbatim */
#include <stdio.h>

bool equals(char* string1, char* string2)
	{
	int i=0;
	while(string1[i]!=0 && string2[i]!=0 && string1[i]==string2[i]){
		i++;
	}
	return (string1[i]==string2[i]);
}

%}

DIGIT [0-9]
CHAR [a-zA-z0-9_]
WS [ \n\t]*
OP \+|-|\*|/|\%|<|<=|>|>=|=|==|!=|&&|\|\||!|;|,|\.
KEY void|string|this|while|break|ReadInteger|int|class|extends|if|New|ReadLine|double|interface|implements|else|NewArray|bool|null|for|return|Print

%%
/* whitespace */
{WS}

/* keywords */    
{KEY} {}

/* identifier 31 chars max */
[a-zA-z]{CHAR}{CHAR}{CHAR}{CHAR}{CHAR}{CHAR}{CHAR}{CHAR}{CHAR}{CHAR}{CHAR}{CHAR}{CHAR}{CHAR}{CHAR}{CHAR}{CHAR}{CHAR}{CHAR}{CHAR}{CHAR}{CHAR}{CHAR}{CHAR}{CHAR}{CHAR}{CHAR}{CHAR}{CHAR}{CHAR} {}

/* parentesis */
"("|")" {printf("%s", equals(yytext, ")") ? "CLOSE" : "OPEN"); }

"["|"]" {}

"{"|"}" {}

/* integer literals */
{DIGIT}+|0[xX][0-9a-eA-E]+ {}

/* double literals */
{DIGIT}+\.{DIGIT}*[eE][+-]?DIGIT+

/* string literal */
\"[^\"\n]\"

/* boolean literals */
true|false {}

/* operators */
{OP} {}
%%
/*** C Code section ***/

int main(void)
{
    /* Call the lexer, then quit. */
    yylex();
    return 0;
}
