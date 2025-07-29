
#include "AsmParser.hpp"

using namespace std;

//======================================================
//======================================================

AsmParser::AsmParser   (unordered_map<string, size_t>& gotTextsToDeclare, string& gotContent) : textsToDeclare(gotTextsToDeclare), content(gotContent)
{
    cout << "  AsmParser INIT\n";
    FindAndDeclareTexts ();
}

//======================================================
//======================================================

void    AsmParser::FindAndDeclareTexts ()
{
    cout << "  FindAndDeclareTexts\n";
    Lexer (tokens, &content[0]);

    while (GetToken())
    {
        if (gotToken->type == TYPE_WORD)
        {
            if (ShowNextToken())
                if (nextToken->type == TYPE_SPECIAL  &&  nextToken->content == ":") //Label which maybe is before command
                {
                    GetToken();
                    continue;
                }

            transform (gotToken->content.begin(), gotToken->content.end(), gotToken->content.begin(), ::toupper);

            if (assemblyCommands.find(gotToken->content) != assemblyCommands.end())
            {
                cout << "    Token is assembly command!" << endl;
                while (GetToken())
                {
                    if (gotToken->type == TYPE_TEXT)
                    {
                        textsToDeclare[gotToken->content] = gotToken->line;
                        break;
                    }
                    else if (gotToken->type == TYPE_LINEEND)
                    {
                        break;
                    }
                }
            }
        }

        // --- GOTO NEXT LINE ---
        while (GetToken())
            if (gotToken->type == TYPE_LINEEND)
                break;
    }
}

//======================================================
//======================================================

bool    AsmParser::GetToken    ()
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
bool    AsmParser::GetTokenOnlyToLineEnd    ()
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
bool    AsmParser::ShowNextToken    ()
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
