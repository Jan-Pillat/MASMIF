#ifndef _HPP_Assembler_
#define _HPP_Assembler_

#include <string>
#include <vector>
#include <windows.h>
#include "Declarations.hpp"
#include "Parser.hpp"
#include "PEData.hpp"
#include "utils/ContentConverter.hpp"
#include "utils/ConvertNumberToHexString.hpp"
#include "../CppCore/include/FileData.hpp"
#include "../CppCore/include/ReadableWinAPI.hpp"

using std::string;
using std::vector;

class Assembler
{

public:

    Assembler (string gotPath, PEData& gotPEData);

private:

    void InvokeMASM ();

    PEData&  baseData;

    string projectPath;
    string masmPath = "C:\\masm32";

};

#endif
