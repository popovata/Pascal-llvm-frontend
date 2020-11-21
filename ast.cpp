#include "ast.h"
using namespace llvm;

llvm::LLVMContext s_Context;
IRBuilder<> Builder = IRBuilder<>(s_Context);
static llvm::Value* s_NumberFormat;
static llvm::Value* s_NewLine;
static BasicBlock* s_Breakpoint = nullptr;
static BasicBlock* s_Returnpoint = nullptr;
static bool s_Break = false;


std::unique_ptr<llvm::Module> TheModule;

std::map<std::string, llvm::Value*> NamedValues;
std::map<std::string, llvm::Value*> ConstValues;

ExprAST* M_Error(const char *Str) {
    fprintf(stderr, "Error: %s\n", Str);
    return 0;
}

PrototypeAST* ErrorP(const char *Str) {
    M_Error(Str);
    return 0;
}
FunctionAST* ErrorF(const char *Str) {
    M_Error(Str);
    return 0;
}

llvm::Value *ErrorV(const char *Str) {
    M_Error(Str);
    return 0;
}


void BodyAst::Print() const {
    for (auto&j:Nodes) {
        j->Print();
    }
}


llvm::Value * BodyAst::Codegen()
{

    for (auto &expr : Nodes) {
        expr->Codegen();
        if(s_Break) {
            break;
        }
    }
    s_Break = false;
    return Constant::getNullValue(Type::getInt32Ty(s_Context));
}


NumberExprAST::NumberExprAST(int Val) :Val(Val) {}

void NumberExprAST::Print() const {
    cout<< Val<< endl;
}

Value *NumberExprAST::Codegen() {

    return llvm::ConstantInt::get(s_Context, llvm::APInt(32, Val, true));

}

VariableExprAST::VariableExprAST(const string &name) : Name(name) {}

void VariableExprAST::Print() const {
    cout<< Name<<endl;
}

Value *VariableExprAST::Codegen() {
    Value *V = NamedValues[Name];
    return V ? Builder.CreateLoad(V, Name.c_str()) : ErrorV("Unknown variable name");
}

std::string VariableExprAST::GetName(){
    return Name;
}

void ArrayExprAST::Print() const {
    cout<< Name<<' ';
    Index->Print();
}
Value * ArrayExprAST::Codegen()
{
    Value *V = NamedValues[Name];
    Value* l = Index->Codegen();
    if (!V)
        throw ("Unknown array name");
    auto ptr = Builder.CreateGEP(Type::getInt32Ty(s_Context), V, l);
    return Builder.CreateLoad(ptr);
}

ConstExprAST::ConstExprAST(std::string identifier, int value) : m_Identifier(identifier), m_Value(value) {};

void ConstExprAST::Print() const {
    cout << m_Identifier << ' '<< m_Value<<endl;
}

Value* ConstExprAST::Codegen() {
    Value * alloca = ConstValues[m_Identifier];
    if (alloca)
        throw ("Constant redeclaration");
    alloca = Builder.CreateAlloca(Type::getInt32Ty(s_Context));
    Builder.CreateStore(ConstantInt::get(s_Context, APInt(32, m_Value, true)), alloca);
    ConstValues[m_Identifier] = alloca;
    NamedValues[m_Identifier] = alloca;
    return alloca;
}

void BinaryExprAST::Print() const {
    LHS->Print();
    RHS->Print();
}

BinaryExprAST::BinaryExprAST(std::unique_ptr<ExprAST> lhs, std::unique_ptr<ExprAST> rhs): LHS(move(lhs)), RHS(move(rhs)) {}

Value* Add::Codegen()
{
    Value* a = LHS->Codegen();
    Value* b = RHS->Codegen();
    if(!a || !b) return nullptr;
    return Builder.CreateAdd(a, b, "addtmp");
}

Value* Sub::Codegen()
{
    Value* a = LHS->Codegen();
    Value* b = RHS->Codegen();
    if(!a || !b) return nullptr;
    return Builder.CreateSub(a, b, "subtmp");
}

