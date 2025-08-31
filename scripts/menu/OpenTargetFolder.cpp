#include "OpenTargetFolder.hpp"
using namespace std;

void Menu_OpenTargetFolder ()
{
    FileData target;
    target.LoadTextFile (GetProjectPath()+"\\target.path");

    char  folderPath[MAX_PATH];
    char* filePart;

    if (GetFullPathNameA(target.GetBeginPointer(), MAX_PATH, folderPath, &filePart)  &&  filePart)
    {
        *filePart           = '\0';
        ShellExecuteA       (NULL, "open", folderPath, NULL, NULL, SW_SHOWNORMAL);
    }
}
