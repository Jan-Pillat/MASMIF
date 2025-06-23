#include "Assembler.hpp"
#include <iostream> //debug
using namespace std;


//======================================================
//======================================================
Assembler::Assembler (Parser& gotParser) :  declarations(gotParser.declarations)
{
    cout << "Assembler Init" << endl;
    LoadBaseInfo            ();
    LoadDLLs                ();
    WriteMASMCode           ();
    InvokeMASM              ();
}

//======================================================
//======================================================

void Assembler::LoadBaseInfo ()
{
    virtualBase = 0x401000;
}

//======================================================
//======================================================

void Assembler::LoadDLLs ()
{

}

//======================================================
//======================================================

void Assembler::WriteMASMCode ()
{
    cout << "WriteMASMCode" << endl;
    int segmentCount    = 0;
    int variableCount   = 0;
    //DWORD lastOrigin    = virtualBase;    <-- to remove unnecessary ORG between variables
    DWORD origin        = virtualBase;


    MASMcode += ".686                   \r\n";
    MASMcode += ".model flat, stdcall   \r\n";
    MASMcode += "option casemap:none    \r\n";
    MASMcode += "                       \r\n";
    MASMcode += "program SEGMENT        \r\n";
    MASMcode += "                       \r\n";
    MASMcode += "                       \r\n";

    cout << "declarations count = " << declarations.size() << endl;

    for (int i = 0;  i<declarations.size();  i++)
    {
        cout << "i = " << i << endl;


        if (declarations[i].address < virtualBase)
        {
            cout << "  Incorrect address: " << hex << "0x" << declarations[i].address << dec << endl;
            continue;
        }

        //lastOrigin  = origin;
        origin      = declarations[i].address-virtualBase;

        if (declarations[i].type == SEGMENT)
        {
            cout << "  It IS segment" << endl;

            // -- MARGIN --
            MASMcode += "\r\n";
            // -- SIGNATURE --
            MASMcode += ";---------- SEGMENT ----------\r\n";
            // -- ORIGIN --
            MASMcode += "ORG ";
            ConvertDWORDToHexString (origin, MASMcode);
            MASMcode += "\r\n";
            // -- BEGIN DECLARATION --
            MASMcode += "__S" + to_string(segmentCount) + "_" + declarations[i].name + "\tPROC" + "\r\n";
            MASMcode += "__S" + to_string(segmentCount) + "_" + declarations[i].name + "\tENDP" + "\r\n";
            // -- CONTENT --
            MASMcode += declarations[i].content + "\r\n";
            // -- END DECLARATION --
            MASMcode += "__S" + to_string(segmentCount) + "_" + declarations[i].name + "__END" + "\tPROC" + "\r\n";
            MASMcode += "__S" + to_string(segmentCount) + "_" + declarations[i].name + "__END" + "\tENDP" + "\r\n";
            // -- MARGIN --
            MASMcode += "\r\n";

            segmentCount++;
        }
        else if (declarations[i].type == VARIABLE)
        {
            cout << "  It IS variable" << endl;

            // -- ORIGIN --
            MASMcode += "ORG ";
            ConvertDWORDToHexString (origin, MASMcode);
            MASMcode += "\r\n";
            // -- DECLARATION --
            MASMcode += "__V" + to_string(variableCount) + "_" + declarations[i].name;

            // -- CONTENT --
            MASMcode += '\t' + declarations[i].declaration + '\t' + declarations[i].content + "\r\n";

            variableCount++;
        }
        else
        {
            cout << "  unrecognized type" << endl;
        }
    }

    MASMcode += "program ENDS           \r\n";
    MASMcode += "                       \r\n";
}

//======================================================
//======================================================

void Assembler::InvokeMASM ()
{
    cout << "InvokeMASM" << endl;
    FileData fileData       (MASMcode);
    if (fileData.SaveTextFile   ("transcription_test.asm"))
        cout << "SUCCESS!" << endl;
    else
        cout << "FAILED!" << endl;
}

//======================================================
//======================================================

void Assembler::ConvertDWORDToHexString (DWORD number, string& destination)
{
    static const char BinNumToChar[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

    char hexNumStr []
    {
        '0',
        BinNumToChar[(number&0xF0000000)>>28],
        BinNumToChar[(number&0x0F000000)>>24],
        BinNumToChar[(number&0x00F00000)>>20],
        BinNumToChar[(number&0x000F0000)>>16],
        BinNumToChar[(number&0x0000F000)>>12],
        BinNumToChar[(number&0x00000F00)>> 8],
        BinNumToChar[(number&0x000000F0)>> 4],
        BinNumToChar[(number&0x0000000F)>> 0],
        'h',
        '\0'
    };


    char* i = hexNumStr + 1;                                //+1 it's skip of the first zero

    while (*i=='0')
        i++;

    if (IsAlphabetic(*i))
        i--;

    destination += i;
}
