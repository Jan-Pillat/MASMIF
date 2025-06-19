
/*
    1.Lexer  - podzieli kod na pojedyñcze tokeny.    Jako jeden token s¹ traktowane teksty napisane miêdzy klamrami.
    2.Parser - zbiera dane
    3.TxtAss - tworzy na podstawie danych kod tekstowy assemblera dla MASM32.
*/


#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <fstream>
#include <windows.h>
using namespace std;
#include "scripts\JanStr.cpp"
#include "scripts\JanFile.cpp"
#include "scripts\Token.cpp"
#include "scripts\Parser\_FunctionDeclarations.cpp"
#include "scripts\Lexicon.cpp"
#include "scripts\Lexer.cpp"
#include "scripts\Parser\Section.cpp"
#include "scripts\Parser\Segment.cpp"
#include "scripts\Parser\Variable.cpp"
#include "scripts\Parser\_Parser.cpp"


int main()
{
    jf_LibraryTest ();

//    string path;

//    cout << "Enter file path: " ;
//    cin  >> path;

//    TokenizeFile (path);
//    PrintTokens  ();
//    ParseTokens  ();
//    PrintSections();
//    PrintSegments();

    system ("pause");
    return 0;
}
