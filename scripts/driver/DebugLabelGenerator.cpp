
#include "DebugLabelGenerator.hpp"

//======================================================
//======================================================

DebugLabelGenerator::DebugLabelGenerator   (string&  gotPath, vector<DebugLabel>& gotDebugLabels,   PEData& gotPEData)
                            : path(gotPath),        debugLabels(gotDebugLabels),     base(gotPEData)
{
    GetFileName   ();
    PrepareLabels ();
    SetNewPath    ();
    SaveToFile    ();
}

//===============================================================================
//===============================================================================

void    DebugLabelGenerator::GetFileName ()
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
    transform(fileName.begin(),fileName.end(),fileName.begin(),::tolower);
}

//===============================================================================
//===============================================================================

void    DebugLabelGenerator::PrepareLabels ()
{
    string comments (" \"comments\": [");
    string labels   (" \"labels\": [");

    bool    itsFirstComment = true;
    bool    itsFirstLabel   = true;

    for (int i = 0;  i < debugLabels.size();  i++)
    {
        //if (debugLabels[i].type == LABEL) //COMMENT
        //{
            DWORD address = debugLabels[i].virtualAddress - base.OptionalHeader.ImageBase;

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
            labels += "   \"text\": \""    +   debugLabels[i].name + "\"\n";
            labels += "  }";
        //}
    }

    comments += "\n ],";
    labels   += "\n ]";

    finalFileData = "{\n" + labels + "\n}";
}

//===============================================================================
//===============================================================================

void    DebugLabelGenerator::SetNewPath ()
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

void    DebugLabelGenerator::SaveToFile ()
{
    FileData fileData       (finalFileData);
    fileData.SaveTextFile   (newPath);
}
