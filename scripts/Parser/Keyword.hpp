#ifndef _HPP_Keyword_
#define _HPP_Keyword_

#include <windows.h>
#include <unordered_map>
using std::unordered_map;

enum KeywordType
{
    UNDEFINED,
    SECTION,
    SEGMENT,
    VARIABLE
};

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

struct varDesc
{
    VarSign isSigned:8;
    VarType type:8;
    BYTE    bytes:8;
};

union keywordValue
{
    varDesc variableDescription;
    DWORD   integer;

    keywordValue (BYTE b, VarType t, VarSign s)
    {
        variableDescription.bytes    = b;
        variableDescription.type     = t;
        variableDescription.isSigned = s;
    }

    keywordValue (DWORD i)
    {
        integer = i;
    }
};

struct keyword
{
    unordered_map <string, keyword>*     ownKeywords;

    void         (*parse)();
    keywordValue value;
};

#endif
