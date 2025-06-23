
#include "Parser.hpp"

#include <iostream> //Debug

using namespace std;

//======================================================
//======================================================

Parser::Parser   (vector<Token>& gotTokens) : tokens(gotTokens)
{
    while (GetToken())
    {
        cout << "Parser: found token: " << gotToken->content << endl;

        if (gotToken->type != TYPE_WORD)
            continue;

        transform (gotToken->content.begin(), gotToken->content.end(), gotToken->content.begin(), ::toupper);

        auto position = generalKeywords.find(gotToken->content);
        if (position != generalKeywords.end())
        {
            (this->*position->second.func)();
        }
    }

    sort  (declarations.begin(), declarations.end(), DeclarationComparison());


    cout << "declarations count:   " << declarations.size()     << endl;

    for (size_t i = 0;  i<declarations.size();  i++)
    {
        cout << "DECLARATION(" << i << ")"
            << endl << "\ttype = "      << declarationTypeDescription[declarations[i].type]
            << endl << "\tname = "      << declarations[i].name
            << endl << "\taddress = "   << declarations[i].address << hex << "(0x" << declarations[i].address << ')' << dec
            << endl;
    }
}

//======================================================
//======================================================

bool    Parser::GetToken    ()
{
    if (iterator < tokens.size())
    {
        gotToken = &tokens[iterator++];
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
    if ( (iterator < tokens.size()) && (gotToken->type != TYPE_LINEEND) )
    {
        gotToken = &tokens[iterator++];
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
        nextToken = &tokens[iterator];
        return true;
    }
    else
    {
        nextToken = nullptr;
        return false;
    }
}

//======================================================
//======================================================




void    Parser::ParseSection    ()
{
    cout << "    Parse Section:" << endl;
    Declaration newSection;

    while (GetTokenOnlyToLineEnd())
    {
        cout << "      gotToken = " << gotToken->content << " ; type = " << tokenTypeDescription[gotToken->type] << endl;
        // ---------- ATTRIBUTES ----------
        if (gotToken->type == TYPE_SPECIAL)
        {
            if (gotToken->content == "(")
            {
                while (GetTokenOnlyToLineEnd())
                {
                    if (gotToken->type == TYPE_NUMBER)
                    {
                        newSection.size = StrGetNum<int>(&gotToken->content[0]);
                    }
                    /*
                    else if (gotToken->type == TYPE_WORD)
                    {
                        if (gotToken->content == "FIX")
                        {
                            if (GetTokenOnlyToLineEnd())
                            {
                                newSection.fixSize  = true;
                            }
                        }
                    }
                    */
                    else if ((gotToken->type == TYPE_SPECIAL) && (gotToken->content == ")") || (gotToken->type == TYPE_LINEEND))
                    {
                        break;
                    }
                }
            }
        }
        // ---------- CONTENT ----------
        if (gotToken->type == TYPE_WORD)
        {
            auto position = sectionKeywords.find(gotToken->content);
            if (position != sectionKeywords.end())
            {
                newSection.attributes   |=  position->second;
            }
        }
        // ---------- CONTENT ----------
        if (gotToken->type == TYPE_CONTENT)
        {
            newSection.content = gotToken->content;
        }
        // ---------- NAME ----------
        else if ( (gotToken->type == TYPE_TEXT) || (gotToken->type == TYPE_CHARS) )
        {
            newSection.name  =   gotToken->content;
        }
        // ---------- END ----------
        else if (gotToken->type == TYPE_LINEEND)
        {
            cout << "      Section Finish" << endl;
            break;
        }
    }

    // --- Check is the next token a begin of a code ---
    if (gotToken->type == TYPE_LINEEND)
    {
        if (newSection.content == "")
            if (ShowNextToken())
            {
                if (nextToken->type == TYPE_CONTENT)
                {
                    GetToken();
                    newSection.content = gotToken->content;
                }
            }
    }

    newSection.type = SECTION;
    declarations.push_back (newSection);
}



//------------------------------------------------------



void    Parser::ParseSegment    ()
{
    cout << "    Parse Segment:" << endl;
    Declaration newSegment;

    while (GetTokenOnlyToLineEnd())
    {
        cout << "      gotToken = " << gotToken->content << " ; type = " << tokenTypeDescription[gotToken->type] << endl;
        // ---------- ATTRIBUTES ----------
        if (gotToken->type == TYPE_SPECIAL)
        {
            if (gotToken->content == "[")
            {
                while (GetTokenOnlyToLineEnd())
                {
                    if (gotToken->type == TYPE_NUMBER)
                    {
                        newSegment.address = StrGetNum<int>(&gotToken->content[0]);
                    }
                    else if ((gotToken->type == TYPE_SPECIAL) && (gotToken->content == "]") || (gotToken->type == TYPE_LINEEND))
                    {
                        break;
                    }
                }
            }
            else if (gotToken->content == "(")
            {
                while (GetTokenOnlyToLineEnd())
                {
                    if (gotToken->type == TYPE_NUMBER)
                    {
                        newSegment.size = StrGetNum<int>(&gotToken->content[0]);
                    }
                    else if (gotToken->type == TYPE_WORD)
                    {
                        if (gotToken->content == "UNTIL")
                        {
                            if (GetTokenOnlyToLineEnd())
                            {
                                if (gotToken->type == TYPE_NUMBER)
                                {
                                    newSegment.until        = true;
                                    newSegment.untilBinNum  = (BYTE)StrGetNum<int>(&gotToken->content[0]);
                                }
                                else if (gotToken->type == TYPE_WORD)
                                {
                                    auto position = assemblyKeyword.find(gotToken->content);
                                    if (position != assemblyKeyword.end())
                                    {
                                        newSegment.until        = true;
                                        newSegment.untilBinNum  = position->second;
                                    }
                                }
                            }
                        }
                    }
                    else if ((gotToken->type == TYPE_SPECIAL) && (gotToken->content == ")") || (gotToken->type == TYPE_LINEEND))
                    {
                        break;
                    }
                }
            }
        }
        // ---------- CONTENT ----------
        if (gotToken->type == TYPE_CONTENT)
        {
            newSegment.content = gotToken->content;
        }
        // ---------- NAME ----------
        else if (gotToken->type == TYPE_WORD)
        {
            if (newSegment.size == 0)
                if (gotToken->content == "SIZE")
                    if (ShowNextToken())
                        if (nextToken->type == TYPE_NUMBER)
                        {
                            GetToken();
                            newSegment.size = StrGetNum<DWORD>(&gotToken->content[0]);
                            goto L_SEGMENT_WORD_DONE;
                        }

            if (newSegment.address == 0)
                if (gotToken->content == "ADDRESS")
                    if (ShowNextToken())
                        if (nextToken->type == TYPE_NUMBER)
                        {
                            GetToken();
                            newSegment.address = StrGetNum<DWORD>(&gotToken->content[0]);
                            goto L_SEGMENT_WORD_DONE;
                        }

            if (newSegment.name == "")
                newSegment.name  =   gotToken->content;

            L_SEGMENT_WORD_DONE:
        }
        // ---------- END ----------
        else if (gotToken->type == TYPE_LINEEND)
        {
            break;
        }
    }

    // --- Check is the next token a begin of a code ---
    if (gotToken->type == TYPE_LINEEND)
        if (newSegment.content == "")
            if (ShowNextToken())
                if (nextToken->type == TYPE_CONTENT)
                {
                    GetToken();
                    newSegment.content = gotToken->content;
                }

    newSegment.type = SEGMENT;
    declarations.push_back (newSegment);
}



//------------------------------------------------------



void    Parser::ParseVariable    ()
{
    Declaration newVariable;
    newVariable.declaration = gotToken->content;

    while (GetTokenOnlyToLineEnd())
    {
        // ---------- ATTRIBUTES ----------
        if (gotToken->type == TYPE_SPECIAL)
        {
            if (gotToken->content == "[")
            {
                while (GetTokenOnlyToLineEnd())
                {
                    if (gotToken->type == TYPE_NUMBER)
                    {
                        newVariable.address = StrGetNum<int>(&gotToken->content[0]);
                    }
                    else if ((gotToken->type == TYPE_SPECIAL) && (gotToken->content == "]") || (gotToken->type == TYPE_LINEEND))
                    {
                        break;
                    }
                }
            }
        }
        // ---------- CONTENT ----------
        if (gotToken->type == TYPE_CONTENT)
        {
            newVariable.content = gotToken->content;
        }
        // ---------- NAME ----------
        else if (gotToken->type == TYPE_WORD)
        {
            if (newVariable.address == 0)
                if (gotToken->content == "ADDRESS")
                    if (ShowNextToken())
                        if (nextToken->type == TYPE_NUMBER)
                        {
                            GetToken();
                            newVariable.address = StrGetNum<DWORD>(&gotToken->content[0]);
                            goto L_VARIABLE_WORD_DONE;
                        }

            if (newVariable.name == "")
                newVariable.name  =   gotToken->content;

            L_VARIABLE_WORD_DONE:
        }
        // ---------- END ----------
        else if (gotToken->type == TYPE_LINEEND)
        {
            break;
        }
    }

    // --- Check is the next token a begin of a code ---
    if (gotToken->type == TYPE_LINEEND)
    {
        if (newVariable.content == "")
            if (ShowNextToken())
                if (nextToken->type == TYPE_CONTENT)
                {
                    GetToken();
                    newVariable.content = gotToken->content;
                }
    }

    newVariable.type = VARIABLE;
    declarations.push_back (newVariable);
}



//------------------------------------------------------



void    Parser::ParseProcedure    ()
{
    Declaration newProcedure;

    while (GetTokenOnlyToLineEnd())
    {
        // ---------- ATTRIBUTES ----------
        if (gotToken->type == TYPE_SPECIAL)
        {
            if (gotToken->content == "[")
            {
                while (GetTokenOnlyToLineEnd())
                {
                    if (gotToken->type == TYPE_NUMBER)
                    {
                        newProcedure.address = StrGetNum<int>(&gotToken->content[0]);
                    }
                    else if ((gotToken->type == TYPE_SPECIAL) && (gotToken->content == "]") || (gotToken->type == TYPE_LINEEND))
                    {
                        break;
                    }
                }
            }
        }
        // ---------- CONTENT ----------
        if (gotToken->type == TYPE_CONTENT)
        {
            newProcedure.content = gotToken->content;
        }
        // ---------- NAME ----------
        else if (gotToken->type == TYPE_WORD)
        {
            if (newProcedure.size == 0)
                if (gotToken->content == "SIZE")
                    if (ShowNextToken())
                        if (nextToken->type == TYPE_NUMBER)
                        {
                            GetToken();
                            newProcedure.size = StrGetNum<DWORD>(&gotToken->content[0]);
                            goto L_PROCEDURE_WORD_DONE;
                        }

            if (newProcedure.address == 0)
                if (gotToken->content == "ADDRESS")
                    if (ShowNextToken())
                        if (nextToken->type == TYPE_NUMBER)
                        {
                            GetToken();
                            newProcedure.address = StrGetNum<DWORD>(&gotToken->content[0]);
                            goto L_PROCEDURE_WORD_DONE;
                        }

            if (newProcedure.name == "")
                newProcedure.name  =   gotToken->content;

            L_PROCEDURE_WORD_DONE:
        }
        // ---------- END ----------
        else if (gotToken->type == TYPE_LINEEND)
        {
            break;
        }
    }

    // --- Check is the next token a begin of a code ---
    if (gotToken->type == TYPE_LINEEND)
    {
        if (newProcedure.content == "")
            if (ShowNextToken())
                if (nextToken->type == TYPE_CONTENT)
                {
                    GetToken();
                    newProcedure.content = gotToken->content;
                }
    }

    newProcedure.type = PROCEDURE;
    declarations.push_back (newProcedure);
}



//------------------------------------------------------



void    Parser::ParseMerge    ()
{
    Merge newMerge;

    while (GetTokenOnlyToLineEnd())
    {
        // ---------- SECTION NAMES ----------
        if ( (gotToken->type == TYPE_TEXT) || (gotToken->type == TYPE_CHARS) )
        {
            static bool gotFirst = false;

            if (!gotFirst)
            {
                newMerge.first  =  gotToken->content;
                gotFirst = true;
            }
            else
            {
                newMerge.second  =  gotToken->content;
                break;
            }
        }
        // ---------- END ----------
        else if (gotToken->type == TYPE_LINEEND)
        {
            break;
        }
    }

    merges.push_back (newMerge);
}
