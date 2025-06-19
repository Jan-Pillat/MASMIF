#ifndef _HPP_Variable_
#define _HPP_Variable_

#include <vector>
#include <string>
using std::vector;
using std::string;

#include "Keyword.hpp"
#include "../Token.hpp"

/*
union VariableValue
{
    long long   cpuVal;
    double      fpuVal;
    char*       txtVal;

    ~VariableValue()
    {
        delete txtVal;
    }
};
*/
//using VariableValue = variant <long long, double, string>; <-- alternatywa pozwalaj¹ca u¿yæ zmienn¹ string.

struct variable
{
    string  name;
    DWORD   address;
    VarType type;
    BYTE    bytes;
    string  value;
};

extern vector <variable> variables;


void Parse_Variable ();

#endif