Value* Mult::Codegen()
{
    Value* a = LHS->Codegen();
    Value* b = RHS->Codegen();
    if(!a || !b) return nullptr;
    return Builder.CreateMul(a, b, "multmp");
}

Value* Div::Codegen()
{
    Value* a = LHS->Codegen();
    Value* b = RHS->Codegen();
    if(!a || !b) return nullptr;
    return Builder.CreateSDiv(a, b, "divtmp");
}

Value* Mod::Codegen()
{
    Value* a = LHS->Codegen();
    Value* b = RHS->Codegen();
    if(!a || !b) return nullptr;
    return Builder.CreateSRem(a, b, "modtmp");
}

Value* Equal::Codegen()
{
    Value* a = LHS->Codegen();
    Value* b = RHS->Codegen();
    if(!a || !b) return nullptr;
    a = Builder.CreateICmpEQ(a, b, "eqtmp");
    return Builder.CreateIntCast(a, Type::getInt32Ty(s_Context), true, "booltmp");
}

Value* NotEqual::Codegen()
{
    Value* a = LHS->Codegen();
    Value* b = RHS->Codegen();
    if(!a || !b) return nullptr;
    a = Builder.CreateICmpNE(a, b, "netmp");
    return Builder.CreateIntCast(a, Type::getInt32Ty(s_Context), true, "booltmp");
}

Value* Less::Codegen()
{
    Value* a = LHS->Codegen();
    Value* b = RHS->Codegen();

    if(!a || !b) return nullptr;
    a = Builder.CreateICmpSLT(a, b, "lttmp");
    return Builder.CreateIntCast(a, Type::getInt32Ty(s_Context), true, "booltmp");
}

Value* Greater::Codegen()
{
    Value* a = LHS->Codegen();
    Value* b = RHS->Codegen();
    if(!a || !b) return nullptr;
    a = Builder.CreateICmpSGT(a, b, "gttmp");
    return Builder.CreateIntCast(a, Type::getInt32Ty(s_Context), true, "booltmp");
}

Value* LessOrEqual::Codegen()
{

    Value* a = LHS->Codegen();
    Value* b = RHS->Codegen();

    if(!a || !b) return nullptr;
    a = Builder.CreateICmpSLE(a, b, "letmp");
    return Builder.CreateIntCast(a, Type::getInt32Ty(s_Context), true, "booltmp");
}

Value* GreaterOrEqual::Codegen()
{
    Value* a = LHS->Codegen();
    Value* b = RHS->Codegen();
    if(!a || !b) return nullptr;
    a = Builder.CreateICmpSGE(a, b, "getmp");
    return Builder.CreateIntCast(a, Type::getInt32Ty(s_Context), true, "booltmp");
}

Value* And::Codegen()
{
    Value* a = LHS->Codegen();
    Value* b = RHS->Codegen();
    if(!a || !b) return nullptr;
    a = Builder.CreateICmpNE(
            a, ConstantInt::get(s_Context, APInt(32, 0, true)), "booltmp");
    b = Builder.CreateICmpNE(
            b, ConstantInt::get(s_Context, APInt(32, 0, true)), "booltmp");
    a = Builder.CreateAnd(a, b, "andtmp");
    return Builder.CreateIntCast(a, Type::getInt32Ty(s_Context), true, "booltmp");
}

Value* Or::Codegen() 
{
    Value* a = LHS->Codegen();
    Value* b = RHS->Codegen();
    if(!a || !b) return nullptr;
    return Builder.CreateOr(a, b, "ortmp");
}




DeclareExprAST::DeclareExprAST(const std::string &Name) : Name(Name), Offset(0), Length(0){};
DeclareExprAST::DeclareExprAST(const std::string &Name, int Offset, int Length) : Name(Name), Offset(Offset), Length(Length){};


