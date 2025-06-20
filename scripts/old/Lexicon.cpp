
// ---------- SECTION KEYWORDS ----------

unordered_map <string, keyword> sectionKeywords =
{
    // -- WHAT CONTAINS --
    {"CODE",                {NULL,   NULL,   0x00000020} },
    {"INITIALIZED_DATA",    {NULL,   NULL,   0x00000040} },
    {"UNINITIALIZED_DATA",  {NULL,   NULL,   0x00000080} },
    // -- PERMITS --
    {"DISCARDABLE",         {NULL,   NULL,   0x02000000} },
    {"NOT_CACHEABLE",       {NULL,   NULL,   0x04000000} },
    {"NOT_PAGEABLE",        {NULL,   NULL,   0x08000000} },
    {"SHAREADBLE",          {NULL,   NULL,   0x10000000} },
    {"EXECUTABLE",          {NULL,   NULL,   0x20000000} },
    {"READABLE",            {NULL,   NULL,   0x40000000} },
    {"WRITABLE",            {NULL,   NULL,   0x80000000} },
};

// ---------- SECTION KEYWORDS ----------

unordered_map <string, keyword> segmentKeywords =
{
    // --  --
    {"ADDRESS",             {NULL,   NULL,   NULL} },
    {"SIZE",                {NULL,   NULL,   NULL} },
};

// ---------- VARIABLE KEYWORDS ----------

unordered_map <string, keyword> variableKeywords =
{
    // --  --
    {"ADDRESS",             {NULL,   NULL,   NULL} },
    {"EQU",                 {NULL,   NULL,   NULL} },
};

// ---------- GENERAL KEYWORDS ----------

unordered_map <string, keyword> generalKeywords =
{
    // --  --
    {"SECTION", {sectionKeywords,    Parse_Section,  NULL} },
    {"SEGMENT", {segmentKeywords,    Parse_Segment,  NULL} },

    // -- VARIABLES --
    //FLOATS
    {"REAL10",  {variableKeywords,   Parse_Variable, {10, FPU, NOT_APPLICABLE} } },
    {"REAL8",   {variableKeywords,   Parse_Variable, { 8, FPU, NOT_APPLICABLE} } },
    {"REAL4",   {variableKeywords,   Parse_Variable, { 4, FPU, NOT_APPLICABLE} } },
    //INTEGERS
    {"OWORD",   {variableKeywords,   Parse_Variable, {16, CPU, UNSIGNED} } },
    {"TBYTE",   {variableKeywords,   Parse_Variable, {10, CPU, UNSIGNED} } },
    {"QWORD",   {variableKeywords,   Parse_Variable, { 8, CPU, UNSIGNED} } },
    {"FWORD",   {variableKeywords,   Parse_Variable, { 6, CPU, UNSIGNED} } },
    {"DWORD",   {variableKeywords,   Parse_Variable, { 4, CPU, UNSIGNED} } },
    {"WORD",    {variableKeywords,   Parse_Variable, { 2, CPU, UNSIGNED} } },
    {"BYTE",    {variableKeywords,   Parse_Variable, { 1, CPU, UNSIGNED} } },
    //SIGNED
    {"SOWORD",  {variableKeywords,   Parse_Variable, {16, CPU, SIGNED} } },
    {"STBYTE",  {variableKeywords,   Parse_Variable, {10, CPU, SIGNED} } },
    {"SQWORD",  {variableKeywords,   Parse_Variable, { 8, CPU, SIGNED} } },
    {"SFWORD",  {variableKeywords,   Parse_Variable, { 6, CPU, SIGNED} } },
    {"SDWORD",  {variableKeywords,   Parse_Variable, { 4, CPU, SIGNED} } },
    {"SWORD",   {variableKeywords,   Parse_Variable, { 2, CPU, SIGNED} } },
    {"SBYTE",   {variableKeywords,   Parse_Variable, { 1, CPU, SIGNED} } },
    //ADDITIONAL
    {"TEXT",    {variableKeywords,   Parse_Variable, NULL} },
};




// ---------- PUNCTAOTRS ----------

unordered_map <char, punctator> punctators =
{
    { ';', {NULL, COMMENTARY, LINE_END_IS_ALSO_TERMINATION,  SAVE_ALL_SPECIALS,                  NONE,                       "semicolon"        }  },
    { '#', {NULL, COMMENTARY, LINE_END_IS_ALSO_TERMINATION,  SAVE_ALL_SPECIALS,                  NONE,                       "hashtag"          }  },
    { '"', {'"',  TEXT,       DONT_ACCEPT_LINE_END,          APPLY_ALL_SPECIALS,                 NONE,                       "quotation mark"   }  },
    { '{', {'}',  CODE,       DONT_ACCEPT_FILE_END,          APPLY_ONLY_OWN_TERMINATE_SPECIAL,   DONT_ACCEPT_ANOTHER_BEGIN,  "curly brace"      }  }
};
