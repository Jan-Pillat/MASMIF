#ifndef _HPP_GetMasmifProjectsFolder_
#define _HPP_GetMasmifProjectsFolder_

#include <windows.h>
#include <shlobj.h>
#include <string>
#include "..\Globals.hpp"

using std::string;

string GetDocumentsPath     ();
string GetAllProjectsPath   ();
string GetProjectPath       ();

#endif
