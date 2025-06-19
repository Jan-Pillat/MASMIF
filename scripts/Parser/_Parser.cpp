

void ParseTokens ()
{
    while (GetToken())
    {
        if (gotToken->type == KEYWORD)
        {
            for (keyword key : generalKeywords)
            {
                if (!strcmp(key.name, &gotToken->content[0]))
                {
                    foundKeyword = &key;
                    key.parse();
                    break;
                }
            }
        }
    }
}
