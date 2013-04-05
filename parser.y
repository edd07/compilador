/* File: parser.y
 * --------------
 * Yacc input file to generate the parser for the compiler.
 *
 * pp2: your job is to write a parser that will construct the parse tree
 *      and if no parse errors were found, print it.  The parser should 
 *      accept the language as described in specification, and as augmented 
 *      in the pp2 handout.
 */

%{

/* Just like lex, the text within this first region delimited by %{ and %}
 * is assumed to be C/C++ code and will be copied verbatim to the y.tab.c
 * file ahead of the definitions of the yyparse() function. Add other header
 * file inclusions or C++ variable declarations/prototypes that are needed
 * by your code here.
 */
#include "scanner.h" // for yylex
#include "parser.h"
#include "errors.h"

void yyerror(char *msg); // standard error-handling routine

%}

/* The section before the first %% is the Definitions section of the yacc
 * input file. Here is where you declare tokens and types, add precedence
 * and associativity options, and so on.
 */
 
/* yylval 
 * ------
 * Here we define the type of the yylval global variable that is used by
 * the scanner to store attibute information about the token just scanned
 * and thus communicate that information to the parser. 
 *
 * pp2: You will need to add new fields to this union as you add different 
 *      attributes to your non-terminal symbols.
 */
%union {
    int integerConstant;
    bool boolConstant;
    char *stringConstant;
    double doubleConstant;
    char identifier[MaxIdentLen+1]; // +1 for terminating null
    Decl *decl;
    VarDecl *variableDecl;
    List<Decl*> *declList;
    List<VarDecl*> *variableList;
    List<Stmt*> *stmtList;
    List<InterfaceDecl*> *interfaceList;
    List<FieldAccess*> *fieldList;
    FnDecl *prototype;
    List<FnDecl*> *prototypeList;

    Expr *expr;
    Operator *operator;
}


/* Tokens
 * ------
 * Here we tell yacc about all the token types that we are using.
 * Yacc will assign unique numbers to these and export the #define
 * in the generated y.tab.h header file.
 */
%token   T_Void T_Bool T_Int T_Double T_String T_Class 
%token   T_LessEqual T_GreaterEqual T_Equal T_NotEqual T_Dims
%token   T_And T_Or T_Null T_Extends T_This T_Interface T_Implements
%token   T_While T_For T_If T_Else T_Return T_Break
%token   T_New T_NewArray T_Print T_ReadInteger T_ReadLine

%token   <identifier> T_Identifier
%token   <stringConstant> T_StringConstant 
%token   <integerConstant> T_IntConstant
%token   <doubleConstant> T_DoubleConstant
%token   <boolConstant> T_BoolConstant


/* Non-terminal types
 * ------------------
 * In order for yacc to assign/access the correct field of $$, $1, we
 * must to declare which field is appropriate for the non-terminal.
 * As an example, this first type declaration establishes that the DeclList
 * non-terminal uses the field named "declList" in the yylval union. This
 * means that when we are setting $$ for a reduction for DeclList ore reading
 * $n which corresponds to a DeclList nonterminal we are accessing the field
 * of the union named "declList" which is of type List<Decl*>.
 * pp2: You'll need to add many of these of your own.
 */
%type <declList>  DeclList 
%type <decl>      Decl
%type <prototype> Prototype
%type <prototypeList> PrototypeAsterisco
%type <decl> Field
%type <variableDecl> Variable
%type <variableDecl> VariableDecl
%type <declList> VariableDeclAsterisco
%type <identifier> Type
%type <decl> FunctionDecl
%type <variableList> Formals
%type <variableList> VariableList
%type <stmtList> StmtBlock /* No se que pedo */
%type <stmtList> StmtAsterisco
%type <identifier> ExtendsQualifier
%type <interfaceList> ImplementsQualifier
%type <fieldList> FieldAsterisco
%type <decl> InterfaceDecl
%type <expr> Expr
%type <expr> ArithmeticExpr
%type <expr> BooleanExpr
%type <expr> OrExpr
%type <expr> AndExpr
%type <expr> RelationalExpr
%type <expr> EqualityExpr
%type <expr> AssignmentExpr
%type <expr> StringExpr
%type <expr> Num
%type <expr> Term
%type <expr> Bool
%type <operator> MultOp
%type <operator> AddOp
%type <operator> RelOp
%type <operator> EqOp

