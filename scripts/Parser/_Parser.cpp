#include "_Parser.hpp"

void ParseTokens ()
{
    while (GetToken())
    {
        if (gotToken->type == KEYWORD)
        {
            if (auto position = generalKeywords.find(token); position != generalKeywords.end())
            {
                const auto& [key, attributes] = *position;
                foundKeyword = &attributes;
                attributes.parse();
                break;
            }
        }
    }
}
