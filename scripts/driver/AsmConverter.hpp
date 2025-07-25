#ifndef _HPP_AsmConverter_
#define _HPP_AsmConverter_

#include <string>
#include "../utils/CppCore/include/StringUtils.hpp"

using std::string;

class AsmConverter
{
private:
//! ---------- PRIVATE VARIABLES ----------
    const string& content;

    const char* begin   = &content[0];
    const char* end     = begin;
    const char* pointer = begin;

    bool  isHex         = false;
    bool  isBin         = false;

    string converted    = "";


//! ---------- PRIVATE METHODS ----------
    bool IsContentNotFullyVerified  ();
    void SkipNotImportantChars      ();
    bool IsItCharsBegin             ();
    void SkipChars                  ();
    bool IsItTextBegin              ();
    void SkipTextButAddNullChar     ();
    bool IsItCommentaryBegin        ();
    void ConvertCommentary          ();
    bool IsItNumberBegin            ();
    void ConvertNumber              ();
    void SkipChar                   ();
    void FinishConvertedText        ();

    void SkipContainedChars         (const char borderChar);

public:
//! ---------- PUBLIC METHODS ----------
    string ConvertScript();

//! ---------- CONSTRUCTORS ----------
    AsmConverter (const string& content) : content(content) {}
};

#endif
