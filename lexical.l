/*** Definition section ***/

%{
/* C code to be copied verbatim */
#include <stdio.h>

int equals(char* string1, char* string2)
	{
	int i=0;
	while(string1[i]!=0 && string2[i]!=0 && string1[i]==string2[i]){
		i++;
	}
	return (string1[i]==string2[i]);
}

%}

%option noyywrap

DIGIT [0-9]
INT {DIGIT}+
CHAR [a-zA-Z0-9_]
WS [ \n\t]*
OP ([+]|-|[*]|[/]|<|<=|>|>=|=|==|!=|&&|[|]|!|;|,|[.]|[%])
BOOL true|false
KEY (void|string|this|while|break|ReadInteger|int|class|extends|if|New|ReadLine|double|interface|implements|else|NewArray|bool|null|for|return|Print)

%%
	/* whitespace */
{WS}
	/* keywords */    
{KEY} { printf("KEY: %s\n", yytext);}

	/* boolean literals */
{BOOL} {printf("BOOL: %s\n", yytext);}

	/* parentesis */
"("|")" {printf("%s\n", equals(yytext, ")") ? "PAREN_CLOSE" : "PAREN_OPEN"); }

"["|"]" {printf("%s\n", equals(yytext, "]") ? "BRACK_CLOSE" : "BRACK_OPEN");}

"{"|"}" {printf("%s\n", equals(yytext, "}") ? "BRACE_CLOSE" : "BRACE_OPEN");}

	/* double literals */
{INT}"."{DIGIT}*[eE][+-]?DIGIT+ {printf("DOUBLE: %s\n", yytext);}

	/* integer literals */
{INT}|0[xX][0-9a-eA-E]+ {printf("INT: %s\n", yytext);}

	/* string literal */
\"[^\"\n]\" {printf("STR: %s\n", yytext); }

	/* operators */
{OP} {printf("OP: %s\n", yytext);}

	/* identifier 31 chars max */
[a-zA-Z]{CHAR}*  {printf("ID: %s\n", yytext);}
%%
/*** C Code section ***/

int main(void)
{
    /* Call the lexer, then quit. */
    yylex();
    return 0;
}
