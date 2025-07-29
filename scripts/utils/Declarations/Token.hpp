#ifndef _HPP_Token_
#define _HPP_Token_

    #include <string>
    using std::string;

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
    TYPE_UNDEF,
    TYPE_UNDEFINED=TYPE_UNDEF,

    TYPE_TEXT,
    TYPE_CHARS,
    TYPE_WORD,
    TYPE_NUMBER,
    TYPE_CONTENT,
    TYPE_SPECIAL,

    TYPE_LINEEND,
};

struct Token
{
    string          content;
    enum TokenType  type;
    size_t          line;

    Token ()
    {
        content = "";
        type    = TYPE_UNDEF;
        line    = 0;
    }

    Token (const string& newContent, const enum TokenType newType, const size_t newLine)
    {
        content = newContent;
        type    = newType;
        line    = newLine;
    }

    Token (const char* newContent, const enum TokenType newType, const size_t newLine)
    {
        content = newContent;
        type    = newType;
        line    = newLine;
    }

    Token (const char newContent, const enum TokenType newType, const size_t newLine)
    {
        content = newContent;
        type    = newType;
        line    = newLine;
    }
};

#endif
