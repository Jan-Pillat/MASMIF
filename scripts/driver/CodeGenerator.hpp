#ifndef _HPP_CodeGenerator_
#define _HPP_CodeGenerator_

#include <string>
#include <vector>
#include <windows.h>

#include "../Globals.hpp"
#include "../utils/GetPath.hpp"

#include "../utils/CppCore/include/PEData.hpp"
#include "../utils/CppCore/include/FileData.hpp"
#include "../utils/CppCore/include/StringUtils.hpp"
#include "../utils/CppCore/include/ReadableWinAPI.hpp"

#include "../utils/Declarations/Thunk.hpp"
#include "../utils/Declarations/Declaration.hpp"

using std::string;
using std::vector;

class CodeGenerator
{

public:

    CodeGenerator (string& gotCode, string gotPath, vector<Thunk>& gotThunks, vector<Declaration>& gotDeclarations, PEData& gotPEData);

private:

//! ---------- METHODS ----------

    void LoadBeginBase        ();
    void ScanAndDeclareDLLs   ();
    void ScanAndDeclareThunks ();
    void DeclareIncludes      ();
    void SortDeclarations     ();
    void WriteMASMCode        ();

    DWORD RvaToOffset       (DWORD rva);
    char* RvaToPointer      (DWORD rva);
    DWORD VaToOffset        (DWORD va);
    char* VaToPointer       (DWORD va);

//! ---------- VARIABLES ----------

    PEData&  baseData;

    vector <string> dllNames;

    DWORD   beginBase   = 0;
    BYTE    pointerSize = 4;    //32-bit pointer
    BYTE    thunkSize   = 6;

    vector  <Declaration>& declarations;
    bool    IsNextDeclarationGroupable(size_t i);
    DWORD   GetOriginalSectionsSize();

    vector <Thunk>&  thunks;

    string  currentDirectory;
    string  projectPath;
    string  masmPath = "C:\\masm32";

    string& MASMcode;
    string  MASMcode_Publications;
    string  MASMcode_Declarations;
    string  MASMcode_DeclarationSegments;
    string  MASMcode_Includes;
    string  MASMcode_NewCode;
    string  MASMcode_NewData;
    string  MASMcode_NewRest;

    bool    autoInclude = false;
};

#endif
