#ifndef _HPP_GetSectionIndex_
#define _HPP_GetSectionIndex_

#include <windows.h>
#include "CppCore/include/PEData.hpp"

int     GetSectionIndex     (PEData& pe, DWORD rva);

#endif
