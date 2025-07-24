#ifndef _HPP_Menu_InjectScript_
#define _HPP_Menu_InjectScript_


#include <iostream>
#include <windows.h>
#include "..\Globals.hpp"
#include "..\PEData.hpp"
#include "..\Token.hpp"
#include "..\Lexer.hpp"
#include "..\Parser.hpp"
#include "..\Compiler.hpp"
#include "..\Assembler.hpp"
#include "..\MapParser.hpp"
#include "..\Injector.hpp"
#include "..\dbgLabeler.hpp"
#include "..\Preprocessor.hpp"
#include "..\utils\GetPath.hpp"
#include "..\..\CppCore\include\ReadableWinAPI.hpp"

void Menu_InjectProjectScript ();
void Menu_InjectOtherScript   ();
void Menu_InjectScript        (const string& scriptPath);


#endif
