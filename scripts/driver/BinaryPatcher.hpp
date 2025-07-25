#ifndef _HPP_BinaryPatcher_
#define _HPP_BinaryPatcher_

#include <algorithm>
#include <windows.h>
#include <string>
#include <vector>

#include "../utils/GetSectionIndex.hpp"
#include "../utils/RvaToOffset.hpp"

#include "../utils/Declarations/Merge.hpp"
#include "../utils/Declarations/Map.hpp"

#include "../utils/CppCore/include/PEData.hpp"
#include "../utils/CppCore/include/FileData.hpp"
#include "../utils/CppCore/include/StringUtils.hpp"

using std::string;
using std::vector;

class BinaryPatcher
{
public:
    BinaryPatcher (string& gotPath,  PEData& gotPEData,  PEData& gotResultData,  vector<SectionToCopy>& gotSectionsToCopy,  vector<RawDataToCopy>& gotRawDataToCopy, vector<Merge>& gotMerges);

private:
    PEData&  base;
    PEData&  result;

    vector   <SectionToCopy>&   sectionsToCopy;
    vector   <RawDataToCopy>&   rawDataToCopy;
    vector   <Merge>&           merges;

    string&  targetPath;

    void    IncludeNewSections      ();
    void    RoundVirtualSize        ();
    void    MergeSections           ();
    void    CorrectImageSize        ();
    void    RewriteRawData          ();
    void    RoundRawDataSize        ();
    void    Inject                  ();
};

#endif
