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
	 
Identifier::Identifier(yyltype loc, const char *n) : Node(loc) {
    name = strdup(n);
}

void Identifier::Check(){
	//aqui es donde pasa toda la magia
	Node* ptr = this->parent;
	reasonT reason;
	
	VarDecl* v = dynamic_cast<VarDecl*>(ptr);
    if(v != 0) 
        reason = LookingForVariable;
	
	Type* t = dynamic_cast<Type*>(ptr);
    NamedType* n = dynamic_cast<NamedType*>(ptr);
    if( t != 0 || n != 0) {
        if (dynamic_cast<InterfaceDecl*>(ptr) != 0 )
            reason = LookingForInterface;
        else
            reason = LookingForType;
    }
    
	ClassDecl* c = dynamic_cast<ClassDecl*>(ptr);
    if ( c != 0)
       reason = LookingForClass;
	
	FnDecl* f = dynamic_cast<FnDecl*>(ptr);
    if(f != 0)
       reason = LookingForFunction;
    
    ArrayType* a = dynamic_cast<ArrayType*>(ptr);
    if( a != 0 )
       reason = LookingForType;
	
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

