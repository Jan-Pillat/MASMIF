#ifndef _HPP_Assembler_
#define _HPP_Assembler_

#include <string>
#include <vector>
#include <windows.h>
#include "Declarations.hpp"
#include "Parser.hpp"
#include "PEData.hpp"
#include "../CppCore/include/FileData.hpp"
#include "../CppCore/include/ReadableWinAPI.hpp"
#include "utils/ConvertNumberToHexString.hpp"

using std::string;
using std::vector;

class Assembler
{

public:

    Assembler (string& gotPath, vector<Thunk>& gotThunks, vector<Declaration>& gotDeclarations, PEData& gotPEData);

private:

    void LoadBeginBase        ();
    void ScanAndDeclareDLLs   ();
    void ScanAndDeclareThunks ();
    void DeclareIncludes      ();
    void SortDeclarations     ();
    void WriteMASMCode        ();
    void InvokeMASM           ();

    DWORD RvaToOffset       (DWORD rva);
    char* RvaToPointer      (DWORD rva);
    DWORD VaToOffset        (DWORD va);
    char* VaToPointer       (DWORD va);

    PEData&  baseData;

    vector <string> dllNames;

    DWORD   beginBase   = 0;
    BYTE    pointerSize = 4;
    BYTE    thunkSize   = 6;

    string  MASMcode;
    vector  <Declaration>& declarations;
    bool    IsNextDeclarationGroupable(size_t i);
    DWORD   GetOriginalSectionsSize();

    vector <Thunk>&  thunks;

    string ConvertContentNumbers    (string& content);

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


/*
    vector   <Section>&     sections;
    vector   <Segment>&     segments;
    vector   <Variable>&    variables;
    vector   <Procedure>&   procedures;
    vector   <Merge>&       merges;
*/

#endif
