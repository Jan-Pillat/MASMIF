#include "OpenProjectFolder.hpp"
using namespace std;

void Menu_OpenProjectFolder ()
{
    string path = GetProjectsPath() + "\\" + currentProjectName;
    ShellExecuteA   (NULL, "open", &path[0], NULL, NULL, SW_SHOWNORMAL);
}
