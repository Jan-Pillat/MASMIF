#include "Work.hpp"

static bool goBack;

static void WorkTitle ()
{
    Write   ("---------- ");
    Write   (currentProjectName);
    WriteLN (" ----------");
}

void Menu_Work_Back ()
{
    goBack = true;
}

void Menu_Work ()
{
    ConsoleInterface workMenu;

    // ---------- VISUAL ----------
    workMenu.buttonLeft[1]      = workMenu.buttonLeft[0];
    workMenu.buttonRight[1]     = workMenu.buttonRight[0];
    workMenu.additionalDrawing  = WorkTitle;

    // ---------- OPTIONS ----------
    workMenu.AddOption  (Menu_InjectProjectScript,  "Inject project script",  "", "Inject script form project folder into target exe.");
    workMenu.AddOption  (Menu_InjectOtherScript,    "Inject other script",    "", "Select script and inject it into target exe.");
    workMenu.AddOption  (Menu_OpenProjectFolder,    "Open project folder",    "", "Open folder that contains all project data.");
    workMenu.AddOption  (Menu_OpenTargetFolder,     "Open target folder",     "", "Open folder that contains the target to be modified.");
    workMenu.AddOption  (Menu_ChangeTarget,         "Change target",          "", "Set new target.");
    workMenu.AddOption  (Menu_Work_Back,            "Back",                   "", "Return to main menu.");
    workMenu.AddOption  (Menu_ExitProgram,          "Exit",                   "", "Exit to Windows.");

    // ---------- ACTION ----------
    goBack = false;

    while (true)
    {
        ConsoleUtils::Clear();
        workMenu.Start();

        if (goBack)
            break;
    }
}
