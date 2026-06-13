#ifndef _HPP_AsmConverter_
#define _HPP_AsmConverter_

#include <string>
#include <algorithm>
#include <unordered_map>
#include "Lexicon.hpp"
#include "../utils/CppCore/include/StringUtils.hpp"

using std::string;
using std::unordered_map;

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
    string gotWord      = "";

    unordered_map<string, size_t>* textsToDeclare;


//! ---------- PRIVATE METHODS ----------
    bool IsContentNotFullyVerified  ();
    void SkipNotImportantChars      ();
    bool IsItCharsBegin             ();
    void SkipChars                  ();
    bool IsItTextBegin              ();
    void ConvertChars               ();
    void ConvertTextToCharsAddNullChar();
    bool IsItCommentaryBegin        ();
    void ConvertCommentary          ();
    bool IsItNumberBegin            ();
    void ConvertNumber              ();
    void SkipChar                   ();
    void FinishConvertedText        ();

    void SkipText                   ();
    void SkipBlanks                 ();
    void SkipNumber                 ();
    bool IsItLineEnd                ();
    bool IsItLineOrContentEnd       ();
    bool IsItWordBegin              ();
    bool IsItWordInside             ();
    void SkipWord                   ();
    bool TryToGetWord               ();
    bool CheckIsItCommandOrDeclaration();
    bool GetWordAndCheckIsItCommandOrDeclaration();
    bool IsItKeywordToAvoid         ();
    bool DeclareText                ();

    void SkipContainedChars         (const char borderChar);
    void SkipLine                   ();

    bool IsItColon                  ();
    void ConvertColonLabelToLabelDirective();

public:
//! ---------- PUBLIC METHODS ----------
    string ExchangeAutodeclaredTexts(unordered_map<string, size_t>* gotTextsToDeclare);
    string ConvertSyntax();
    string ConvertLabels();

//! ---------- CONSTRUCTORS ----------
    AsmConverter (const string& content) : content(content) {}
};

#endif
