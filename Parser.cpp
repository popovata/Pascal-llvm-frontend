#include "Parser.hpp"

Parser::Parser(std::string fileName) :
        m_Lexer(fileName) {
}


void Parser::CompareToken(Token token) {
    bool match = CurTok == token;
    std::ostringstream oss;
    oss << "Invalid token. Expected " << token << ", got " << CurTok;
    if(!match)
        throw runtime_error(oss.str());
}


void Parser::CompareIdentifier(string  & ident) {
    bool match = CurTok == Token::tok_identifier;
    std::ostringstream oss;
    oss << "Invalid token. Expected " << Token::tok_identifier << ", got " << CurTok;
    if(!match)
        throw runtime_error(oss.str());
    ident = m_Lexer.identifierStr();
}


std::unique_ptr<Main> Parser::Parse() {
    getNextToken();
    CompareToken(Token::tok_program);
    getNextToken();
    string nameOfProgram = m_Lexer.identifierStr();
    getNextToken();
    CompareToken(Token::tok_semicolon);
    auto decl =  Declarations();

    auto main = block();
   // main->Print();
    return make_unique<Main>(nameOfProgram, move(decl), move(main));

}


std::vector<std::unique_ptr<ExprAST>> Parser::Declarations ()
{
    getNextToken();
    std::vector <std::unique_ptr <ExprAST>> decl;
    while ( true )
    {
        if ( CurTok == tok_var )
        {
            for(auto& declaration : Variable())
                decl.emplace_back(move(declaration));
        }
         else if ( CurTok == tok_const )
        {
            for(auto& declaration : Constant())
                decl.emplace_back(move(declaration));
        }
       else  if ( CurTok == tok_function )
        {
                decl.emplace_back(move(Function()));
        }
        /*else if ( CurTok == tok_procedure )
        {
            auto procDecl = procedure (CurTok );
            decl . push_back ( std::move ( procDecl ) );
        }*/
         else  break;
    }
    return decl;
}



std::vector<std::unique_ptr<ExprAST>> Parser::Variable () {
    CompareToken(tok_var);
    std::vector<std::string> names;
    std::vector<std::unique_ptr<ExprAST>> decl;
    getNextToken();
    while (true) {
        while (true) {
            string name;
            CompareIdentifier(name);
            names.push_back(name);
            getNextToken();
            if (CurTok == tok_colon) {
                break;
            }
            if (CurTok != tok_comma && CurTok != tok_colon) {
                std::ostringstream oss;
                oss << "Invalid token. Expected ',' or ':' got" << CurTok;
                throw runtime_error(oss.str());
            }
            getNextToken();
        }
        getNextToken();
        switch (CurTok) {
            case tok_integer: {
                for (std::string &name : names){
                    decl.push_back(make_unique<DeclareExprAST>(name)); }
                break;
            }
            case tok_array: {
                getNextToken();
                CompareToken(tok_lbra);
                getNextToken();
                CompareToken(tok_number);
                int from_array = m_Lexer.numVal();
                getNextToken();
                CompareToken(tok_dot);
                getNextToken();
                CompareToken(tok_dot);
                getNextToken();
                CompareToken(tok_number);
                int to_array = m_Lexer.numVal();
                getNextToken();
                CompareToken(tok_rbra);
                getNextToken();
                CompareToken(tok_of);
                getNextToken();
                CompareToken(tok_integer);
                for (std::string &name : names)
                    decl.push_back(make_unique<DeclareExprAST>(name, -from_array, to_array - from_array + 1));
                break;
            }
            default:
                M_Error("Variable type must be 'integer' or 'array'");
        }
        names.clear();
        getNextToken();
        CompareToken(tok_semicolon);
        getNextToken();
        if (CurTok != tok_identifier) {
            break;
        }
    }
    return decl;
}


