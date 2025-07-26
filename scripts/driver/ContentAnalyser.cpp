#include "ContentAnalyser.hpp"

//===============================================================
//===============================================================

string ContentAnalyser::GetPublications ()
{
    LexContent              ();
    FindAndWritePublications();
    return  publications;
}

//!===============================================================
//!===============================================================

inline void ContentAnalyser::LexContent ()
{
    Lexer lexer (tokens, &content[0]);
}

//---------------------------------------------------------------

inline void ContentAnalyser::FindAndWritePublications ()
{
    while (GetToken())
    {
        if (gotToken->type == TYPE_WORD)
            if (ShowNextToken())
                if (nextToken->type == TYPE_WORD)
                    if (assemblyDeclarations.find(nextToken->content) != assemblyDeclarations.end())
                    {
                        string& labelName = gotToken->content;
                        bool    dontPublic = false;

                        if (nextToken->content == "PROC") //Skip procedure content - don't public anything inside procedure
                        {
                            while (GetToken())
                                if (gotToken->type == TYPE_WORD  &&  gotToken->content == "ENDP")
                                    break;
                        }
                        else if (nextToken->content == "STRUCT")
                        {
                            while (GetToken())
                                if (gotToken->type == TYPE_WORD  &&  gotToken->content == "ENDS")
                                    break;

                            dontPublic = true;
                        }
                        else if (nextToken->content == "MACRO")
                        {
                            while (GetToken())
                                if (gotToken->type == TYPE_WORD  &&  gotToken->content == "ENDM")
                                    break;

                            dontPublic = true;
                        }

                        if (!dontPublic)
                            publications += "PUBLIC\t" + labelName + "\r\n";
                    }

        //Goto next line
        if (gotToken->type != TYPE_LINEEND)
            while (GetToken())
                if (gotToken->type == TYPE_LINEEND)
                    break;

        //Take off line end
        GetToken();
    }
}

//!===============================================================
//!===============================================================


bool    ContentAnalyser::GetToken    ()
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
bool    ContentAnalyser::ShowNextToken    ()
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
