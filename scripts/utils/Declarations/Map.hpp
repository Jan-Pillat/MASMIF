#ifndef _HPP_Map_
#define _HPP_Map_

    #include <windows.h>

// ======================== MAP DECLARATIONS ========================

struct SectionToCopy
{
    DWORD   size;
    string  name;
};

struct RawDataToCopy
{
    DWORD   virtualAddress;
    DWORD   size;
};

struct MapDeclaration
{
    DWORD   virtualAddress;
    string  name;
};

#endif
