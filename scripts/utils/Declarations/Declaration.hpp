#ifndef _HPP_Declaration_
#define _HPP_Declaration_

#include <string>
#include <windows.h>

using std::string;

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

    //for: Procedure
    string uses;
    string parameters;
    string locals;
};

struct DeclarationComparison
{
    bool operator () (const Declaration& l,  const Declaration& r)
    {
        return (l.address < r.address);
    }
};

#endif
