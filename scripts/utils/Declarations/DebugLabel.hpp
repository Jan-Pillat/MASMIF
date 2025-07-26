#ifndef _HPP_DebugLabel_
#define _HPP_DebugLabel_

    #include <string>
    #include <windows.h>

    using std::string;

struct DebugLabel
{
    DWORD   virtualAddress;
    string  name;

    DebugLabel () {}
    DebugLabel (const string& gotName, const DWORD gotVirtualAddress) : name(gotName), virtualAddress(gotVirtualAddress) {}
    DebugLabel (const char*   gotName, const DWORD gotVirtualAddress) : name(gotName), virtualAddress(gotVirtualAddress) {}
};

#endif
