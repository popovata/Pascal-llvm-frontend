
#include "Lexer.hpp"
#include "ast.h"
#include <memory>
#include <iostream>     // std::cout, std::ios
#include <sstream>
using namespace llvm;

class Parser {
public:
    Parser(std::string fileName);
    ~Parser() = default;
    void CompareToken(Token token);
    void CompareIdentifier(string & ident);
    std::unique_ptr<Main> Parse();
    std::vector<std::unique_ptr<ExprAST>> Variable ();
    std::vector<std::unique_ptr<ExprAST>> Constant ( );
    std::unique_ptr<FunctionAST> Function();
    std::vector<std::unique_ptr<DeclareExprAST>> Parameters();
    std::unique_ptr<BodyAst> block ();
    std::vector <std::unique_ptr<ExprAST>> Statements();
    unique_ptr<StatementAST> ProcedureCall(string name);
    std::unique_ptr<IfStatementExpr> IfStatement();
    std::unique_ptr<ForStatementExpr> ForStatement();
    std::unique_ptr<WhileStatementExpr> WhileStatement();
    std::unique_ptr<BreakStatementExpr> BreakStatement ();
    std::unique_ptr<ExprAST> NoStatement();
    std::unique_ptr<StatementAST> Assigment(std::string name);
    std::unique_ptr<ExprAST> E();
    std::unique_ptr<ExprAST> F();
    std::unique_ptr<ExprAST> G();
    std::unique_ptr<ExprAST> H();
    std::vector<std::unique_ptr<ExprAST>> Declarations ();

private:
    Token getNextToken();
    Lexer m_Lexer;            // lexer is used to read tokens
    Token CurTok;               // to keep the current token
    string m_FileName;
};
