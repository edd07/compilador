/* File: ast_decl.cc
 * -----------------
 * Implementation of Decl node classes.
 */
 
#include "ast_decl.h"
#include "ast_type.h"
#include "ast_stmt.h"
#include "errors.h"
#include "hashtable.h"
        
         
Decl::Decl(Identifier *n) : Node(*n->GetLocation()) {
    Assert(n != NULL);
    (id=n)->SetParent(this); 
}
void Decl::Check(){
	id->Check();
}


VarDecl::VarDecl(Identifier *n, Type *t) : Decl(n) {
    Assert(n != NULL && t != NULL);
    (type=t)->SetParent(this);
}
void VarDecl::Check(){
    
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
    		Decl* prev = table->Lookup(decl->id->name);
    		
            if (table->Lookup(decl->id->name) != NULL) {
                ReportError::DeclConflict(decl, prev);
    		 } else {
                table->Enter(decl->id->name, decl);
            }
     }
}
void ClassDecl::Check(){	
	//Decl::Check(); // no checar el nombre que se esta definiendo
	if(extends) extends->Check();
	
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
        Decl* prev = table->Lookup(decl->id->name);
        
        if (table->Lookup(decl->id->name) != NULL) {
            ReportError::DeclConflict(decl, prev);
        } else {
            table->Enter(decl->id->name, decl);
        }
     }
}
void InterfaceDecl::Check(){     
//Decl::Check(); // no checar el nombre que se esta definiendo
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
    		VarDecl* decl = d->Nth(i);
            Decl* prev = table->Lookup(decl->id->name);
        
            if (table->Lookup(decl->id->name) != NULL) {
                ReportError::DeclConflict(decl, prev);
            } else {
                table->Enter(decl->id->name, decl);
            }
    }
}

void FnDecl::SetFunctionBody(Stmt *b) {
 
    (body=b)->SetParent(this);
    
    Iterator<Decl*> iter = b->table->GetIterator();
    Decl *decl;
    Decl *prev;
    while ((decl = iter.GetNextValue()) != NULL) {
        prev = table->Lookup(decl->id->name);
        
        if (table->Lookup(decl->id->name) != NULL) {
            ReportError::DeclConflict(decl, prev);
        } else {
            table->Enter(decl->id->name, decl);
        }
    }
}

void FnDecl::Check(){
    //Decl::Check(); no checar el nombre que se esta definiendo
    returnType->Check();
    body->Check();

     
    for (int i = 0; i < formals->NumElements(); i++) {
        VarDecl* vDecl = formals->Nth(i);
        vDecl->Check();
    }
}




