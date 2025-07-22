#include "GetPath.hpp"

static string gotDocumentsPath;
static string gotProjectsPath;
static bool   hasPath = false;

inline void SetPaths ()
{
    if (!hasPath)
    {
        gotDocumentsPath.resize (MAX_PATH);
        HRESULT hr = SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, 0, &gotDocumentsPath[0]);

        //Check is it correct
        if (!SUCCEEDED(hr))
        {
            gotDocumentsPath.clear();
            gotProjectsPath.clear();
        }
        else
        {
            gotDocumentsPath.resize (strlen(&gotDocumentsPath[0]));

            gotProjectsPath = gotDocumentsPath + "\\MASMIF-projects";

            hasPath = true;
        }
    }
}

string GetDocumentsPath ()
{
    SetPaths();
    return gotDocumentsPath;
}

string GetProjectsPath ()
{
    SetPaths();
    return gotProjectsPath;
}
