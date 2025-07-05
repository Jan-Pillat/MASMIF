#ifndef _HPP_Parser_
#define _HPP_Parser_

#include <unordered_map>
#include <algorithm>
#include <windows.h>
#include <vector>
#include "Token.hpp"
#include "StrUniLoadNumber.hpp"
#include "Lexer.hpp"
#include "Declarations.hpp"

using std::vector;
using std::unordered_map;

class Parser
{
    friend class Assembler;

public:
    Parser (vector<Token>& gotTokens, vector<Merge>& gotMerges, vector<Thunk>& gotThunks, vector<Declaration>& gotDeclarations);

private:
    bool GetToken               ();
    bool GetTokenOnlyToLineEnd  ();
    bool ShowNextToken          ();

    bool SetAddress             (Declaration& destination);
    bool SetSize                (Declaration& destination);

    void ParseSection           ();
    void ParseSegment           ();
    void ParseVariable          ();
    void ParseProcedure         ();
    void ParseMerge             ();
    void ParseThunk             ();
    void ParseDeclaration       ();

    vector   <Token>&   tokens;
    size_t   iterator   = 0;
    Token*   gotToken;
    Token*   nextToken;

    struct FuncParse
    {
        void        (Parser::*func)();
        FuncArgs    argument;
    };

    static unordered_map <string, BYTE>        assemblyKeyword;
    static unordered_map <string, DWORD>       sectionKeywords;
    static unordered_map <string, FuncParse>   generalKeywords;

    vector <Declaration>&   declarations;
    vector <Merge>&         merges;
    vector <Thunk>&         thunks;
};


/*
    vector   <Section>      sections;
    vector   <Segment>      segments;
    vector   <Variable>     variables;
    vector   <Procedure>    procedures;
    vector   <Merge>        merges;
*/
#endif
