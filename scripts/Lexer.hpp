#ifndef _HPP_Lexer_
#define _HPP_Lexer_

    #include <string>
    #include <vector>
    #include <iostream>

    #include "../CppCore/include/StringUtils.hpp"
    #include "../CppCore/include/FileData.hpp"
    #include "StrUniLoadNumber.hpp"
    #include "Token.hpp"

    using std::string;
    using std::vector;

class Lexer
{
public:
    Lexer           (vector<Token>& gotTokens);
    Lexer           (vector<Token>& gotTokens, char* filePath);

    void Tokenize   (char* filePath);

    vector   <Token>&   tokens;

private:
    void LexText    ();
    void LexChars   ();
    void LexWord    ();
    void LexNumber  ();
    void LexContent ();
    void LexSpecial ();
    void SkipComment();
    void LexLineEnd ();

    bool IsTextBegin();
    bool IsCharBegin();
    bool IsWordBegin();
    bool IsNumberBegin();
    bool IsContentBegin();
    bool IsSpecialBegin();
    bool IsSpecialNumBegin();
    bool IsCommentBegin();
    bool IsLineEnd();

    void SkipBlanks ();

// ---------- VARIABLES ----------
    FileData data;
//  vector   <Token>    tokens;
    char*    pointer    = nullptr;
};

#endif // _HPP_Lexer_