%nonassoc T_Else

%right '='
%left T_Or
%left T_And
%left T_Equal
%left T_NotEqual
%left '<'
%left T_LessEqual
%left '>'
%left T_GreaterEqual
%left '-'
%left '+'
%left '/'
%left '%'
%left '*'
%right '!'
%left '.'
%left '['
%left ']'
%left T_Dims

%%
/* Rules
 * -----
 * All productions and actions should be placed between the start and stop
 * %% markers which delimit the Rules section.
   
 */
Program   :    DeclList            { 
                                      @1; 
                                      /* pp2: The @1 is needed to convince 
                                       * yacc to set up yylloc. You can remove 
                                       * it once you have other uses of @n*/
                                      Program *program = new Program($1);
                                      // if no errors, advance to next phase
                                      if (ReportError::NumErrors() == 0) 
                                          program->Print(0);
                                    }
          ;

DeclList  :    DeclList Decl        { ($$=$1)->Append($2); }
          |    Decl                 { ($$ = new List<Decl*>)->Append($1); }
          ;

Decl      : VariableDecl		{ $$=$1; } 
          | FunctionDecl		{ $$=$1; }		
          | ClassDecl			{ $$=$1; }
          | InterfaceDecl		{ $$=$1; }
          ;
          
VariableDecl  :    Variable ';'     {$$ = $1;}
              ;

Variable  : Type T_Identifier       {$$ = new VarDecl(new Identifier(yylloc,$2) , $1 );}
          ;

Type      : T_Int			{$$ = intType;}
          | T_Double		{$$ = doubleType;}
          | T_String		{$$ = stringType;}
          | T_Bool			{$$ = boolType;}
          | T_Identifier	{$$ = new NamedType($1);}
          | Type T_Dims		{$$ = new ArrayType(yylloc,$1);}
          ;

FunctionDecl : Type T_Identifier '(' Formals ')' StmtBlock		{$$ = new FnDecl(new Identifier($2),$1,$4);$$.setFunctionBody($6);}
             | T_Void T_Identifier '(' Formals ')' StmtBlock	{$$ = new FnDecl(new Identifier($2),voidType,$4);$$.setFunctionBody($6);}
             ;

Formals   : VariableList	{$$=$1;}
          | /* empty */		{$$ = new List<VarDecl*>;}
          ;

VariableList : VariableList ',' Variable	{($$=$1)->Append($3);}
             | Variable						{($$=new List<VarDecl*>)->Append($1);}
             |  /* empty */					{$$=new List<VarDecl*>;}
             ;

/* PENDIENTE */
ClassDecl : T_Class T_Identifier ExtendsQualifier ImplementsQualifier '{' FieldAsterisco '}'	{$$=new ClassDecl(new Identifier($2),new NamedType($3),$4,$6);}
          ;

ExtendsQualifier : T_Extends T_Identifier	{$$=$2;}
                 | /* empty */				{$$=NULL}
                 ;

ImplementsQualifier : T_Implements InterfaceList	{($$=$1)->Append($2);}
                    | /* empty */					{$$=new List<NamedType*>;}
                    ;

InterfaceList : InterfaceList ',' T_Identifier		{($$=$1)->Append(new NamedType($3));}
              | T_Identifier						{($$=new List<NamedType*>)->Append(new NamedType($1));}
              ;

FieldAsterisco : FieldAsterisco Field	{($$=$1)->Append($2);}
               | /* empty */			{$$=new List<Decl*>;}
               ;

Field     : VariableDecl	{$$=$1;}
          | FunctionDecl	{$$=$1;}
          ;

