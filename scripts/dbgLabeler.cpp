
#include "dbgLabeler.hpp"

//======================================================
//======================================================

DbgLabeler::DbgLabeler   (string&  gotPath, vector<Declaration>& gotDeclarations,   PEData& gotPEData)
                            : path(gotPath),        declarations(gotDeclarations),     base(gotPEData)
{
    GetFileName   ();
    PrepareLabels ();
    SetNewPath    ();
    SaveToFile    ();
}

//===============================================================================
//===============================================================================

void    DbgLabeler::GetFileName ()
{
    char* begin         = &path[0];
    char* pathPointer   = begin;
    char* lastSlash     = begin;

    while (*pathPointer != '\0')
    {
        if (*pathPointer == '\\' || *pathPointer == '/')
            lastSlash  = pathPointer;

        pathPointer++;
    }

    fileName.assign(lastSlash+1);
}

//===============================================================================
//===============================================================================

void    DbgLabeler::PrepareLabels ()
{
    string comments (" \"comments\": [");
    string labels   (" \"labels\": [");

    bool    itsFirstComment = true;
    bool    itsFirstLabel   = true;

    for (int i = 0;  i < declarations.size();  i++)
    {
        if ( (declarations[i].type == VARIABLE)
        ||   (declarations[i].type == PROCEDURE) )
        {
            DWORD address = declarations[i].address - base.OptionalHeader.ImageBase;

            if (itsFirstLabel)
            {
                itsFirstLabel    = false;
                labels          += "\n";
            }
            else
            {
                labels          += ",\n";
            }
            labels += "  {\n";
            labels += "   \"module\": \""  +   fileName  + "\",\n";
            labels += "   \"address\": \"" +   ConvertNumberToHexString0x(address) + "\",\n";
            labels += "   \"manual\": true,\n";
            labels += "   \"text\": \""    +   declarations[i].name + "\"\n";
            labels += "  }";
        }
    }

    comments += "\n ],";
    labels   += "\n ]";

    finalFileData = "{\n" + labels + "\n}";
}

//===============================================================================
//===============================================================================

void    DbgLabeler::SetNewPath ()
{
    char* begin         = &path[0];
    char* pathPointer   = begin;
    char* lastPoint     = begin;
    bool  foundPoint    = false;

    while (*pathPointer != '\0')
    {
        if (*pathPointer == '.')
        {
            foundPoint = true;
            lastPoint  = pathPointer;
        }

        pathPointer++;
    }

    if (foundPoint)
        newPath.assign(begin, lastPoint-begin);
    else
        newPath = path;

    newPath += ".dd32";
}

//===============================================================================
//===============================================================================

void    DbgLabeler::SaveToFile ()
{
    FileData fileData       (finalFileData);
    fileData.SaveTextFile   (newPath);
}
