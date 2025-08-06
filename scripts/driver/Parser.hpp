#ifndef _HPP_Parser_
#define _HPP_Parser_

#include <unordered_map>
#include <algorithm>
#include <windows.h>
#include <string>
#include <vector>

#include "Lexer.hpp"
#include "AsmConverter.hpp"

#include "../utils/Declarations/Token.hpp"
#include "../utils/Declarations/Thunk.hpp"
#include "../utils/Declarations/Merge.hpp"
#include "../utils/Declarations/Declaration.hpp"
#include "../utils/Declarations/Parsing func.hpp"

#include "../utils/CppCore/include/StringUtils.hpp"
#include "../utils/GetString.hpp"

using std::string;
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
    void ParseOwnVar            ();
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

#endif
