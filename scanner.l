/* File:  scanner.l
 * ----------------
 * Lex inupt file to generate the scanner for the compiler.
 */

%{

/* The text within this first region delimited by %{ and %} is assumed to
 * be C/C++ code and will be copied verbatim to the lex.yy.c file ahead
 * of the definitions of the yylex() function. Add other header file inclusions
 * or C++ variable declarations/prototypes that are needed by your code here.
 */

#include <string.h>
#include "scanner.h"
#include "utility.h" // for PrintDebug()
#include "errors.h"

int primera_col=1;
int ultima_col=1;
int num_linea=1;

/* Global variable: yylval
 * -----------------------
 * This global variable is how we get attribute information about the token
 * just scanned to the client. The scanner sets the global variable
 * appropriately and since it's global the client can just read it.  In the
 * future, this variable will be declared for us in the y.tab.c file
 * produced by Yacc, but for now, we declare it manually.
 */
YYSTYPE yylval;  // manually declared for pp1, later Yacc provides

/* Global variable: yylloc
 * -----------------------
 * This global variable is how we get position information about the token
 * just scanned to the client. (Operates similarly to yylval above)
 */
struct yyltype yylloc; // manually dclared for pp1, later Yacc provides

/* Macro: YY_USER_ACTION 
 * ---------------------
 * This flex built-in macro can be defined to provide an action which is
 * always executed prior to any matched rule's action. Basically, it is
 * a way of having a piece of code common to all actions factored out to
 * this routine.  We already defined it for you and left the empty
 * function DoBeforeEachAction ready for your use as needed. It will
 * be called once for each pattern scanned from the file, before
 * executing its action.
 */
static void DoBeforeEachAction(); 
#define YY_USER_ACTION DoBeforeEachAction();

%}

DIGIT [0-9]
INT {DIGIT}+
CHAR [a-zA-Z0-9_]
WS [ \t]*
BOOL true|false

 /* The section before the first %% is the Definitions section of the lex
  * input file. Here is where you set options for the scanner, define lex
  * states, and can set up definitions to give names to regular expressions
  * as a simple substitution mechanism that allows for more readable
  * entries in the Rules section later. 
  */


%%             /* BEGIN RULES SECTION */
 /* All patterns and actions should be placed between the start and stop
  * %% markers which delimit the Rules section. 
  */ 
  

  
  	/* whitespace */
{WS}


	/* keywords & operators */    
	
void {return(256);}
int {return(257);}
double {return(258);}
bool {return(259);}
string {return(260);}
class {return(261);}
null {return(262);}
"<=" {return(263);}
">=" {return(264);}
"==" {return(265);}
"!=" {return(266);}
"&&" {return(267);}
"||" {return(268);}
while {return(269);}
for {return(270);}
if {return(271);}
else {return(272);}
return {return(273);}
break {return(274);}
extends {return(275);}
this {return(276);}
implements {return(277);}
interface {return(278);}
New {return(279);}
NewArray {return(280);}

	/* boolean literals */
{BOOL} {yylval.boolConstant=strcmp(yytext,"false");return(285);}

	/* parentesis */
"(" {return('(');}
")" {return(')');}
"[" {return('[');}
"]" {return(']');}
"{" {return('{');}
"}" {return('}');}

	/* double literals */
{INT}"."{DIGIT}*[eE][+-]?DIGIT+ {yylval.doubleConstant=atof(yytext);return(284);}

	/* integer literals */
{INT}|0[xX][0-9a-eA-E]+ {yylval.integerConstant=atoi(yytext);return(283);}

	/* string literal */
["][^"\n]*["] {yylval.stringConstant=yytext;return(282); }

	/* identifier 31 chars max */
[a-zA-Z]{CHAR}*  {strcpy(yylval.identifier,yytext);return(281);}

	/* errors */
["][^"]*\n {ReportError::UntermString(&yylloc, yytext);num_linea++;}

\n {num_linea++;}

%%
/* The closing %% above marks the end of the Rules section and the beginning
 * of the User Subroutines section. All text from here to the end of the
 * file is copied verbatim to the end of the generated lex.yy.c file.
 * This section is where you put definitions of helper functions.
 */


/* Function: InitScanner
 * ---------------------
 * This function will be called before any calls to yylex().  It is designed
 * to give you an opportunity to do anything that must be done to initialize
 * the scanner (set global variables, configure starting state, etc.). One
 * thing it already does for you is assign the value of the global variable
 * yy_flex_debug that controls whether flex prints debugging information
 * about each token and what rule was matched. If set to false, no information
 * is printed. Setting it to true will give you a running trail that might
 * be helpful when debugging your scanner. Please be sure the variable is
 * set to false when submitting your final version.
 */
void InitScanner()
{
    PrintDebug("lex", "Initializing scanner");
    yy_flex_debug = false;
}


/* Function: DoBeforeEachAction()
 * ------------------------------
 * This function is installed as the YY_USER_ACTION. This is a place
 * to group code common to all actions.
 */
static void DoBeforeEachAction()
{

primera_col = ultima_col;
ultima_col += strlen(yytext);

yylloc.first_line = yylloc.last_line = num_linea;
yylloc.first_column = primera_col;
yylloc.last_column = ultima_col-1;
}
