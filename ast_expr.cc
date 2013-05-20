/* File: ast_expr.cc
 * -----------------
 * Implementation of expression node classes.
 */
#include "ast_expr.h"
#include "ast_type.h"
#include "ast_decl.h"
#include <string.h>



IntConstant::IntConstant(yyltype loc, int val) : Expr(loc) {
    value = val;
}
void IntConstant::Check(){    type = Type::intType;}

DoubleConstant::DoubleConstant(yyltype loc, double val) : Expr(loc) {
    value = val;
}
void DoubleConstant::Check(){    type = Type::doubleType;}

BoolConstant::BoolConstant(yyltype loc, bool val) : Expr(loc) {
    value = val;
}
void BoolConstant::Check(){    type = Type::boolType;}

StringConstant::StringConstant(yyltype loc, const char *val) : Expr(loc) {
    Assert(val != NULL);
    value = strdup(val);
}
void StringConstant::Check(){    type = Type::stringType;}

Operator::Operator(yyltype loc, const char *tok) : Node(loc) {
    Assert(tok != NULL);
    strncpy(tokenString, tok, sizeof(tokenString));
}
void Operator::Check(){}

CompoundExpr::CompoundExpr(Expr *l, Operator *o, Expr *r) 
  : Expr(Join(l->GetLocation(), r->GetLocation())) {
    Assert(l != NULL && o != NULL && r != NULL);
    (op=o)->SetParent(this);
    (left=l)->SetParent(this); 
    (right=r)->SetParent(this);
}
CompoundExpr::CompoundExpr(Operator *o, Expr *r) 
  : Expr(Join(o->GetLocation(), r->GetLocation())) {
    Assert(o != NULL && r != NULL);
    left = NULL; 
    (op=o)->SetParent(this);
    (right=r)->SetParent(this);
}
void CompoundExpr::Check(){
if(left!=NULL) left->Check();
right->Check();
}
   
  
ArrayAccess::ArrayAccess(yyltype loc, Expr *b, Expr *s) : LValue(loc) {
    (base=b)->SetParent(this); 
    (subscript=s)->SetParent(this);
}
void ArrayAccess::Check(){
base->Check();
subscript->Check();
ArrayType* arrayType;

if(! (arrayType=dynamic_cast<ArrayType*>(base->type)) ){
	  ReportError::BracketsOnNonArray(base);
	  type=Type::errorType;
}else
	type = arrayType->elemType; 
	

if(subscript->type!=Type::intType)
	 ReportError::SubscriptNotInteger(subscript);
}
     
FieldAccess::FieldAccess(Expr *b, Identifier *f) 
  : LValue(b? Join(b->GetLocation(), f->GetLocation()) : *f->GetLocation()) {
    Assert(f != NULL); // b can be be NULL (just means no explicit base)
    base = b; 
    
	(field=f)->SetParent(this);
}
void FieldAccess::Check(){
Hashtable<Decl*>* scope;

	if (base){
		 base->Check();
    	 base->SetParent(this);
    	 
    	 //busca la clase a ver si tiene el field y determinar el tipo
    	 NamedType* namedType = dynamic_cast<NamedType*>(base->type);
    	 if(!namedType){
    	 //no es ni siquiera una clase
				ReportError::FieldNotFoundInBase(field, base->type);
				type = Type::errorType;
    	 }else{
			 //busca la declaracion de la clase
			 Decl* decl = buscaDecl(namedType->typeName);
			 ClassDecl* classDecl = dynamic_cast<ClassDecl*>(decl);
			 if(classDecl){
			 	VarDecl* valor = dynamic_cast<VarDecl*>(classDecl->table->Lookup(namedType->typeName));
			 	if(valor){
			 		type = valor->type;
			 	}else{
			 		ReportError::FieldNotFoundInBase(field, base->type);
			 		type = Type::errorType;
			 	}
			 }else{
			 	type = Type::errorType;
			 }
    	 }
    }else{
    	// determinar el tipo de un id (un field sin base)
		Decl* decl = buscaDecl(field->name);
		
		ClassDecl* classDecl;
		VarDecl* varDecl;
		
		if( (varDecl=dynamic_cast<VarDecl*>(decl)) ){
			type = varDecl->type;
		}else{
		
			if ( (classDecl=dynamic_cast<ClassDecl*>(decl)) ) {
				type = new NamedType(classDecl->id);
			}else{
			 	type = Type::errorType;
			}
		}
	}
}


