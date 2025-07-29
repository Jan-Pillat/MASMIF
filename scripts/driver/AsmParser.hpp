#ifndef _HPP_AsmParser_
#define _HPP_AsmParser_

#include <algorithm>
#include <windows.h>
#include <string>
#include <vector>
#include <unordered_map>
#include "Lexer.hpp"
#include "Lexicon.hpp"
#include "../utils/Declarations/Token.hpp"
#include "../utils/Declarations/Declaration.hpp"
#include "../utils/CppCore/include/StringUtils.hpp"

using std::string;
using std::vector;
using std::unordered_map;

class AsmParser
{
public:
    AsmParser (unordered_map<string, size_t>& gotTextsToDeclare, string& gotContent);

private:
    void FindAndDeclareTexts	();

    string& content;
    unordered_map   <string, size_t>&  textsToDeclare;

    bool GetToken               ();
    bool GetTokenOnlyToLineEnd  ();
    bool ShowNextToken          ();

    vector   <Token>    tokens;
    size_t   iterator   = 0;
    Token*   gotToken;
    Token*   nextToken;
};

#endif
