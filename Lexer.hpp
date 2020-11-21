
#include <vector>
#include <fstream>
#include <set>
#include <iostream>


using namespace std;

enum Token {
    tok_eof =           -1,

    // numbers and identifiers
    tok_identifier =    -2,
    tok_number =        -3,

    // keywords
    tok_begin =         -4,
    tok_end =           -5,
    tok_const =         -6,
    tok_procedure =     -7,
    tok_forward =       -8,
    tok_function =      -9,
    tok_if =            -10,
    tok_then =          -11,
    tok_else =          -12,
    tok_program =       -13,
    tok_while =         -14,
    tok_break =          -15,
    tok_var =           -16,
    tok_integer =       -17,
    tok_for =           -18,
    tok_do =            -19,

    // 2-character operators
    tok_notequal =      -20,
    tok_lessequal =     -21,
    tok_greaterequal =  -22,
    tok_assign =        -23,
    tok_or =            -24,

    // 3-character operators (keywords)
    tok_mod =           -25,
    tok_div =           -26,
    tok_not =           -27,
    tok_and =           -28,
    tok_xor =           -29,

    // keywords in for loop
    tok_to =            -30,
    tok_downto =        -31,

    //IO keywords
    tok_read = -32,
    tok_write = -33,
    tok_readln = -34,
    tok_writeln = -35,

    //array keywords
    tok_array = -36,
    tok_of = -37,

    tok_lpar = -38,
    tok_rpar = -39,

    tok_lbra = -40,
    tok_rbra = -41,

    tok_plus = -42,
    tok_minus = -43,
    tok_divide = -44,
    tok_multiply = -45,

    tok_semicolon = -46,
    tok_comma = -47,
    tok_dot = -48,

    tok_less = -49,
    tok_greater = -50,

    tok_colon = -51,
    tok_equal = -52,

    tok_error_token = 1

};




class Lexer {
public:
    Lexer(const string & fileName);
    ~Lexer();
    Token gettok();
    void readInput();
    void readString(std::string &str);
    const std::string& identifierStr() const { return this->m_IdentifierStr; }
    int numVal() const{ return this->m_NumVal; }
private:
    string m_FileName;
    ifstream m_Ifstream;
    std::string m_IdentifierStr;
    int m_NumVal;
    vector<int> buffer;
};


/*
 * Lexer returns tokens [0-255] if it is an unknown character, 
 * otherwise one of these for known things.
 * Here are all valid tokens:
 */

enum InputCharType {LETTER, NUMBER, WHITE_SPACE, END, NO_TYPE};

/*set <string> keywords =
        {
                "begin", "end", "const", "procedure",
                "forward", "function", "if", "then", "else",
                "program", "while", "exit", "var", "integer",
                "for", "do", "to", "downto"
        };*/
