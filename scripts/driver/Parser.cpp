
#include "Parser.hpp"

#include <iostream> //Debug

using namespace std;

//======================================================
//======================================================

Parser::Parser   (vector<Token>& gotTokens, vector<Merge>& gotMerges, vector<Thunk>& gotThunks, vector<Declaration>& gotDeclarations)
                    :    tokens (gotTokens),       merges (gotMerges),       thunks (gotThunks),       declarations (gotDeclarations)
{
    while (GetToken())
    {
        if (gotToken->type != TYPE_WORD)
            continue;

        auto position = generalKeywords.find(GetUppercase(gotToken->content));
        if (position != generalKeywords.end())
        {
            (this->*position->second.func)();
        }
    }
/*
    cout << "declarations count:   " << declarations.size()     << endl;

    for (size_t i = 0;  i<declarations.size();  i++)
    {
        cout << "DECLARATION(" << i << ")"
            << endl << "\ttype = "      << declarationTypeDescription[declarations[i].type]
            << endl << "\tname = "      << declarations[i].name
            << endl << "\taddress = "   << declarations[i].address << hex << "(0x" << declarations[i].address << ')' << dec
            << endl;
    }
*/
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


bool Parser::IsItAddressBegin ()
{
    if ( (gotToken->type == TYPE_SPECIAL) && (gotToken->content == "[") )
        return true;

    if (!ShowNextToken())
        return false;

    if ( (gotToken->type == TYPE_WORD) && (!stricmp(gotToken->content.c_str(),"ADDRESS")) )
        if ( (nextToken->type == TYPE_SPECIAL) && (nextToken->content == ":") )
            return true;

    return false;
}

//------------------------------------------------------

bool Parser::SetAddress (Declaration& destination)
{
    static DWORD newIndex = 1; // This is so that the declarations of new sections are in order

    // --- CHECK IS BEGIN CORRECT ---
    if (!IsItAddressBegin())
        return false;

    // --- DON'T SET ADDRESS SECOND A TIME ---
    if (destination.address != 0)
    {
        if (gotToken->type == TYPE_SPECIAL)
        {
            while (GetTokenOnlyToLineEnd())
            {
                if ( ((gotToken->type == TYPE_SPECIAL) && (gotToken->content == "]"))  ||  (gotToken->type == TYPE_LINEEND) )
                    return false;
            }
        }
        else
        {
            GetToken(); //pop ADDRESS
            GetToken(); //pop :
        }
        return false;
    }

    // --- TRY TO SET ADDRESS ---
    bool result = false;

    if (gotToken->type == TYPE_SPECIAL)
    {
        while (GetTokenOnlyToLineEnd())
        {
            if (gotToken->type == TYPE_NUMBER)
            {
                destination.address         = StrGetNum<int>(&gotToken->content[0]);
                destination.intoNewSection  = false;
                result                      = true;
            }
            else if ((gotToken->type == TYPE_WORD) && (!stricmp(gotToken->content.c_str(),"NEW")))
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
    else if (gotToken->type == TYPE_WORD)
    {
        GetToken(); // pop ADDRESS

        if (!GetToken()) return false; // pop :

        if (gotToken->type == TYPE_NUMBER)
        {
            destination.address         =   StrGetNum<DWORD>(&gotToken->content[0]);
            destination.intoNewSection  =   false;
            result                      =   true;
        }
        else if ( (gotToken->type == TYPE_WORD) && (!stricmp(gotToken->content.c_str(),"NEW")) )
        {
            destination.address         =   newIndex++;
            destination.intoNewSection  =   true;
            result                      =   true;
        }
    }

    return result;
}

//------------------------------------------------------

bool Parser::IsItSizeBegin ()
{
    if ( (gotToken->type == TYPE_SPECIAL) && (gotToken->content == "(") )
        return true;

    if (!ShowNextToken())
        return false;

    if ( (gotToken->type == TYPE_WORD) && (!stricmp(gotToken->content.c_str(),"SIZE")) )
        if ( (nextToken->type == TYPE_SPECIAL) && (nextToken->content == ":") )
            return true;

    return false;
}


//------------------------------------------------------

bool Parser::SetSize (Declaration& destination)
{
    // --- CHECK IS BEGIN CORRECT ---
    if (!IsItSizeBegin())
        return false;

    // -- DON'T SET SIZE A SECOND TIME --
    if (destination.size != 0)
    {
        if (gotToken->type == TYPE_SPECIAL)
        {
            while (GetTokenOnlyToLineEnd())
            {
                if ( ((gotToken->type == TYPE_SPECIAL) && (gotToken->content == ")"))  ||  (gotToken->type == TYPE_LINEEND) )
                    return false;
            }
        }
        else
        {
            GetToken(); //pop SIZE
            GetToken(); //pop :
        }
        return false;
    }

    // -- TRY TO SET SIZE --
    bool result = false;

    //  SIZE IN ( )
    if (gotToken->type == TYPE_SPECIAL)
    {
        while (GetTokenOnlyToLineEnd())
        {
            if (gotToken->type == TYPE_NUMBER)
            {
                destination.size = StrGetNum<int>(&gotToken->content[0]);
            }
            else if ( (gotToken->type == TYPE_WORD) && (!stricmp(gotToken->content.c_str(),"UNTIL")) )
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
                        auto position = assemblyKeyword.find(GetUppercase(gotToken->content));
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
    else if (gotToken->type == TYPE_WORD)
    {
        GetToken(); // pop SIZE

        if (!GetToken()) // pop :
            return false;

        if (gotToken->type == TYPE_NUMBER)
        {
            destination.size = StrGetNum<DWORD>(&gotToken->content[0]);
        }
        else if ( (gotToken->type == TYPE_WORD) && (!stricmp(gotToken->content.c_str(),"UNTIL")) )
        {
            if (GetToken())
                if (nextToken->type == TYPE_NUMBER)
                {
                    destination.until        = true;
                    destination.untilBinNum  = (BYTE)StrGetNum<int>(&nextToken->content[0]);
                }
                else if (nextToken->type == TYPE_WORD)
                {
                    auto position = assemblyKeyword.find(GetUppercase(nextToken->content));
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

//------------------------------------------------------

bool Parser::SetUses (Declaration& destination)
{
    if (!ShowNextToken())
        return false;

    if ( (gotToken->type == TYPE_WORD) && (!stricmp(gotToken->content.c_str(),"USES")) && (nextToken->type==TYPE_SPECIAL) && (nextToken->content==":") )
    {
        GetToken(); //pop USES

        while (GetTokenOnlyToLineEnd()) //pop : and rest
            if (gotToken->type == TYPE_WORD)
            {
                if (asmRegisters.find(GetUppercase(gotToken->content)) != asmRegisters.end())
                {
                    destination.uses += " " + gotToken->content;
                }

                if (ShowNextToken()  &&  nextToken->type==TYPE_SPECIAL  && nextToken->content==",")
                {
                    GetToken();
                    continue;
                }

                break;
            }
            else break;


        if (destination.uses != "")
            return true;
    }

    return false;
}

//------------------------------------------------------

bool Parser::IsItParamBegin ()
{
    if (!ShowNextToken())
        return false;

    if ( (gotToken->type == TYPE_SPECIAL) && (gotToken->content == "(") )
        if ( (nextToken->type == TYPE_SPECIAL) && (nextToken->content == "(") )
            return true;

    if ( (gotToken->type == TYPE_WORD) && (!stricmp(gotToken->content.c_str(),"PARAM")) )
        if ( (nextToken->type == TYPE_SPECIAL) && (nextToken->content == ":") )
            return true;

    return false;
}

//------------------------------------------------------

bool Parser::SetParam (Declaration& destination)
{
    if (!IsItParamBegin())
        return false;

    GetToken();

    if (gotToken->content == "(")
    {
        while (GetTokenOnlyToLineEnd())
        {
            if ( (gotToken->type == TYPE_SPECIAL) && (gotToken->content == ")") )
            {
                if (ShowNextToken())
                    if ( (nextToken->type == TYPE_SPECIAL) && (nextToken->content == ")") )
                        GetToken();
                break;
            }
            else if (gotToken->type==TYPE_WORD)
            {
                if ( (ShowNextToken()) && (nextToken->type==TYPE_WORD) )
                {
                    if (!destination.parameters.empty())
                        destination.parameters += ", ";
                    destination.parameters += nextToken->content + ":" + gotToken->content;
                }

                GetToken();
            }
        }
    }
    else
    {
        while (GetTokenOnlyToLineEnd())
        {
            if (ShowNextToken())
            {
                if (nextToken->type==TYPE_WORD)
                {
                    if (!destination.parameters.empty())
                        destination.parameters += ", ";
                    destination.parameters += nextToken->content + ":" + gotToken->content;
                }

                GetToken();

                if (ShowNextToken())
                {
                    if ( (nextToken->type == TYPE_SPECIAL) && (nextToken->content == ",") )
                    {
                        GetToken();
                        continue;
                    }
                }
            }

            break;
        }
    }


    if (destination.parameters != "")
        return true;
    else
        return false;
}


//======================================================
//======================================================




void    Parser::ParseSection    ()
{
    Declaration newSection;

    while (GetTokenOnlyToLineEnd())
    {
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
            auto position = sectionKeywords.find(GetUppercase(gotToken->content));
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
    Declaration newSegment;

    while (GetTokenOnlyToLineEnd())
    {
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

    newVariable.declaration = gotToken->content;

    auto position = generalKeywords.find(GetUppercase(gotToken->content));
    if (position != generalKeywords.end())
        newVariable.size = position->second.argument.attributes.bytes;

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

    if (!newVariable.content.empty()  && !stricmp(newVariable.declaration.c_str(),"TEXT")  &&  *newVariable.content.cbegin()=='"'  &&  *(newVariable.content.cend()-1)=='"')
    {
        newVariable.size = 0;
        vector <Token> tokensFromContent;

        string convertedContent = AsmConverter(newVariable.content).ConvertSyntax();
        Lexer (tokensFromContent, convertedContent.c_str());

        for (size_t i=0; i<tokensFromContent.size(); i++)
        {
            if (tokensFromContent[i].type == TYPE_CHARS)
                newVariable.size += strlen (&tokensFromContent[i].content[0])-2; //-2*'\''

            else if (tokensFromContent[i].type == TYPE_NUMBER)
                newVariable.size += 1;
        }
    }

    newVariable.type = VARIABLE;
    declarations.push_back (newVariable);
}



//------------------------------------------------------



void    Parser::ParseOwnVar    ()
{
    if (!GetTokenOnlyToLineEnd() || gotToken->type!= TYPE_WORD)
    {
        while (GetTokenOnlyToLineEnd()) {}
        return;
    }
    else
    {
        ParseVariable ();
    }
}



//------------------------------------------------------



void    Parser::ParseProcedure    ()
{
    Declaration newProcedure;

    while (GetTokenOnlyToLineEnd())
    {
        // ---------- ADDRESS ----------
        if (!SetAddress(newProcedure))
        // ---------- PARAM ----------
        if (!SetParam(newProcedure))
        // ---------- SIZE ----------
        if (!SetSize(newProcedure))
        // ---------- USES ----------
        if (!SetUses(newProcedure))
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
    bool  gotFirst = false;

    while (GetTokenOnlyToLineEnd())
    {
        // ---------- SECTION NAMES ----------
        if ( (gotToken->type == TYPE_TEXT) || (gotToken->type == TYPE_CHARS) )
        {
            if (!gotFirst)
            {
                newMerge.first  =  GetString (gotToken->content);
                gotFirst = true;
            }
            else
            {
                newMerge.second =  GetString (gotToken->content);
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
                if (!stricmp(gotToken->content.c_str(),"SIZE"))
                    if (ShowNextToken())
                        if (nextToken->type == TYPE_NUMBER)
                        {
                            GetToken();
                            newThunk.count = StrGetNum<DWORD>(&gotToken->content[0]);
                            goto L_PROCEDURE_WORD_DONE;
                        }

            if (newThunk.address == 0)
                if (!stricmp(gotToken->content.c_str(),"ADDRESS"))
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
                if (!stricmp(gotToken->content.c_str(),"NR"))
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
