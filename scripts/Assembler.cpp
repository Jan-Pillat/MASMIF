#include "Assembler.hpp"
#include <iostream> //debug
using namespace std;


//======================================================
//======================================================
Assembler::Assembler (Parser& gotParser) :  declarations(gotParser.declarations)
{
    cout << "Assembler Init" << endl;
    LoadPEHeaders           ();
    ScanAndDeclareDLLs      ();
    WriteMASMCode           ();
    InvokeMASM              ();
}

//======================================================
//======================================================

void Assembler::LoadPEHeaders ()
{
    PEHeaders.OptionalHeader.ImageBase = 0x400000;
    PEHeaders.OptionalHeader.BaseOfCode = 0x1000;
    programBase         = PEHeaders.OptionalHeader.ImageBase + PEHeaders.OptionalHeader.BaseOfCode;
}

//======================================================
//======================================================

void Assembler::ScanAndDeclareDLLs ()
{

}

//======================================================
//======================================================

void Assembler::WriteMASMCode ()
{
    // ---------- DEBUG INFO ----------
    cout << "WriteMASMCode" << endl;

    // ---------- VARIABLES ----------
    string  MASMcode_Publications;
    string  MASMcode_Declarations;

    int segmentCount    = 0;
    int variableCount   = 0;
    int procedureCount  = 0;

    //DWORD lastOrigin    = 0;    <-- to remove unnecessary ORG between variables
    DWORD origin        = 0;

    // ---------- DEBUG INFO ----------
    cout << "declarations count = " << declarations.size() << endl;

    // ---------- WRITE ----------
    for (int i = 0;  i<declarations.size();  i++)
    {
        cout << "i = " << i << endl;


        if (declarations[i].address < programBase)
        {
            cout << "  Incorrect address: " << hex << "0x" << declarations[i].address << dec << endl;
            continue;
        }

        // -- SIGNATURE --
        if (declarations[i].type == SEGMENT)
            MASMcode_Declarations += "\r\n\r\n;========== SEGMENT:  " + declarations[i].name + " ==========\r\n";

        else if ( (declarations[i].type == PROCEDURE) /*&& (declarations[i].content != "")*/ )
            MASMcode_Declarations += "\r\n\r\n;---------- PROCEDURE:  " + declarations[i].name + " ----------\r\n";

        // -- ORIGIN --
        //lastOrigin  = origin;
        origin      = declarations[i].address-programBase;

        if (origin > 0)
        {
            MASMcode_Declarations   += "ORG ";
            ConvertNumberToHexString (MASMcode_Declarations, origin);
            MASMcode_Declarations   += "\r\n";
        }

        // -- DECLARATION
        if (declarations[i].type == SEGMENT)
        {
            cout << "  It IS segment" << endl;

            // -- LABEL BEGIN DECLARATION --
            MASMcode_Declarations   +=  "____SEGM_"        + to_string(segmentCount) + ":\r\n";
            MASMcode_Publications   += "PUBLIC\t____SEGM_" + to_string(segmentCount) + "\r\n";
            // -- CONTENT --
            MASMcode_Declarations   +=  ConvertContentNumbers(declarations[i].content) + "\r\n";
            // -- LABEL END DECLARATION --
            MASMcode_Declarations   +=  "____ENDS_"        + to_string(segmentCount) + ":\r\n";
            MASMcode_Publications   += "PUBLIC\t____ENDS_" + to_string(segmentCount) + "\r\n";
            // -- MARGIN --
            MASMcode_Declarations   +=  "\r\n";

            segmentCount++;
        }
        else if (declarations[i].type == VARIABLE)
        {
            cout << "  It IS variable" << endl;

            // -- LABEL BEGIN DECLARATION --
            MASMcode_Declarations   +=  "____DATA_"        + to_string(variableCount) + ":\r\n";
            MASMcode_Publications   += "PUBLIC\t____DATA_" + to_string(variableCount) + "\r\n";

            // -- DECLARATION --
            MASMcode_Declarations   += declarations[i].name;

            // -- CONTENT --
            MASMcode_Declarations   += '\t' + declarations[i].declaration + '\t' + ConvertContentNumbers(declarations[i].content) + "\r\n";

            // -- LABEL END DECLARATION --
            MASMcode_Declarations   +=  "____ENDD_"        + to_string(variableCount) + ":\r\n";
            MASMcode_Publications   += "PUBLIC\t____ENDD_" + to_string(variableCount) + "\r\n";

            variableCount++;
        }
        else if (declarations[i].type == PROCEDURE)
        {
            cout << "  It IS procedure" << endl;
            // -- LABEL BEGIN DECLARATION --
            MASMcode_Declarations   +=  "____PROC_"        + to_string(procedureCount) + ":\r\n";
            MASMcode_Publications   += "PUBLIC\t____PROC_" + to_string(procedureCount) + "\r\n";
            // -- PROCEDURE DECLARATION --
            MASMcode_Declarations   += declarations[i].name + "\tPROC\r\n";
            // -- CONTENT --
            if (declarations[i].content!="")
                MASMcode_Declarations   += ConvertContentNumbers(declarations[i].content) + "\r\n";
            // -- FINISH --
            MASMcode_Declarations   += declarations[i].name + "\tENDP\r\n";
            // -- LABEL END DECLARATION --
            MASMcode_Declarations   +=  "____ENDP_"        + to_string(procedureCount) + ":\r\n";
            MASMcode_Publications   += "PUBLIC\t____ENDP_" + to_string(procedureCount) + "\r\n";

            procedureCount++;
        }
        else
        {
            cout << "  unrecognized type" << endl;
        }
    }

    // ---------- PREPARE FINAL MASM CODE ----------
    MASMcode += ".686p                  \r\n";
    MASMcode += ".model flat, stdcall   \r\n";
    MASMcode += "option casemap:none    \r\n";
    MASMcode += "                       \r\n";
    MASMcode += ".code                  \r\n";
    MASMcode += "____start:             \r\n";
    MASMcode += "                       \r\n";
    MASMcode += "                       \r\n";

    MASMcode += MASMcode_Publications;
    MASMcode += MASMcode_Declarations;

    MASMcode += "END ____start          \r\n";
}

