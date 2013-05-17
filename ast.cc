/* File: ast.cc
 * ------------
 */

#include "ast.h"
#include "ast_type.h"
#include "ast_decl.h"
#include <string.h> // strdup
#include <stdio.h>  // printf
#include "errors.h"



Node::Node(yyltype loc) {
    location = new yyltype(loc);
    parent = NULL;
    table = new Hashtable<Declaracion*>();
    
}

Node::Node() {
    location = NULL;
    parent = NULL;
    table = new Hashtable<Declaracion*>();
}

void Node::Check(){
}
	 
Identifier::Identifier(yyltype loc, const char *n) : Node(loc) {
    name = strdup(n);
}

void Identifier::Check(){
	//aqui es donde pasa toda la magia
	int i = global::stack_i;
	while(i>=0 && global::scope_stack[i].Lookup(name)==NULL ){
		i--;
	}
	
	if(i<0)
		ReportError::IdentifierNotDeclared(this, LookingForVariable);
}

