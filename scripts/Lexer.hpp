#ifndef _HPP_Lexer_
#define _HPP_Lexer_

    #include <string>
    #include <vector>
    #include <iostream>

    #include "../CppCore/include/StringUtils.hpp"
    #include "../CppCore/include/FileData.hpp"
    #include "StrUniLoadNumber.hpp"

    using std::string;
    using std::vector;

class Lexer
{
public:
    void Tokenize    (char* filePath);
    void PrintTokens ();

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

    static constexpr char tokenTypeDescription[0x10][0x10] =
    {
        "UNDEFINED",

        "TEXT",
        "CHARS",
        "WORD",
        "NUMBER",
        "CONTENT",
        "SPECIAL",

        "LINE END",
    };

    enum TokenType
    {
        UNDEF,
        UNDEFINED=UNDEF,

        TEXT,
        CHARS,
        WORD,
        NUMBER,
        CONTENT,
        SPECIAL,

        LINEEND,
    };

    struct Token
    {
        string      content;
        TokenType   type;

        Token ()
        {
            content = "";
            type    = UNDEF;
        }

        Token (string& newContent, TokenType newType)
        {
            content = newContent;
            type    = newType;
        }

        Token (const char* newContent, TokenType newType)
        {
            content = newContent;
            type    = newType;
        }
    };

    vector   <Token> tokens;
    FileData data;
    char*    pointer = nullptr;
};

#endif // _HPP_Lexer_
