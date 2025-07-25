#ifndef _HPP_BuildDriver_
#define _HPP_BuildDriver_

#include <string>
#include <vector>
#include <cstdlib>
#include <windows.h>

#include "../utils/GetPath.hpp"
#include "../utils/CppCore/include/PEData.hpp"
#include "../utils/CppCore/include/StringUtils.hpp"

using std::string;
using std::vector;

class BuildDriver
{

public:

    BuildDriver (PEData& gotPEData);

private:

    void InvokeMASM ();

    PEData&  baseData;

    string masmPath = "C:\\masm32";

};

#endif
