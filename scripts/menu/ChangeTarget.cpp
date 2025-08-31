#include "ChangeTarget.hpp"

using namespace std;

void Menu_ChangeTarget ()
{
    FileData target;
    FileSelector selectedTarget;

    target.LoadTextFile (GetProjectPath()+"\\target.path");

    // ---------- GET TARGET PATH ----------
    cout << "Set target: ";

    selectedTarget.settings.initialPath = target.GetBeginPointer();
    selectedTarget.GetPath_OpenFile();

    cout << selectedTarget.path << endl;

    // ---------- target.path ----------
    if (selectedTarget.path[0] == '\0')
    {
        cout << "\nNo selected file\n";
    }
    else
    {
        target.SetData      (selectedTarget.path);
        target.SaveTextFile (GetProjectPath()+"\\target.path");
    }

    // ---------- start work ----------
    system      ("pause & cls");
}
