#include "Lexer.hpp"
#include <set>
#include <cstring>

using namespace std;


/*
 * Function to return the next token from standard input
 * the variable 'm_IdentifierStr' is set there in case of an identifier,
 * the variable 'm_NumVal' is set there in case of a number.
 */
static char character;   // vstupni znak
static InputCharType input; // vstupni symbol

Lexer::Lexer(const string & fileName) :
        m_FileName(move(fileName)) {

        m_Ifstream.open(m_FileName);
        readInput();

}

Lexer::~Lexer() {
    m_Ifstream.close();
}
void Lexer::readInput() {
     character = m_Ifstream.get();
    if ((character>='A' && character<='Z') || (character>='a' && character<='z'))
        input = LETTER;
    else if (character>='0' && character<='9')
        input = NUMBER;
        else if (character == EOF)
            input = END;
            else if (character == ' ' || character == '\n' || character == '\t' || character == '\v' ||
            character == '\f' || character =='\r') {
                    input = WHITE_SPACE;
                }
                else
                    input = NO_TYPE;
}
const struct {const char* slovo; Token symb;} keyWordTable[] = {
        {"program", tok_program},
        {"begin", tok_begin},
        {"end", tok_end},
        {"var", tok_var},
        {"const", tok_const},
        {"integer", tok_integer},
        {"array", tok_array},
        {"function", tok_function},
        {"if", tok_if},
        {"then", tok_then},
        {"else", tok_else},
        //{"switch", tok_s},
        {"for", tok_for},
        {"while", tok_while},
        {"to", tok_to},
        {"do", tok_do},
        {"downto", tok_downto},
        {"break", tok_break},
        {"write", tok_write},
        {"writeln", tok_writeln},
        {"readln", tok_readln},
        {"and", tok_and},
        {"or", tok_or},
        {"div", tok_div},
        {"mod", tok_mod},
        {"of", tok_of},
        {NULL, (Token) 0}
};

Token keyWord(string const & id) {
    int i = 0;

    while (keyWordTable[i].slovo)
        if (id ==  keyWordTable[i].slovo)
            return keyWordTable[i].symb;
        else
            i++;
    return tok_identifier;
}
int ifPositive = 1;
Token Lexer::gettok() {
    m_NumVal = 0;
    m_IdentifierStr = "";
    int delkaId = 0;
    Token type = tok_eof;
  //  readInput();
    q0:
    switch (character) {
        case '{':
          //  readInput();
            goto q1;
        case '(':
            readInput();

            return tok_lpar;
        case ')':
            readInput();

            return tok_rpar;
        case '[':
            readInput();

            return tok_lbra;
        case ']':
            readInput();

            return tok_rbra;
        case '*':
            readInput();
            return tok_multiply;
        case '+':
            readInput();

            return tok_plus;
        case '-':
            readInput();

            goto qnegatoveorminus;
        case '<':
            type = tok_less;
            readInput();

            goto q4;
        case '>':
            type = tok_greater;
            readInput();
            goto q41;
        case ':':
            readInput();
            goto q5;
        case '$':
            //type = tok_number;
            readInput();
            goto q31;
        case '&':
           // type = tok_number;
            readInput();
            goto q32;
        case ';':
           readInput();
            return tok_semicolon;
        case ',':
            readInput();

            return  tok_comma;
        case '.':
            readInput();

            return  tok_dot;
        case '=':
            readInput();
            goto q51;
        case '!':
            readInput();
            goto q52;
        default:;
    }

    switch (input) {
        case WHITE_SPACE:

            readInput();
            goto q0;
        case END:
            return tok_eof;
        case LETTER:
            delkaId = 1;
            m_IdentifierStr = character;
            readInput();
            goto q2;
        case NUMBER:
            m_NumVal = character - '0';
            ifPositive = 1;
            type = tok_number;
            readInput();
            goto q3;
        default:
            //data.type = ERR;
            cout<<character<<endl;
            cout<<"Nedovoleny znak."<<endl;

            return tok_error_token;
    }

    qnegatoveorminus:
        switch (input) {
            case NUMBER:
                type = tok_number;
                m_NumVal = 10 * m_NumVal + (character - '0');
                ifPositive = -1;
                readInput();
                goto q3;
            default:
                ifPositive = 1;
               // readInput();
                return tok_minus;
        }
    q1:
    switch(character) {
        case '}':
            readInput();
            goto q0;
        default:;
    }
    switch(input) {
        case END:
            cout<<"Neocekavany konec souboru v komentari."<<endl;
            return tok_error_token;
        default:
            readInput();
            goto q1;
    }

    q2:
    switch(input) {
        case LETTER:
        case NUMBER:
            m_IdentifierStr += character;
            delkaId++;
            readInput();
            goto q2;
        default:
            type = keyWord(m_IdentifierStr);
          //  m_IdentifierStr = "";
           // delkaId = 0;
            return type;
    }

    q3:
    switch(input) {
        case NUMBER:
            m_NumVal = 10 * m_NumVal + (character - '0');

            readInput();
            goto q3;
        default:
           // readInput();
            //cout<<m_NumVal<<endl;
            m_NumVal *= ifPositive;
            return tok_number;
    }

    q31: //HEXA
    switch (input) {
        case NUMBER:
        case LETTER:
            if ((character < 'a' && character > 'F') ||
                character > 'f') {
                //type = ERR;
                cout << "Invalid hexa number"<< endl;
                return tok_error_token;
            }
            m_NumVal = 16 * m_NumVal + ((character - '0') > 9 ? (character > 'F' ? character - 'a' + 10 : character - 'A' + 10) : character - '0');
            readInput();
            goto q31;
        default:
            return tok_number;
    }

    q32: //OCTAL
    switch (input) {
        case NUMBER:
        case LETTER:
            if (character > '7' || character < '0') {
                cout << "Invalid octav number"<< endl;
                return tok_error_token;
            }
            m_NumVal = 8 * m_NumVal + (character - '0');
            readInput();
            goto q32;
        default:
            return tok_number;
    }
    q4:
    switch(character) {
        case '=':
            readInput();
            return tok_lessequal;
        default:;
    }
    switch(input) {
        default:
            return tok_less;
    }

    q41:
    switch (character) {
        case '=':
            readInput();
            return tok_greaterequal;
        default:;
    }
    switch(input) {
        default:
            return tok_greater;
    }

    q5:
    switch(character) {
        case '=':
            readInput();

            return tok_assign;
        default : {

            return tok_colon;
        }
    }
//    switch(input) {
//        default:
//            data.type = ERR;
//            error("Ocekava se \'=\'.");
//            return data;
//    }


    q51:
    switch(character) {
        case '<':
            readInput();
            return tok_lessequal;
        case '>':
            readInput();
            return tok_greaterequal;
        default:
            return tok_equal;
    }


    q52:
    switch(character) {
        case '=':
            readInput();
            return tok_notequal;
    }
    switch(input) {
        default:
            cout << "Ocekava se \'=\'." << endl;
            return tok_error_token;
    }
    return tok_error_token;

}

