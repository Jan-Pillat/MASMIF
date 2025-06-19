#ifndef _HPP_Segment_
#define _HPP_Segment_

#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
using std::vector;
using std::string;

#include "../Token.hpp"

struct segment
{
    string name;
    DWORD  address;
    DWORD  size;
    string code;
};

extern vector <segment> segments;


void Parse_Segment ();
void PrintSegments ();

#endif