std::vector<std::unique_ptr<ExprAST>> Parser::Constant ()
{
    CompareToken(tok_const);
    std::vector<std::unique_ptr<ExprAST>> decl;
    getNextToken();
    while (true)
    {
        str:string ident;
        CompareIdentifier(ident);
        getNextToken();
        CompareToken(tok_equal);
        getNextToken();
        CompareToken(tok_number);
        int cur_num = m_Lexer.numVal();
        getNextToken();
        CompareToken(tok_semicolon);
        decl . push_back ( make_unique <ConstExprAST> ( ident, cur_num ) );
        getNextToken();
        if (CurTok != tok_identifier) {
            break;
        }
    }
    return decl;
}


std::vector<std::unique_ptr<DeclareExprAST>> Parser::Parameters()
{
    std::vector<std::string> args;
    CompareToken(tok_lpar);
    getNextToken();
    while ( true )
    {
        if ( CurTok == tok_rpar )
            break;
        std::string name_var;
        CompareIdentifier(name_var);
        args . emplace_back ( move(name_var) );
        getNextToken();
        CompareToken(tok_colon);
        getNextToken();
        CompareToken(tok_integer);
        getNextToken();
        if ( CurTok != tok_semicolon && CurTok != tok_rpar )
            M_Error("unexpected character");
    }
    std::vector<std::unique_ptr<DeclareExprAST>> decl;
    for(auto& name : args) {
        decl.emplace_back(make_unique<DeclareExprAST>(move(name)));
    }
    return decl;
}


std::unique_ptr<FunctionAST> Parser::Function ()
{
    if ( CurTok != tok_function )
        M_Error ("function expected");
    getNextToken();
    std::string name_function;
    CompareIdentifier(name_function);
    getNextToken();
    CompareToken(tok_lpar);
    std::vector<std::unique_ptr<DeclareExprAST>> args = Parameters();
    getNextToken();
    CompareToken(tok_colon);
    getNextToken();
    CompareToken(tok_integer);
    getNextToken();
    CompareToken(tok_semicolon);
    auto proto = make_unique <PrototypeAST> ( name_function, std::move ( args ) );
    getNextToken();
    std::vector <std::unique_ptr<ExprAST>> declarations;
    if ( CurTok == tok_forward )
    {
        CompareToken(tok_semicolon);
        return make_unique<FunctionAST>(move(proto), make_unique<BodyAst>(std::move(declarations)));
    }
    std::vector <std::unique_ptr<ExprAST>> body;
    while (true) {
        if ( CurTok == tok_var )
        {
            for(auto& decl : Variable())
                declarations.emplace_back(move(decl));
        }
        else
            break;
    }
    auto def = block ();
    CompareToken(tok_semicolon);
    body . push_back ( std::move ( def ) );
    getNextToken();
    return make_unique <FunctionAST> ( std::move ( proto ), make_unique<BodyAst>(std::move ( body ) ));
}



std::unique_ptr<BodyAst> Parser::block ()
{
    CompareToken(tok_begin);
    std::vector <std::unique_ptr<ExprAST>> body = move(Statements());
    return make_unique <BodyAst> ( std::move ( body ) );
}


std::vector <std::unique_ptr<ExprAST>> Parser::Statements() {
    std::vector<std::unique_ptr<ExprAST>> statements;

    if (CurTok == tok_end) {
        getNextToken();
        return statements;
    }
    while (true) {
        if (CurTok == tok_end) {
            getNextToken();
            break;
        }
        getNextToken();
        if (CurTok == tok_end) {
            getNextToken();
            break;
        }
        switch (CurTok) {
            case tok_if:
                statements.emplace_back(IfStatement());
                break;
            case tok_for:
                statements.emplace_back(ForStatement());
                break;
            case tok_while:
                statements.emplace_back(WhileStatement());
                break;
            case tok_break:
                statements.emplace_back(BreakStatement());
                break;
            default:
                statements.emplace_back(move(NoStatement()));
                break;
        }
    }
    return statements;
}


