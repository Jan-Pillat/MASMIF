#include "AsmConverter.hpp"
#include <iostream> //debug

using namespace std;

//!===============================================================
//!===============================================================

string AsmConverter::ExchangeAutodeclaredTexts (unordered_map<string, size_t>* textsToDeclare)
{
    this->textsToDeclare = textsToDeclare;

    while (IsContentNotFullyVerified())
    {
        //! ----- FIND COMMAND -----
        while (IsContentNotFullyVerified())
        {
            SkipBlanks  ();

            if (TryToGetWord() && !IsItKeywordToAvoid())
            {
                SkipBlanks  ();

                if (!IsItWordBegin() || (TryToGetWord() && !IsItKeywordToAvoid()) )
                    break;
            }
            else if (!IsItCommentaryBegin() && !IsItLineOrContentEnd())
                break;

            SkipLine ();
        }

        //! ----- FIND TEXT -----
        while (!IsItLineOrContentEnd())
        {
            SkipBlanks  ();

            if (IsItCharsBegin())
                SkipChars ();

            else if (IsItTextBegin())
            {
                DeclareText ();
            }

            else if (IsItCommentaryBegin())
            {
                SkipLine ();
                break;
            }

            else if (IsItNumberBegin())
                SkipNumber ();

            else if (IsItWordBegin())
                //if (IsItOffsetWord())
                //    DeleteOffsetWord()
                //else
                    SkipWord();

            else if (IsContentNotFullyVerified())
                SkipChar();
        }

    }

    FinishConvertedText();

    return  converted;
}


//!===============================================================
//!===============================================================


string AsmConverter::ConvertSyntax ()
{
    while (IsContentNotFullyVerified())
    {
        SkipNotImportantChars ();

        if (IsItCharsBegin())
            ConvertChars ();

        else if (IsItTextBegin())
            ConvertTextToCharsAddNullChar();

//        else if (IsItCommentaryBegin())
//            ConvertCommentary();

        else if (IsItNumberBegin())
            ConvertNumber();

        else if (IsContentNotFullyVerified())
            SkipChar();
    }

    FinishConvertedText();

    return  converted;
}


//===============================================================


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

inline void AsmConverter::SkipText ()
{
    pointer++;
    SkipContainedChars ('"');
}

//---------------------------------------------------------------

inline void AsmConverter::ConvertChars ()
{
    end = pointer;
    converted.append(begin, end-begin);
    bool charsAreOpen = false;
    bool neededComma = false;

    pointer++;
    begin = pointer;

    while (true)
    {
        if (!charsAreOpen)
            if (*pointer != '\r' && *pointer != '\n' && *pointer != '\0' && *pointer != '\\' && *pointer != '\'')
            {
                if (neededComma)
                    converted.append (",");
                converted.append ("'");
                charsAreOpen = true;
                neededComma  = true;
            }


        while (*pointer != '\r' && *pointer != '\n' && *pointer != '\0' && *pointer != '\\' && *pointer != '\'')
            pointer++;


        if (*pointer == '\\')
        {
            end = pointer;
            converted.append(begin, end-begin);
            pointer++;

            if (*pointer == '\0')
            {
                begin = pointer;
                converted.append(1,'\\');
                break;
            }
            else
            {
                begin = pointer+1;

                if (charsAreOpen)
                {
                    converted.append(1,'\'');
                    charsAreOpen = false;
                }

                if (neededComma)
                    converted.append(1,',');
                else
                    neededComma = true;


                if (*pointer == 'a')    converted.append(ConvertNumberToHexStringH('\a')); else
                if (*pointer == 'b')    converted.append(ConvertNumberToHexStringH('\b')); else
                if (*pointer == 'e')    converted.append(ConvertNumberToHexStringH('\e')); else
                if (*pointer == 'f')    converted.append(ConvertNumberToHexStringH('\f')); else
                if (*pointer == 'n')    converted.append(ConvertNumberToHexStringH('\n')); else
                if (*pointer == 'r')    converted.append(ConvertNumberToHexStringH('\r')); else
                if (*pointer == 't')    converted.append(ConvertNumberToHexStringH('\t')); else
                if (*pointer == 'v')    converted.append(ConvertNumberToHexStringH('\v')); else
                if (*pointer == '0')    converted.append(ConvertNumberToHexStringH('\0')); else
                if (*pointer == '\'')   converted.append(ConvertNumberToHexStringH('\'')); else
                                        converted.append(ConvertNumberToHexStringH(*pointer));
            }
        }
        else
        {
            break;
        }

        pointer++;
    }


    end = pointer;
    converted.append(begin, end-begin);

    if (charsAreOpen)
        converted.append("'");

    if (*pointer == '\'')
        pointer++;

    begin = pointer;
}

