#ifndef _HPP_Token_
#define _HPP_Token_

    #include <string>
    using std::string;

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

    Token ()
    {
        content = "";
        type    = TYPE_UNDEF;
    }

    Token (string& newContent, enum TokenType newType)
    {
        content = newContent;
        type    = newType;
    }

    Token (const char* newContent, enum TokenType newType)
    {
        content = newContent;
        type    = newType;
    }
};

#endif
