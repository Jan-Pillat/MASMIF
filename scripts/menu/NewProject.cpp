#include "NewProject.hpp"

using namespace std;

/*
Menu_NewProject:
    - Name (Create project folder)
    - Set target
    - Inject script
    - Inject project script
*/

void Menu_NewProject ()
{
    string newName, txtPath, projectPath, basePath;
    FileSelector selectedTarget;

    while (true)
    {
        // ---------- GET PROJECT NAME ----------
        system ("cls");             //Clear screen
        cout << "Set name:   ";     //Info
        getline (cin, newName);     //Read full line

        // ---------- CHECK IS IT CORRECT ----------
        char* pointer = &newName[0];

        while (IsDigitOrAlpha(*pointer) || *pointer=='_' || *pointer==' ')  //Cut exe name
            pointer++;

        if (*pointer != '\0')
        {
            cout << "Incorrect name! Try again..." << endl;
            system ("pause");
            continue;
        }

        break;
    }

    // ---------- GET TARGET PATH ----------
    cout << "Set target: ";
    selectedTarget.GetPath_OpenFile();
    cout << selectedTarget.path << endl;

    // ---------- Create project folder ----------
    // Get path to "documents"
    projectPath.resize (MAX_PATH);
    HRESULT hr = SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, 0, &projectPath[0]);

    //Check is it correct
    if (!SUCCEEDED(hr))
    {
        cout << "Can't find folder \"documents\"!" << endl;
        system ("pause");
        return;
    }
    projectPath.resize (strlen(&projectPath[0]));

    projectPath += "\\MASMIF-projects";
    CreateDirectoryA (&projectPath[0], NULL);

    projectPath += "\\" + newName;
    CreateDirectoryA (&projectPath[0], NULL);

    // ---------- target.path ----------
    txtPath = projectPath + "\\target.path";

    FileData saveTarget;
    saveTarget.SetData      (selectedTarget.path);
    saveTarget.SaveTextFile (txtPath);

    // ---------- base.exe ----------
    basePath = projectPath + "\\base.exe";

    CopyFileA   (&selectedTarget.path[0],
                 &basePath[0],
                 FALSE);

    // ---------- start work ----------
    system      ("pause & cls");

    currentProjectName = newName;
    Menu_Work   ();
}
