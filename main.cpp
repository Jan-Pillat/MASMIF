
#include <iostream>
#include <windows.h>
#include "scripts\Token.hpp"
#include "scripts\Lexer.hpp"
#include "scripts\Parser.hpp"
#include "scripts\Assembler.hpp"
#include "CppCore/include/ReadableWinAPI.hpp"

using namespace std;


void PrintTokens (Lexer& lexer)
{
    for (size_t i = 0;  i<lexer.tokens.size();  i++)
        cout << "TOKEN " << i << ':' << endl << "  type: " << tokenTypeDescription[lexer.tokens[i].type] << endl << "  content: " << lexer.tokens[i].content << endl << endl;
}


int main()
{
    FileSelector selector;
    selector.GetPath_OpenFile();

    Lexer lexer;
    lexer.Tokenize  (selector.path);
    PrintTokens     (lexer);

    Parser parser(lexer.tokens);

    Assembler assembler (parser);

    system ("pause");
    return 0;
}
