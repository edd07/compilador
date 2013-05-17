/* File: ast_stmt.cc
 * -----------------
 * Implementation of statement node classes.
 */
#include "ast_stmt.h"
#include "ast_type.h"
#include "ast_decl.h"
#include "ast_expr.h"
#include "errors.h"


Program::Program(List<Decl*> *d) {
    Assert(d != NULL);
    (decls=d)->SetParentAll(this);
    
    for (int i = 0; i < d->NumElements(); i++) {
    		Decl* decl = d->Nth(i);
            Decl* prev = table->Lookup(decl->id->name)->decl;
    		
    		if (table->Lookup(decl->id->name) != NULL) {
                ReportError::DeclConflict(decl, prev);
    		 } else {
    		    Declaracion dcl;
    		    dcl.tipo = decl->typeName;
    		    dcl.decl = decl;
                table->Enter(decl->id->name, dcl);
            }
     }
}

void Program::Check() {
    /* pp3: here is where the semantic analyzer is kicked off.
     *      The general idea is perform a tree traversal of the
     *      entire program, examining all constructs for compliance
     *      with the semantic rules.  Each node can have its own way of
     *      checking itself, which makes for a great use of inheritance
     *      and polymorphism in the node classes.
     */
      for (int i = 0; i < decls->NumElements(); i++) {
    		Decl* decl = decls->Nth(i);
    		//printf("%s",decl->id->name);
			decl->Check();
     }
     
}

StmtBlock::StmtBlock(List<VarDecl*> *d, List<Stmt*> *s) {
    Assert(d != NULL && s != NULL);
    (decls=d)->SetParentAll(this);
    (stmts=s)->SetParentAll(this);
    
    for (int i = 0; i < d->NumElements(); i++) {
    		Decl* decl = d->Nth(i);
    		Decl* prev = table->Lookup(decl->id->name)->decl;
    		
    		if (table->Lookup(decl->id->name) != NULL) {
                ReportError::DeclConflict(decl, prev);
    		 } else {
    		    Declaracion dcl;
    		    dcl.tipo = decl->typeName;
    		    dcl.decl = decl;
                table->Enter(decl->id->name, dcl);
            }
     }
}

void StmtBlock::Check(){
	for (int i = 0; i < decls->NumElements(); i++) {
    		Decl* decl = decls->Nth(i);
            decl->Check();
     }
     for (int i = 0; i < stmts->NumElements(); i++) {
    		Stmt* stmt = stmts->Nth(i);
            stmt->Check();
     }
}

ConditionalStmt::ConditionalStmt(Expr *t, Stmt *b) { 
    Assert(t != NULL && b != NULL);
    (test=t)->SetParent(this); 
    (body=b)->SetParent(this);
}
void ConditionalStmt::Check(){
	test->Check();
    body->Check();
}

ForStmt::ForStmt(Expr *i, Expr *t, Expr *s, Stmt *b): LoopStmt(t, b) { 
    Assert(i != NULL && t != NULL && s != NULL && b != NULL);
    (init=i)->SetParent(this);
    (step=s)->SetParent(this);
}
void ForStmt::Check(){
	init->Check();
	test->Check();
	step->Check();
	body->Check();
}

IfStmt::IfStmt(Expr *t, Stmt *tb, Stmt *eb): ConditionalStmt(t, tb) { 
    Assert(t != NULL && tb != NULL); // else can be NULL
    elseBody = eb;
    if (elseBody) elseBody->SetParent(this);
}
void IfStmt::Check(){
test->Check();
body->Check();
elseBody->Check();
}


ReturnStmt::ReturnStmt(yyltype loc, Expr *e) : Stmt(loc) { 
    Assert(e != NULL);
    (expr=e)->SetParent(this);
}
void ReturnStmt::Check(){
expr->Check();
}
  
PrintStmt::PrintStmt(List<Expr*> *a) {    
    Assert(a != NULL);
    (args=a)->SetParentAll(this);
}
void PrintStmt::Check(){
for (int i = 0; i < args->NumElements(); i++) {
    		Expr* expr = args->Nth(i);
			expr->Check();
     }
}


