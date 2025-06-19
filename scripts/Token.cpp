#include "Token.hpp"
using std::cout;

// ---------- VARIABLES ----------
vector <st_token> tokens;
int tokens_i = 0;

st_token* gotToken;
st_token* nextToken;

// ---------- FUNCTIONS ----------
bool GetToken ()
{
    if (tokens_i < tokens.size())
    {
        gotToken = &tokens[tokens_i++];
        return true;
    }
    return false;
}

//-----------------------------------------------------------
//-----------------------------------------------------------

bool ShowNextToken ()
{
    if (tokens_i < tokens.size())
    {
        nextToken = &tokens[tokens_i];
        return true;
    }
    return false;
}

//-----------------------------------------------------------
//-----------------------------------------------------------

bool LoadNumFromTokens (string beginToken, string finalToken, DWORD* destination)
{
    if ( gotToken->content == beginToken )
    {
        while (GetToken())
        {
            if (gotToken->type == NUMBER)
            {
                *destination = StrGetNum<DWORD>(&gotToken->content[0]);
            }
            else if (gotToken->type == PUNCTATOR)
            {
                if ( gotToken->content == finalToken ) break;
                else cout << "Unexpected punctator \n";
            }
            else if (gotToken->type == LINE_END)
            {
                break;
            }
            else
            {
                cout << "Unexpected token \n";
            }

            if (finalToken == "") break;
        }
        return true;
    }
    return false;
}