//---------------------------------------------------------------

inline void AsmConverter::ConvertTextToCharsAddNullChar ()
{
    end = pointer;
    converted.append(begin, end-begin);
    bool charsAreOpen = false;
    bool neededComma = false;

    pointer++;
    begin = pointer;

    while (true)
    {
        if (!charsAreOpen)
            if (*pointer != '\r' && *pointer != '\n' && *pointer != '\0' && *pointer != '\\' && *pointer != '\'' && *pointer != '"')
            {
                if (neededComma)
                    converted.append (",");
                converted.append ("'");
                charsAreOpen = true;
                neededComma  = true;
            }


        while (*pointer != '\r' && *pointer != '\n' && *pointer != '\0' && *pointer != '\\' && *pointer != '\'' && *pointer != '"')
            pointer++;


        if (*pointer == '\\')
        {
            end = pointer;
            converted.append(begin, end-begin);
            pointer++;

            if (*pointer == '\0')
            {
                begin = pointer;
                converted.append(1,'\\');
                break;
            }
            else
            {
                begin = pointer+1;

                if (*pointer == '"')
                {
                    if (!charsAreOpen)
                    {
                        if (neededComma)
                            converted.append(1,',');
                        else
                            neededComma = true;

                        converted.append(1,'\'');
                        charsAreOpen = true;
                    }

                    converted.append(1,'"');
                }
                else
                {
                    if (charsAreOpen)
                    {
                        converted.append(1,'\'');
                        charsAreOpen = false;
                    }

                    if (neededComma)
                        converted.append(1,',');
                    else
                        neededComma = true;


                    if (*pointer == 'a')    converted.append(ConvertNumberToHexStringH('\a')); else
                    if (*pointer == 'b')    converted.append(ConvertNumberToHexStringH('\b')); else
                    if (*pointer == 'e')    converted.append(ConvertNumberToHexStringH('\e')); else
                    if (*pointer == 'f')    converted.append(ConvertNumberToHexStringH('\f')); else
                    if (*pointer == 'n')    converted.append(ConvertNumberToHexStringH('\n')); else
                    if (*pointer == 'r')    converted.append(ConvertNumberToHexStringH('\r')); else
                    if (*pointer == 't')    converted.append(ConvertNumberToHexStringH('\t')); else
                    if (*pointer == 'v')    converted.append(ConvertNumberToHexStringH('\v')); else
                    if (*pointer == '0')    converted.append(ConvertNumberToHexStringH('\0')); else
                    if (*pointer == '\'')   converted.append(ConvertNumberToHexStringH('\'')); else
                                            converted.append(ConvertNumberToHexStringH(*pointer));
                }
            }
        }
        else if (*pointer == '\'')
        {
            end = pointer;
            converted.append(begin, end-begin);
            begin = pointer+1;

            if (charsAreOpen)
            {
                converted.append(1,'\'');
                charsAreOpen = false;
            }

            if (neededComma)
                converted.append(1,',');
            else
                neededComma = true;

            converted.append("27h");
        }
        else
        {
            break;
        }

        pointer++;
    }


    end = pointer;
    converted.append(begin, end-begin);

    if (charsAreOpen)
        converted.append("',");
    else if (neededComma)
        converted.append (",");

    converted.append ("0");

    if (*pointer == '"')
        pointer++;

    begin = pointer;
}

