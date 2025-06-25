#include "Lexer.hpp"
using namespace std;


//======================================================
//======================================================
Lexer::Lexer ()
{

}
Lexer::Lexer (char* filePath)
{
    Tokenize (filePath);
}


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

    bool  hasFinishChar =  false;
    char* begin         =  pointer++;

    //Find text termination
    while (true)
    {
        while (*pointer!='\0' && *pointer!='"' && *pointer!='\r' && *pointer!='\n' && *pointer!='\\')
            pointer++;

        if (*pointer=='\\')
        {
            pointer++;

            if (*pointer=='"')
                pointer++;
        }
        else
        {
            if (*pointer=='"')
            {
                pointer++;
                hasFinishChar = true;
            }
            break;
        }
    }

    char* end = pointer++;


    newToken.content.append(begin, end-begin);

    if (!hasFinishChar)
        newToken.content += '"';

    newToken.content += ",0";
    newToken.type = TYPE_TEXT;
    tokens.push_back (newToken);
}
//------------------------------------------------------
void Lexer::LexChars    ()
{
    Token newToken;

    bool hasFinishChar = false;
    char* begin = pointer++;


    //Find text termination
    while (true)
    {
        while (*pointer!='\0' && *pointer!='\'' && *pointer!='\r' && *pointer!='\n' && *pointer!='\\')
            pointer++;

        if (*pointer=='\\')
        {
            pointer++;

            if (*pointer=='\'')
                pointer++;
        }
        else
        {
            if (*pointer=='\'')
            {
                pointer++;
                hasFinishChar = true;
            }
            break;
        }
    }

    char* end = pointer++;


    newToken.content.append(begin, end-begin);

    if (!hasFinishChar)
        newToken.content += '\'';

    newToken.type = TYPE_CHARS;
    tokens.push_back (newToken);
}
//------------------------------------------------------
void Lexer::LexWord    ()
{
    Token newToken;

    char* begin = pointer;

    //Find text termination
    while (IsAlOrDigit(*pointer) || *pointer=='_')
        pointer++;

    char* end = pointer;


    newToken.content.assign(begin, end-begin);
    newToken.type = TYPE_WORD;
    tokens.push_back (newToken);
}
//------------------------------------------------------
void Lexer::LexNumber  ()
{
    Token newToken;
    char* begin = pointer;      //begin of data to save

    // --- SKIP SPECIAL CHAR ---
    if (IsSpecialNumBegin())
        pointer++;

    // --- FIND END ---
    while (true)
    {
        while (IsAlOrNum(*pointer))
            pointer++;

        if (*pointer=='.')
            pointer++;
        else
            break;
    }

    char* end = pointer;


    // --- SAVE TOKEN ---
    newToken.content.assign(begin, end-begin);
    newToken.type = TYPE_NUMBER;
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
    newToken.type = TYPE_CONTENT;
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
    newToken.type = TYPE_SPECIAL;
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
    tokens.emplace_back ("", TYPE_LINEEND);
}

