#ifndef _HPP_AsmConverter_
#define _HPP_AsmConverter_

#include <string>
#include "../utils/CppCore/include/StringUtils.hpp"

using std::string;

class DebugLabelDeclarer
{
private:
//! ---------- PRIVATE VARIABLES ----------
    const string& MASMCode;

    const char* begin   = &MASMCode[0];
    const char* end     = begin;
    const char* pointer = begin;

    string converted    = "";

    vector <Token>& tokens;


//! ---------- PRIVATE METHODS ----------
    bool IsContentNotFullyVerified  ();

public:
//! ---------- PUBLIC METHODS ----------
    string DeclareLabels();

//! ---------- CONSTRUCTORS ----------
    DebugLabelDeclarer (const string& MASMCode, const vector <Token>& tokens) : MASMCode(MASMCode), tokens(tokens) {}
};

#endif
