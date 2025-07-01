
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


bool Parser::SetAddress (Declaration& destination)
{
    static DWORD newIndex = 1; // This is so that the declarations of new sections are in order

    // --- CHECK IS BEGIN CORRECT ---
    if ( (gotToken->type != TYPE_SPECIAL) || (gotToken->content != "[") )
        return false;

    // --- DON'T SET ADDRESS SECOND A TIME ---
    if (destination.address != 0)
    {
        while (GetTokenOnlyToLineEnd())
        {
            if ( ((gotToken->type == TYPE_SPECIAL) && (gotToken->content == "]"))  ||  (gotToken->type == TYPE_LINEEND) )
                return false;
        }
        return false;
    }

    // --- TRY TO SET ADDRESS ---
    bool result = false;

    if (gotToken->type == TYPE_SPECIAL)
    {
        if (gotToken->content == "[")
        {
            while (GetTokenOnlyToLineEnd())
            {
                if (gotToken->type == TYPE_NUMBER)
                {
                    destination.address         = StrGetNum<int>(&gotToken->content[0]);
                    destination.intoNewSection  = false;
                    result                      = true;
                }
                else if ((gotToken->type == TYPE_WORD) && (gotToken->content == "NEW"))
                {
                    destination.address         = newIndex++;
                    destination.intoNewSection  = true;
                    result                      = true;
                }
                else if ((gotToken->type == TYPE_SPECIAL) && (gotToken->content == "]") || (gotToken->type == TYPE_LINEEND))
                {
                    break;
                }
            }
        }
    }
    else if (gotToken->type == TYPE_WORD)
    {
        if (gotToken->content == "ADDRESS")
            if (ShowNextToken())
                if (nextToken->type == TYPE_NUMBER)
                {
                    GetToken();
                    destination.address         =   StrGetNum<DWORD>(&gotToken->content[0]);
                    destination.intoNewSection  =   false;
                    result                      =   true;
                }
                else if ( (nextToken->type == TYPE_WORD) && (nextToken->content == "NEW") )
                {
                    GetToken();
                    destination.address         =   newIndex++;
                    destination.intoNewSection  =   true;
                    result                      =   true;
                }
    }

    return result;
}

//------------------------------------------------------

