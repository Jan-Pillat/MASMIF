#include "Section.hpp"

using namespace std;

vector <section> sections;

void Parse_Section ()
{
    section newSection = {"", 0};

    while (GetToken())
    {
        if (gotToken->type == TEXT)
        {
            newSection.name = (*gotToken).content;
            cout << endl << newSection.name << endl;
        }

        else if (gotToken->type == KEYWORD)
        {
            if (auto position = sectionKeywords.find(&gotToken->content[0]); position != variables.end())
            {
                const auto& [key, attributes] = *position;
                cout << key << " = " << bitset<32>(attributes.value.integer) << endl;
                newSection.characteristics |= attributes.value.integer;
                break;
            }
        }

        else if (gotToken->type == LINE_END)
        {
            cout << "LINE END!" << endl;
            break;
        }

        else
        {
            cout << "Invalid expression!" << endl;
        }
    }

    if (newSection.name != "")
    {
        sections.push_back (newSection);
    }
}

//-----------------------------------------
//-----------------------------------------

void PrintSections ()
{
    int sectionCount = sections.size();
    cout << "  Section count = " << sectionCount << endl;
    for (int i = 0; i<sectionCount; i++)
    {
        cout << hex << uppercase;
        cout << "    (" << i << ")\n";
        cout << "    name = " << sections[i].name << endl;
        cout << "    characteristics = 0x" << sections[i].characteristics << " (" <<bitset<32>(sections[i].characteristics) << ')' << endl;
        cout << dec;
    }
}

