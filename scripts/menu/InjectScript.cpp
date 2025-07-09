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

void Menu_InjectScript ()
{
    // ---------- USER SCRIPT ----------
    FileSelector userScript;
    userScript.GetPath_OpenFile();

    if (userScript.path[0] == '\0')
    {
        cout << "NO selected script!" << endl;
        system ("pause");
        return;
    }

    // ---------- Find project ----------
    // Get path to "documents"
    string projectPath;
    projectPath.resize (MAX_PATH);
    HRESULT hr = SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, 0, &projectPath[0]);

    //Check is it correct
    if (!SUCCEEDED(hr))
    {
        cout << "Can't find folder \"documents\"!" << endl;
        system ("pause");
        return;
    }
    projectPath.resize (strlen(&projectPath[0]));

    projectPath += "\\MASMIF-projects\\" + currentProjectName + "\\";

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

    Lexer   lexer       (tokens, &userScript.path[0]);

    Parser  parser      (tokens, merges, thunks, declarations);

    Assembler assembler (projectPath, thunks, declarations, peData);
    system ("pause");

    // ---------- MAP ----------
    vector   <Token>    mapTokens;

    string mapPath      = projectPath+"result.map";
    Lexer   mapLexer    (mapTokens, &mapPath[0]);
    PrintTokens         (mapLexer);

    vector   <SectionToCopy>    sectionsToCopy;
    vector   <RawDataToCopy>    rawDataToCopy;

    MapParser mapParser (mapTokens, sectionsToCopy, rawDataToCopy);

    cout << "sectionsToCopy.size() = " << sectionsToCopy.size() << endl;
    cout << "rawDataToCopy.size()  = " << rawDataToCopy.size()  << endl;

    // ---------- INJECT ----------
    string      resultPath  = projectPath+"result.exe";
    PEData      resultData  (&resultPath[0]);

    FileData    fileWithPath;
    fileWithPath.LoadTextFile(projectPath+"target.path");

    if (fileWithPath.IsEmpty())
        cout << "Incorrect target.path!" << endl;
    else
    {
        string      targetPath  (fileWithPath.GetBeginPointer());
        cout << "targetPath = " << targetPath << endl;
        system ("pause");
        Injector    injector    (targetPath, peData, resultData, sectionsToCopy, rawDataToCopy, merges);
    }


    // ---------- PROGRAM END ----------
    system ("pause");
    return;
}
