#include "Lexer.hpp"
using namespace std;


//======================================================
//======================================================

void Lexer::Tokenize (char* filePath)
{
    data.LoadTextFile(filePath);

    if (data.IsEmpty())
    {
        cout << "No data!" << endl;
        return;
    }

    pointer = data.GetBeginPointer();

    while (*pointer != '\0')
    {
        SkipBlanks();

        if (IsTextBegin     ()) LexText     (); else
        if (IsCharBegin     ()) LexChars    (); else
        if (IsWordBegin     ()) LexWord     (); else
        if (IsNumberBegin   ()) LexNumber   (); else
        if (IsContentBegin  ()) LexContent  (); else
        if (IsCommentBegin  ()) SkipComment (); else
        if (IsSpecialBegin  ()) LexSpecial  (); else
        if (IsLineEnd       ()) LexLineEnd  (); else pointer++;
    }

}

//======================================================
//======================================================

void Lexer::SkipBlanks ()
{
    while (IsBlank(*pointer))   pointer++;
}

//======================================================
//======================================================

bool Lexer::IsTextBegin    ()
{
    return (*pointer=='"');
}
bool Lexer::IsCharBegin    ()
{
    return (*pointer=='\'');
}
bool Lexer::IsWordBegin    ()
{
    return (IsAlpha(*pointer) || *pointer=='_');
}
bool Lexer::IsNumberBegin  ()
{
    return (IsDigit(*pointer) || IsSpecialNumBegin());
}
bool Lexer::IsContentBegin ()
{
    return (*pointer=='{');
}
bool Lexer::IsSpecialBegin ()
{
    return IsPunct(*pointer);
}
bool Lexer::IsCommentBegin ()
{
    return (*pointer==';' || *pointer=='#');
}
bool Lexer::IsSpecialNumBegin ()
{
    return (*pointer=='$' || *pointer=='%');
}
bool Lexer::IsLineEnd ()
{
    return (*pointer=='\r' || *pointer=='\n');
}

//======================================================
//======================================================

