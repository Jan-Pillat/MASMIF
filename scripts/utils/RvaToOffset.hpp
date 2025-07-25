#ifndef _HPP_RvaToOffset_
#define _HPP_RvaToOffset_

#include <windows.h>
#include "CppCore/include/PEData.hpp"

int     RvaToOffset     (PEData& pe, DWORD rva);

#endif
