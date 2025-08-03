#include "_driver.hpp"
#include <iostream> //debug

using namespace std;
using namespace ConsoleUtils;

// ---------- CONSTRUCTORS ----------

Driver::Driver ()
{

}

Driver::Driver (const string& scriptPath) : scriptPath(scriptPath)
{
    Run();
}

// ---------- START ----------

void Driver::Run ()
{
    try
    {
        LoadBase        ();
        LoadScript      ();
        DoPreprocessing ();
        DoLexing        ();
        DoParsing       ();
        ConvertAndAutoDeclareText();
        GenerateCode    ();
        ConvertCode     ();
        SaveCode        ();
        Build           ();
        LoadMap         ();
        LexMap          ();
        ParseMap        ();
        LoadResult      ();
        LoadTargetPath  ();
        Patch           ();
        GenerateLabels  ();
        SuccessInfo     ();
    }
    catch (const char* msg)
    {
        SetColors(LIGHT_RED,BLACK);
        cout << msg << endl;
        SetColors(WHITE,BLACK);
        system ("pause");
        return;
    }
}

void Driver::Run (const string& scriptPath)
{
    this->scriptPath = scriptPath;
    Run();
}



//!===============================================================================
//!===============================================================================



//! ---------- Load base.exe ----------

void Driver::LoadBase()
{
    cout << "Base.exe loading..." << endl;
    string basePath     = GetProjectPath()+"\\base.exe";

    base.LoadPEFile(&basePath[0]);

    if (base.data.IsEmpty())
    {
        string  msg  = "NO base.exe!  Used path = ";
                msg += &basePath[0];
                msg += "\n";
        throw  msg.c_str();
    }
}

//! ---------- Load script ----------

void Driver::LoadScript()
{
    cout << "Script loading..." << endl;
    script.LoadTextFile(scriptPath);

    if (script.IsEmpty())
    {
        string  msg  = "Load script failed!   ERROR = ";
                msg += script.GetErrorDescribePointer();
                msg += "Used path: ";
                msg += scriptPath;
                msg += "\n";
        throw msg;
    }

    MASMcode = script.GetBeginPointer();
    script.Remove();
}

//! ---------- Preprocessor ----------

void Driver::DoPreprocessing()
{
    cout << "Preprocessing..." << endl;
    MASMcode = Preprocessor().ApplyIncludes(&MASMcode[0]);

    //Save
    string path = GetProjectPath() + "\\Preprocessed.masmif";
    script = MASMcode;
    script.SaveTextFile(path);
}

//! ---------- Lexer ----------

void Driver::DoLexing()
{
    cout << "Lexing..." << endl;
    Lexer   lexer       (tokens, &MASMcode[0]);
    //PrintTokens         (tokens);

    if (tokens.size() == 0)
    {
        throw "  (STOP) Effect: No tokens \n";
    }
}

//! ---------- Parser ----------

void Driver::DoParsing()
{
    cout << "Parsing..." << endl;
    Parser  parser      (tokens, merges, thunks, declarations);

    if (declarations.size() == 0  &&  merges.size() == 0)
    {
        throw "STOP - NO important data \n";
    }
}

//! ---------- Text declaration ----------

void Driver::ConvertAndAutoDeclareText()
{
    cout << "Auto text declaring..." << endl;

    for (size_t i=0; i<declarations.size(); i++)
        if (declarations[i].type == SEGMENT  ||  declarations[i].type == PROCEDURE  ||  declarations[i].type == VARIABLE)
            declarations[i].content = AsmConverter(declarations[i].content).ExchangeAutodeclaredTexts(&textsToDeclare);

    for (const auto& [key, value] : textsToDeclare)
    {
        Declaration newDeclaration;
        newDeclaration.intoNewSection   = true;
        newDeclaration.type             = VARIABLE;
        newDeclaration.content          = key;
        newDeclaration.declaration      = "BYTE";
        newDeclaration.name             = "____TXT_" + to_string(value);
        declarations.push_back (newDeclaration);
    }
}

//! ---------- Generator ----------

void Driver::GenerateCode()
{
    cout << "Code generating..." << endl;
    CodeGenerator codeGenerator (MASMcode, GetProjectPath(), thunks, declarations, base);
}

//! ---------- Converter ----------

void Driver::ConvertCode()
{
    cout << "Time to convert. " << endl;
    system ("pause");
    cout << "Code converting..." << endl;
    MASMcode = AsmConverter(MASMcode).ConvertSyntax();
}

