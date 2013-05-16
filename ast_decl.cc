/* File: ast_decl.cc
 * -----------------
 * Implementation of Decl node classes.
 */
#include "ast_decl.h"
#include "ast_type.h"
#include "ast_stmt.h"
#include "errors.h"
        
         
Decl::Decl(Identifier *n) : Node(*n->GetLocation()) {
    Assert(n != NULL);
    (id=n)->SetParent(this); 
}
void Decl::Check(){
	// Checar que no este ya declarado este id en el scope actual
		printf("%s", id->name);
	Decl* prev=parent->table->Lookup(id->name);
	if( prev != this )
		ReportError::DeclConflict(this, prev);
    id->Check();
}


VarDecl::VarDecl(Identifier *n, Type *t) : Decl(n) {
    Assert(n != NULL && t != NULL);
    (type=t)->SetParent(this);
}
void VarDecl::Check(){
	Decl::Check();
	type->Check();
}
  

ClassDecl::ClassDecl(Identifier *n, NamedType *ex, List<NamedType*> *imp, List<Decl*> *m) : Decl(n) {
    // extends can be NULL, impl & mem may be empty lists but cannot be NULL
    Assert(n != NULL && imp != NULL && m != NULL);     
    extends = ex;
    if (extends) extends->SetParent(this);
    (implements=imp)->SetParentAll(this);
    (members=m)->SetParentAll(this);
    
    for (int i = 0; i < m->NumElements(); i++) {
    		Decl* decl = m->Nth(i);
            table->Enter(decl->id->name, decl);
     }
}
void ClassDecl::Check(){
	Decl::Check();
	extends->Check();
	
	for (int i = 0; i < implements->NumElements(); i++) {
        NamedType* nType = implements->Nth(i);
        nType->Check();
     }
     
    for (int i = 0; i < members->NumElements(); i++) {
        Decl* decl = members->Nth(i);
        decl->Check();
    }
}


InterfaceDecl::InterfaceDecl(Identifier *n, List<Decl*> *m) : Decl(n) {
    Assert(n != NULL && m != NULL);
    (members=m)->SetParentAll(this);
    
    for (int i = 0; i < m->NumElements(); i++) {
        Decl* decl = m->Nth(i);
        table->Enter(decl->id->name, decl);
     }
}
void InterfaceDecl::Check(){     
Decl::Check();
    for (int i = 0; i < members->NumElements(); i++) {
        Decl* decl = members->Nth(i);
        decl->Check();
    }
}


	
FnDecl::FnDecl(Identifier *n, Type *r, List<VarDecl*> *d) : Decl(n) {
    Assert(n != NULL && r!= NULL && d != NULL);
    (returnType=r)->SetParent(this);
    (formals=d)->SetParentAll(this);
    body = NULL;
    
    for (int i = 0; i < d->NumElements(); i++) {
    		Decl* decl = d->Nth(i);
            table->Enter(decl->id->name, decl);
     }
}

void FnDecl::SetFunctionBody(Stmt *b) {
 
    (body=b)->SetParent(this);
    
    Iterator<Decl*> iter = b->table->GetIterator();
            Decl *decl;
            while ((decl = iter.GetNextValue()) != NULL) {
                 table->Enter(decl->id->name,decl);
            }
}

void FnDecl::Check(){
printf("FnDecl::Check");
Decl::Check();
    returnType->Check();
    body->Check();

     
    for (int i = 0; i < formals->NumElements(); i++) {
        VarDecl* vDecl = formals->Nth(i);
        vDecl->Check();
    }
}




