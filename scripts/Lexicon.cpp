#include "Parser.hpp"

using namespace std;

// ---------- ASSEMBLY KEYWORDS ----------

unordered_map <string, BYTE> Parser::assemblyKeyword =
{
    // -- WHAT CONTAINS --
    {"NOP",                0x90},
};

// ---------- SECTION KEYWORDS ----------

unordered_map <string, DWORD> Parser::sectionKeywords =
{
    // -- WHAT CONTAINS --
    {"CODE",                0x00000020},
    {"INITIALIZED_DATA",    0x00000040},
    {"UNINITIALIZED_DATA",  0x00000080},
    // -- PERMITS --
    {"DISCARDABLE",         0x02000000},
    {"NOT_CACHEABLE",       0x04000000},
    {"NOT_PAGEABLE",        0x08000000},
    {"SHAREADBLE",          0x10000000},
    {"EXECUTABLE",          0x20000000},
    {"READABLE",            0x40000000},
    {"WRITABLE",            0x80000000},
};

// ---------- GENERAL KEYWORDS ----------

unordered_map <string, Parser::FuncParse> Parser::generalKeywords =
{
    // --  --
    {"SECTION", {ParseSection,  NULL} },
    {"SEGMENT", {ParseSegment,  NULL} },

    // -- VARIABLES --
    //FLOATS
    {"REAL10",  {ParseVariable, (10, FPU, NOT_APPLICABLE) } },
    {"REAL8",   {ParseVariable, ( 8, FPU, NOT_APPLICABLE) } },
    {"REAL4",   {ParseVariable, ( 4, FPU, NOT_APPLICABLE) } },
    //INTEGERS
    {"OWORD",   {ParseVariable, (16, CPU, UNSIGNED) } },
    {"TBYTE",   {ParseVariable, (10, CPU, UNSIGNED) } },
    {"QWORD",   {ParseVariable, ( 8, CPU, UNSIGNED) } },
    {"FWORD",   {ParseVariable, ( 6, CPU, UNSIGNED) } },
    {"DWORD",   {ParseVariable, ( 4, CPU, UNSIGNED) } },
    {"WORD",    {ParseVariable, ( 2, CPU, UNSIGNED) } },
    {"BYTE",    {ParseVariable, ( 1, CPU, UNSIGNED) } },
    //SIGNED
    {"SOWORD",  {ParseVariable, (16, CPU, SIGNED) } },
    {"STBYTE",  {ParseVariable, (10, CPU, SIGNED) } },
    {"SQWORD",  {ParseVariable, ( 8, CPU, SIGNED) } },
    {"SFWORD",  {ParseVariable, ( 6, CPU, SIGNED) } },
    {"SDWORD",  {ParseVariable, ( 4, CPU, SIGNED) } },
    {"SWORD",   {ParseVariable, ( 2, CPU, SIGNED) } },
    {"SBYTE",   {ParseVariable, ( 1, CPU, SIGNED) } },
    //ADDITIONAL
    {"TEXT",    {ParseVariable, NULL} },
};

