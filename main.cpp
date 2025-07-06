
#include <iostream>
#include <windows.h>
#include "CppCore/include/ConsoleUtils.hpp"
#include "CppCore/include/ConsoleInterface.hpp"
#include "scripts/menu/InjectScript.hpp"
#include "scripts/menu/ExitProgram.hpp"

using namespace std;



// ========== MAIN ========== \\

// TO DO...
    //mainMenu.AddOption(InjectScript, "New Project",       "", "Create new project.");
    //mainMenu.AddOption(ExitProgram,  "Existing Project",  "", "Use existing project.");

int main()
{
    ConsoleInterface mainMenu;

    // ---------- VISUAL ----------
    mainMenu.buttonLeft[1]  = mainMenu.buttonLeft[0];
    mainMenu.buttonRight[1] = mainMenu.buttonRight[0];

    // ---------- OPTIONS ----------
    mainMenu.AddOption(InjectScript, "Inject script",   "", "Select script and inject it to target exe.");
    mainMenu.AddOption(ExitProgram,  "Exit",            "", "Exit to Windows.");

    while (true)
    {
        ConsoleUtils::Clear();
        mainMenu.Start();
    }

    return 0;
}