InterfaceDecl : T_Interface T_Identifier '{' PrototypeAsterisco '}' {$$=new InterfaceDecl(new Identifier($2),$4);}
              ;

PrototypeAsterisco : PrototypeAsterisco Prototype	{($$=$1)->Append($2);}
                   | /* empty */					{$$=new List<FnDecl*>;}
                   ;

Prototype : Type T_Identifier '(' Formals ')' ';'		{$$ = new FnDecl(new Identifier($2),$1,$4);}
          | T_Void T_Identifier '(' Formals ')' ';'		{$$ = new FnDecl(new Identifier($2),voidType,$4);}
          ;

StmtBlock : '{' VariableDeclAsterisco StmtAsterisco '}'	{$$=new StmtBlock($1,$2);}
          ;
VariableDeclAsterisco : VariableDeclAsterisco Variable 	{($$=$1)->Append($2);}
                      | /* empty */						{$$=new List<VarDecl*>;}
                      ;

StmtAsterisco : StmtAsterisco Stmt		{($$=$1)->Append($2);}
              | /* empty */				{$$=new List<Stmt*>;}
              ;

Stmt : ';'			{$$=new EmptyExpr();}
     | Expr ';'		{$$=$1;}
     | IfStmt		{$$=$1;}
     | WhileStmt	{$$=$1;}
     | ForStmt		{$$=$1;}
     | BreakStmt	{$$=$1;}
     | ReturnStmt	{$$=$1;}
     | PrintStmt	{$$=$1;}
     | StmtBlock	{$$=$1;}
     ;

IfStmt : T_If '(' Expr ')' Stmt					{$$=new IfStmt($3,$5,new EmptyExpr());}
       | T_If '(' Expr ')' Stmt T_Else Stmt		{$$ = new IfStmt($3,$5,$7);}
       ;

WhileStmt : T_While '(' Expr ')' Stmt	{$$ = new WhileStmt($3,$5);}
          ;

ForStmt : T_For '(' ExprOpcional ';' Expr ';' ExprOpcional ')' Stmt		{$$=new ForStmt($3,$5,$7,$9);}
        ;
        
ExprOpcional : Expr				{$$=$1;}
             | /* empty */		{$$=new EmtyExpression();}
             ; 	

ReturnStmt : T_Return ';'		{$$=new ReturnStmt(@1,new EmptyExpr());}
           | T_Return Expr ';'	{$$=new ReturnStmt(@1,$2);}
           ;

BreakStmt : T_Break ';'		{$$=new BreakStmt(@1);}
          ;

PrintStmt : T_Print '(' ExprList ')' ';'	{$$=new PrintStmt($3);}
          ;

ExprList : ExprList ',' Expr	{($$=$1)->Append($3);}
         | Expr					{($$=new List<Expr*>)->Append($1);}
         | /* empty */			{$$=new List<Expr*>;}
         ;

Expr : ArithmeticExpr	{$$=$1;}
     | BooleanExpr		{$$=$1;}
     | StringExpr		{$$=$1;}
     | AssignmentExpr	{$$=$1;}
     | Constant			{$$=$1;}
     | LValue			{$$=$1;}
     | T_This			{$$=$1;}
     | Call				{$$=$1;}
     | '(' Expr ')'		{$$=$1;}
     | T_New '(' T_Identifier ')'			{$$=$1;} 
     | T_NewArray '(' Expr ',' Type ')'		{$$=$1;}
     ;

Num : T_IntConstant				{$$=new IntConstant(@1,$1);}
    | T_DoubleConstant			{$$=new DoubleConstant(@1,$1);}
    | T_ReadInteger '(' ')'		{$$=new ReadIntegerExpr(@1);}
    | '-' Num					{$$=new ArithmeticExpr($1,$2);}
    ;

Term : Num						{$$=$1;}
     | Term MultOp Num			{$$=ArithmeticExpr($1,$2,$3);}
     ;