void DeclareExprAST::Print() const {
    cout << Name<<' '<< Offset<< ' '<<Length<<endl;
}

Value * DeclareExprAST::Codegen()
{


    Value * alloca = NamedValues[Name];

    if (alloca)
        throw ("Variable redeclaration.");

    if (Length)
    {
        alloca = Builder.CreateAlloca(Type::getInt32Ty(s_Context),
                                      ConstantInt::get(s_Context, APInt(32, Length, true)));
        alloca = Builder.CreateGEP(Type::getInt32Ty(s_Context), alloca, ConstantInt::get(s_Context, APInt(32, Offset, true)));
    }
    else
    {

        alloca = Builder.CreateAlloca(Type::getInt32Ty(s_Context));
    }
    NamedValues[Name] = alloca;
    return alloca;
}


PrototypeAST::PrototypeAST(std::string name,  std::vector<std::unique_ptr<DeclareExprAST>> args)
        : Name(name){
    for (auto &j: args) {
        Args.emplace_back(move(j));
    }
}


void PrototypeAST::Print() const {
    cout<<Name<<endl;
    for (auto &j: Args) {
       j->Print();
    }
}


Function *PrototypeAST::Codegen() {
    std::vector<Type*> Ints(Args.size(),
                                     Type::getInt32Ty(s_Context));
    FunctionType *FT = FunctionType::get(Type::getInt32Ty(s_Context),
                                         Ints, false);
    Function *F = Function::Create(FT, Function::ExternalLinkage, Name, TheModule.get());
    if (F->getName() != Name) {
        F->eraseFromParent();
        F = TheModule->getFunction(Name);
        if (!F->empty()) {
            ErrorF("redefinition of function");
            return 0;
        }
        if (F->arg_size() != Args.size()) {
            ErrorF("redefinition of function with different number of args");
            return 0;
        }
    }

    unsigned Idx = 0;
    for(auto& arg : F->args()) {
       arg.setName(Args[Idx++]->GetName());
    }
    return F;
}


static inline llvm::AllocaInst * AllocBlock(llvm::Function* function,  const std::string& identifier)
{
    llvm::IRBuilder<> builder(&function->getEntryBlock(), function->getEntryBlock().begin());
    return builder.CreateAlloca(Type::getInt32Ty(s_Context), llvm::ConstantInt::get(llvm::Type::getInt32Ty(s_Context),
            Type::getInt32Ty(s_Context)->isArrayTy() ? Type::getInt32Ty(s_Context)->getArrayNumElements() : 0, false), identifier.c_str());
}


FunctionAST::FunctionAST(std::unique_ptr<PrototypeAST> proto, std::unique_ptr<BodyAst> body)
: Proto(move(proto)), Body(move(body)) {}


void FunctionAST::Print() const{
    Proto->Print();
    Body->Print();
}


Function *FunctionAST::Codegen() {
    NamedValues.clear();
    Function *TheFunction = TheModule->getFunction(Proto->GetName());
    if (!TheFunction) {
        TheFunction = Proto->Codegen();
    }
    //cout <<Proto->GetName()<<endl;
    if (!TheFunction)
        return nullptr;
 //   cout <<TheFunction->getName().str()<<endl;
    BasicBlock *BB = BasicBlock::Create(s_Context, "entry", TheFunction);
    Builder.SetInsertPoint(BB);
    NamedValues[this->Proto->GetName()] = AllocBlock(TheFunction,  this->Proto->GetName());
    for (auto &Arg : TheFunction->args())
    {
        std::string sugar = Builder.GetInsertBlock()->getParent()->getName().str();
        Value *Alloca = AllocBlock(TheFunction, sugar);
        Builder.CreateStore(&Arg, Alloca);
        NamedValues[sugar] = Alloca;
    }
    BasicBlock *Ret = BasicBlock::Create(s_Context, "return", TheFunction);
    //TheFunction->getBasicBlockList().push_back(Ret);
    //auto pblock = Builder.GetInsertBlock();
    auto prevReturnpoint = s_Returnpoint;
    s_Returnpoint = Ret;
    Body->Codegen();
    Builder.CreateBr(Ret);
    Builder.SetInsertPoint(Ret);
    s_Returnpoint = prevReturnpoint;
  //  Builder.CreateRetVoid();
 //   Builder.SetInsertPoint(pblock);
    verifyFunction(*TheFunction);
  //  Builder.SetInsertPoint()
    return TheFunction;
}


