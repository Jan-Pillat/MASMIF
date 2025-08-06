#include "Lexer.hpp"
using namespace std;


//======================================================
//======================================================
Lexer::Lexer (vector<Token>& gotTokens) : tokens(gotTokens)
{

}
Lexer::Lexer (vector<Token>& gotTokens, const char* script) : tokens(gotTokens)
{
    currentLine = 0;
    Tokenize (script);
}

//======================================================
//======================================================

void Lexer::Tokenize (const char* script)
{
    pointer = script;

    if (pointer == 0)
        throw  "  LEXER: invalid script pointer! \n";

    //! ----- LEX -----
    while (*pointer != '\0')
    {
        SkipBlanks();

        if (IsWordBegin     ()) LexWord     (); else
        if (IsNumberBegin   ()) LexNumber   (); else
        if (IsIncludingBegin()) Include     (); else
        if (IsTextBegin     ()) LexText     (); else
        if (IsCharBegin     ()) LexChars    (); else
        if (IsContentBegin  ()) LexContent  (); else
        if (IsCommentBegin  ()) LexComment  (); else
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

bool Lexer::IsWordBegin    ()
{
    return (IsAlpha(*pointer) || *pointer=='_');
}
bool Lexer::IsNumberBegin  ()
{
    return (IsDigit(*pointer) || IsSpecialNumBegin());
}
bool Lexer::IsIncludingBegin    ()
{
    return (*pointer=='#');
}
bool Lexer::IsTextBegin    ()
{
    return (*pointer=='"');
}
bool Lexer::IsCharBegin    ()
{
    return (*pointer=='\'');
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
    return (*pointer==';');
}
bool Lexer::IsSpecialNumBegin ()
{
    return (*pointer=='$' || *pointer=='%');
}
bool Lexer::IsLineEnd ()
{
    return (*pointer=='\r' || *pointer=='\n');
}

//!======================================================
//!======================================================

void Lexer::LexText    ()
{
    Token newToken;

    bool  hasFinishChar =  false;
    const char* begin   =  pointer++;

    //Find text termination
    while (true)
    {
        while (*pointer!='\0' && *pointer!='"' && *pointer!='\r' && *pointer!='\n' && *pointer!='\\')
            pointer++;

        if (*pointer=='\\')
        {
            pointer++;

            if ( (*pointer=='"') || (*pointer=='\\') )
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

    const char* end = pointer;


    newToken.content.append(begin, end-begin);

    if (!hasFinishChar)
        newToken.content += '"';

    newToken.line = currentLine;
    newToken.type = TYPE_TEXT;
    tokens.push_back (newToken);
}
//!------------------------------------------------------
void Lexer::LexChars    ()
{
    Token newToken;

    bool hasFinishChar = false;
    const char* begin  = pointer++;


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

    const char* end = pointer;


    newToken.content.append(begin, end-begin);

    if (!hasFinishChar)
        newToken.content += '\'';

    newToken.line = currentLine;
    newToken.type = TYPE_CHARS;
    tokens.push_back (newToken);
}
//!------------------------------------------------------
void Lexer::LexWord    ()
{
    Token newToken;

    const char* begin = pointer;

    //Find text termination
    while (IsAlOrDigit(*pointer) || *pointer=='_')
        pointer++;

    const char* end = pointer;


    newToken.content.assign(begin, end-begin);
    newToken.line = currentLine;
    newToken.type = TYPE_WORD;
    tokens.push_back (newToken);
}
//!------------------------------------------------------
void Lexer::LexNumber  ()
{
    Token newToken;
    const char* begin = pointer;      //begin of data to save

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

    const char* end = pointer;


    // --- SAVE TOKEN ---
    newToken.content.assign(begin, end-begin);
    newToken.line = currentLine;
    newToken.type = TYPE_NUMBER;
    tokens.push_back (newToken);

}
//!------------------------------------------------------
void Lexer::LexContent ()
{
    Token newToken;
    newToken.line = currentLine;
    pointer++;

    const char* begin = pointer;

    //Find text termination
    while(true)
    {
        while (*pointer!='\0' && *pointer!='}')
        {
            if (IsLineEnd())
                SkipLineEnd();
            else
                pointer++;
        }

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

    const char* end = pointer++;


    newToken.content.append(begin, end-begin);
    newToken.type = TYPE_CONTENT;
    tokens.push_back (newToken);
}
//!------------------------------------------------------
void Lexer::LexSpecial ()
{
    tokens.emplace_back (*pointer++, TYPE_SPECIAL, currentLine);
}
//!------------------------------------------------------
void Lexer::SkipComment ()
{
    while (!IsLineEnd())
        pointer++;
}
//!------------------------------------------------------
void Lexer::SkipLineEnd ()
{
    if (*pointer=='\r'&&*(pointer+1)=='\n')
        pointer++;
    pointer++;
    currentLine++;
}
//!------------------------------------------------------
void Lexer::LexComment ()
{
    const char* begin = ++pointer;

    while (!IsLineEnd())
        pointer++;

    const char* end = pointer;

    string gotComment (begin, end-begin);

    LexLineEnd (&gotComment[0]);
}
//!------------------------------------------------------
void Lexer::LexLineEnd (const char* content)
{
    if (!IsLineEnd())
        return;

    while (IsLineEnd())
    {
        while (IsLineEnd())
            SkipLineEnd();
        if (IsCommentBegin())
            SkipComment();
    }
    tokens.emplace_back (content, TYPE_LINEEND, currentLine);
}
//!------------------------------------------------------
void Lexer::LexLineEnd ()
{
    LexLineEnd("");
}
//!------------------------------------------------------
void Lexer::Include ()
{
    const char* begin = ++pointer;

    while (*pointer != '\0' && *pointer != '#')
        pointer++;

    if (*pointer == '\0')
        return;

    //Tokenize will change pointer value, so it must be save
    const char* previousPointer = ++pointer;

    //Remove blanks at the end of the path
    const char* end = pointer-2;  //pointer = after hashtag,  pointer-1 = hashtag,  pointer-2 = before hashtag
    while (IsBlank(*end))
        end--;
    if (*end == '#')
        end = begin;
    else
        end++;

    //If path is empty, skip it.
    if (end == begin)
        return;

    //Tokenize file using the path.
    string path (begin, end-begin);
    Tokenize (&path[0]);

    //Tokenize has changed pointer value, so return previous pointer.
    pointer = previousPointer;
}

