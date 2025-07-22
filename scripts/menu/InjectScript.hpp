#ifndef _HPP_Menu_InjectScript_
#define _HPP_Menu_InjectScript_


#include <iostream>
#include <windows.h>
#include "..\PEData.hpp"
#include "..\Token.hpp"
#include "..\Lexer.hpp"
#include "..\Parser.hpp"
#include "..\Assembler.hpp"
#include "..\MapParser.hpp"
#include "..\Injector.hpp"
#include "..\GetPath.hpp"
#include "..\..\CppCore\include\ReadableWinAPI.hpp"

extern string currentProjectName;

void Menu_InjectProjectScript ();
void Menu_InjectOtherScript   ();
void Menu_InjectScript        (const string& scriptPath);


#endif
