/* File: ast.cc
 * ------------
 */

#include "ast.h"
#include "ast_type.h"
#include "ast_decl.h"
#include <string.h> // strdup
#include <stdio.h>  // printf
#include "errors.h"


// stack de scopes

struct Declaracion {
	std::string tipo;
	Decl* decl;
};

Hashtable<Declaracion*>* scope_stack[1024];
int stack_i = 0;


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
	int i = stack_i;
	while(i>=0 && scope_stack[i]->Lookup(name)==NULL ){
		i--;
	}
	
	if(i<0)
		ReportError::IdentifierNotDeclared(this, LookingForVariable);
}

