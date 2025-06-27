#ifndef _HPP_Declarations_
#define _HPP_Declarations_

    #include <windows.h>
    #include <string>
    using std::string;


// ======================== PARSING FUNC DECLARATION ========================

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

    keywordValue (BYTE b, VarType t, VarSign s)
    {
        attributes.bytes    = b;
        attributes.type     = t;
        attributes.sign     = s;
    }

    keywordValue (DWORD i)
    {
        integer = i;
    }
};

// ======================== MERGE DECLARATION ========================

struct Merge
{
    string  first;
    string  second;
};

// ======================== ELEMENT DECLARATION ========================

static constexpr char declarationTypeDescription[0x8][0x10] =
{
    "UNDEFINED",
    "SECTION",
    "SEGMENT",
    "VARIABLE",
    "PROCEDURE"
};

enum DeclarationType
{
    UNDEF,
    UNDEFINED=UNDEF,
    SECTION,
    SEGMENT,
    VARIABLE,
    PROCEDURE,
    DLL
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

#endif

// ---------- OLD, MORE COMPLICATED ----------
/*
#ifndef _HPP_Declarations_
#define _HPP_Declarations_

struct Section
{
    string  name;
    string  content;
    DWORD   attributes;
    DWORD   size;
    bool    fixSize;
};

struct Segment
{
    string  name;
    string  content;

    DWORD   address;
    DWORD   size;

    bool    until;
    BYTE    untilBinNum;
};

struct Variable
{
    string  name;
    string  content;

    DWORD   address;
    DWORD   size;
};

struct Procedure
{
    string  name;
    string  content;

    DWORD   address;
    DWORD   size;
};

struct Merge
{
    string  first;
    string  second;
};

union Declaration
{
    Section     section;
    Segment     segment;
    Variable    variable;
    Procedure   procedure;
    Merge       merges;
};

enum DeclarationType
{
    SECTION,
    SEGMENT,
    VARIABLE,
    PROCEDURE,
    MERGE
}

#endif
*/