//---------------------------------------------------------------

inline void AsmConverter::SkipBlanks ()
{
    while (IsBlank(*pointer))
        pointer++;
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

    SkipLine ();
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

inline void AsmConverter::SkipNumber()
{
    while (IsHexDigit(*pointer))
        pointer++;
}

//---------------------------------------------------------------

inline bool AsmConverter::IsItLineEnd ()
{
    return (*pointer == '\r' || *pointer == '\n');
}

//---------------------------------------------------------------

inline bool AsmConverter::IsItLineOrContentEnd ()
{
    return (*pointer == '\r' || *pointer == '\n' || *pointer == '\0');
}

//---------------------------------------------------------------

inline void AsmConverter::SkipLine ()
{
    //Goto line end
    while (!IsItLineOrContentEnd())
        pointer++;
    //Skip line end
    while (IsItLineEnd())
        pointer++;
}

//---------------------------------------------------------------

inline bool AsmConverter::IsItWordBegin ()
{
    return (IsAlphabetic(*pointer) || *pointer == '_');
}

//---------------------------------------------------------------

inline bool AsmConverter::IsItWordInside ()
{
    return (IsAlphabetic(*pointer) || IsNum(*pointer) || *pointer == '_');
}

//---------------------------------------------------------------

inline void AsmConverter::SkipWord ()
{
    while (IsItWordInside())
        pointer++;
}

//---------------------------------------------------------------

inline bool AsmConverter::TryToGetWord ()
{
    if (!IsItWordBegin())
        return false;

    end = pointer;
    converted.append (begin, end-begin);

    begin = pointer;
    while (IsItWordInside())
        pointer++;
    end = pointer;
    gotWord.assign (begin, end-begin);

    transform (gotWord.begin(), gotWord.end(), gotWord.begin(), ::toupper);

    return true;
}

//---------------------------------------------------------------

inline bool AsmConverter::CheckIsItCommandOrDeclaration()
{
    if (assemblyCommands.find(gotWord) != assemblyCommands.end())
        return true;
    else if (assemblyDeclarations.find(gotWord) != assemblyDeclarations.end())
        return true;
    else
        return false;
}

//---------------------------------------------------------------

inline bool AsmConverter::GetWordAndCheckIsItCommandOrDeclaration()
{
    if (TryToGetWord())
        return CheckIsItCommandOrDeclaration();
    else
        return false;
}

//---------------------------------------------------------------

inline bool AsmConverter::IsItKeywordToAvoid ()
{
    if (keywordsToAvoidByConverter.find(gotWord) != keywordsToAvoidByConverter.end())
        return true;
    else
        return false;
}

//---------------------------------------------------------------

inline bool AsmConverter::DeclareText()
{
    if (*pointer=='"')
    {
        end = pointer;
        converted.append (begin, end-begin);
        begin = pointer++;

        while (true)
        {
            while (*pointer!='"' && *pointer!='\\' && !IsItLineOrContentEnd())
                pointer++;

            if (*pointer=='\\')
            {
                pointer++;
                if (*pointer=='\0')
                    break;
                else
                    *pointer++;
            }
            else
            {
                break;
            }
        }

        end = ++pointer;
        string foundText (begin, end-begin);

        if (textsToDeclare->find(foundText) == textsToDeclare->end())
        {
            converted += "OFFSET ____TXT_";
            converted += to_string(textsToDeclare->size());
            converted += " ";
            (*textsToDeclare)[foundText] = textsToDeclare->size();
        }
        else
        {
            converted += "OFFSET ____TXT_";
            converted += to_string((*textsToDeclare)[foundText]);
            converted += " ";
        }

        begin = pointer;
        return true;
    }
    return false;
}

//---------------------------------------------------------------

inline void AsmConverter::FinishConvertedText()
{
    converted += begin;
}
