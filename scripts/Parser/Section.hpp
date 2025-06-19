#ifndef _HPP_Section_
#define _HPP_Section_

#include <windows.h>
#include <iostream>
#include <bitset>
#include <vector>
#include <string>

using std::vector;
using std::string;

#include "../Token.hpp"
#include "Keyword.hpp"

struct section
{
    string name;
    DWORD  characteristics;
};

extern vector <section> sections;

void Parse_Section ();
void PrintSections ();

#include "../Lexicon.hpp"

#endif
