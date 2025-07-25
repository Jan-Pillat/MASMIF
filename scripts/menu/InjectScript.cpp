#include "InjectScript.hpp"

using namespace std;


// ========== OPTION - INJECT ========== \\

void Menu_InjectProjectScript ()
{
    Menu_InjectScript (GetProjectPath() + "\\main.masmif");
}

//===============================================================================
//===============================================================================


void Menu_InjectOtherScript ()
{
    cout << "Enter script path: ";

    FileSelector userScript;
    userScript.GetPath_OpenFile();

    if (userScript.path[0] == '\0')
    {
        cout << "NO selected script!" << endl;
        system ("pause");
        return;
    }

    cout << &userScript.path[0] << endl;
    Menu_InjectScript (userScript.path);
}

//===============================================================================
//===============================================================================


void Menu_InjectScript (const string& scriptPath)
{
    system ("cls");

    Driver driver (scriptPath);

    return;
}
