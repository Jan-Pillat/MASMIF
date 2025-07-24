#ifndef _HPP_ContentConverter_
#define _HPP_ContentConverter_

#include <string>
#include "../../CppCore/include/StringUtils.hpp"

using std::string;

class ContentConverter
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
    void SkipNonPunctatorChars      ();
    bool IsItCommentaryBegin        ();
    void ConvertCommentary          ();
    bool IsItNumberBegin            ();
    void ConvertNumber              ();
    void SkipChar                   ();
    void FinishConvertedText        ();


public:
//! ---------- PUBLIC METHODS ----------
    string ConvertNumbers               ();
    string ConvertCommentaries          ();
    string ConvertNumbersAndCommentaries();

//! ---------- CONSTRUCTORS ----------
    ContentConverter (const string& content) : content(content) {}
};

#endif
