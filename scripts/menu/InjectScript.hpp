#ifndef _HPP_Menu_InjectScript_
#define _HPP_Menu_InjectScript_


#include <iostream>
#include <windows.h>

#include "../driver/_driver.hpp"
#include "../utils/GetPath.hpp"
#include "../utils/CppCore/include/ReadableWinAPI.hpp"


void Menu_InjectProjectScript ();
void Menu_InjectOtherScript   ();
void Menu_InjectScript        (const string& scriptPath);


#endif
