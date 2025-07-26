#ifndef _HPP_MapParser_
#define _HPP_MapParser_

#include <algorithm>
#include <windows.h>
#include <string>
#include <vector>
#include "../utils/Declarations/Token.hpp"
#include "../utils/Declarations/Map.hpp"
#include "../utils/Declarations/DebugLabel.hpp"
#include "../utils/CppCore/include/StringUtils.hpp"

using std::vector;

class MapParser
{
public:
    MapParser (vector<Token>& gotTokens,  vector<SectionToCopy>& gotSectionsToCopy,  vector<RawDataToCopy>& gotRawDataToCopy,  vector<DebugLabel>& gotDebugLabels);

private:
    bool GetToken               ();
    bool GetTokenOnlyToLineEnd  ();
    bool ShowNextToken          ();

    void FindSectionBegin       ();
    void PrepareSectionsToCopy  ();
    void PrepareMapDeclarations ();
    void ParseMapDeclarations   ();

    vector   <Token>&   tokens;
    size_t   iterator   = 0;
    Token*   gotToken;
    Token*   nextToken;

    vector   <SectionToCopy>&   sectionsToCopy;
    vector   <RawDataToCopy>&   rawDataToCopy;
    vector   <DebugLabel>&      debugLabels;

    vector   <MapDeclaration>   mapDeclarations;
};

#endif
