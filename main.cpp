
#include <iostream>
#include <windows.h>
#include "scripts\PEData.hpp"
#include "scripts\Token.hpp"
#include "scripts\Lexer.hpp"
#include "scripts\Parser.hpp"
#include "scripts\Assembler.hpp"
#include "CppCore\include\ReadableWinAPI.hpp"

using namespace std;


void PrintTokens (Lexer& lexer)
{
    for (size_t i = 0;  i<lexer.tokens.size();  i++)
        cout << "TOKEN " << i << ':' << endl << "  type: " << tokenTypeDescription[lexer.tokens[i].type] << endl << "  content: " << lexer.tokens[i].content << endl << endl;
}

// ========== BASE FILE ========== \\


// ========== MAIN ========== \\

int main()
{
    FileSelector userScript;
    userScript.GetPath_OpenFile();

    if (userScript.path[0] == '\0')
        return 1;

    vector   <Token>    tokens;

    PEData peData       ("base.exe");

    Lexer   lexer       (tokens, &userScript.path[0]);
    PrintTokens         (lexer);

    Parser  parser      (tokens);

    Assembler assembler (parser, peData);

    system ("pause");
    return 0;
}
