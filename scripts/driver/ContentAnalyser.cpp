#include "ContentAnalyser.hpp"
#include <iostream>
using namespace std;

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
    Lexer (tokens, &(*content)[0]);
}

//---------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//FindAndWritePublications
//------------------------
//The function looks for procedure declarations in CONTENT to make their names visible so that labels can be created for x64dbg
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void ContentAnalyser::FindAndWritePublications ()
{
    while (GetToken())
    {
        if (gotToken->type == TYPE_WORD) // if got token is a word...
            if (assemblyCommands.find(GetUppercase(gotToken->content)) == assemblyCommands.end()) // if got token isn't an assembly command...
                if (ShowNextToken())
                    if (nextToken->type == TYPE_WORD) //if the next token is a word...
                        if (assemblyDeclarations.find(GetUppercase(nextToken->content)) != assemblyDeclarations.end()) // if the next token is an assembly declaration...
                        {
                            string& labelName = gotToken->content;
                            bool    dontPublic = false;

                            cout << "---------- " << labelName << endl;

                            if (nextToken->content == "PROC") //Skip procedure content - don't public anything inside procedure
                            {
                                while (GetToken())
                                    if (gotToken->type == TYPE_LINEEND)
                                        break;
                            }
                            else if (nextToken->content == "LABEL")
                            {
                                while (GetToken())
                                    if (gotToken->type == TYPE_LINEEND)
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
                            else
                            {
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
    }
}

//!===============================================================
//!===============================================================


void    ContentAnalyser::SetContent    (const string& content)
{
    this->content = &content;
}


void    ContentAnalyser::SetAndLexContent    (const string& content)
{
    this->content = &content;
    Lexer (tokens, &content[0]);
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
