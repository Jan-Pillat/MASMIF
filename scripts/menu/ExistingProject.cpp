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
    string path = GetProjectsPath() + "\\*";

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
