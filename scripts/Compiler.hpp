#ifndef _HPP_Compiler_
#define _HPP_Compiler_

#include <string>
#include <vector>
#include <windows.h>
#include "Declarations.hpp"
#include "Parser.hpp"
#include "PEData.hpp"
#include "utils/ContentConverter.hpp"
#include "utils/ConvertNumberToHexString.hpp"
#include "../CppCore/include/FileData.hpp"
#include "../CppCore/include/ReadableWinAPI.hpp"

using std::string;
using std::vector;

class Compiler
{

public:

    Compiler (string& gotPath, vector<Thunk>& gotThunks, vector<Declaration>& gotDeclarations, PEData& gotPEData);

private:

    void LoadBeginBase        ();
    void ScanAndDeclareDLLs   ();
    void ScanAndDeclareThunks ();
    void DeclareIncludes      ();
    void SortDeclarations     ();
    void WriteMASMCode        ();
    void ConvertMASMCode      ();
    void SaveToFile           ();

    DWORD RvaToOffset       (DWORD rva);
    char* RvaToPointer      (DWORD rva);
    DWORD VaToOffset        (DWORD va);
    char* VaToPointer       (DWORD va);

    PEData&  baseData;

    vector <string> dllNames;

    DWORD   beginBase   = 0;
    BYTE    pointerSize = 4;    //32-bit pointer
    BYTE    thunkSize   = 6;

    string  MASMcode;
    vector  <Declaration>& declarations;
    bool    IsNextDeclarationGroupable(size_t i);
    DWORD   GetOriginalSectionsSize();

    vector <Thunk>&  thunks;

    string projectPath;
    string masmPath = "C:\\masm32";

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
