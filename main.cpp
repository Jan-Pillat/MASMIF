
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
#include "CppCore/include/ReadableWinAPI.hpp"
#include "CppCore/include/FileData.hpp"
using namespace std;
#include "scripts\Lexer.hpp"


int main()
{
    FileSelector source;

    cout << "Enter file path: ";
    source.GetPath_OpenFile();
    cout << source.path << endl;

    TokenizeFile (source.path);
    PrintTokens  ();
//    ParseTokens  ();
//    PrintSections();
//    PrintSegments();

    system ("pause");
    return 0;
}
