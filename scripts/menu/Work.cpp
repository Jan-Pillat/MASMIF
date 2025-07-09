#include "Work.hpp"



static void WorkTitle ()
{
    Write   ("---------- ");
    Write   (&currentProjectName[0]);
    WriteLN (" ----------");
}

void Menu_Work ()
{
    ConsoleInterface workMenu;

    // ---------- VISUAL ----------
    workMenu.buttonLeft[1]      = workMenu.buttonLeft[0];
    workMenu.buttonRight[1]     = workMenu.buttonRight[0];
    workMenu.additionalDrawing  = WorkTitle;

    // ---------- OPTIONS ----------
    workMenu.AddOption  (Menu_InjectScript,   "Inject script",    "", "Select script and inject it into target exe.");
    workMenu.AddOption  (Menu_ExitProgram,    "Exit",             "", "Exit to Windows.");

    // ---------- ACTION ----------
    while (true)
    {
        ConsoleUtils::Clear();
        workMenu.Start();
    }
}
