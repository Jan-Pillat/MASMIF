#include "Parser.hpp"

using namespace std;

// ---------- ASSEMBLY KEYWORDS ----------

unordered_map <string, BYTE> Parser::assemblyKeyword =
{
    {"NOP",     0x90},
    {"INT3",    0xCC},
    {"PUSHAD",  0x60},
    {"POPAD",   0x61},
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
    // -- CONTAINERS --
    {"SECTION",     {ParseSection,      NULL} },
    {"SEGMENT",     {ParseSegment,      NULL} },
    {"PROC",        {ParseProcedure,    NULL} },
    {"DECLARATION", {ParseDeclaration,  NULL} },

    // -- THUNK --
    {"THUNK",       {ParseThunk,        NULL} },

    // -- OPERATION --
    {"MERGE",       {ParseMerge,        NULL} },

    // -- VARIABLES --
    //FLOATS
    {"REAL10",  {ParseVariable, FuncArgs(10, FPU, NOT_APPLICABLE) } },
    {"REAL8",   {ParseVariable, FuncArgs( 8, FPU, NOT_APPLICABLE) } },
    {"REAL4",   {ParseVariable, FuncArgs( 4, FPU, NOT_APPLICABLE) } },
    //INTEGERS
 // {"OWORD",   {ParseVariable, FuncArgs(16, CPU, UNSIGNED) } },
    {"TBYTE",   {ParseVariable, FuncArgs(10, CPU, UNSIGNED) } },
    {"QWORD",   {ParseVariable, FuncArgs( 8, CPU, UNSIGNED) } },
    {"FWORD",   {ParseVariable, FuncArgs( 6, CPU, UNSIGNED) } },
    {"DWORD",   {ParseVariable, FuncArgs{ 4, CPU, UNSIGNED} } },
    {"WORD",    {ParseVariable, FuncArgs( 2, CPU, UNSIGNED) } },
    {"BYTE",    {ParseVariable, FuncArgs( 1, CPU, UNSIGNED) } },
    //SIGNED
 // {"SOWORD",  {ParseVariable, FuncArgs(16, CPU, SIGNED) } },
    {"STBYTE",  {ParseVariable, FuncArgs(10, CPU, SIGNED) } },
    {"SQWORD",  {ParseVariable, FuncArgs( 8, CPU, SIGNED) } },
    {"SFWORD",  {ParseVariable, FuncArgs( 6, CPU, SIGNED) } },
    {"SDWORD",  {ParseVariable, FuncArgs( 4, CPU, SIGNED) } },
    {"SWORD",   {ParseVariable, FuncArgs( 2, CPU, SIGNED) } },
    {"SBYTE",   {ParseVariable, FuncArgs( 1, CPU, SIGNED) } },
    //ADDITIONAL
//  {"TEXT",    {ParseVariable, NULL} },
};

