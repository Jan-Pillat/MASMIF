
#include <iostream>
#include <windows.h>
#include "CppCore/include/ConsoleUtils.hpp"
#include "CppCore/include/ConsoleInterface.hpp"
#include "scripts/menu/NewProject.hpp"
#include "scripts/menu/ExistingProject.hpp"
#include "scripts/menu/ExitProgram.hpp"
#include "scripts/menu/Settings.hpp"

using namespace std;

void MenuTitle ()
{
    WriteLN ("---------- MASMIF ----------");
}


// ========== MAIN ========== \\

int main()
{
    ConsoleInterface mainMenu;

    // ---------- VISUAL ----------
    mainMenu.buttonLeft[1]      = mainMenu.buttonLeft[0];
    mainMenu.buttonRight[1]     = mainMenu.buttonRight[0];
    mainMenu.additionalDrawing  = MenuTitle;

    // ---------- OPTIONS ----------
    mainMenu.AddOption(Menu_NewProject,     "New Project",       "", "Create new project.");
    mainMenu.AddOption(Menu_ExistingProject,"Existing Project",  "", "Use existing project.");
    mainMenu.AddOption(Menu_Settings,       "Settings",          "", "MASM32 path, etc.");
    mainMenu.AddOption(Menu_ExitProgram,    "Exit",              "", "Exit to Windows.");

    // ---------- ACTION ----------
    while (true)
        mainMenu.Start();

    return 0;
}

