#include "Lexer.hpp"

using namespace std;

char token_i = 0;       //indeks to ostatniego wczytanego znaku znalezionego tokenu
char token[0x200];      //Wczytany token

enum TokenType lastTokenType = NOONE;

char* currentPos    = nullptr;
int   currentLine   = 1;

punctator  foundPunctator;

unordered_map <string, keyword>*   currentKeywords  = &generalKeywords;

//-----------------------------------------
//-----------------------------------------

bool TryToLoadToken (char* src)
{
    //Clear token
    token_i = 0;
    token[0] = 0;

    //Skip blanks
    while (isblank(*src))    src++;

    //Text end
    if (*src == '\0')
    {
        token[token_i] = '\0';
        currentPos = src;
        return false;
    }
    //Symbols, texts, numbers and keywords
    else if (isalnum(*src))
    {
        while (true)
        {
            while (isalnum(*src))
                token[token_i++] = toupper(*src++);

            if (*src == '.' || *src  == '_')
            {
                token[token_i++] = *src++;
            }
            else break;
        }
    }
    //Znaki specjalne
    else if (ispunct(*src) || *src == '\r' || *src == '\n')
    {
        token[token_i++] = *src++;
    }
    //Zignorowanie nierozpoznanych znaków + przy okazji nieistotnych (isblank())
    else
    {
        do
        {
            src++;
        }
        while (!isalnum(*src) && !ispunct(*src) && *src != '\0');
        return TryToLoadToken(src);
    }
    token[token_i] = '\0';
    currentPos = src;
    return true;
}

//-----------------------------------------
//-----------------------------------------

bool CheckIsTokenKeywordAndSetReadmode ()
{
    auto position = (*currentKeywords).find(token);
    if (position != (*currentKeywords).end())
    {
        const auto& [key, attributes] = *position;
        if (attributes.ownKeywords != NULL)
            currentKeywords = attributes.ownKeywords;
        return true;
    }
    return false;
}

//-----------------------------------------
//-----------------------------------------

bool CheckIsTokenExtendedPunctator ()
{
    if (token[1]=='\0')
        if (auto position = punctators.find(token[0]); position != punctators.end())
        {
            const auto& [key, attributes] = *position;
            foundPunctator = &attributes;
            return true;
        }
    return false;
}

//-----------------------------------------
//-----------------------------------------

bool FoundWrongTermination ()
{
    // ANOTHER BEGIN
    if (foundPunctator.attitudeTowardsAnotherBegin == DONT_ACCEPT_ANOTHER_BEGIN)
    {
        if ( (*currentPos     == foundPunctator.sign)
        &&   (*(currentPos-1) != '\\')  )
        {
            return true;
        }
    }

    // LINE END and FILE END
    if (foundPunctator.attitudeTowardsLineEndsAndFileEnd == DONT_ACCEPT_LINE_END)
    {
        if ( (*currentPos == '\r') || (*currentPos == '\n') || (*currentPos == '\0') )
        {
            return true;
        }
    }
    else if (foundPunctator.attitudeTowardsLineEndsAndFileEnd == DONT_ACCEPT_FILE_END)
    {
        if (*currentPos == '\0')
        {
            return true;
        }
    }

    return false;
}

