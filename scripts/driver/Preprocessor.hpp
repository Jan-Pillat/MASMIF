#ifndef _HPP_Preprocessor_
#define _HPP_Preprocessor_

#include <string>
#include <cstring>
#include <windows.h>
#include "../utils/GetPath.hpp"
#include "../utils/CppCore/include/FileData.hpp"
#include "../utils/CppCore/include/StringUtils.hpp"
#include "../utils/CppCore/include/ConsoleUtils.hpp"

using std::string;

class Preprocessor
{

public:

//! ---------- CONSTRUCTORS ----------

    Preprocessor ();
    Preprocessor (const string& beginDirectory);

//! ---------- METHODS ----------

    string  ApplyIncludes (const char* gotCode);

//! ---------- VARIABLES ----------

    string  currentDirectory;
};

#endif