//======================================================
//======================================================

void Assembler::InvokeMASM ()
{
    cout << "InvokeMASM" << endl;

    FileData fileData       (MASMcode);
    if (fileData.SaveTextFile   ("test_result.asm"))
    {
        string mlCommand = "C:\\masm32\\bin\\ml.exe /c /coff test_result.asm";

        string base; ConvertNumberToHexString (base, PEHeaders.OptionalHeader.ImageBase);
        base.resize (base.size()-1);
        base = "0x" + base;

        string linkCommand = "C:\\masm32\\bin\\link.exe /subsystem:windows /map /pdb:test_result_PDB /base:" + base + ' ' + "test_result.obj";


        system (&mlCommand[0]);
        system (&linkCommand[0]);

    }
    else
    {
        cout << "Can't create ASM file!" << endl;
    }
}

//======================================================
//======================================================

static const char BinNumToChar[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

void Assembler::ConvertNumberToHexString (string& destination, long long number)
{

    char hexNumStr []
    {
        '0',
        BinNumToChar[(number&0xF000000000000000)>>60],
        BinNumToChar[(number&0x0F00000000000000)>>56],
        BinNumToChar[(number&0x00F0000000000000)>>52],
        BinNumToChar[(number&0x000F000000000000)>>48],
        BinNumToChar[(number&0x0000F00000000000)>>44],
        BinNumToChar[(number&0x00000F0000000000)>>40],
        BinNumToChar[(number&0x000000F000000000)>>36],
        BinNumToChar[(number&0x0000000F00000000)>>32],
        BinNumToChar[(number&0x00000000F0000000)>>28],
        BinNumToChar[(number&0x000000000F000000)>>24],
        BinNumToChar[(number&0x0000000000F00000)>>20],
        BinNumToChar[(number&0x00000000000F0000)>>16],
        BinNumToChar[(number&0x000000000000F000)>>12],
        BinNumToChar[(number&0x0000000000000F00)>> 8],
        BinNumToChar[(number&0x00000000000000F0)>> 4],
        BinNumToChar[(number&0x000000000000000F)>> 0],
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

//======================================================
//======================================================

string Assembler::ConvertContentNumbers (string& content)
{
    //return content;

    char* begin   = &content[0];
    char* end     = begin;
    char* pointer = begin;

    bool  isHex   = false;
    bool  isBin   = false;

    string converted = "";

    //Check full text
    while (*pointer!='\0')
    {
        //Skip commentary
        if (*pointer==';' || *pointer=='#')
        {
            *pointer++ = ';';

            while (*pointer != '\r' && *pointer != '\n')    pointer++;
            while (*pointer == '\r' || *pointer == '\n')    pointer++;
        }

        //Convert numbers
        if ( IsDigit(*pointer)
        ||   *pointer=='$' && IsHexDigit(*(pointer+1))
        ||   *pointer=='%' && IsDigit(*(pointer+1))  )
        {
            //Copy last content
            end = pointer;
            converted.append(begin, end-begin);


            //Check type
            if (*pointer == '$')
            {
                isHex = true;
                pointer++;
            }
            else if (*pointer == '%')
            {
                isBin = true;
                pointer++;
            }
            else if (*pointer == '0')
            {
                char* pointerToX = pointer;

                while (*pointerToX == '0')
                    pointerToX++;

                if ( (*pointerToX == 'x') || (*pointerToX == 'X') )
                {
                    isHex   = true;
                    pointer = pointerToX+1;
                }
            }

            //Real begin of number
            begin = pointer;

            //Find end
            while (true)
            {
                while (IsAlOrNum(*pointer))
                    pointer++;

                if (*pointer!='.')
                    break;
                else
                    pointer++;
            }

            if ( (isHex) && IsAlphabetic(*begin))
            {
                converted += "0";
            }

            end = pointer;
            converted.append(begin, end-begin);
            if (isHex)
            {
                isHex = false;
                converted += "h";
            }
            if (isBin)
            {
                isBin = false;
                converted += "b";
            }

            begin = pointer;
        }
        pointer++;
    }

    converted += begin;

    return  converted;
}
