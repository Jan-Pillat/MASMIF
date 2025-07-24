#ifndef _HPP_Lexer_
#define _HPP_Lexer_

    #include <string>
    #include <vector>
    #include <iostream>

    #include "../CppCore/include/StringUtils.hpp"
    #include "../CppCore/include/FileData.hpp"
    #include "utils/StrUniLoadNumber.hpp"
    #include "Token.hpp"

    using std::string;
    using std::vector;

class Lexer
{
public:
    Lexer           (vector<Token>& gotTokens);
    Lexer           (vector<Token>& gotTokens, string& script);

    void Tokenize   (string& script);

    vector   <Token>&   tokens;

private:
    void LexText    ();
    void LexChars   ();
    void LexWord    ();
    void LexNumber  ();
    void LexContent ();
    void LexSpecial ();
    void LexComment ();
    void LexLineEnd ();
    void LexLineEnd (char* content);

    void SkipComment();
    void Include    ();

    bool IsWordBegin        ();
    bool IsNumberBegin      ();
    bool IsIncludingBegin   ();
    bool IsTextBegin        ();
    bool IsCharBegin        ();
    bool IsContentBegin     ();
    bool IsSpecialBegin     ();
    bool IsSpecialNumBegin  ();
    bool IsCommentBegin     ();
    bool IsLineEnd          ();

    void SkipBlanks ();

// ---------- VARIABLES ----------
    char*    pointer    = nullptr;
};

#endif // _HPP_Lexer_
