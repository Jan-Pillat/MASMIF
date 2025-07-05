#ifndef _HPP_Injector_
#define _HPP_Injector_

#include <algorithm>
#include <windows.h>
#include <vector>
#include "PEData.hpp"
#include "StrUniLoadNumber.hpp"
#include "Lexer.hpp"
#include "Declarations.hpp"

using std::vector;

class Injector
{
public:
    Injector (string& gotPath,  PEData& gotPEData,  PEData& gotResultData,  vector<SectionToCopy>& gotSectionsToCopy,  vector<RawDataToCopy>& gotRawDataToCopy, vector<Merge>& gotMerges);

private:
    PEData&  base;
    PEData&  result;

    vector   <SectionToCopy>&   sectionsToCopy;
    vector   <RawDataToCopy>&   rawDataToCopy;
    vector   <Merge>&           merges;

    string&  targetPath;

    void    IncludeNewSections      ();
    void    MergeSections           ();
    void    CorrectVirtualSize      ();
    void    RewriteRawData          ();
    void    CorrectRawDataSize      ();
    void    Inject                  ();

    //int   GetSectionIndex   (DWORD rva);
    //DWORD RvaToOffset       (DWORD rva);
};

#endif
