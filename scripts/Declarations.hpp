#ifndef _HPP_Declarations_
#define _HPP_Declarations_

    #include <windows.h>
    #include <string>
    #include "../CppCore/include/BinaryData.hpp"
    using std::string;


// ======================== PARSING FUNC DECLARATIONS ========================

enum VarType
{
    NON,
    CPU,
    FPU,
    TXT,
};

enum VarSign
{
    NOT_APPLICABLE,
    UNSIGNED,
    SIGNED,
};

struct VariableAttributes
{
    VarType type:8;
    VarSign sign:8;
    BYTE    bytes:8;
};

union FuncArgs
{
    long long           integer;
    VariableAttributes  attributes;

    FuncArgs (BYTE b, VarType t, VarSign s)
    {
        attributes.bytes    = b;
        attributes.type     = t;
        attributes.sign     = s;
    }

    FuncArgs (DWORD i)
    {
        integer = i;
    }

    FuncArgs ()
    {
        integer = 0;
    }
};

// ======================== MERGE DECLARATION ========================

struct Merge
{
    string  first;
    string  second;
};

// ======================== THUNK DECLARATION ========================

struct Thunk
{
    DWORD   count       = 0;
    DWORD   address     = 0;
};

// ======================== DECLARATION DECLARATIONS ========================

static constexpr char declarationTypeDescription[0x8][0x10] =
{
    "UNDEFINED",
    "SECTION",
    "SEGMENT",
    "VARIABLE",
    "PROCEDURE",
    "DLL",
    "THUNK",
    "DECLARATION",
};

enum DeclarationType
{
    UNDEF,
    UNDEFINED=UNDEF,
    SECTION,
    SEGMENT,
    VARIABLE,
    PROCEDURE,
    DLL,
    THUNK,
    DECLARATION,
};

struct Declaration
{
    DeclarationType type;

    //for: Section, Segment, Variable, Procedure
    string  name;
    string  content;
    DWORD   size        = 0;

    //for: Segment, Variable, Procedure
    DWORD   address     = 0;
    bool    intoNewSection= false;

    //for: Section
    DWORD   attributes  = 0;
    bool    fixSize     = false;

    //for: Segment
    bool    until       = false;
    BYTE    untilBinNum = 0;

    //for: Variable
    string declaration;
//    VariableAttributes variableAttributes = {NON, NOT_APPLICABLE, 0};
};

struct DeclarationComparison
{
    operator () (const Declaration& l,  const Declaration& r)
    {
        if (l.address <= r.address)
            return true;
        else
            return false;
    }
};

// ======================== PE DECLARATIONS ========================

struct PESection
{
    IMAGE_SECTION_HEADER    header;
    BinaryData              rawData;

    PESection (IMAGE_SECTION_HEADER* gotHeader, char* gotRawData)
    {
        header = *gotHeader;
        rawData.SetData(gotRawData, header.SizeOfRawData);
    }

    PESection ()
    {

    }
};

// ======================== MAP DECLARATIONS ========================

struct SectionToCopy
{
    DWORD   size;
    string  name;
};

struct RawDataToCopy
{
    DWORD   virtualAddress;
    DWORD   size;
};

struct MapDeclaration
{
    DWORD   virtualAddress;
    string  name;
};



#endif