Call::Call(yyltype loc, Expr *b, Identifier *f, List<Expr*> *a) : Expr(loc)  {
    Assert(f != NULL && a != NULL); // b can be be NULL (just means no explicit base)
    base = b;
    if (base) base->SetParent(this);
    (field=f)->SetParent(this);
    (actuals=a)->SetParentAll(this);
    
}
void Call::Check(){
//field->Check();
for (int i = 0; i < actuals->NumElements(); i++) {
    		Expr* expr = actuals->Nth(i);
			expr->Check();
     }
     
    // determinar tipo
    FnDecl* fnDecl;
    type = Type::errorType;
    if(!base){
	    Decl* decl = buscaDecl(field->name);
	    if(decl!=NULL && (fnDecl = dynamic_cast<FnDecl*>(decl)) )
    		type = fnDecl->returnType;
	}else{
		base->Check();
		// encontrar la definicion de la clase donde se define la funcion		
		NamedType* namedType = dynamic_cast<NamedType*>( base->type );
		if(namedType){
			ClassDecl* classDecl = dynamic_cast<ClassDecl*>(buscaDecl(namedType->typeName));
			FnDecl* fnDecl;
			if(classDecl && (fnDecl=dynamic_cast<FnDecl*>(classDecl->table->Lookup(field->name)) )  )
				type=fnDecl->returnType;
			else{
				//no esta en la clase
				ReportError::FieldNotFoundInBase(field, base->type);
			}
		}else{
			//ni siquiera es una clase
			ArrayType* arrayType;
			if( (arrayType=dynamic_cast<ArrayType*>(base->type)) && strcmp(field->name,"length")==0 )
				type=Type::intType;
			else
				ReportError::FieldNotFoundInBase(field, base->type);
		}
	
	}
}
 

NewExpr::NewExpr(yyltype loc, NamedType *c) : Expr(loc) { 
  Assert(c != NULL);
  (cType=c)->SetParent(this);
  
  
}
void NewExpr::Check(){
cType->Check();

//determinar el tipo
  Decl* decl = buscaDecl(cType->id->name);
  ClassDecl* classDecl;
  if(decl!=NULL && (classDecl=dynamic_cast<ClassDecl*>(decl)) ){
  	type = cType;
  } else{
  	type = Type::errorType;
  }
}


NewArrayExpr::NewArrayExpr(yyltype loc, Expr *sz, Type *et) : Expr(loc) {
    Assert(sz != NULL && et != NULL);
    (size=sz)->SetParent(this); 
    (elemType=et)->SetParent(this);
}
void NewArrayExpr::Check(){
size->Check();
elemType->Check();
type=elemType;
}

void Expr::Check(){

}

void ArithmeticExpr::Check(){
	if(left)
		left->Check();
	right->Check();
	if(left && left->type!=right->type){ //binary
		ReportError::IncompatibleOperands(op, left->type, right->type);	
	}else if(right->type!=Type::doubleType && right->type!=Type::intType){
		ReportError::IncompatibleOperand(op, right->type); // unary
	}
	if(left->type==right->type)
		type=left->type;
}

void RelationalExpr::Check(){
	left->Check();
	right->Check();
	if(left->type!=right->type){ //binary
		ReportError::IncompatibleOperands(op, left->type, right->type);	
	}
	type=Type::boolType;
}

void EqualityExpr::Check(){
	left->Check();
	right->Check();
	if(left->type!=right->type){ //binary
		ReportError::IncompatibleOperands(op, left->type, right->type);	
	}
	type=Type::boolType;
}

void LogicalExpr::Check(){
	left->Check();
	right->Check();
	if(left->type!=Type::boolType ||  right->type!=Type::boolType){ 
		ReportError::IncompatibleOperands(op, left->type, right->type);	
	}
	type=Type::boolType;
}

void AssignExpr::Check(){
	//TODO: hacer que las clases padres sean compatibles con las hijas
	left->Check();
	right->Check();
	if(left->type!=Type::boolType ||  right->type!=Type::boolType){ 
		ReportError::IncompatibleOperands(op, left->type, right->type);	
	}
	type=left->type;
}



       
