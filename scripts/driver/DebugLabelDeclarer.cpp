#include "DeclareLabels.hpp"

//===============================================================
//===============================================================

string DeclareLabels::DeclareLabels ()
{
    while (GetToken())
    {
        bool failed = true;

        if (gotToken->type == TYPE_WORD)
            if (GetToken())
                if (gotToken->type == TYPE_WORD)
                    if (assemblyDeclarations[gotToken->name] != assemblyDeclarations.end())
                    {
                        labels.emplance_back(targetLine);
                        targetLine = gotToken->line;
                        failed     = false;
                    }

        if (failed)
            while (GetToken())
                if (gotToken->type == TYPE_LINEEND)
                    continue;

        Code_GotoLine       (targetLine);
        Code_DeclareLabel   ();
    }

    while (IsContentNotFullyVerified())
    {
    }

    FinishConvertedText();

    return  converted;
}

//!===============================================================
//!===============================================================

inline bool AsmConverter::IsContentNotFullyVerified ()
{
    return (*pointer!='\0');
}

//---------------------------------------------------------------

inline void AsmConverter::FinishConvertedText()
{
    converted += begin;
}

//!===============================================================
//!===============================================================

bool    MapParser::GetToken    ()
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
bool    MapParser::GetTokenOnlyToLineEnd    ()
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
bool    MapParser::ShowNextToken    ()
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
