#ifndef _HPP_Parser_
#define _HPP_Parser_

#include <unordered_map>
#include <windows.h>
#include <vector>
#include "Token.hpp"
#include "StrUniLoadNumber.hpp"
#include "Lexer.hpp"

using std::vector;
using std::unordered_map;

class Parser
{
public:
    Parser (vector<Token>& gotTokens);

private:
    bool GetToken               ();
    bool GetTokenOnlyToLineEnd  ();
    bool ShowNextToken          ();

    void ParseSection           ();
    void ParseSegment           ();
    void ParseVariable          ();

    vector   <Token>&   tokens;
    size_t   iterator   = 0;
    Token&   gotToken;
    Token&   nextToken;


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

    struct FuncParse
    {
        void        (Parser::*func)();
        FuncArgs    arguments;
    };

    static unordered_map <string, BYTE>        assemblyKeyword;
    static unordered_map <string, DWORD>       sectionKeywords;
    static unordered_map <string, FuncParse>   generalKeywords;



    struct Section
    {
        string  name;
        string  content;
        DWORD   attributes;
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


    vector   <Section>   sections;
    vector   <Segment>   segments;
    vector   <Variable>  variables;

};

#endif