void Lexer::LexText    ()
{
    Token newToken;

    char* begin = ++pointer;

    //Find text termination
    while (true)
    {
        while (*pointer!='\0' && *pointer!='"' && *pointer!='\r' && *pointer!='\n' && *pointer!='\\')
            pointer++;

        if (*pointer=='\\')
        {
            newToken.content.append(begin, pointer-begin);

            pointer++;
            if (*pointer == 'a')    newToken.content += '\a'; else
            if (*pointer == 'b')    newToken.content += '\b'; else
            if (*pointer == 'f')    newToken.content += '\f'; else
            if (*pointer == 'n')    newToken.content += '\n'; else
            if (*pointer == 'r')    newToken.content += '\r'; else
            if (*pointer == 't')    newToken.content += '\t'; else
            if (*pointer == 'v')    newToken.content += '\v'; else
            if (*pointer == '0')    newToken.content += '\0'; else
            if (*pointer == '"')    newToken.content += '"';  else
            if (*pointer == '\\')   newToken.content += '\\';
            else {   newToken.content += '\\';  newToken.content += *pointer;  }

            pointer++;
            begin = pointer;
        }
        else
        {
            break;
        }
    }

    char* end = pointer++;


    newToken.content.append(begin, end-begin);
    newToken.type = TEXT;
    tokens.push_back (newToken);
}
//------------------------------------------------------
void Lexer::LexChars    ()
{
    Token newToken;

    char* begin = ++pointer;

    //Find text termination
    while (true)
    {
        while (*pointer!='\0' && *pointer!='\'' && *pointer!='\r' && *pointer!='\n' && *pointer!='\\')
            pointer++;

        if (*pointer=='\\')
        {
            newToken.content.append(begin, pointer-begin);

            pointer++;
            if (*pointer == 'a')    newToken.content += '\a'; else
            if (*pointer == 'b')    newToken.content += '\b'; else
            if (*pointer == 'f')    newToken.content += '\f'; else
            if (*pointer == 'n')    newToken.content += '\n'; else
            if (*pointer == 'r')    newToken.content += '\r'; else
            if (*pointer == 't')    newToken.content += '\t'; else
            if (*pointer == 'v')    newToken.content += '\v'; else
            if (*pointer == '0')    newToken.content += '\0'; else
            if (*pointer == '\'')   newToken.content += '\''; else
            if (*pointer == '\\')   newToken.content += '\\';
            else {   newToken.content += '\\';  newToken.content += *pointer;  }

            pointer++;
            begin = pointer;
        }
        else
        {
            break;
        }
    }

    char* end = pointer++;


    newToken.content.append(begin, end-begin);
    newToken.type = CHARS;
    tokens.push_back (newToken);
}
//------------------------------------------------------
void Lexer::LexWord    ()
{
    Token newToken;

    char* begin = pointer;

    //Find text termination
    while (IsAlphabetic(*pointer) || *pointer=='_')
        pointer++;

    char* end = pointer;


    newToken.content.assign(begin, end-begin);
    newToken.type = WORD;
    tokens.push_back (newToken);
}
//------------------------------------------------------
void Lexer::LexNumber  ()
{
    Token newToken;
    bool  itsFloat = false;
    char* begin = pointer;      //begin of data to save

    if (IsSpecialNumBegin())
        pointer++;

    while (true)
    {
        while (IsAlOrNum(*pointer))
            pointer++;

        if (*pointer=='.')
        {
            itsFloat=true;
            pointer++;
        }
        else
        {
            break;
        }
    }

    char* end = pointer;

    string gotTxt (begin, end-begin);

    if (!itsFloat)
    {
        int    gotNum = StrGetNum<int>(&gotTxt[0]);

        static const char BinNumToChar[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

        gotTxt  = "0";
        gotTxt += BinNumToChar[(gotNum&0xF0000000)>>28];
        gotTxt += BinNumToChar[(gotNum&0x0F000000)>>24];
        gotTxt += BinNumToChar[(gotNum&0x00F00000)>>20];
        gotTxt += BinNumToChar[(gotNum&0x000F0000)>>16];
        gotTxt += BinNumToChar[(gotNum&0x0000F000)>>12];
        gotTxt += BinNumToChar[(gotNum&0x00000F00)>> 8];
        gotTxt += BinNumToChar[(gotNum&0x000000F0)>> 4];
        gotTxt += BinNumToChar[(gotNum&0x0000000F)>> 0];
        gotTxt += "h";
    }


    newToken.content = gotTxt;
    newToken.type = NUMBER;
    tokens.push_back (newToken);

}
//------------------------------------------------------
void Lexer::LexContent ()
{
    Token newToken;
    pointer++;

    char* begin = pointer;

    //Find text termination
    while(true)
    {
        while (*pointer!='\0' && *pointer!='}')
            pointer++;

        if (*(pointer-1)=='\\')
        {
            newToken.content.append(begin, pointer-1-begin);
            newToken.content += '}';
            pointer++;
            begin = pointer;
        }
        else
        {
            break;
        }
    }

    char* end = pointer++;


    newToken.content.append(begin, end-begin);
    newToken.type = CONTENT;
    tokens.push_back (newToken);
}
//------------------------------------------------------
void Lexer::SkipComment ()
{
    while (!IsLineEnd())
        pointer++;
}
//------------------------------------------------------
void Lexer::LexSpecial ()
{
    Token newToken;
    newToken.content = *pointer++;
    newToken.type = SPECIAL;
    tokens.push_back (newToken);
}
//------------------------------------------------------
void Lexer::LexLineEnd ()
{
    while (true)
    {
        while (IsLineEnd())
            pointer++;
        if (IsCommentBegin())
            SkipComment();
        else
            break;
    }
    tokens.emplace_back ("", LINEEND);
}


//======================================================
//======================================================

void Lexer::PrintTokens ()
{
    for (size_t i = 0;  i<tokens.size();  i++)
        cout << "TOKEN " << i << ':' << endl << "  type: " << tokenTypeDescription[tokens[i].type] << endl << "  content: " << tokens[i].content << endl << endl;
}
