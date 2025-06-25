
void Lexer::Unescape    (string& txt)
{/*
    static const char unescapeTable[] {
        '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0',
        '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0',
         ' ', '!', '"', '#', '$', '%', '&','\'', '(', ')', '*', '+', ',', '-', '.', '/',
        '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=', '>', '?',
         '@','\A','\B', 'C', 'D','\E','\F', 'G', 'H', 'I', 'J', 'K', 'L', 'M','\N', 'O',
         'P', 'Q', 'R', 'S', 'T', 'U', 'W', 'X', 'Y', 'Z', '[','\\', ']', '^', '_', '`',
         'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '[','\\', ']', '^', '_',
         '`','\a','\b', 'c', 'd','\e','\f', 'g', 'h', 'i', 'j', 'k', 'l', 'm','\n', 'o',
         'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '{', '|', '}', '~', ' ',
    }*/

    char* pointer       = &txt[0];
    char* begin         = ++pointer;
	char* destination   = begin

    //Find text termination
    while (true)
    {
        while (*pointer!='\0' && *pointer!='\\')
		{
            *destination++ = *pointer++;
		}

        if (*pointer=='\\')
        {
            pointer++;
            if (*pointer == 'a')    *destination++ = '\a'; else
            if (*pointer == 'b')    *destination++ = '\b'; else
            if (*pointer == 'e')    *destination++ = '\e'; else
            if (*pointer == 'f')    *destination++ = '\f'; else
            if (*pointer == 'n')    *destination++ = '\n'; else
            if (*pointer == 'r')    *destination++ = '\r'; else
            if (*pointer == 't')    *destination++ = '\t'; else
            if (*pointer == 'v')    *destination++ = '\v'; else
            if (*pointer == '0')    *destination++ = '\0'; else
            else {   *destination++ = *pointer;  }

            pointer++;
        }
        else
        {
            *destination++ = '\0';
            break;
        }
    }

    if (&txt[0]=='"')
        destination -= 3;   // delete: ",0

    txt.resize(destination-begin);
}
