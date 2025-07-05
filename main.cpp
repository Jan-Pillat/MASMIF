
#include <iostream>
#include <windows.h>
#include "scripts\PEData.hpp"
#include "scripts\Token.hpp"
#include "scripts\Lexer.hpp"
#include "scripts\Parser.hpp"
#include "scripts\Assembler.hpp"
#include "scripts\MapParser.hpp"
#include "scripts\Injector.hpp"
#include "CppCore\include\ReadableWinAPI.hpp"

using namespace std;


void PrintTokens (Lexer& lexer)
{
    for (size_t i = 0;  i<lexer.tokens.size();  i++)
        cout << "TOKEN " << i << ':' << endl << "  type: " << tokenTypeDescription[lexer.tokens[i].type] << endl << "  content: " << lexer.tokens[i].content << endl << endl;
}

// ========== MAIN ========== \\

int main()
{
    // ---------- USER SCRIPT ----------
    FileSelector userScript;
    userScript.GetPath_OpenFile();

    if (userScript.path[0] == '\0')
        return 1;

    // - BASE -
    PEData peData       ("base.exe");

    if (peData.data.IsEmpty())
    {
        cout << "NO base.exe!" << endl;
        return 1;
    }

    // - - -
    vector  <Token>         tokens;
    vector  <Merge>         merges;
    vector  <Thunk>         thunks;
    vector  <Declaration>   declarations;

    Lexer   lexer       (tokens, &userScript.path[0]);

    Parser  parser      (tokens, merges, thunks, declarations);

    Assembler assembler (thunks, declarations, peData);
    system ("pause");

    // ---------- MAP ----------
    vector   <Token>    mapTokens;

    Lexer   mapLexer    (mapTokens, "test_result.map");
    PrintTokens         (mapLexer);

    vector   <SectionToCopy>    sectionsToCopy;
    vector   <RawDataToCopy>    rawDataToCopy;

    MapParser mapParser (mapTokens, sectionsToCopy, rawDataToCopy);

    cout << "sectionsToCopy.size() = " << sectionsToCopy.size() << endl;
    cout << "rawDataToCopy.size()  = " << rawDataToCopy.size()  << endl;

    // ---------- INJECT ----------
    PEData resultData   ("test_result.exe");

    string     targetPath = "target.exe";
    Injector   injector (targetPath, peData, resultData, sectionsToCopy, rawDataToCopy, merges);


    // ---------- PROGRAM END ----------
    system ("pause");
    return 0;
}
