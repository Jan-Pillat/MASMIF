#include "Preprocessor.hpp"
#include <iostream> //debug
using namespace std;
using namespace ConsoleUtils;


//======================================================
//======================================================
Preprocessor::Preprocessor ()
{
    currentDirectory = GetProjectPath();
    currentFile      = "MAIN";
}
Preprocessor::Preprocessor (const string& beginDirectory)
{
    currentDirectory = beginDirectory;
    currentFile      = "BEGIN SCRIPT";
}

//======================================================
//======================================================

string Preprocessor::ApplyIncludes (const char* gotCode)
{
    FileData data;

    //! ----- INIT VARIABLES -----
    string newCode      = "";
    const char* begin   = gotCode;
    const char* pointer = begin;

    while (true)
    {
        //! ----- FIND INCLUDING -----
        // -- Find any punctator --
        while (*pointer != '\0' && !IsPunctator(*pointer))
            pointer++;

        // -- Skip commentary --
        if (*pointer == ';')
        {
            while (*pointer != '\r' && *pointer != '\n' && *pointer != '\0')
                pointer++;
        }

        // -- Skip chars --
        if (*pointer == '\'')
        {
            pointer++;

            while (*pointer != '\r' && *pointer != '\n' && *pointer != '\0' && *pointer != '\'')
                pointer++;
        }

        // -- Skip text --
        if (*pointer == '"')
        {
            pointer++;

            while (*pointer != '\r' && *pointer != '\n' && *pointer != '\0' && *pointer != '"')
                pointer++;
        }

        // -- Pass if it is # or \0 --
        if (*pointer != '#' && *pointer != '\0')
        {
            pointer++;
            continue;
        }

        //! ----- BEGIN OR DATA END IS FOUND -----

        //Set end of normal code
        const char* end = pointer;

        //Copy normal code
        newCode.append(begin, end-begin);

        //Stop if no includings
        if (*pointer == '\0')
            return newCode;

        //Begin is found
        pointer++;
        begin  = pointer;

        //Skip beginning punctator
        pointer++;
        //Skip blanks
        while (IsBlank(*pointer) && *pointer!='\0')
            pointer++;

        //Skip if path is empty
        if (*pointer == '#')
        {
            SetColors (YELLOW,BLACK);
            cout << "      (SKIP) - Path is empty in file " << currentDirectory << endl;
            SetColors (WHITE,BLACK);

            pointer++;
            begin = pointer;
            continue;
        }

        //Read command
        bool    asBinary    = false;

        if (*pointer == '(')
        {
            begin         = ++pointer;

            while (*pointer != '\0' && *pointer != ')')
                pointer++;

            //Check is not closed
            if (*pointer == '\0')
            {
                SetColors(RED,BLACK);
                cout << "      (STOP) - Including has not closing bracket in file " << currentDirectory << endl;
                SetColors(WHITE,BLACK);
                return newCode;
            }

            string command (begin, pointer-begin);

            if (!stricmp (&command[0], "binary"))
                asBinary = true;

            pointer++;
            begin = pointer;
        }

        //! ----- GET PATH -----
        //Find end
        while (*pointer!='#' && *pointer!='\0')
            pointer++;

        //Stop if no closing #
        if (*pointer == '\0')
        {
            SetColors(RED,BLACK);
            cout << "      (STOP) - Including has not closing hashtag in file " << currentDirectory << endl;
            SetColors(WHITE,BLACK);
            return newCode;
        }

        //Skip ending punctator
        pointer++;

        //Remove blanks at the end of the path
        end = pointer-2;  //pointer = after hashtag,  pointer-1 = hashtag,  pointer-2 = before hashtag

        while (IsBlank(*end))
            end--;

        if (*end == '#' || *end == ')')
            end = begin;
        else
            end++;

        //Stop if path is empty.
        if (end == begin)
        {
            SetColors(YELLOW,BLACK);
            cout << "      (SKIP) - Path is empty in file " << currentDirectory << endl;
            SetColors(WHITE,BLACK);
            begin = pointer;
            continue;
        }

        //Get path.
        string path (begin, end-begin);


        //! ----- CHECK PATH -----
        begin = &path[0];

        const char* pathPointer = begin;
        const char* lastSlash   = begin;
        bool        isFullPath  = false;
        bool        foundSlash  = false;

        while (*pathPointer != '\0')
        {
            if (*pathPointer == '\\' || *pathPointer == '/')
            {
                foundSlash = true;
                lastSlash  = pathPointer;
            }

            if (*pathPointer == ':')
                isFullPath = true;

            pathPointer++;
        }


        //! ----- SET NEW DIRECTORY -----
        string previousFile      = currentFile;
        string previousDirectory = currentDirectory;
        string newDirectory   (begin, lastSlash-begin);
        string fullPath;

        if (isFullPath)
        {
            fullPath            = begin;
            currentDirectory    = newDirectory;
        }
        else
        {
            fullPath            = currentDirectory + "\\" + begin;
            currentDirectory    = currentDirectory + "\\" + newDirectory;
        }

        char* fileName;
        string finalPath;
        finalPath.resize(MAX_PATH);

        GetFullPathNameA (&fullPath[0], MAX_PATH, &finalPath[0], &fileName);

        finalPath.resize(strlen(&finalPath[0]));

        //! ----- LOAD FILE -----
        data.LoadTextFile(finalPath);

        if (data.IsEmpty())
        {
            SetColors(YELLOW,BLACK);
            cout << "      (SKIP) - cannot load file from path \"" << finalPath << "\" Which is in file \"" << currentFile << "\".   ERROR = " << data.GetErrorDescribePointer() << endl;
            SetColors(WHITE,BLACK);
        }
        else
        {
            currentFile = finalPath;
            //! ----- INCLUDE DATA -----
            newCode += ApplyIncludes(data.GetBeginPointer());
        }

        currentDirectory    =   previousDirectory;
        currentFile         =   previousFile;
        begin               =   pointer;
    }
}
