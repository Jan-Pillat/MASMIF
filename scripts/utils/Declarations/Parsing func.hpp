#ifndef _HPP_Parsing_func_
#define _HPP_Parsing_func_


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

#endif
