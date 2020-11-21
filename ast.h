#include <vector>
#include <map>
#include <iostream>
#include <string>

#include <llvm/IR/Value.h>
#include <llvm/IR/Function.h>
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IR/IRBuilder.h"
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/FormattedStream.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include </usr/lib/llvm-10/include/llvm/Target/TargetOptions.h>

using namespace std;


/*extern llvm::Module *TheModule;

extern llvm::IRBuilder<> Builder;
*/

//extern llvm::BasicBlock *MainBlock;*/


class ExprAST {
protected:

public:
    ExprAST() = default;
    virtual  void Print() const = 0;
    virtual llvm::Value *Codegen() = 0;
    virtual ~ExprAST() = default;
};

class BodyAst: public ExprAST {
    std::vector<std::unique_ptr<ExprAST>> Nodes;

public:
    void Print() const override ;
    BodyAst(std::vector<std::unique_ptr<ExprAST>> Nodes) : Nodes(std::move(Nodes)) {}
    llvm::Value *Codegen() override;
};

class StatementAST:public ExprAST {

};

class IfStatementExpr : public ExprAST
{
    std::unique_ptr<ExprAST> Cond, Then, Else;

public:
    void Print() const override ;
    IfStatementExpr(std::unique_ptr<ExprAST> Cond, std::unique_ptr<ExprAST> Then,
              std::unique_ptr<ExprAST> Else)
            : Cond(std::move(Cond)), Then(std::move(Then)), Else(std::move(Else)) {}

    llvm::Value *Codegen() override;
};

class Reference : public ExprAST
{
public:
    Reference() = default;
    //virtual llvm::Value* Codegen()  = 0;
    virtual llvm::Value* Allocate() const = 0;
    virtual std::string GetName()  =  0;
};


class VariableExprAST : public Reference {
public:
    string Name;
public:
    void Print() const override ;
    VariableExprAST(const string &name);
    llvm::Value *Codegen() override ;
    string GetName() override ;
    llvm::Value* Allocate() const override;
};

class ArrayExprAST : public Reference
{
    std::unique_ptr<VariableExprAST> var;
    std::unique_ptr<ExprAST> Index;
    std::string Name;

public:
    void Print() const override ;
    ArrayExprAST(std::unique_ptr<VariableExprAST> Var, std::unique_ptr<ExprAST> Index,  const std::string Name) : var(move(Var)), Index(std::move(Index)), Name(Name) {}
    llvm::Value *Codegen() override;
    llvm::Value* Allocate() const override;
    string GetName() override;
};
class ForStatementExpr:public StatementAST {
private:
    std::unique_ptr<VariableExprAST> name;
    std::unique_ptr<ExprAST> from;
    std::unique_ptr<ExprAST> to;
    int Step;
    std::unique_ptr<BodyAst> body;
public:
    void Print() const override ;
    ForStatementExpr(std::unique_ptr<VariableExprAST>  Name, std::unique_ptr<ExprAST> From, std::unique_ptr<ExprAST> To, int Step, std::unique_ptr<BodyAst> Body);
    llvm::Value* Codegen() override;
};

class WhileStatementExpr: public  StatementAST {
private:
    std::unique_ptr<ExprAST> condition;
    std::unique_ptr<BodyAst> body;
public:
    WhileStatementExpr(std::unique_ptr<ExprAST> Condition, std::unique_ptr<BodyAst> Body);
    llvm::Value* Codegen() override;
    void Print() const override ;
};

class BreakStatementExpr: public StatementAST {
public:
    BreakStatementExpr() = default;
    llvm::Value* Codegen()override;
    void Print() const override {}
};

class CallExprAST : public StatementAST
{
    std::string Callee;
    std::vector<std::unique_ptr<ExprAST>> Args;

public:

    CallExprAST(const std::string &Callee,
                std::vector<std::unique_ptr<ExprAST>> Args)
            : Callee(Callee), Args(std::move(Args)) {}
    void Print() const override ;
    llvm::Value *Codegen() override;
};


class NumberExprAST : public ExprAST{
    int Val;
public:
    void Print() const override ;
    NumberExprAST(int Val);
    llvm::Value *Codegen() override ;
   // llvm::Value* Allocate() const override;
};


/// BinaryExprAST - Класс узла выражения для бинарных операторов.
class BinaryExprAST : public ExprAST {
protected:
    std::unique_ptr<ExprAST> LHS, RHS;
public:
    void Print() const override ;
    BinaryExprAST( std::unique_ptr<ExprAST> lhs,  std::unique_ptr<ExprAST> rhs);
};

class Add : public BinaryExprAST {
public:

    Add( std::unique_ptr<ExprAST> lhs,  std::unique_ptr<ExprAST> rhs) : BinaryExprAST(move(lhs), move(rhs)) {}
    llvm::Value* Codegen() override ;
};

class Sub : public BinaryExprAST {
public:
    Sub( std::unique_ptr<ExprAST> lhs,  std::unique_ptr<ExprAST> rhs) : BinaryExprAST(move(lhs), move(rhs)) {}
    llvm::Value* Codegen() override ;
};

