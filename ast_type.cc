/* File: ast_type.cc
 * -----------------
 * Implementation of type node classes.
 */
 
#include "ast_type.h"
#include "ast_decl.h"
#include "ast_stmt.h"
#include <string.h>

 
/* Class constants
 * ---------------
 * These are public constants for the built-in base types (int, double, etc.)
 * They can be accessed with the syntax Type::intType. This allows you to
 * directly access them and share the built-in types where needed rather that
 * creates lots of copies.
 */

Type *Type::intType    = new Type("int");
Type *Type::doubleType = new Type("double");
Type *Type::voidType   = new Type("void");
Type *Type::boolType   = new Type("bool");
Type *Type::nullType   = new Type("null");
Type *Type::stringType = new Type("string");
Type *Type::errorType  = new Type("error"); 

Type::Type(const char *n) {
    Assert(n);
    typeName = strdup(n);
}
void Type::Check(){

}

bool Type::IsCompatibleTo(Type* other,Node* scope){
	if(this->IsEquivalentTo(other)) return true;
	if(other==Type::nullType || other==Type::errorType || this==Type::errorType) return true;
	return false;
}


	
NamedType::NamedType(Identifier *i) : Type(*i->GetLocation()) {
    Assert(i != NULL);
    (id=i)->SetParent(this);
    Type::typeName = id->name;
}
void NamedType::Check(){
Type::Check();
id->Check();
}

bool NamedType::IsEquivalentTo(Type *other){

	NamedType* namedOther = dynamic_cast<NamedType*>(other);
	if(namedOther){
		return strcmp(namedOther->typeName,this->typeName)==0;
	}else
		return false;
}

bool NamedType::IsCompatibleTo(Type *other,Node* scope){
	// busca si other es subclase o implementacion de this
	if(Type::IsCompatibleTo(other,scope)) return true;
	NamedType* namedOther = dynamic_cast<NamedType*>(other);
	if(namedOther){
		//extends
		ClassDecl* extended = dynamic_cast<ClassDecl*>(scope->buscaDecl(namedOther->typeName));
		while(extended->extends){
			if(this->IsEquivalentTo(extended->extends)) return true;
			extended = dynamic_cast<ClassDecl*>(scope->buscaDecl(extended->extends->typeName));
		}
		//implements
		List<NamedType*>* implements = dynamic_cast<ClassDecl*>(scope->buscaDecl(namedOther->typeName))->implements;
		for (int i = 0; i < implements->NumElements(); i++) {
		    NamedType* nType = implements->Nth(i);
		    if(this->IsEquivalentTo(nType)) return true;
     	}	
	}
	return false;

}

ArrayType::ArrayType(yyltype loc, Type *et) : Type(loc) {
    Assert(et != NULL);
    (elemType=et)->SetParent(this);
}
void ArrayType::Check(){

elemType->Check();
}

bool ArrayType::IsEquivalentTo(Type *other){
	ArrayType* arrayOther = dynamic_cast<ArrayType*>(other);
	if (arrayOther){
		return this->elemType->IsEquivalentTo(arrayOther->elemType);
	}else
		return false;
}

bool ArrayType::IsCompatibleTo(Type* other,Node* scope){
	if(Type::IsCompatibleTo(other,scope)) return true;
ArrayType* arrayOther = dynamic_cast<ArrayType*>(other);
	if (arrayOther){
		return this->elemType->IsEquivalentTo(arrayOther->elemType);
	}else
		return false;
}

