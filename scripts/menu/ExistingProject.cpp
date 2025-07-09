#include "ExistingProject.hpp"
using namespace std;

static ConsoleInterface projectsMenu;

static void NoOptions ()
{
    return;
}

static void SelectProject ()
{
    currentProjectName = ConvertWstringToString(projectsMenu.options[projectsMenu.optionFocus].name);
    Menu_Work   ();
}

void Menu_ExistingProject ()
{

    // ---------- VISUAL ----------
    projectsMenu.buttonLeft[1]      = projectsMenu.buttonLeft[0];
    projectsMenu.buttonRight[1]     = projectsMenu.buttonRight[0];

    // ---------- Find projects ----------
    // Get path to "documents"
    string path;
    path.resize (MAX_PATH);
    HRESULT hr = SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, 0, &path[0]);

    //Check is it correct
    if (!SUCCEEDED(hr))
    {
        cout << "Can't find folder \"documents\"!" << endl;
        system ("pause");
        return;
    }
    path.resize (strlen(&path[0]));

    path += "\\MASMIF-projects\\*";

    WIN32_FIND_DATAA foundData;

    HANDLE hFind = FindFirstFileA(&path[0], &foundData);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (foundData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                string folderName = foundData.cFileName;

                if (folderName != "." && folderName != "..")
                    projectsMenu.AddOption(SelectProject, folderName,  "", "");
            }
        } while (FindNextFileA(hFind,&foundData));

        FindClose(hFind);
    }

    if (projectsMenu.options.size() == 0)
        projectsMenu.AddOption(NoOptions, "NO PROJECTS",  "", "");

    projectsMenu.Start();
}
