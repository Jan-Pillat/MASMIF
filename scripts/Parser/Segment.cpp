
struct segment
{
    string name;
    DWORD  address;
    DWORD  size;
    string code;
};

vector <segment> segments;


void Parse_Segment ()
{
    segment newSegment = {"", 0, 0, ""};

    while (GetToken())
    {
        if (gotToken->type == IDENTIFIER)
        {
            newSegment.name = gotToken->content;
            cout << endl << newSegment.name << endl;
        }

        else if (gotToken->type == PUNCTATOR)
        {
            if (!LoadNumFromTokens("[", "]", &newSegment.address))
                if (!LoadNumFromTokens ("(", ")", &newSegment.size))
                    if (gotToken->type == LINE_END) break;
        }

        else if (gotToken->type == KEYWORD)
        {
            if (!LoadNumFromTokens ("ADDRESS", "", &newSegment.address))
                if (!LoadNumFromTokens("SIZE", "", &newSegment.size))
                    if (gotToken->type == LINE_END) break;
        }

        else if (gotToken->type == CODE)
        {
            newSegment.code = gotToken->content;
        }

        else if (gotToken->type == LINE_END)
        {
            break;
        }
    }

    if (newSegment.code == "")
    {
        if ( (ShowNextToken())
        &&   (nextToken->type == CODE)  )
        {
            GetToken();
            newSegment.code = (*gotToken).content;
        }
    }

    if (newSegment.address != 0) segments.push_back(newSegment);
}

//-----------------------------------------
//-----------------------------------------

void PrintSegments ()
{
    int segmentCount = segments.size();
    cout << "  Segment count = " << segmentCount << endl;
    for (int i = 0; i<segmentCount; i++)
    {
        cout << hex << uppercase;
        cout << "    (" << i << ")\n";
        cout << "    name    = "   << segments[i].name << endl;
        cout << "    address = 0x" << segments[i].address << endl;
        cout << "    size    = 0x" << segments[i].size << endl;
        cout << "    code    = "   << segments[i].code << endl;
        cout << dec;
    }
}