void IfStatementExpr::Print() const {
    Cond->Print();
    Then->Print();
    Else->Print();
}


Value * IfStatementExpr::Codegen() {
    Function*  func = Builder.GetInsertBlock()->getParent();
    BasicBlock* ifBlock = llvm::BasicBlock::Create(s_Context, "ifThen", func);
    BasicBlock* elseBlock = llvm::BasicBlock::Create(s_Context, "ifElse");
    func->getBasicBlockList().push_back(elseBlock);
    BasicBlock* afterBlock = llvm::BasicBlock::Create(s_Context, "ifAfter");
    func->getBasicBlockList().push_back(afterBlock);
    Value* condition = Builder.CreateICmpNE(Cond->Codegen(), llvm::ConstantInt::get(s_Context, llvm::APInt(32, 0, true)));
    Builder.CreateCondBr(condition, ifBlock, elseBlock);
    Builder.SetInsertPoint(ifBlock);
    Then->Codegen();
    Builder.CreateBr(afterBlock);
    Builder.SetInsertPoint(elseBlock);
    if(Else)
        Else->Codegen();
    Builder.CreateBr(afterBlock);
    Builder.SetInsertPoint(afterBlock);
    return Constant::getNullValue(Type::getInt32Ty(s_Context));
}


ForStatementExpr::ForStatementExpr(std::unique_ptr<VariableExprAST>  Name, std::unique_ptr<ExprAST> From, std::unique_ptr<ExprAST> To, int Step, std::unique_ptr<BodyAst> Body) :
   name(move(Name)), from(move(From)), to(move(To)), Step(Step), body(move(Body)) {}


void ForStatementExpr::Print() const {
    name->Print();
    from->Print();
    to->Print();
    body->Print();
}


llvm::Value* ForStatementExpr::Codegen() {
    Function* func = Builder.GetInsertBlock()->getParent();
    BasicBlock* forBlock = llvm::BasicBlock::Create(s_Context, "forBody", func);
    BasicBlock* afterBlock = llvm::BasicBlock::Create(s_Context, "forAfter");
    func->getBasicBlockList().push_back(afterBlock);
    string pblock = func->getName();
    Value* init = from->Codegen();
    if(!init)
        throw runtime_error("For loop invalid init value.");
    Builder.CreateStore(init, name->Allocate());
    Builder.CreateBr(forBlock);
    Builder.SetInsertPoint(forBlock);
    BasicBlock* prevBreakpoint = s_Breakpoint;
    s_Breakpoint = afterBlock;
    Value* body_codegen = body->Codegen();
    s_Breakpoint = prevBreakpoint;
    Value* step = llvm::ConstantInt::get(s_Context, llvm::APInt(32, Step, true));
    Value* end = to->Codegen();
    if(!end)
        throw runtime_error("For loop invalid end value.");
    Value* cur = Builder.CreateLoad(name->Allocate(), "curValue");
    Value* next = Builder.CreateAdd(cur, step, "nextValue");
    Builder.CreateStore(next, name->Allocate());
    Value* endCond =  Builder.CreateICmpNE(cur, to->Codegen());// : s_Builder.CreateICmpSGT(cur, m_End->Value());
    Builder.CreateCondBr(endCond, forBlock, afterBlock);
    Builder.SetInsertPoint(afterBlock);
    return Constant::getNullValue(Type::getInt32Ty(s_Context));
}