//! ---------- Save script ----------

void Driver::SaveCode()
{
    cout << "  Save To File" << endl;

    string projectPath      = GetProjectPath();
    SetCurrentDirectoryA    (&projectPath[0]);
    string asmResultPath    = projectPath + "\\result.asm";

    FileData fileData       (MASMcode);

    if (fileData.SaveTextFile   (asmResultPath))
        cout << "    Done." << endl;
    else
        throw   "    Can't create ASM file! \n";
}

//! ---------- Builder ----------

void Driver::Build()
{
    cout << "Time to assemble. ";
    system ("pause");
    system ("cls");

    cout << "Assembling..." << endl;
    BuildDriver buildDriver (base);

    string resultPath      = GetProjectPath()+"\\result.obj";
    DWORD attributes = GetFileAttributesA (&resultPath[0]);
    if (attributes == INVALID_FILE_ATTRIBUTES  ||  attributes&FILE_ATTRIBUTE_DIRECTORY)
        throw "Build FAILED!";

    system ("pause");
    system ("cls");
}

//! ---------- Load map ----------

void Driver::LoadMap()
{
    cout << "Map loading..." << endl;
    string mapPath      = GetProjectPath()+"\\result.map";
    cout << "  mapPath = " << mapPath << endl;
    mapData.LoadTextFile(mapPath);

    if (mapData.IsEmpty())
    {
        string  msg  = "Load map FAILED!   ERROR = ";
                msg += mapData.GetErrorDescribePointer();
                msg += "Used path: ";
                msg += mapPath;
        throw msg;
    }
}

//! ---------- Map lexer ----------

void Driver::LexMap()
{
    cout << "Lexing map..." << endl;
    Lexer  mapLexer     (mapTokens, mapData.GetBeginPointer());

    if (mapTokens.size() == 0)
        throw "  (STOP) - Effect: no map tokens \n";
}

//! ---------- Map parser ----------

void Driver::ParseMap()
{
    cout << "Parsing map..." << endl;
    MapParser mapParser (mapTokens, sectionsToCopy, rawDataToCopy, debugLabels);
}

//! ---------- Load result.exe ----------

void Driver::LoadResult()
{
    cout << "Result.exe loading..." << endl;

    string      resultPath  = GetProjectPath()+"\\result.exe";

    result.LoadPEFile(&resultPath[0]);

    if (result.data.IsEmpty())
        throw "  (STOP) - Load result.exe FAILED! \n";
}

//! ---------- Target path ----------

void Driver::LoadTargetPath()
{
    cout << "Target path loading..." << endl;

    FileData    fileWithPath;
    fileWithPath.LoadTextFile(GetProjectPath()+"\\target.path");

    if (fileWithPath.IsEmpty())
        throw "  (STOP) - Incorrect target.path or no file! \n";

    targetPath.assign(fileWithPath.GetBeginPointer());
}

//! ---------- Patcher ----------

void Driver::Patch()
{
    cout << "Injecting..." << endl;
    BinaryPatcher    binaryPatcher    (targetPath, base, result, sectionsToCopy, rawDataToCopy, merges);
}

//! ---------- Label generator ----------

void Driver::GenerateLabels()
{
    cout << "Creating x64dbg labels..." << endl;
    DebugLabelGenerator debugLabelGenerator (targetPath, debugLabels, base);
}

//! ---------- Success ----------

void Driver::SuccessInfo()
{
    SetColors(GREEN,BLACK);
    cout << "Success" << endl;
    SetColors(WHITE,BLACK);
    system ("pause");
}



//!===============================================================================
//!===============================================================================


// ========== CLEAR MEMORY ==========

void Driver::ClearMemory()
{
    script.Remove();
    mapData.Remove();
}

// ========== PRINT DEBUG ==========

void Driver::PrintTokens (vector <Token>& tokens)
{
    for (size_t i = 0;  i<tokens.size();  i++)
        cout << "TOKEN " << i << ':' << endl << "  type: " << tokenTypeDescription[tokens[i].type] << endl << "  content: " << tokens[i].content << endl << endl;
}

// ========== PRINT DEBUG ==========

void Driver::SlowPrintTokens (vector <Token>& tokens)
{
    for (size_t i = 0;  i<tokens.size();  i++)
    {
        cout << "TOKEN " << i << ':' << endl << "  type: " << tokenTypeDescription[tokens[i].type] << endl << "  content: " << tokens[i].content << endl << endl;
        system ("pause>nul");
    }
}
