#include "Variable.hpp"

using namespace std;

vector <variable> variables;

void Parse_Variable ()
{
    variable newVariable = {"", 0x0, NON, 0x0, ""};

    newVariable.bytes = foundKeyword->value.variableDescription.bytes;
    newVariable.type  = foundKeyword->value.variableDescription.type;

    while (GetToken())
    {
        if (gotToken->type == IDENTIFIER)
        {
            newVariable.name = gotToken->content;
            cout << endl << newVariable.name << endl;
        }

        else if (gotToken->type == PUNCTATOR)
        {
            if (!LoadNumFromTokens("[", "]", &newVariable.address))
            {
                if ( gotToken->content == "=" )
                {
                    if (GetToken())
                    {
                        if ( (newVariable.type == TXT  &&  gotToken->type == TEXT)
                        ||   (newVariable.type == CPU  &&  gotToken->type == NUMBER)
                        ||   (newVariable.type == FPU  &&  gotToken->type == NUMBER) )
                        {
                            newVariable.value = gotToken->content;
                        }
                    }
                }
            }
        }

        else if (gotToken->type == KEYWORD)
        {
            if (!LoadNumFromTokens("ADDRESS", "", &newVariable.address))
            {
                if ( gotToken->type == KEYWORD  &&  gotToken->content == "EQU" )
                {
                    if (GetToken())
                    {
                        if ( (newVariable.type == TXT  &&  gotToken->type == TEXT)
                        ||   (newVariable.type == CPU  &&  gotToken->type == NUMBER)
                        ||   (newVariable.type == FPU  &&  gotToken->type == NUMBER) )
                        {
                            newVariable.value = gotToken->content;
                        }
                    }
                }
            }
        }


        //Jeżeli powyższe pobrania nowego tokenu nie dadzą oczekiwanego efektu, bo napotkany zostanie koniec linii, to ten skrypt zareaguje adekwatnie.
        if (gotToken->type == LINE_END)
        {
            break;
        }
    }

    return;
}

