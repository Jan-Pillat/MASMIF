#ifndef _HPP_MapParser_
#define _HPP_MapParser_

#include <unordered_map>
#include <algorithm>
#include <windows.h>
#include <vector>
#include "Token.hpp"
#include "StrUniLoadNumber.hpp"
#include "Lexer.hpp"
#include "Declarations.hpp"

using std::vector;

class MapParser
{
public:
    MapParser (vector<Token>& gotTokens,  vector<SectionToCopy>& gotSectionsToCopy,  vector<RawDataToCopy>& gotRawDataToCopy);

private:
    bool GetToken               ();
    bool GetTokenOnlyToLineEnd  ();
    bool ShowNextToken          ();

    void PrepareSectionsToCopyAndMapDeclarations    ();
    void CalculateAndDeclareRawDataToCopy           ();

    vector   <Token>&   tokens;
    size_t   iterator   = 0;
    Token*   gotToken;
    Token*   nextToken;

    vector   <SectionToCopy>&   sectionsToCopy;
    vector   <RawDataToCopy>&   rawDataToCopy;

    vector   <MapDeclaration>   mapDeclarations;
};

#endif
