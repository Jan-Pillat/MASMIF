#ifndef _HPP_Lexer_
#define _HPP_Lexer_

    #include <string>
    #include <vector>
    #include <iostream>

    #include "../utils/CppCore/include/StringUtils.hpp"
    #include "../utils/CppCore/include/FileData.hpp"
    #include "../utils/Declarations/Token.hpp"

    using std::string;
    using std::vector;

class Lexer
{
public:
    Lexer           (vector<Token>& gotTokens);
    Lexer           (vector<Token>& gotTokens, const char* script);

    void Tokenize   (const char* script);

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
    const char* pointer    = nullptr;
};

#endif // _HPP_Lexer_
