
#include "Parser.hpp"

#include <iostream> //Debug

using namespace std;

//======================================================
//======================================================

Parser::Parser   (vector<Token>& gotTokens) : tokens(gotTokens), gotToken(gotTokens[0]), nextToken(gotTokens[1])
{
    while (GetToken())
    {
        auto position = generalKeywords.find(gotToken.content);
        if (position != generalKeywords.end())
        {
            (this->*position->second.func)();
        }
    }

    cout << "sections count: " << sections.size() << endl;
    cout << "segments count: " << segments.size() << endl;
    cout << "variables count: " << variables.size() << endl;
}

//======================================================
//======================================================

bool    Parser::GetToken    ()
{
    if (iterator < tokens.size())
    {
        gotToken = tokens[iterator++];
        return true;
    }
    else
    {
        return false;
    }
}
//------------------------------------------------------
bool    Parser::GetTokenOnlyToLineEnd    ()
{
    if ( (iterator < tokens.size()) && (gotToken.type != TYPE_LINEEND) )
    {
        gotToken = tokens[iterator++];
        return true;
    }
    else
    {
        return false;
    }
}
//------------------------------------------------------
bool    Parser::ShowNextToken    ()
{
    if (iterator < tokens.size())
    {
        nextToken = tokens[iterator+1];
        return true;
    }
    else
    {
        return false;
    }
}

//======================================================
//======================================================




void    Parser::ParseSection    ()
{
    Section newSection;

    while (GetTokenOnlyToLineEnd())
    {
        // ---------- ATTRIBUTES ----------
        if (gotToken.type == TYPE_WORD)
        {
            auto position = sectionKeywords.find(gotToken.content);
            if (position != sectionKeywords.end())
            {
                newSection.attributes   |=  position->second;
            }
        }
        // ---------- NAME ----------
        else if (gotToken.type == TYPE_TEXT)
        {
            newSection.name  =   gotToken.content;
        }
        // ---------- END ----------
        else if (gotToken.type == TYPE_LINEEND)
        {
            break;
        }
    }

    // --- Check is the next token a begin of a code ---
    if (gotToken.type == TYPE_LINEEND)
    {
        if (newSection.content == "")
            if (ShowNextToken())
            {
                nextToken.type == TYPE_CONTENT;

                GetToken();
                newSection.content = gotToken.content;
            }
    }

    sections.push_back (newSection);
}



//------------------------------------------------------



void    Parser::ParseSegment    ()
{
    Segment newSegment;

    while (GetTokenOnlyToLineEnd())
    {
        // ---------- ATTRIBUTES ----------
        if (gotToken.type == TYPE_SPECIAL)
        {
            if (gotToken.content == "[")
            {
                while (GetTokenOnlyToLineEnd())
                {
                    if (gotToken.type == TYPE_NUMBER)
                    {
                        newSegment.address = StrGetNum<int>(&gotToken.content[0]);
                    }
                    else if ((gotToken.type == TYPE_SPECIAL) && (gotToken.content == "]") || (gotToken.type == TYPE_LINEEND))
                    {
                        break;
                    }
                }
            }
            else if (gotToken.content == "(")
            {
                while (GetTokenOnlyToLineEnd())
                {
                    if (gotToken.type == TYPE_NUMBER)
                    {
                        newSegment.size = StrGetNum<int>(&gotToken.content[0]);
                    }
                    else if (gotToken.type == TYPE_WORD)
                    {
                        if (gotToken.content == "UNTIL")
                        {
                            if (GetTokenOnlyToLineEnd())
                            {
                                if (gotToken.type == TYPE_NUMBER)
                                {
                                    newSegment.until        = true;
                                    newSegment.untilBinNum  = (BYTE)StrGetNum<int>(&gotToken.content[0]);
                                }
                                else if (gotToken.type == TYPE_WORD)
                                {
                                    auto position = assemblyKeyword.find(gotToken.content);
                                    if (position != assemblyKeyword.end())
                                    {
                                        newSegment.until        = true;
                                        newSegment.untilBinNum  = position->second;
                                    }
                                }
                            }
                        }
                    }
                    else if ((gotToken.type == TYPE_SPECIAL) && (gotToken.content == ")") || (gotToken.type == TYPE_LINEEND))
                    {
                        break;
                    }
                }
            }
        }
        // ---------- NAME ----------
        else if (gotToken.type == TYPE_TEXT)
        {
            newSegment.name  =   gotToken.content;
        }
        // ---------- END ----------
        else if (gotToken.type == TYPE_LINEEND)
        {
            break;
        }
    }

    // --- Check is the next token a begin of a code ---
    if (gotToken.type == TYPE_LINEEND)
    {
        if (newSegment.content == "")
            if (ShowNextToken())
            {
                nextToken.type == TYPE_CONTENT;

                GetToken();
                newSegment.content = gotToken.content;
            }
    }

    segments.push_back (newSegment);
}



//------------------------------------------------------



void    Parser::ParseVariable    ()
{
    return;
}
