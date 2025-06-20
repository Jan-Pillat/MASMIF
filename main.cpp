
#include <iostream>
#include <windows.h>
#include "scripts\Lexer.hpp"
#include "CppCore/include/ReadableWinAPI.hpp"

using namespace std;


int main()
{
    FileSelector selector;
    selector.GetPath_OpenFile();

    Lexer lexer;
    lexer.Tokenize(selector.path);
    lexer.PrintTokens();

    system ("pause");
    return 0;
}
