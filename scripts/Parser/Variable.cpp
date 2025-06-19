
/*
union VariableValue
{
    long long   cpuVal;
    double      fpuVal;
    char*       txtVal;

    ~VariableValue()
    {
        delete txtVal;
    }
};
*/
//using VariableValue = variant <long long, double, string>; <-- alternatywa pozwalaj¹ca u¿yæ zmienn¹ string.

struct variable
{
    string  name;
    DWORD   address;
    VarType type;
    BYTE    bytes;
    string  value;
};

vector <variable> variables;


void Parse_Variable ()
{
    variable newVariable = {"", 0x0, NON, 0x0, ""};

    //newVariable.bytes = LOBYTE(foundKeyword->value);
    //newVariable.type  = HIBYTE(foundKeyword->value);

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

