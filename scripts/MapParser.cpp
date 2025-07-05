
#include "MapParser.hpp"

#include <iostream> //Debug

using namespace std;

//======================================================
//======================================================

MapParser::MapParser   (vector<Token>& gotTokens,  vector<SectionToCopy>& gotSectionsToCopy,   vector<RawDataToCopy>& gotRawDataToCopy)
                            :  tokens (gotTokens),        sectionsToCopy (gotSectionsToCopy),         rawDataToCopy  (gotRawDataToCopy)
{
    PrepareSectionsToCopyAndMapDeclarations ();
    CalculateAndDeclareRawDataToCopy        ();
}

//======================================================
//======================================================

void    MapParser::PrepareSectionsToCopyAndMapDeclarations ()
{
        // --- find section declaration segment ---
    while (GetToken())
    {
        GetToken();
        if ( (gotToken->type != TYPE_WORD) || (gotToken->content != "Start") )
            continue;

        GetToken();
        if ( (gotToken->type != TYPE_WORD) || (gotToken->content != "Length") )
            continue;

        GetToken();
        if ( (gotToken->type != TYPE_WORD) || (gotToken->content != "Name") )
            continue;

        break; // found!
    }



    // --- find section declaration ---
    SectionToCopy  newSectionToCopy;
    while (true)
    {
        //----------------------------------- EXAMPLE
        //0001:00000000 00001111H ____main
        //-----------------------------------
        //0001      - skip section number
        //:         - skip
        //00000000  - skip
        //00001111H - get as count of bytes to copy (it's only for ____code, ____data and ____other)
        //____main  - get as section name
        //-----------------------------------


        // - FIND AND SKIP SECTION NUMBER -
        if (!GetToken()  ||  (gotToken->type==TYPE_WORD && gotToken->content=="Address") )
            break;
        if (gotToken->type != TYPE_NUMBER)
            continue;


        // - SKIP PUNCTATOR -
        if (!GetToken())
            break;
        if ( (gotToken->type != TYPE_SPECIAL) || (gotToken->content != ":") )
            continue;


        // - SKIP -
        if (!GetToken())
            break;
        if (gotToken->type != TYPE_NUMBER)
            continue;


        // - GET LENGTH -
        if (!GetToken())
            break;
        if (gotToken->type != TYPE_NUMBER)
            continue;

        newSectionToCopy.size = StrGetNum<DWORD>(&gotToken->content[0]);


        // - GET NAME -
        if (!GetToken())
            break;
        if (gotToken->type != TYPE_WORD)
            continue;

        newSectionToCopy.name = gotToken->content;

        // - FINISH -
        sectionsToCopy.push_back(newSectionToCopy);
    }



    // --- find element declaration ---
    MapDeclaration newMapDeclaration;
    while (true)
    {
        //----------------------------------- EXAMPLE
        //0001:00000030       _____PROC_0                00401030
        //-----------------------------------
        //0001      - skip
        //:         - skip
        //00000030  - skip
        //_____PROC_0-get as name
        //00401030  - get as virtual address
        //-----------------------------------


        // - FIND AND SKIP SECTION NUMBER -
        if (!GetToken())
            break;
        if (gotToken->type != TYPE_NUMBER)
            continue;


        // - SKIP PUNCTATOR -
        if (!GetToken())
            break;
        if ( (gotToken->type != TYPE_SPECIAL) || (gotToken->content != ":") )
            continue;


        // - SKIP RAW DATA OFFSET -
        if (!GetToken())
            break;
        if (gotToken->type != TYPE_NUMBER)
            continue;


        // - GET NAME -
        if (!GetToken())
            break;
        if (gotToken->type != TYPE_WORD)
            continue;

        newMapDeclaration.name = gotToken->content;

        // - GET VIRTUAL ADDRESS -
        if (!GetToken())
            break;
        if (gotToken->type != TYPE_NUMBER)
            continue;

        gotToken->content = "0x" + gotToken->content;
        newMapDeclaration.virtualAddress = StrGetNum<DWORD>(&gotToken->content[0]);


        // - FINISH -
        mapDeclarations.push_back(newMapDeclaration);
    }
}

//======================================================
//======================================================

// It is: ____BEG_
#define RAW_DATA_BEGIN 0x5F4745425F5F5F5F
// It is: ____END_
#define RAW_DATA_END 0x5F444E455F5F5F5F

void    MapParser::CalculateAndDeclareRawDataToCopy ()
{
    cout << "CalculateAndDeclareRawDataToCopy" << endl;
    cout << "  mapDeclarations.size() = " << mapDeclarations.size() << endl;

    for (int i = 0;  i<mapDeclarations.size();  i++)
    {
        if (mapDeclarations[i].name.length() >= 10)
        {
            if (*reinterpret_cast<unsigned long long*>(&mapDeclarations[i].name[1]) == RAW_DATA_BEGIN)
            {
                cout << "  mapDeclaration begin! (" << mapDeclarations[i].name << ")" << endl;

                string number = &mapDeclarations[i].name[9];

                for (int nextI = i+1;  nextI<mapDeclarations.size();  nextI++)
                    if (*reinterpret_cast<unsigned long long*>(&mapDeclarations[nextI].name[1]) == RAW_DATA_END)
                    {
                        cout << "    mapDeclaration end! (" << mapDeclarations[nextI].name << ")" << endl;
                        if (&mapDeclarations[nextI].name[9] == number)
                        {
                            RawDataToCopy  newRawDataToCopy;

                            newRawDataToCopy.virtualAddress = mapDeclarations[i].virtualAddress;
                            newRawDataToCopy.size           = mapDeclarations[nextI].virtualAddress - mapDeclarations[i].virtualAddress;

                            rawDataToCopy.push_back(newRawDataToCopy);

                            if (nextI-i == 1)
                                i++;

                            break;
                        }
                    }
                    else
                    {
                        cout << "  " << &mapDeclarations[nextI].name[1] << " != ____END_  " << hex << "0x" << *reinterpret_cast<unsigned long long*>(&mapDeclarations[nextI].name[1]) << " != 0x" << RAW_DATA_END << dec << endl;
                    }
            }
            else
            {
                cout << "  " << &mapDeclarations[i].name[1] << " != ____BEG_  " <<hex << "0x" << *reinterpret_cast<unsigned long long*>(&mapDeclarations[i].name[1]) << " != 0x" << RAW_DATA_BEGIN << dec << endl;
            }
        }
        else
        {
            cout << "  mapDeclarations[i].name.length() [name=" << mapDeclarations[i].name << ";length=" << mapDeclarations[i].name.length() << "] !>= 10" << endl;
        }
    }
}

//======================================================
//======================================================

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