WhileStatementExpr::WhileStatementExpr(unique_ptr<ExprAST> Condition, unique_ptr<BodyAst> Body) : condition(move(Condition)), body(move(Body)) {
}


void WhileStatementExpr::Print() const{
    condition->Print();
    body->Print();
}


llvm::Value* WhileStatementExpr::Codegen() {
    Function *TheFunction = Builder.GetInsertBlock()->getParent();
    BasicBlock *CondBB = BasicBlock::Create(s_Context, "cond", TheFunction);
    BasicBlock *LoopBB = BasicBlock::Create(s_Context, "loop");
    BasicBlock *ExitBB = BasicBlock::Create(s_Context, "exit");
    Builder.CreateBr(CondBB);
    Builder.SetInsertPoint(CondBB);
    Value *CondV = condition->Codegen();
    if (!CondV)
        return nullptr;
    CondV = Builder.CreateICmpNE(
            CondV, ConstantInt::get(s_Context, APInt(32, 0, true)), "ifcond");
    Builder.CreateCondBr(CondV, LoopBB, ExitBB);
    TheFunction->getBasicBlockList().push_back(LoopBB);
    Builder.SetInsertPoint(LoopBB);
    BasicBlock* prevBreakpoint = s_Breakpoint;
    s_Breakpoint = ExitBB;
    Value *LoopV = body->Codegen();
    if (!LoopV)
        return nullptr;
    s_Breakpoint = prevBreakpoint;
    Builder.CreateBr(CondBB);
    TheFunction->getBasicBlockList().push_back(ExitBB);
    Builder.SetInsertPoint(ExitBB);
    return Constant::getNullValue(Type::getInt32Ty(s_Context));
}


llvm::Value* BreakStatementExpr::Codegen() {
    Function * mainFunc = Builder.GetInsertBlock()->getParent();
    BasicBlock* breakBlock = BasicBlock::Create(s_Context, "break", mainFunc);
    BasicBlock* afterBlock = BasicBlock::Create(s_Context, "aftbreak");
    Builder.CreateBr(breakBlock);
    Builder.SetInsertPoint(breakBlock);
    if(s_Breakpoint){
        Builder.CreateBr(s_Breakpoint);
    }
    s_Break = true;
    mainFunc->getBasicBlockList().push_back(afterBlock);
    Builder.SetInsertPoint(afterBlock);
    return nullptr;
}


void CallExprAST::Print() const {
    cout<<Callee<<": ";
    for (auto& j :Args) {
        j->Print();
    }
    cout<<endl;
}
Value * CallExprAST::Codegen()
{
    if(Callee == "writeln")
    {
        Value* params = Args[0]->Codegen() ;
        return Builder.CreateCall(TheModule->getFunction("writeln"), params);
    }
    else if(Callee == "readln")
    {
        Reference* arg (dynamic_cast<Reference *>((move(Args[0])).get()));
        Value* address = arg->Allocate();
        return Builder.CreateCall(TheModule->getFunction("readln"), address);
    }
    else if (Callee == "write") {

        Value* params = Args[0]->Codegen() ;

        return Builder.CreateCall(TheModule->getFunction("write"), params);
    }
    else if(Callee == "inc")
    {
        Value* val = Args[0]->Codegen();
        Value* address = dynamic_cast<VariableExprAST*>(Args[0].get())->Allocate();
       return Builder.CreateStore(Builder.CreateAdd(val, llvm::ConstantInt::get(llvm::Type::getInt32Ty(s_Context), 1, true)), address);
    }
    else if(Callee == "dec")
    {
        Value* val = Args[0]->Codegen();
        Value* address = dynamic_cast<VariableExprAST*>(Args[0].get())->Allocate();
       return  Builder.CreateStore(Builder.CreateSub(val, llvm::ConstantInt::get(llvm::Type::getInt32Ty(s_Context), 1, true)), address);
    }
else {
        Function *CalleeF = TheModule->getFunction(Callee);
      //  cout<<Callee<<endl;
        if (!CalleeF)
            throw ("Unknown function referenced");
        if (CalleeF->arg_size() != Args.size())
            throw ("Incorrect argument number passed");

        std::vector<Value *> ArgsV;

        for (unsigned i = 0, e = Args.size(); i != e; ++i) {

            ArgsV.push_back(Args[i]->Codegen());
           // cout<<"rer"<<endl;
        }
        return Builder.CreateCall(CalleeF, ArgsV, "calltmp");
    }
    return nullptr;
}


