#include "AsmConverter.hpp"

//===============================================================
//===============================================================

string AsmConverter::ConvertScript ()
{
    while (IsContentNotFullyVerified())
    {
        SkipNotImportantChars ();

        if (IsItCharsBegin())
            SkipChars ();

        else if (IsItTextBegin())
            SkipTextButAddNullChar();

        else if (IsItCommentaryBegin())
            ConvertCommentary();

        else if (IsItNumberBegin())
            ConvertNumber();

        else if (IsContentNotFullyVerified())
            SkipChar();
    }

    FinishConvertedText();

    return  converted;
}

//===============================================================
//===============================================================

string AsmConverter::AddDebugDeclarations ()
{
    LexContent              ();
    DeclareImportantLines   ();
    AddLabels               ();
    return  converted;
}

//!===============================================================
//!===============================================================

inline void AsmConverter::LexContent ()
{
    Lexer lexer (tokens, content);
}

//!===============================================================
//!===============================================================

inline bool AsmConverter::IsContentNotFullyVerified ()
{
    return (*pointer!='\0');
}

//---------------------------------------------------------------

inline void AsmConverter::SkipNotImportantChars ()
{
    while ( !IsPunctator(*pointer) && !IsDigit(*pointer) && *pointer!='\0' )
        pointer++;
}

//---------------------------------------------------------------

inline bool AsmConverter::IsItCharsBegin ()
{
    return (*pointer=='\'');
}

//---------------------------------------------------------------

inline void AsmConverter::SkipContainedChars (const char borderChar)
{
    while (true)
    {
        while (*pointer != '\r' && *pointer != '\n' && *pointer != '\0' && *pointer != '\\' && *pointer != borderChar)
            pointer++;

        if (*pointer == '\\')
        {
            pointer++;
            if (*pointer != '\0')
                pointer++;
            else
                break;
        }
        else
            break;
    }
}

//---------------------------------------------------------------

inline void AsmConverter::SkipChars ()
{
    pointer++;
    SkipContainedChars ('\'');
}

//---------------------------------------------------------------

inline bool AsmConverter::IsItTextBegin ()
{
    return (*pointer=='"');
}

//---------------------------------------------------------------

inline void AsmConverter::SkipTextButAddNullChar ()
{
    pointer++;
    SkipContainedChars ('"');

    end = pointer;
    converted.append(begin, end-begin);

    if (*pointer == '"')
        converted.append("\",0");
    else
        converted.append("\"\",0");

    pointer++;
    begin = pointer;
}

//---------------------------------------------------------------

inline bool AsmConverter::IsItCommentaryBegin ()
{
    return (*pointer==';' || *pointer=='#');
}

//---------------------------------------------------------------

inline void AsmConverter::ConvertCommentary ()
{
    //Rewrite and commentary begin is always ;
    end = pointer;
    converted.append(begin, end-begin);
    converted.append(";");              //Replace # -> ;
    pointer++;
    begin = pointer;

    //Goto line end
    while (*pointer != '\r' && *pointer != '\n' && *pointer != '\0')
        pointer++;
    //Skip line end
    while (*pointer == '\r' || *pointer == '\n')
        pointer++;
}

//---------------------------------------------------------------

inline bool AsmConverter::IsItNumberBegin ()
{
    return (     IsDigit(*pointer)
            ||   *pointer=='$' && IsHexDigit(*(pointer+1))
            ||   *pointer=='%' && IsDigit(*(pointer+1))    );
}

//---------------------------------------------------------------

inline void AsmConverter::ConvertNumber()
{
    //Copy last content
    end = pointer;
    converted.append(begin, end-begin);


    //Check type
    if (*pointer == '$')
    {
        isHex = true;
        pointer++;
    }
    else if (*pointer == '%')
    {
        isBin = true;
        pointer++;
    }
    else if (*pointer == '0')
    {
        const char* pointerToX = pointer;

        while (*pointerToX == '0')
            pointerToX++;

        if ( (*pointerToX == 'x') || (*pointerToX == 'X') )
        {
            isHex   = true;
            pointer = pointerToX+1;
        }
    }

    //Real begin of number
    begin = pointer;

    //Find end
    while (true)
    {
        while (IsAlOrNum(*pointer))
            pointer++;

        if (*pointer!='.')
            break;
        else
            pointer++;
    }

    if ( (isHex) && IsAlphabetic(*begin))
    {
        converted += "0";
    }

    end = pointer;
    converted.append(begin, end-begin);
    if (isHex)
    {
        isHex = false;
        converted += "h";
    }
    if (isBin)
    {
        isBin = false;
        converted += "b";
    }

    begin = pointer;
}

//---------------------------------------------------------------

inline void AsmConverter::SkipChar()
{
    pointer++;
}

//---------------------------------------------------------------

inline void AsmConverter::FinishConvertedText()
{
    converted += begin;
}