MultOp : '*'	{$$= new Operator(@1,$1);}
       | '/'	{$$= new Operator(@1,$1);}
       ;

ArithmeticExpr : Term							{$$=$1;}
               | ArithmeticExpr AddOp Term		{$$=ArithmeticExpr($1,$2,$3);}
               ;

AddOp : '+'	{$$= new Operator(@1,$1);}
      | '-'	{$$= new Operator(@1,$1);}
      ;

Bool : T_BoolConstant	{$$=BoolConstant(@1,$1);}
     | '!' BooleanExpr	{$$=LogicalExpr($1,$2);}
     ;

BooleanExpr : OrExpr			{$$=$1;}
           	| Bool				{$$=$1;}
           	| RelationalExpr	{$$=$1;}
           	| EqualityExpr		{$$=$1;}
           	;

OrExpr : AndExpr T_Or AndExpr	{$$=new LogicalExpr($1,$2,$3);}
      | AndExpr					{$$=$1;}
      ;

AndExpr : BooleanExpr T_And BooleanExpr	{$$=new LogicalExpr($1,$2,$3);}
       | BooleanExpr					{$$=$1;}
       ;

RelationalExpr : ArithmeticExpr RelOp ArithmeticExpr	{$$=new RelationalExpr($1,$2,$3);}
               ;

RelOp : T_LessEqual
      | T_GreaterEqual
      | '<'
      | '>'
      ; 

EqualityExpr : ArithmeticExpr EqOp ArithmeticExpr	{$$=new EqualitylExpr($1,$2,$3);}
             | BooleanExpr EqOp BooleanExpr			{$$=new EqualitylExpr($1,$2,$3);}
             ;

EqOp : T_Equal		{$$= new Operator(@1,$1);}
     | T_NotEqual	{$$= new Operator(@1,$1);}
     ;

AssignmentExpr : LValue '=' Expr	{$$=new AssignExpr($1,$2,$3);}
               ;

StringExpr : T_String				{$$=new StringConstant(@1,$1);}
           | T_ReadLine '(' ')'		{$$=new ReadLineExpr(@1);}
           ;

LValue : T_Identifier			{$$=new Identifier(@1,$1);}
       | Expr '.' T_Identifier	{$$=new FieldAccess($1,$3);}
       | Expr '[' Expr ']'		{$$=new ArrayAccess(@1,$1,$3);}
       ;

Call : T_Identifier '(' Actuals ')'				{$$=new Call(@1,NULL,$1,$3);}
     | Expr '.' T_Identifier '(' Actuals ')'	{$$=new Call(@1,$1,$3,$5);}
     ;

Actuals : ExprList		{$$=$1;}
        | /* empty */	{$$=new List<Expr*>;}
        ;

Constant : T_IntConstant		{$$=new IntConstant(@1,$1);}
         | T_DoubleConstant		{$$=new DoubleConstant(@1,$1);}
         | T_BoolConstant		{$$=new BoolConstant(@1,$1);}
         | T_StringConstant		{$$=new StringConstant(@1,$1);}
         | T_Null				{$$=new NullConstant(@1,$1);}
         ;

%%

/* The closing %% above marks the end of the Rules section and the beginning
 * of the User Subroutines section. All text from here to the end of the
 * file is copied verbatim to the end of the generated y.tab.c file.
 * This section is where you put definitions of helper functions.
 */

/* Function: InitParser
 * --------------------
 * This function will be called before any calls to yyparse().  It is designed
 * to give you an opportunity to do anything that must be done to initialize
 * the parser (set global variables, configure starting state, etc.). One
 * thing it already does for you is assign the value of the global variable
 * yydebug that controls whether yacc prints debugging information about
 * parser actions (shift/reduce) and contents of state stack during parser.
 * If set to false, no information is printed. Setting it to true will give
 * you a running trail that might be helpful when debugging your parser.
 * Please be sure the variable is set to false when submitting your final
 * version.
 */
void InitParser()
{
   PrintDebug("parser", "Initializing parser");
   yydebug = false;
}
