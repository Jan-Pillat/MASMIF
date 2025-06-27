#ifndef _HPP_Assembler_
#define _HPP_Assembler_

#include <vector>
#include <windows.h>
#include "Declarations.hpp"
#include "Parser.hpp"
#include "../CppCore/include/FileData.hpp"
#include "../CppCore/include/ReadableWinAPI.hpp"

using std::vector;

class Assembler
{

public:

    Assembler (Parser& gotParser);

private:

    void LoadPEHeaders      ();
    void ScanAndDeclareDLLs ();
    void SortDeclarations   ();
    void WriteMASMCode      ();
    void InvokeMASM         ();

    DWORD RvaToOffset       (DWORD rva);

    FileData baseData;

    IMAGE_NT_HEADERS32*   PEHeaders;
    IMAGE_SECTION_HEADER* sectionHeaders;

    vector <string> dllNames;

    DWORD   programBase;

    string  MASMcode;
    vector  <Declaration>& declarations;

    void   ConvertNumberToHexString (string& destination, long long number);
    string ConvertContentNumbers    (string& content);

};


/*
    vector   <Section>&     sections;
    vector   <Segment>&     segments;
    vector   <Variable>&    variables;
    vector   <Procedure>&   procedures;
    vector   <Merge>&       merges;
*/

#endif
