#ifndef _HPP_Token_
#define _HPP_Token_

#include "StrUniLoadNumber.hpp"
#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
using std::string;
using std::vector;


    enum TokenType
    {
        NOONE = -1,
        UNDEF,

        PUNCTATOR,
        KEYWORD,

        IDENTIFIER,
        NUMBER,
        TEXT,
        CODE,

        COMMENTARY,

        LINE_END
    };

    char tokenTypeName[][0x10] =
    {
        { "UNDEF" },
        { "PUNCTATOR" },
        { "KEYWORD" },
        { "IDENTIFIER" },
        { "NUMBER" },
        { "TEXT" },
        { "CODE" },
        { "COMMENTARY" },
        { "LINE_END" },
    };




struct st_token
{
    string content;
    enum TokenType type;
};

// ---------- VARIABLES ----------
extern vector <st_token> tokens;
extern int tokens_i;

extern st_token* gotToken;
extern st_token* nextToken;

// ---------- FUNCTIONS ----------
bool GetToken ();
bool ShowNextToken ();
bool LoadNumFromTokens (string beginToken, string finalToken, DWORD* destination);


#endif // _HPP_Token_