bool Parser::SetSize (Declaration& destination)
{
    // --- CHECK IS BEGIN CORRECT ---
    if ( (gotToken->type != TYPE_SPECIAL) || (gotToken->content != "(") )
        return false;

    // -- DON'T SET SIZE A SECOND TIME --
    if (destination.size != 0)
    {
        while (GetTokenOnlyToLineEnd())
        {
            if ( ((gotToken->type == TYPE_SPECIAL) && (gotToken->content == ")"))  ||  (gotToken->type == TYPE_LINEEND) )
                return false;
        }
        return false;
    }

    // -- TRY TO SET SIZE --
    bool result = false;

    //  SIZE IN ( )
    if ( (gotToken->type == TYPE_SPECIAL) && (gotToken->content == "(") )
    {
        while (GetTokenOnlyToLineEnd())
        {
            if (gotToken->type == TYPE_NUMBER)
            {
                destination.size = StrGetNum<int>(&gotToken->content[0]);
            }
            else if ( (gotToken->type == TYPE_WORD) && (gotToken->content == "UNTIL") )
            {
                if (GetTokenOnlyToLineEnd())
                {
                    if (gotToken->type == TYPE_NUMBER)
                    {
                        destination.until        = true;
                        destination.untilBinNum  = (BYTE)StrGetNum<int>(&gotToken->content[0]);
                    }
                    else if (gotToken->type == TYPE_WORD)
                    {
                        auto position = assemblyKeyword.find(gotToken->content);
                        if (position != assemblyKeyword.end())
                        {
                            destination.until        = true;
                            destination.untilBinNum  = position->second;
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

    //  SIZE AFTER "SIZE"
    else if ( (gotToken->type == TYPE_WORD) && (gotToken->content == "SIZE") )
    {
        if (ShowNextToken())
            if (nextToken->type == TYPE_NUMBER)
            {
                GetToken();
                destination.size = StrGetNum<DWORD>(&gotToken->content[0]);
            }
            else if ( (nextToken->type == TYPE_WORD) && (nextToken->content == "UNTIL") )
            {
                if (ShowNextToken())
                    if (nextToken->type == TYPE_NUMBER)
                    {
                        destination.until        = true;
                        destination.untilBinNum  = (BYTE)StrGetNum<int>(&nextToken->content[0]);
                    }
                    else if (nextToken->type == TYPE_WORD)
                    {
                        auto position = assemblyKeyword.find(nextToken->content);
                        if (position != assemblyKeyword.end())
                        {
                            destination.until        = true;
                            destination.untilBinNum  = position->second;
                        }
                    }
            }
    }

    return result;
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
        // ---------- ADDRESS ----------
        if (!SetAddress(newSegment))
        // ---------- SIZE ----------
        if (!SetSize(newSegment))
        // ---------- CONTENT ----------
        if (gotToken->type == TYPE_CONTENT)
        {
            newSegment.content = gotToken->content;
        }
        // ---------- NAME ----------
        else if (gotToken->type == TYPE_WORD)
        {
            if (newSegment.name == "")
                newSegment.name  =   gotToken->content;
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

    auto position = generalKeywords.find(gotToken->content);
    newVariable.size = position->second.argument.attributes.bytes;

    newVariable.declaration = gotToken->content;

    while (GetTokenOnlyToLineEnd())
    {
        // ---------- ADDRESS ----------
        if (!SetAddress(newVariable))
        // ---------- VALUE ----------
        if (gotToken->type == TYPE_SPECIAL)
        {
            if ( (gotToken->content == "=") || (gotToken->content == ":") )
            {
                if (GetTokenOnlyToLineEnd())
                {
                    newVariable.content = gotToken->content;
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
            if (newVariable.name == "")
                newVariable.name  =   gotToken->content;
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
        // ---------- ADDRESS ----------
        if (!SetAddress(newProcedure))
        // ---------- SIZE ----------
        if (!SetSize(newProcedure))
        // ---------- CONTENT ----------
        if (gotToken->type == TYPE_CONTENT)
        {
            newProcedure.content = gotToken->content;
        }
        // ---------- NAME ----------
        else if (gotToken->type == TYPE_WORD)
        {
            if (newProcedure.name == "")
                newProcedure.name  =   gotToken->content;
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



//------------------------------------------------------



void    Parser::ParseThunk    ()
{
    Thunk newThunk;

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
                        newThunk.address = StrGetNum<int>(&gotToken->content[0]);
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
                        newThunk.count = StrGetNum<int>(&gotToken->content[0]);
                    }
                    else if ((gotToken->type == TYPE_SPECIAL) && (gotToken->content == ")") || (gotToken->type == TYPE_LINEEND))
                    {
                        break;
                    }
                }
            }
        }
        // ---------- NAME ----------
        if (gotToken->type == TYPE_WORD)
        {
            if (newThunk.count == 0)
                if (gotToken->content == "SIZE")
                    if (ShowNextToken())
                        if (nextToken->type == TYPE_NUMBER)
                        {
                            GetToken();
                            newThunk.count = StrGetNum<DWORD>(&gotToken->content[0]);
                            goto L_PROCEDURE_WORD_DONE;
                        }

            if (newThunk.address == 0)
                if (gotToken->content == "ADDRESS")
                    if (ShowNextToken())
                        if (nextToken->type == TYPE_NUMBER)
                        {
                            GetToken();
                            newThunk.address = StrGetNum<DWORD>(&gotToken->content[0]);
                            goto L_PROCEDURE_WORD_DONE;
                        }

            L_PROCEDURE_WORD_DONE:
        }
        // ---------- END ----------
        else if (gotToken->type == TYPE_LINEEND)
        {
            break;
        }
    }

    thunks.push_back (newThunk);
}



//------------------------------------------------------



void    Parser::ParseDeclaration    ()
{
    Declaration newDeclaration;

    while (GetTokenOnlyToLineEnd())
    {
        if (gotToken->type == TYPE_SPECIAL)
        {
            if (gotToken->content == "[")
            {
                while (GetTokenOnlyToLineEnd())
                {
                    if (gotToken->type == TYPE_NUMBER)
                    {
                        newDeclaration.address = StrGetNum<int>(&gotToken->content[0]);
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
            newDeclaration.content = gotToken->content;
        }
        // ---------- NAME ----------
        else if (gotToken->type == TYPE_WORD)
        {
            if (newDeclaration.address == 0)
                if (gotToken->content == "NR")
                    if (ShowNextToken())
                        if (nextToken->type == TYPE_NUMBER)
                        {
                            GetToken();
                            newDeclaration.address = StrGetNum<DWORD>(&gotToken->content[0]);
                            goto L_DECLARATION_WORD_DONE;
                        }

            if (newDeclaration.name == "")
                newDeclaration.name  =   gotToken->content;

            L_DECLARATION_WORD_DONE:
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
        if (newDeclaration.content == "")
            if (ShowNextToken())
                if (nextToken->type == TYPE_CONTENT)
                {
                    GetToken();
                    newDeclaration.content = gotToken->content;
                }
    }

    newDeclaration.type = DECLARATION;
    declarations.push_back (newDeclaration);
}
