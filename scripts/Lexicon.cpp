
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

struct varDesc
{
    VarType type:8;
    BYTE    bytes:8;
    bool    isSigned:8;
};

union keywordValue
{
    varDesc variableDescription;
    DWORD   integer;

    keywordValue (BYTE b, VarType t, bool s)
    {
        variableDescription.bytes  = b;
        variableDescription.type   = t;
        variableDescription.isSigned = s;
    }

    keywordValue (DWORD i)
    {
        integer = i;
    }
};

struct keyword
{
    char         name[0x18];
    keyword*     ownKeywords;
    void         (*parse)();
    keywordValue value;
};

// ---------- SECTION KEYWORDS ----------

keyword sectionKeywords[] =
{
    // -- WHAT CONTAINS --
    {"CODE",                NULL,   NULL,   0x00000020},
    {"INITIALIZED_DATA",    NULL,   NULL,   0x00000040},
    {"UNINITIALIZED_DATA",  NULL,   NULL,   0x00000080},
    // -- PERMITS --
    {"DISCARDABLE",         NULL,   NULL,   0x02000000},
    {"NOT_CACHEABLE",       NULL,   NULL,   0x04000000},
    {"NOT_PAGEABLE",        NULL,   NULL,   0x08000000},
    {"SHAREADBLE",          NULL,   NULL,   0x10000000},
    {"EXECUTABLE",          NULL,   NULL,   0x20000000},
    {"READABLE",            NULL,   NULL,   0x40000000},
    {"WRITABLE",            NULL,   NULL,   0x80000000},
    // -- END --
    {"",                    NULL,   NULL,   NULL},
};

// ---------- SECTION KEYWORDS ----------

keyword segmentKeywords[] =
{
    // --  --
    {"ADDRESS",             NULL,   NULL,   NULL},
    {"SIZE",                NULL,   NULL,   NULL},
    // -- END --
    {"",                    NULL,   NULL,   NULL},
};

// ---------- VARIABLE KEYWORDS ----------

keyword variableKeywords[] =
{
    // --  --
    {"ADDRESS",             NULL,   NULL,   NULL},
    {"EQU",                 NULL,   NULL,   NULL},
    // -- END --
    {"",                    NULL,   NULL,   NULL},
};

// ---------- GENERAL KEYWORDS ----------

keyword generalKeywords[] =
{
    // --  --
    {"SECTION", sectionKeywords,    Parse_Section,  NULL},
    {"SEGMENT", segmentKeywords,    Parse_Segment,  NULL},

    // -- VARIABLES --
    //FLOATS
    {"REAL10",  variableKeywords,   Parse_Variable, {10, FPU, NULL} },
    {"REAL8",   variableKeywords,   Parse_Variable, { 8, FPU, NULL} },
    {"REAL4",   variableKeywords,   Parse_Variable, { 4, FPU, NULL} },
    //INTEGERS
    {"OWORD",   variableKeywords,   Parse_Variable, {16, CPU, false} },
    {"TBYTE",   variableKeywords,   Parse_Variable, {10, CPU, false} },
    {"QWORD",   variableKeywords,   Parse_Variable, { 8, CPU, false} },
    {"FWORD",   variableKeywords,   Parse_Variable, { 6, CPU, false} },
    {"DWORD",   variableKeywords,   Parse_Variable, { 4, CPU, false} },
    {"WORD",    variableKeywords,   Parse_Variable, { 2, CPU, false} },
    {"BYTE",    variableKeywords,   Parse_Variable, { 1, CPU, false} },
    //SIGNED
    {"SOWORD",  variableKeywords,   Parse_Variable, {16, CPU, true} },
    {"STBYTE",  variableKeywords,   Parse_Variable, {10, CPU, true} },
    {"SQWORD",  variableKeywords,   Parse_Variable, { 8, CPU, true} },
    {"SFWORD",  variableKeywords,   Parse_Variable, { 6, CPU, true} },
    {"SDWORD",  variableKeywords,   Parse_Variable, { 4, CPU, true} },
    {"SWORD",   variableKeywords,   Parse_Variable, { 2, CPU, true} },
    {"SBYTE",   variableKeywords,   Parse_Variable, { 1, CPU, true} },
    //ADDITIONAL
    {"TEXT",    variableKeywords,   Parse_Variable, NULL},

    // -- END --
    {"",        NULL,               NULL,           NULL},
};

keyword*   currentKeywords  = generalKeywords;
keyword*   foundKeyword     = NULL;




// ---------- PUNCTAOTR ----------

    enum ReadType
    {
        ACCEPT_LINE_END_AND_FILE_END,   //Save every line end and file end ('\0') is also termination - it will be not incorrect
        DONT_ACCEPT_FILE_END,           //Save line ends, but if you found file end instead own terminate, state that it's ERROR
        DONT_ACCEPT_LINE_END,           //If you found line end ('\r'or'\n') or file end ('\0'), state that it's ERROR
        LINE_END_IS_ALSO_TERMINATION,   //Apart from own termination, line end and file end are also correct termination
        LINE_END_AND_FILE_END_ARE_ALSO_TERMINATION = LINE_END_IS_ALSO_TERMINATION,
    };

    enum Special
    {
        SAVE_ALL_SPECIALS,
        APPLY_ALL_SPECIALS,
        APPLY_ONLY_OWN_TERMINATE_SPECIAL,
    };

    enum SpecialBegins
    {
        NONE,
        IGNORE_ANOTHER_BEGIN,
        DONT_ACCEPT_ANOTHER_BEGIN,
    };

    struct punctator
    {
        char                sign;
        char                termination;
        enum TokenType      type;
        enum ReadType       attitudeTowardsLineEndsAndFileEnd;
        enum Special        attitudeTowardsSpecials;
        enum SpecialBegins  attitudeTowardsAnotherBegin;
        char                name[0x10];
    };

punctator punctators[] =
{
    { ';', NULL, COMMENTARY, LINE_END_IS_ALSO_TERMINATION,  SAVE_ALL_SPECIALS,                  NONE,                       "semicolon"},
    { '#', NULL, COMMENTARY, LINE_END_IS_ALSO_TERMINATION,  SAVE_ALL_SPECIALS,                  NONE,                       "hashtag"},
    { '"', '"',  TEXT,       DONT_ACCEPT_LINE_END,          APPLY_ALL_SPECIALS,                 NONE,                       "quotation mark"},
    { '{', '}',  CODE,       DONT_ACCEPT_FILE_END,          APPLY_ONLY_OWN_TERMINATE_SPECIAL,   DONT_ACCEPT_ANOTHER_BEGIN,  "curly brace"}
};