std::unique_ptr<IfStatementExpr> Parser::IfStatement() {
    CompareToken(tok_if);
    getNextToken();
    std::unique_ptr<ExprAST> condition = E();
    CompareToken(tok_then);
    getNextToken();
    std::unique_ptr<ExprAST> ifBlock = nullptr;
    if (CurTok == tok_begin) {
        ifBlock = block();
    } else {
        switch (CurTok) {

            case tok_if:
                ifBlock = IfStatement();
                break;
            case tok_for:
                ifBlock = ForStatement();
                break;
            case tok_while:
                ifBlock = WhileStatement();
                break;
            case tok_break:
                ifBlock = BreakStatement();
                break;
            default:
                ifBlock= NoStatement();
        }
    }
    std::unique_ptr<ExprAST> elseBlock = nullptr;
    if(CurTok == tok_else)
    {
        getNextToken();
        if (CurTok == tok_begin) {
            elseBlock = block();
        } else {
            switch (CurTok) {
                case tok_if:
                    elseBlock= IfStatement();
                    break;
                case tok_for:
                    elseBlock = ForStatement();
                    break;
                case tok_while:
                    elseBlock = WhileStatement();
                    break;
                case tok_break:
                    elseBlock = BreakStatement();
                    break;
                default:
                    elseBlock = NoStatement();
            }
        }
    }
    return make_unique<IfStatementExpr>(move(condition), move(ifBlock), move(elseBlock));
}


std::unique_ptr<ForStatementExpr> Parser::ForStatement(){
    string identifier;
    getNextToken();
    CompareIdentifier(identifier);
   unique_ptr<VariableExprAST> name = make_unique<VariableExprAST>(identifier);
    getNextToken();
    CompareToken(tok_assign);
    getNextToken();
    auto start = E();
    int step;
    switch (CurTok) {
        case tok_to:
            step = 1;
            break;
        case tok_downto:

            step = -1;
            break;
        default:
            M_Error("To or downto keywords are expected");
    }
    getNextToken();
    auto end = E();
    CompareToken(tok_do);
    getNextToken();
    std::unique_ptr<BodyAst> body = block();
    return make_unique<ForStatementExpr>(move(name), move(start), move(end), step, move(body));
}


std::unique_ptr<WhileStatementExpr> Parser::WhileStatement() {
    CompareToken(tok_while);
    getNextToken();
    unique_ptr<ExprAST> condition = E();
    CompareToken(tok_do);
    getNextToken();
    unique_ptr<BodyAst> whileBlock = block();
    return make_unique<WhileStatementExpr>(move(condition), move(whileBlock));
}


std::unique_ptr<BreakStatementExpr> Parser::BreakStatement () {
        CompareToken(tok_break);
        getNextToken();
        getNextToken();
        return make_unique<BreakStatementExpr>();
}


std::unique_ptr<ExprAST> Parser::NoStatement()
{
    std::string name = m_Lexer.identifierStr();
    getNextToken();
    if(CurTok == tok_lpar) {
        return ProcedureCall(name);
    }
    else {
        return Assigment(name);
    }
}


unique_ptr<StatementAST> Parser::ProcedureCall(string name)
{
    CompareToken(tok_lpar);
    vector<unique_ptr<ExprAST>> params;
    getNextToken();
    while(CurTok != tok_rpar)
    {;
        if(params.size() > 0) {
            CompareToken(tok_comma);
            getNextToken();
        }
        params.emplace_back(move(E()));
    }
    CompareToken(tok_rpar);
    getNextToken();
    CompareToken(tok_semicolon);
    return make_unique<CallExprAST>(move(name), move(params));
}


std::unique_ptr<StatementAST> Parser::Assigment(std::string name) {
    std::unique_ptr<Reference> ref = make_unique<VariableExprAST> (name);
    std::unique_ptr<VariableExprAST> ref1 = make_unique<VariableExprAST> (name);
    switch (CurTok) {
        case tok_lbra:
            getNextToken();
            std::unique_ptr<ExprAST> index = E();
            CompareToken(tok_rbra);
            std::unique_ptr<ArrayExprAST> ref2 = make_unique<ArrayExprAST> (move(ref1),  move(index), name);
            ref = move(ref2);
            getNextToken();
            break;

    }
    CompareToken(tok_assign);
    getNextToken();
    std::unique_ptr<ExprAST> expression = E();
    return make_unique<AssignAST>(move(ref), move(expression) );
}


