#ifndef _HPP_Preprocessor_
#define _HPP_Preprocessor_

#include <string>
#include <windows.h>
#include "utils/GetPath.hpp"
#include "../CppCore/include/FileData.hpp"
#include "../CppCore/include/StringUtils.hpp"

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
