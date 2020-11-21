#include "Parser.hpp"
#include <memory>
using namespace std;
//Use tutorials in: https://llvm.org/docs/tutorial/

int main () {
   // Parser parser("../samples/arrayMax.mila");
   // Parser parser("../samples/arrayTest.mila");
    Parser parser("../samples/consts.mila");
   // Parser parser("../samples/expressions.mila");
    //Parser parser("../samples/expressions2.mila");
    // Parser parser("../samples/factorialCycle.mila");
    // Parser parser("../samples/inputOutput.mila");
  //  Parser parser("../samples/sortBubble.mila");
    //Parser parser("../samples/breakTest.mila");
    //Parser parser("../samples/fibonacciCycle.mila");
    unique_ptr<Main> main = parser.Parse();
   main->GetLLVM()->print(outs(), nullptr);

    return 0;
}