std::unique_ptr<ExprAST> Parser::E()
{
    std::unique_ptr<ExprAST> expression = F();
    bool cont = true;
    while(cont)
    {
        switch(CurTok)
        {
            case tok_equal:
               getNextToken();
                expression = make_unique<Equal>(move(expression), F());
                break;
            case tok_notequal:
                getNextToken();
                expression = make_unique<NotEqual>(move(expression), F());
                break;
            case tok_less:
                getNextToken();
                expression = make_unique<Less>(move(expression), F());
                break;
            case tok_lessequal:
                getNextToken();
                expression = make_unique<LessOrEqual>(move(expression), F());
                break;
            case tok_greater:
                getNextToken();
                expression = make_unique<Greater>(move(expression), F());
                break;
            case tok_greaterequal:
                getNextToken();
                expression = make_unique<GreaterOrEqual>(move(expression), F());
                break;
            case tok_and:
                getNextToken();
                expression = make_unique<And>(move(expression), F());
                break;
            case tok_or:
                getNextToken();
                expression = make_unique<Or>(move(expression), F());
                break;
            default:
                cont = false;
                break;
        }
    }
    return expression;
}


std::unique_ptr<ExprAST> Parser::F()
{
    std::unique_ptr<ExprAST> expression = G();
    bool cont = true;
    while(cont)
    {
        switch(CurTok)
        {
            case tok_plus:
                getNextToken();
                expression = make_unique<Add>(move(expression), G());
                break;
            case tok_minus:
                getNextToken();
                expression = make_unique<Sub>(move(expression), G());
                break;
            default:
                cont = false;
                break;
        }
    }
    return expression;
}


std::unique_ptr<ExprAST> Parser::G()
{
    std::unique_ptr<ExprAST> expression = H();
    bool cont = true;
    while(cont)
    {
        switch(CurTok)
        {
            case tok_multiply:
                getNextToken();
                expression = make_unique<Mult>(move(expression), H());
                break;
            case tok_div:
                getNextToken();
                expression = make_unique<Div>(move(expression), H());
                break;
            case tok_mod:
                getNextToken();
                expression = make_unique<Mod>(move(expression), H());
                break;
            default:
                cont = false;
                break;
        }
    }
    return expression;
}


std::unique_ptr<ExprAST> Parser::H()
{
    if(CurTok == tok_number)
    {
        int val = m_Lexer.numVal();
        getNextToken();
        return make_unique<NumberExprAST>(val);
    }
    else if(CurTok == tok_identifier)
    {
        string id;
        CompareIdentifier(id);
        getNextToken();
        if(CurTok == tok_lpar) {
            vector<unique_ptr<ExprAST>> params;
            getNextToken();
            while(CurTok != tok_rpar)
            {
                if(params.size() > 0) {
                    CompareToken(tok_comma);
                    getNextToken();
                }
                params.emplace_back(move(E()));
            }
            CompareToken(tok_rpar);
            getNextToken();
            if (CurTok == tok_semicolon) {
                getNextToken();
            }
            return make_unique<CallExprAST>(move(id), move(params));
        }
        else {
            if (CurTok ==tok_lbra) {
                    getNextToken();
                    std::unique_ptr<ExprAST> index = E();
                    CompareToken(tok_rbra);
                    getNextToken();
                 std::unique_ptr<VariableExprAST> var = make_unique<VariableExprAST>(id);
                return make_unique<ArrayExprAST>(move(var),move(index), id);
            }
            else {
                return make_unique<VariableExprAST>(id);
            }
        }
    }
    else if (CurTok == tok_lpar)
    {
        getNextToken();
        std::unique_ptr<ExprAST> exp = E();
        CompareToken(tok_rpar);
        getNextToken();
        return exp;
    }
    return nullptr;
}


Token Parser::getNextToken() {
    return CurTok = m_Lexer.gettok();
}

