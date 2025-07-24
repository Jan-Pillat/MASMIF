#include "InjectScript.hpp"

string currentProjectName;

using namespace std;


// ========== PRINT DEBUG ========== \\

void PrintTokens (vector <Token>& tokens)
{
    for (size_t i = 0;  i<tokens.size();  i++)
        cout << "TOKEN " << i << ':' << endl << "  type: " << tokenTypeDescription[tokens[i].type] << endl << "  content: " << tokens[i].content << endl << endl;
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
    PrintTokens         (tokens);

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

    cout << "Compiling..." << endl;
    Compiler compiler (projectPath, thunks, declarations, peData);

    cout << "Time to assemble. ";
    system ("pause");
    system ("cls");

    cout << "Assembling..." << endl;
    Assembler assembler (projectPath, peData);
    system ("pause");
    system ("cls");


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
    {
        cout << "Incorrect target.path!" << endl;
        system ("pause");
        return;
    }

    string      targetPath  (fileWithPath.GetBeginPointer());

    cout << "Injecting..." << endl;
    Injector    injector    (targetPath, peData, resultData, sectionsToCopy, rawDataToCopy, merges);


    cout << "Creating dbg labels..." << endl;
    DbgLabeler dbgLabeler (targetPath, declarations, peData);


    // ---------- PROGRAM END ----------
    system ("pause");
    return;
}