class Mult : public BinaryExprAST {
public:
    Mult( std::unique_ptr<ExprAST> lhs,  std::unique_ptr<ExprAST> rhs) : BinaryExprAST(move(lhs), move(rhs)) {}
    llvm::Value* Codegen() override ;
};

class Div : public BinaryExprAST {
public:
    Div( std::unique_ptr<ExprAST> lhs,  std::unique_ptr<ExprAST> rhs) : BinaryExprAST(move(lhs), move(rhs)) {}
    llvm::Value* Codegen() override ;
};

class Mod : public BinaryExprAST {
public:
    Mod( std::unique_ptr<ExprAST> lhs,  std::unique_ptr<ExprAST> rhs) : BinaryExprAST(move(lhs), move(rhs)) {}
    llvm::Value* Codegen() override ;
};

class Equal : public BinaryExprAST {
public:
    Equal( std::unique_ptr<ExprAST> lhs,  std::unique_ptr<ExprAST> rhs) : BinaryExprAST(move(lhs), move(rhs)) {}
    llvm::Value* Codegen() override ;
};

class NotEqual : public BinaryExprAST {
public:
    NotEqual( std::unique_ptr<ExprAST> lhs,  std::unique_ptr<ExprAST> rhs) : BinaryExprAST(move(lhs), move(rhs)) {}
    llvm::Value* Codegen() override ;
};

class Less : public BinaryExprAST {
public:
    Less( std::unique_ptr<ExprAST> lhs,  std::unique_ptr<ExprAST> rhs) : BinaryExprAST(move(lhs), move(rhs)) {}
    llvm::Value* Codegen() override ;
};

class Greater : public BinaryExprAST {
public:
    Greater( std::unique_ptr<ExprAST> lhs,  std::unique_ptr<ExprAST> rhs) : BinaryExprAST(move(lhs), move(rhs)) {}
    llvm::Value* Codegen() override ;
};

class LessOrEqual : public BinaryExprAST {
public:
    LessOrEqual( std::unique_ptr<ExprAST> lhs,  std::unique_ptr<ExprAST> rhs) : BinaryExprAST(move(lhs), move(rhs)) {}
    llvm::Value* Codegen() override ;
};

class GreaterOrEqual : public BinaryExprAST {
public:
    GreaterOrEqual( std::unique_ptr<ExprAST> lhs,  std::unique_ptr<ExprAST> rhs) : BinaryExprAST(move(lhs), move(rhs)) {}
    llvm::Value* Codegen() override ;
};

class And : public BinaryExprAST {
public:
    And( std::unique_ptr<ExprAST> lhs,  std::unique_ptr<ExprAST> rhs) : BinaryExprAST(move(lhs), move(rhs)) {}
    llvm::Value* Codegen() override ;
};

class Or : public BinaryExprAST {
public:
    Or( std::unique_ptr<ExprAST> lhs,  std::unique_ptr<ExprAST> rhs) : BinaryExprAST(move(lhs), move(rhs)) {}
    llvm::Value* Codegen() override ;
};



class DeclareExprAST : public ExprAST
{
    std::string Name;
    int Offset;
    int Length;

public:
    void Print() const override ;
    DeclareExprAST(const std::string & Name);
    DeclareExprAST(const std::string &Name, int Offset, int Length);
    llvm::Value *Codegen() override;
    string GetName() const { return Name;}

};

class ConstExprAST : public ExprAST {
public:
    ConstExprAST(std::string identifier, int value);
     void Print() const override ;
    llvm::Value *Codegen() override;
private:
    const std::string m_Identifier;
    const int m_Value;

};

class PrototypeAST{
    string Name;
    vector<unique_ptr<DeclareExprAST>> Args;
public:
    void Print() const;
    PrototypeAST( string name, vector<unique_ptr<DeclareExprAST>> args);
    virtual llvm::Function *Codegen();
    string GetName() const {
        return Name;
    }
};

class FunctionAST: public ExprAST {
    std::unique_ptr<PrototypeAST> Proto;
    std::unique_ptr<BodyAst> Body;
public:
    FunctionAST(std::unique_ptr<PrototypeAST> proto, std::unique_ptr<BodyAst> body);
    void Print() const override ;
    llvm::Function *Codegen() override ;
};

class AssignAST : public StatementAST {
public:
    void Print() const override ;
    AssignAST(std::unique_ptr<Reference> var, std::unique_ptr<ExprAST> expr) : Var(move(var)), Expression(move(expr)) {}
    virtual ~AssignAST() {}
    llvm::Value * Codegen() override ;
private:
    std::unique_ptr<Reference> Var;
    std::unique_ptr<ExprAST> Expression;
};



class Main
{
public:
    Main(std::string identifier, std::vector<std::unique_ptr<ExprAST>> declaration, std::unique_ptr<BodyAst> inner);

    void LLVM() const;
    unique_ptr<llvm::Module> GetLLVM() const;
private:
    const std::string m_Identifier;
    const std::vector<std::unique_ptr<ExprAST>> m_Declarations;
    const  std::unique_ptr<BodyAst> m_Inner;
};



ExprAST * M_Error(const char *Str);
PrototypeAST *ErrorP(const char *Str);
FunctionAST *ErrorF(const char *Str);
llvm::Value *ErrorV(const char *Str);