string ArrayExprAST::GetName() {
    return Name;
}


void AssignAST::Print() const {
    Var->Print();
    Expression->Print();
}


llvm::Value * AssignAST::Codegen() {
    string ident = Var->GetName();
    llvm::Value * expr = Expression->Codegen();
    llvm::Value * var = Var->Allocate();

     Builder.CreateStore(expr, var);
     return expr;
}


Value * VariableExprAST::Allocate (  ) const
{
    Value * ret = NamedValues [Name ];
    if (!ret) {
        throw ("Unknown variable name");
    }

    return ret;
}


Value * ArrayExprAST::Allocate ( ) const
{
    Value * V = NamedValues [ Name];
    if (!V)
        throw ("Unknown array name");
    auto ptr = Builder.CreateGEP(Type::getInt32Ty(s_Context), V, Index->Codegen());
    return ptr;
}


Main::Main(string identifier,  std::vector<std::unique_ptr<ExprAST>> declaration, std::unique_ptr<BodyAst> inner) :
        m_Identifier(move(identifier)),
        m_Declarations(move(declaration)),
        m_Inner(move(inner))
{}


void Main::LLVM() const
{
    std::vector<Type*> Ints(1, Type::getInt32Ty(s_Context));
    FunctionType * FT = FunctionType::get(Type::getInt32Ty(s_Context), Ints, false);
    llvm::Function * writeln = llvm::Function::Create(FT, Function::ExternalLinkage, "writeln", TheModule.get());
    for (auto & Arg : writeln->args())
        Arg.setName("x");
    llvm::Function * write = llvm::Function::Create(FT, Function::ExternalLinkage, "write", TheModule.get());
    for (auto & Arg : write->args())
        Arg.setName("x");
    Function* readln  = llvm::Function::Create(llvm::FunctionType::get(llvm::Type::getInt32Ty(s_Context), PointerType::getUnqual(IntegerType::getInt32Ty(s_Context)), true),
            llvm::Function::ExternalLinkage, llvm::Twine("readln"), TheModule.get());
    llvm::Function * MainFunction = llvm::Function::Create(FT, Function::ExternalLinkage, "main", TheModule.get());
    MainFunction->setCallingConv(llvm::CallingConv::C);
    llvm::BasicBlock * block = llvm::BasicBlock::Create(s_Context, "MainBegin", MainFunction);
    llvm::BasicBlock * mainEnd = llvm::BasicBlock::Create(s_Context, "MainEnd");
    MainFunction->getBasicBlockList().push_back(mainEnd);
    Builder.SetInsertPoint(block);
    s_NumberFormat = Builder.CreateGlobalStringPtr("%d");
    s_NewLine = Builder.CreateGlobalStringPtr("\n");
    for(auto& decl : m_Declarations) {
        decl->Codegen();
    }

    s_Returnpoint = mainEnd;
   // cout<<"efw"<<endl;
    m_Inner->Codegen();
    Builder.CreateBr(mainEnd);
    Builder.SetInsertPoint(mainEnd);
    s_Returnpoint = nullptr;
    Builder.CreateRet(llvm::ConstantInt::get(s_Context, llvm::APInt(32, 0)));
}

unique_ptr<llvm::Module> Main::GetLLVM() const
{
    TheModule = make_unique<llvm::Module>("", s_Context);
    LLVM();
    return move(TheModule);
}