bool FoundTermination ()
{
    if (*currentPos == foundPunctator.termination)
    {
        return true;
    }


    if (foundPunctator.attitudeTowardsLineEndsAndFileEnd == LINE_END_IS_ALSO_TERMINATION)
    {
        if ( (*currentPos == '\r') || (*currentPos == '\n') || (*currentPos == '\0') )
        {
            return true;
        }
    }
    else
    {
        if (*currentPos == '\0')
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------
//-----------------------------------------

bool Tokenize (char* src)
{
    currentPos = src;

    while (TryToLoadToken(currentPos))
    {
        cout << "(line:" << currentLine << ") Found token:  " ;//<< (token[0]=='\r') ?"\\r" :(token[0]=='\n') ?"\\n" :token;
        if (token[0]=='\r') cout << "\\r"; else
        if (token[0]=='\n') cout << "\\n"; else cout << token;

        st_token newToken;
        newToken.content = token;


        // --------  NEW LINE  --------
        if (token[0]=='\r' || token[0]=='\n')
        {
            currentLine++;
            currentKeywords = &generalKeywords;

            if (token[0]=='\r' && *currentPos=='\n')
            {
                cout << "\\n";
                currentPos++;
            }


            if (lastTokenType == LINE_END)
            {
                cout << "  <--- ignore \n";
                continue;
            }
            else
            {
                newToken.type = LINE_END;
            }
        }
        // --------  NUMBER OTHER SYSTEM  --------
        else if ( (token[0] == '$' || token[0]=='%') && (token[1]=='\0') ) //If the loaded token is one of these symbols...
        {
            if (isxdigit(*currentPos))    //...followed by a number, it's an other number system
            {
                TryToLoadToken(currentPos);
                newToken.content += token;
                newToken.type = NUMBER;
                cout << token;
            }
            else
            {
                newToken.type=PUNCTATOR; //otherwise it's a special char
            }
        }
        // --------  EXTENDED PUNCTATORS  --------
        else if (CheckIsTokenExtendedPunctator())
        {
            cout << "...";
            newToken.type = foundPunctator.type;

            int   beginLine= currentLine;   //W razie błędu odczytu, zostanie podana linia, w której zaczyna się punktator
            char* beginPos = currentPos;

            while (true)
            {
                if (FoundWrongTermination())
                {
                    cout << "\nERROR! There is not terminated " << foundPunctator.name << " in line " << beginLine << "!\n";
                    return false;
                }
                else if (FoundTermination())
                {
                    char previousChar = *currentPos;

                    *currentPos = '\0';

                    if (foundPunctator.attitudeTowardsSpecials == SAVE_ALL_SPECIALS)
                    {
                        //Przepisz wszystko
                        newToken.content = beginPos;
                    }
                    else
                    {
                        string temp = beginPos;
                        char *src = &temp[0];
                        char *dest= &temp[0];
                        while (*src != '\0')
                        {
                            if (*src == '\\')
                            {
                                src++;
                                if (*src == foundPunctator.termination) //APPLY OWN TERMINATE SPECIAL
                                {
                                    *dest++ = *src;
                                }
                                else if (foundPunctator.attitudeTowardsSpecials == APPLY_ALL_SPECIALS)
                                {
                                    if (*src == 'a')    *dest++ = '\a'; else
                                    if (*src == 'b')    *dest++ = '\b'; else
                                    if (*src == 'f')    *dest++ = '\f'; else
                                    if (*src == 'n')    *dest++ = '\n'; else
                                    if (*src == 'r')    *dest++ = '\r'; else
                                    if (*src == 't')    *dest++ = '\t'; else
                                    if (*src == 'v')    *dest++ = '\v'; else  {   *dest++ = '\\'; *dest++ = *src; }
                                }
                                else
                                {
                                    *dest++ = '\\'; *dest++ = *src;
                                }
                            }
                            else
                            {
                                *dest++ = *src;
                            }
                            *src++;
                        }//while end
                        *dest = '\0';
                        newToken.content = (&temp[0]);//It must be as a char table, otherwise '\0' will be ignored and the rest of the text will be rewritten
                    }
                    *currentPos = previousChar;

                    if (previousChar == foundPunctator.termination)
                        currentPos++;

                    break;
                }
                else
                {
                    //Count lines
                    if (*currentPos == '\r')
                    {
                        currentLine++;
                        if (*(currentPos+1) == '\n') currentPos++;
                    }
                    else if (*currentPos == '\n')
                    {
                        currentLine++;
                    }
                    //Ignore termination
                    else if ( (*currentPos == '\\') && (*(currentPos+1) == foundPunctator.termination) )
                    {
                        currentPos++;
                    }
                    //Next char
                    currentPos++;
                }
            }
        }
        // --------  REST  --------
        else if (CheckIsTokenKeywordAndSetReadmode())
        {
            newToken.type=KEYWORD;
        }
        else if (isalpha(token[0]))
        {
            newToken.type=IDENTIFIER;
        }
        else if (isdigit(token[0]))
        {
            newToken.type=NUMBER;
        }
        else if (ispunct(token[0]))
        {
            newToken.type=PUNCTATOR;
        }
        else
        {
            newToken.type=UNDEF;
        }


        lastTokenType = newToken.type;
        tokens.push_back(newToken);

        cout << endl;
    }
    cout << endl;
    return true;
}

//-----------------------------------------
//-----------------------------------------

void PrintTokens ()
{
    int tokenCount = tokens.size();
    cout << "  Token count = " << tokenCount << endl;
    for (int i = 0; i<tokenCount; i++)
    {
        int typeNum = tokens[i].type;
        char* typeName = &tokenTypeName [ typeNum ][0];

        cout << "    (" << i << ")\n";
        cout << "    type = " << typeName << endl;
        cout << "    content = " << tokens[i].content << endl;
    }
}

//-----------------------------------------
//-----------------------------------------

void TokenizeFile (string path)
{
    FileData fileData;
    fileData.LoadTextFile(path);

    cout << "File size is:  " << fileData.GetLength() << endl << endl;
    cout << "File content is:" << endl << endl << fileData.GetBeginPointer() << endl;

    Tokenize (&fileData[0]);
}


