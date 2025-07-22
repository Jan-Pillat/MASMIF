#include "InjectScript.hpp"

string currentProjectName;

using namespace std;


// ========== PRINT DEBUG ========== \\

void PrintTokens (Lexer& lexer)
{
    for (size_t i = 0;  i<lexer.tokens.size();  i++)
        cout << "TOKEN " << i << ':' << endl << "  type: " << tokenTypeDescription[lexer.tokens[i].type] << endl << "  content: " << lexer.tokens[i].content << endl << endl;
}

// ========== OPTION - INJECT ========== \\

void Menu_InjectProjectScript ()
{
    Menu_InjectScript (GetProjectsPath() + "\\" + currentProjectName + "\\main.masmif");
}

//===============================================================================
//===============================================================================


void Menu_InjectOtherScript ()
{
    cout << "Enter script path: ";

    FileSelector userScript;
    userScript.GetPath_OpenFile();

    if (userScript.path[0] == '\0')
    {
        cout << "NO selected script!" << endl;
        system ("pause");
        return;
    }

    cout << &userScript.path[0] << endl;
    Menu_InjectScript (userScript.path);
}

//===============================================================================
//===============================================================================


void Menu_InjectScript (const string& scriptPath)
{
    system ("cls");

    // ---------- Find project ----------
    string projectPath = GetProjectsPath() + "\\" + currentProjectName + "\\";

    // ---------- Load base.exe ----------
    string basePath     = projectPath+"base.exe";
    PEData peData       (&basePath[0]);

    if (peData.data.IsEmpty())
    {
        cout << "NO base.exe!" << endl << "path = " << &basePath[0] << endl;
        system ("pause");
        return;
    }

    // - - -
    vector  <Token>         tokens;
    vector  <Merge>         merges;
    vector  <Thunk>         thunks;
    vector  <Declaration>   declarations;

    cout << "Lexing..." << endl;
    Lexer   lexer       (tokens, scriptPath);

    if (tokens.size() == 0)
    {
        cout << "STOP - No tokens" << endl;
        system ("pause");
        return;
    }

    cout << "Parsing..." << endl;
    Parser  parser      (tokens, merges, thunks, declarations);

    if (declarations.size() == 0  /*&&  merges.size == 0*/)
    {
        cout << "STOP - NO important data" << endl;
    }

    cout << "Assembling..." << endl;
    Assembler assembler (projectPath, thunks, declarations, peData);
    system ("pause");


    // ---------- MAP ----------
    vector   <Token>    mapTokens;

    cout << "Lexing map..." << endl;
    string mapPath      = projectPath+"result.map";
    Lexer  mapLexer     (mapTokens, &mapPath[0]);

    if (mapTokens.size() == 0)
    {
        cout << "STOP - No map tokens" << endl;
        system ("pause");
        return;
    }

    vector   <SectionToCopy>    sectionsToCopy;
    vector   <RawDataToCopy>    rawDataToCopy;

    cout << "Parsing map..." << endl;
    MapParser mapParser (mapTokens, sectionsToCopy, rawDataToCopy);


    // ---------- INJECT ----------
    string      resultPath  = projectPath+"result.exe";
    PEData      resultData  (&resultPath[0]);

    FileData    fileWithPath;
    fileWithPath.LoadTextFile(projectPath+"target.path");

    if (fileWithPath.IsEmpty())
        cout << "Incorrect target.path!" << endl;
    else
    {
        cout << "Injecting..." << endl;
        string      targetPath  (fileWithPath.GetBeginPointer());
        cout << "targetPath = " << targetPath << endl;
        system ("pause");
        Injector    injector    (targetPath, peData, resultData, sectionsToCopy, rawDataToCopy, merges);
    }


    // ---------- PROGRAM END ----------
    system ("pause");
    return;
}
