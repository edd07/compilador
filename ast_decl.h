/* File: ast_decl.h
 * ----------------
 * In our parse tree, Decl nodes are used to represent and
 * manage declarations. There are 4 subclasses of the base class,
 * specialized for declarations of variables, functions, classes,
 * and interfaces.
 *
 * pp3: You will need to extend the Decl classes to implement 
 * semantic processing including detection of declaration conflicts 
 * and managing scoping issues.
 */

#ifndef _H_ast_decl
#define _H_ast_decl

#include "ast.h"
#include "list.h"

class Type;
class NamedType;
class Identifier;
class Stmt;

class Decl : public Node 
{
  
  public:
      Identifier *id;
    Decl(Identifier *name);
    friend std::ostream& operator<<(std::ostream& out, Decl *d) { return out << d->id; }
    
    virtual void Check();
};

class VarDecl : public Decl 
{    
  public:   
    Type *type;
    VarDecl(Identifier *name, Type *type);
    
    void Check();
};

class ClassDecl : public Decl 
{
  protected:
    List<Decl*> *members;
    
    

  public:
    NamedType *extends;
    List<NamedType*> *implements;
    ClassDecl(Identifier *name, NamedType *extends, 
    List<NamedType*> *implements, List<Decl*> *members);
    
    void Check();
};

class InterfaceDecl : public Decl 
{
  protected:
    List<Decl*> *members;
    
  public:
    InterfaceDecl(Identifier *name, List<Decl*> *members);
    
    void Check();
};

class FnDecl : public Decl 
{
  public:
    List<VarDecl*> *formals;
    Type *returnType;
    Stmt *body;
    bool checked;
    
    FnDecl(Identifier *name, Type *returnType, List<VarDecl*> *formals);
    void SetFunctionBody(Stmt *b);
    
    void Check();
};

#endif
