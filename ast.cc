/* File: ast.cc
 * ------------
 */

#include "ast.h"
#include "ast_type.h"
#include "ast_decl.h"
#include "ast_stmt.h"
#include <string.h> // strdup
#include <stdio.h>  // printf
#include "errors.h"
#include <algorithm>



Node::Node(yyltype loc) {
    location = new yyltype(loc);
    parent = NULL;
    table = new Hashtable<Decl*>();
    
}

Node::Node() {
    location = NULL;
    parent = NULL;
    table = new Hashtable<Decl*>();
}

Decl* Node::buscaDecl(char* name){
	// busca la Decl del nombre dado en todos los scopes visibles
	bool flag = true;
	Node* ptr = this;
	while(flag && ptr->table->Lookup(name)==NULL ){
		ptr = ptr->parent;
		Program* prg = dynamic_cast<Program*>(ptr);
		if(prg!=NULL) flag=false;
	}
    return ptr->table->Lookup(name);
	
}
	 
Identifier::Identifier(yyltype loc, const char *n) : Node(loc) {
    name = strdup(n);
}

void Identifier::Check(){
	//aqui es donde pasa toda la magia
	Node* ptr = this->parent;
	reasonT reason;
	
    if(dynamic_cast<VarDecl*>(ptr) != 0) 
        reason = LookingForVariable;
    else if (dynamic_cast<ClassDecl*>(ptr) != 0)
        reason = LookingForClass;
    else if (dynamic_cast<InterfaceDecl*>(ptr) != 0)
        reason = LookingForInterface;
    else if (dynamic_cast<NamedType*>(ptr) != 0 || dynamic_cast<Type*>(ptr) != 0 || dynamic_cast<ArrayType*>(ptr) != 0)
        if (dynamic_cast<ClassDecl*>(ptr->parent) != 0) {
            List<NamedType*> elements = *dynamic_cast<ClassDecl*>(ptr->parent)->implements;
            bool found = 0;
            
            for (int i = 0; i < elements.NumElements(); i++) {
                if (elements.Nth(i)->id->name == dynamic_cast<NamedType*>(ptr)->id->name)
                    found = 1;
            }
            if (found == 1)
                reason = LookingForInterface;
            else
                reason = LookingForClass;
        } else
            reason = LookingForType;
    else if (dynamic_cast<FnDecl*>(ptr) != 0)
        reason = LookingForFunction;
    else
    	reason = LookingForVariable; // default
	
	bool flag = true;
	while(flag && ptr->table->Lookup(name)==NULL ){
		ptr = ptr->parent;
		Program* prg = dynamic_cast<Program*>(ptr);
		if(prg!=NULL) flag=false;
	}
	

	if(!flag && ptr->table->Lookup(name)==NULL){
	
		ReportError::IdentifierNotDeclared(this, reason);
	}
}

