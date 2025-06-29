#ifndef _HPP_PEData_
#define _HPP_PEData_

#include <windows.h>
#include <vector>
#include "Declarations.hpp"
#include "../CppCore/include/FileData.hpp"

using std::vector;

//    IMAGE_NT_HEADERS32      PEHeaders;    <--- I don't use it to shorten references.
class PEData
{
public:
    FileData data;

    PEData ();
    PEData (const char* path);

    IMAGE_DOS_HEADER        DOSHeader;
    //IMAGE_NT_HEADERS32:
    ULONG                   Signature;
    IMAGE_FILE_HEADER       FileHeader;
    IMAGE_OPTIONAL_HEADER32 OptionalHeader;

    vector   <PESection>    sections;

    bool LoadPEFile (const char* path);
};

#endif
