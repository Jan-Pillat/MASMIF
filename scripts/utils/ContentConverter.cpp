#include "ContentConverter.hpp"

//===============================================================
//===============================================================

string ContentConverter::ConvertNumbers()
{
    while (IsContentNotFullyVerified())
    {
        SkipNotImportantChars ();

        if   (IsItNumberBegin())
            ConvertNumber();

        else if (IsContentNotFullyVerified())
            SkipChar();
    }

    FinishConvertedText();

    return  converted;
}

//===============================================================
//===============================================================

string ContentConverter::ConvertCommentaries()
{
    while (IsContentNotFullyVerified())
    {
        SkipNonPunctatorChars ();

        if (IsItCommentaryBegin())
            ConvertCommentary();

        else if (IsContentNotFullyVerified())
            SkipChar();
    }

    FinishConvertedText();

    return  converted;
}

//===============================================================
//===============================================================

string ContentConverter::ConvertNumbersAndCommentaries ()
{
    while (IsContentNotFullyVerified())
    {
        SkipNotImportantChars ();

        if (IsItCommentaryBegin())
            ConvertCommentary();

        else if (IsItNumberBegin())
            ConvertNumber();

        else if (IsContentNotFullyVerified())
            SkipChar();
    }

    FinishConvertedText();

    return  converted;
}

//!===============================================================
//!===============================================================

inline bool ContentConverter::IsContentNotFullyVerified ()
{
    return (*pointer!='\0');
}

//---------------------------------------------------------------

inline void ContentConverter::SkipNotImportantChars ()
{
    while ( !IsPunctator(*pointer) && !IsDigit(*pointer) && *pointer!='\0' )
            pointer++;
}

//---------------------------------------------------------------

inline void ContentConverter::SkipNonPunctatorChars ()
{
    while ( !IsPunctator(*pointer) && *pointer!='\0' )
            pointer++;
}

//---------------------------------------------------------------

inline bool ContentConverter::IsItCommentaryBegin ()
{
    return (*pointer==';' || *pointer=='#');
}

//---------------------------------------------------------------

inline void ContentConverter::ConvertCommentary ()
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

inline bool ContentConverter::IsItNumberBegin ()
{
    return (     IsDigit(*pointer)
            ||   *pointer=='$' && IsHexDigit(*(pointer+1))
            ||   *pointer=='%' && IsDigit(*(pointer+1))    );
}

//---------------------------------------------------------------

inline void ContentConverter::ConvertNumber()
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

inline void ContentConverter::SkipChar()
{
    pointer++;
}

//---------------------------------------------------------------

inline void ContentConverter::FinishConvertedText()
{
    converted += begin;
}
