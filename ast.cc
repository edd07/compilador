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
	bool flag = true;
	while(flag && ptr->table->Lookup(name)==NULL ){
		ptr = ptr->parent;
		Program* prg = dynamic_cast<Program*>(ptr);
		if(prg!=NULL) flag=false;
	}
	
	if(!flag){	
		ReportError::IdentifierNotDeclared(this, LookingForVariable);
	}
}

