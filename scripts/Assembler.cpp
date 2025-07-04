#include "Assembler.hpp"
#include <iostream> //debug
using namespace std;


//======================================================
//======================================================
Assembler::Assembler (Parser& gotParser, PEData& gotPEData) :  declarations(gotParser.declarations), thunks(gotParser.thunks), baseData(gotPEData)
{
    cout << "Assembler Init" << endl;

    if (baseData.data.IsEmpty())
        return;

    LoadBeginBase           ();
    ScanAndDeclareDLLs      ();
    ScanAndDeclareThunks    ();
    DeclareIncludes         ();
    SortDeclarations        ();
    WriteMASMCode           ();
    InvokeMASM              ();
}

//======================================================
//======================================================

DWORD Assembler::RvaToOffset (DWORD rva) //rva - relative virtual address
{
    for (int i = 0;  i<baseData.sections.size();   i++)
    {
        DWORD begin = baseData.sections[i].header.VirtualAddress;
        DWORD size  = baseData.sections[i].header.Misc.VirtualSize;

        if (rva >= begin  &&  rva < begin+size)
            return rva - begin + baseData.sections[i].header.PointerToRawData;
    }
    return 0;
}

char* Assembler::RvaToPointer (DWORD rva) //rva - relative virtual address
{
    return baseData.data.GetBeginPointer()+RvaToOffset(rva);
}

DWORD Assembler::VaToOffset (DWORD va) //va - virtual address
{
    return RvaToOffset (va - baseData.OptionalHeader.ImageBase);
}

char* Assembler::VaToPointer (DWORD va) //va - virtual address
{
    return baseData.data.GetBeginPointer()+VaToOffset(va);
}

//======================================================
//======================================================


void Assembler::LoadBeginBase ()
{
    if (0<baseData.sections.size())
        beginBase     = baseData.sections[0].header.VirtualAddress;


    for (int i=1;  i<baseData.sections.size();  i++)
        if (baseData.sections[i].header.VirtualAddress < beginBase)
            beginBase = baseData.sections[i].header.VirtualAddress;

    beginBase += baseData.OptionalHeader.ImageBase;
}

//======================================================
//======================================================

void Assembler::ScanAndDeclareDLLs ()
{
    cout << "ScanAndDeclareDLLs" << endl;

    // -- SAFEGUARD --
    if (baseData.data.IsEmpty())
        return;

    // -- MORE READABLE POINTER --
    char* pointer   = baseData.data.GetBeginPointer();

    cout << "Get VirtualAddress" << endl;
    DWORD importTableRva = baseData.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;

    cout << "Check VirtualAddress" << endl;
    if (importTableRva == 0)
    {
        cout << "  No import table!" << endl;
        return;
    }
    else
    {
        cout << "  Import table is OK, equals " << hex << importTableRva << dec<< endl;
    }

    IMAGE_IMPORT_DESCRIPTOR* dllDescriptor  = reinterpret_cast<IMAGE_IMPORT_DESCRIPTOR*>(pointer+RvaToOffset(importTableRva));

    //if RvaToOffset returned 0
    if (dllDescriptor == reinterpret_cast<IMAGE_IMPORT_DESCRIPTOR*>(pointer))
        return;

    while (dllDescriptor->Name != 0)
    {
        dllNames.emplace_back (&baseData.data[RvaToOffset(dllDescriptor->Name)]);

        DWORD  dllFuncPointerRVA    = dllDescriptor->FirstThunk + baseData.OptionalHeader.ImageBase - 4; //-4 because loop always makes +4
        DWORD* dllFuncNameRVAs      = reinterpret_cast<DWORD*>(pointer+RvaToOffset(dllDescriptor->OriginalFirstThunk));

        if (dllFuncPointerRVA != 0)
        while (*dllFuncNameRVAs != 0)
        {
            Declaration dllDeclaration;

            dllDeclaration.address  = dllFuncPointerRVA+=pointerSize;
            dllDeclaration.type     = DLL;
            dllDeclaration.size     = pointerSize;
            dllDeclaration.name     = reinterpret_cast<char*>(pointer+2+RvaToOffset(*dllFuncNameRVAs++));

            declarations.push_back (dllDeclaration);
        }
        dllDescriptor++;
    }
}

//======================================================
//======================================================

void Assembler::ScanAndDeclareThunks ()
{
    cout << "ScanAndDeclareThunks" << endl;
    for (int i = 0;  i<thunks.size();  i++)
    {
        char* thunkPointer      =   baseData.data.GetBeginPointer() + VaToOffset(thunks[i].address);
        DWORD currentAddress    =   thunks[i].address - thunkSize; //-thunkSize because loop uses +=thunkSize
        DWORD count             =   0;

        cout << "Start thunk jumps finding\n";
        while ((unsigned char)*(thunkPointer) == 0xFF  &&  *(thunkPointer+1) == 0x25) //jmp dword ptr []
        {
            if (thunks[i].count > 0   &&   count++ >= thunks[i].count)
                break;

            cout << "Jump nr." << count << endl;

            Declaration thunkDeclaration;

            thunkDeclaration.type    = THUNK;
            thunkDeclaration.size    = thunkSize;
            thunkDeclaration.address = currentAddress+=thunkSize;

            thunkPointer += 2;

            DWORD pointerValue = *reinterpret_cast<DWORD*>(thunkPointer);
            cout << "pointerValue = " << pointerValue << endl;

            thunkPointer += 4;

            for (int decl_i=0;  decl_i<declarations.size();  decl_i++)
            {
                if (declarations[decl_i].type == DLL)
                {
                    cout << hex;
                    cout << "    declarations[decl_i].address = "   << declarations[decl_i].address << endl;
                    cout << "    pointerValue = "                   << pointerValue                 << endl;
                    cout << dec;
                }

                if ( (declarations[decl_i].type == DLL)  &&  (declarations[decl_i].address == pointerValue) )
                {
                    cout << "Found matching dll! (" << declarations[decl_i].name << ")" << endl;
                    thunkDeclaration.name = declarations[decl_i].name;
                    break;
                }
            }

            declarations.push_back (thunkDeclaration);
        }
        cout << "Finish thunk jumps finding\n";
    }
}

//======================================================
//======================================================
static bool IsFileExist(const char* path)
{
    DWORD   attributes = GetFileAttributesA(path);
    return (attributes != INVALID_FILE_ATTRIBUTES) && !(attributes & FILE_ATTRIBUTE_DIRECTORY);
}

void Assembler::DeclareIncludes ()
{
    cout << "DeclareIncludes" << endl;

    if (!autoInclude)
    {
        cout << "    autoInclude = false";
        return;
    }


    for (int i = 0;  i<dllNames.size();  i++)
    {
        string pathOfIncludeINC = masmPath + "\\include\\" + dllNames[i];
        pathOfIncludeINC.resize( pathOfIncludeINC.size()-4 ); //delete .dll
        pathOfIncludeINC += ".inc";

        string pathOfIncludeLIB = masmPath + "\\lib\\" + dllNames[i];
        pathOfIncludeLIB.resize( pathOfIncludeLIB.size()-4 ); //delete .dll
        pathOfIncludeLIB += ".lib";

        if (IsFileExist (&pathOfIncludeINC[0]) && IsFileExist (&pathOfIncludeLIB[0]))
        {
            MASMcode_Includes   +=  "include ";
            MASMcode_Includes   +=  &pathOfIncludeINC[2];
            MASMcode_Includes   +=  "\r\n";

            MASMcode_Includes   +=  "includelib ";
            MASMcode_Includes   +=  &pathOfIncludeLIB[2];
            MASMcode_Includes   +=  "\r\n";
        }
    }
}

//======================================================
//======================================================

void Assembler::SortDeclarations ()
{
    sort  (declarations.begin(), declarations.end(), DeclarationComparison());
}

//======================================================
//======================================================

bool Assembler::IsNextDeclarationGroupable (size_t i)
{
    if ( (i+1<declarations.size())
      && (declarations[i+1].type == declarations[i].type)
      && (declarations[i+1].address - declarations[i].address == declarations[i].size) )
    {
        return true;
    }

    return false;
}

//------------------------------------------------------

DWORD Assembler::GetEndBase ()
{
    DWORD foundBase = baseData.sections[0].header.VirtualAddress + baseData.sections[0].header.SizeOfRawData;


    for (int i=1;  i<baseData.sections.size();  i++)
    {
        DWORD end =   baseData.sections[i].header.VirtualAddress  +  baseData.sections[i].header.SizeOfRawData;
        if (end > foundBase)
            foundBase = end;
    }

    return foundBase + baseData.OptionalHeader.ImageBase;
}

//------------------------------------------------------

void Assembler::WriteMASMCode ()
{
    // ---------- DEBUG INFO ----------
    cout << "WriteMASMCode" << endl;

    // ---------- VARIABLES ----------
    int labelCount      = 0;
    int dllCount        = 0;
    int thunkCount      = 0;

    DWORD origin        = 0;

    // ---------- DEBUG INFO ----------
    cout << "declarations count = " << declarations.size() << endl;

    // ---------- WRITE ----------
    for (int i = 0;  i<declarations.size();  i++)
    {
        cout << "i = " << i << endl;

        // ---- DECLARATION SEGMENTS ARE FIRST ----
        if (declarations[i].type == DECLARATION)
        {
            cout << "  It IS declaration" << endl;

            if ( declarations[i].name != "" )
                MASMcode_DeclarationSegments += "\r\n\r\n;---------- " + declarations[i].name + " ----------\r\n";
            else
                MASMcode_DeclarationSegments += "\r\n\r\n;---------- DECLARATION SEGMENT ----------\r\n";


            // -- DECLARATION SEGMENT --
            MASMcode_DeclarationSegments   +=  declarations[i].content + "\r\n";

            // -- MARGIN --
            MASMcode_DeclarationSegments   +=  "\r\n";

            continue;
        }



        if ( (declarations[i].address < beginBase) && (!declarations[i].intoNewSection) )
        {
            cout << "  Incorrect address: " << hex << "0x" << declarations[i].address << dec << endl;
            continue;
        }



        // ---- DESTINATION ----
        string* destination = &MASMcode_Declarations;

        if (declarations[i].intoNewSection)
        {
            if (declarations[i].type == SEGMENT)
            {
                destination = &MASMcode_NewRest;
            }

            else if (declarations[i].type == PROCEDURE)
            {
                destination = &MASMcode_NewCode;
            }

            else if (declarations[i].type == VARIABLE)
            {
                destination = &MASMcode_NewData;
            }
        }



        // ---- SIGNATURE ----
        if (declarations[i].type == SEGMENT)
        {
            *destination += "\r\n\r\n;========== SEGMENT:  " + declarations[i].name + " ==========\r\n";
        }

        else if ( (declarations[i].type == PROCEDURE) /*&& (declarations[i].content != "")*/ )
        {
            *destination += "\r\n\r\n;---------- PROCEDURE:  " + declarations[i].name + " ----------\r\n";
        }

        else if (declarations[i].type == VARIABLE)
        {
            if ( IsNextDeclarationGroupable(i) )
                *destination += "\r\n\r\n;---------- VARIABLES:  ----------\r\n";
            else
                *destination += "\r\n\r\n;---------- VARIABLE:  " + declarations[i].name + " ----------\r\n";
        }

        else if (declarations[i].type == DLL)
        {
            if ( IsNextDeclarationGroupable(i) )
                *destination += "\r\n\r\n;---------- DLL POINTERS:  ----------\r\n";
            else
                *destination += "\r\n\r\n;---------- DLL POINTER:  " + declarations[i].name + " ----------\r\n";
        }

        else if (declarations[i].type == THUNK)
        {
            if ( IsNextDeclarationGroupable(i) )
                *destination += "\r\n\r\n;---------- THUNKS:  ----------\r\n";
            else
                *destination += "\r\n\r\n;---------- THUNK:  " + declarations[i].name + " ----------\r\n";
        }



        // -------- ORIGIN NAD LABEL BEGIN --------
        if (!declarations[i].intoNewSection)
        {
            // ---- ORIGIN ----
            origin      = declarations[i].address-beginBase;

            if (origin > 0)
            {
                *destination   += "ORG ";
                ConvertNumberToHexString (*destination, origin);
                *destination   += "\r\n";
            }


            // -- LABEL BEGIN DECLARATION --
            if ( (declarations[i].type == SEGMENT)
            ||   (declarations[i].type == VARIABLE)
            ||   (declarations[i].type == PROCEDURE) )
            {
                *destination            +=  "____BEG_"        + to_string(labelCount) + ":\r\n";
                MASMcode_Publications   += "PUBLIC\t____BEG_" + to_string(labelCount) + "\r\n";
            }
        }



        // -------- CONTENT --------
        if (declarations[i].type == SEGMENT)
        {
            *destination   +=  ConvertContentNumbers(declarations[i].content) + "\r\n";
        }
        else if (declarations[i].type == VARIABLE)
        {
            while (true)
            {
                // -- DECLARATION --
                *destination   += declarations[i].name;

                // -- CONTENT --
                *destination   += '\t' + declarations[i].declaration + '\t' + ConvertContentNumbers(declarations[i].content) + "\r\n";

                if ( IsNextDeclarationGroupable(i) )
                    i++;
                else
                    break;
            }
        }
        else if (declarations[i].type == PROCEDURE)
        {
            // -- PROCEDURE DECLARATION --
            *destination   += declarations[i].name + "\tPROC\r\n";
            // -- CONTENT --
            if (declarations[i].content!="")
                *destination   += ConvertContentNumbers(declarations[i].content) + "\r\n";
            // -- FINISH --
            *destination   += declarations[i].name + "\tENDP\r\n";
        }
        else if (declarations[i].type == DLL)
        {
            // -- DLL FUNC POINTER DECLARATIONS --
            while (true)
            {
                *destination   +=  declarations[i].name + "_pointer" + "\tDWORD\t0FEFEFEFEh" + "\r\n";

                if ( IsNextDeclarationGroupable(i) )
                    i++;
                else
                    break;
            }

            // -- MARGIN --
            *destination   +=  "\r\n";

            dllCount++;
        }
        else if (declarations[i].type == THUNK)
        {
            // -- JUMP THUNK DECLARATIONS --
            while (true)
            {
                if (autoInclude)
                    *destination += "_";

                *destination   +=  declarations[i].name + ":\tjmp dword ptr [" + declarations[i].name + "_pointer]\r\n";

                if ( IsNextDeclarationGroupable(i) )
                    i++;
                else
                    break;
            }

            // -- MARGIN --
            *destination   +=  "\r\n";

            thunkCount++;
        }
        else
        {
            cout << "  unrecognized type  (" << declarations[i].type << ")\n";
        }



        // -------- LABEL END DECLARATION --------
        if (!declarations[i].intoNewSection)
        {
            if ( (declarations[i].type == SEGMENT)
            ||   (declarations[i].type == VARIABLE)
            ||   (declarations[i].type == PROCEDURE) )
            {
                *destination            +=  "____END_"        + to_string(labelCount) + ":\r\n";
                MASMcode_Publications   += "PUBLIC\t____END_" + to_string(labelCount) + "\r\n";
                labelCount++;
            }
        }
    } // LOOP END (for)

    // ---------- PREPARE FINAL MASM CODE ----------
    MASMcode += ".686p                  \r\n";
    MASMcode += ".model flat, stdcall   \r\n";
    MASMcode += "option casemap:none    \r\n";
    MASMcode += "                       \r\n";

    MASMcode += MASMcode_Includes;
    MASMcode += "\r\n\r\n\r\n";
    MASMcode += MASMcode_Publications;
    MASMcode += MASMcode_DeclarationSegments;

    MASMcode += "\r\n";
    MASMcode += ";################################################\r\n";
    MASMcode += ";########## ORIGINAL PROGRAM SECTIONS: ##########\r\n";
    MASMcode += ";################################################\r\n";
    MASMcode += "                       \r\n";
    MASMcode += "____main SEGMENT PARA 'CODE'\r\n";
    MASMcode += "____start:             \r\n";
    MASMcode += "                       \r\n";
    MASMcode += "                       \r\n";

    MASMcode += MASMcode_Declarations;

    MASMcode += "ORG ";
    ConvertNumberToHexString (MASMcode, GetEndBase());
    MASMcode += "\r\n";
    MASMcode += "____main ENDS\r\n";

    if (MASMcode_NewCode != "")
    {
        MASMcode += "\r\n\r\n";
        MASMcode += ";#######################################\r\n";
        MASMcode += ";########## NEW CODE SECTION: ##########\r\n";
        MASMcode += ";#######################################\r\n";
        MASMcode += "\r\n";
        MASMcode += "____code SEGMENT PARA 'CODE'";
        MASMcode += MASMcode_NewCode;
        MASMcode += "____code ENDS";
    }

    if (MASMcode_NewData != "")
    {
        MASMcode += "\r\n\r\n";
        MASMcode += ";#######################################\r\n";
        MASMcode += ";########## NEW DATA SECTION: ##########\r\n";
        MASMcode += ";#######################################\r\n";
        MASMcode += "\r\n";
        MASMcode += "____data SEGMENT PARA 'DATA'";
        MASMcode += MASMcode_NewData;
        MASMcode += "____data ENDS";
    }

    if (MASMcode_NewRest != "")
    {
        MASMcode += "\r\n\r\n";
        MASMcode += ";#######################################\r\n";
        MASMcode += ";########## NEW REST SECTION: ##########\r\n";
        MASMcode += ";#######################################\r\n";
        MASMcode += "\r\n";
        MASMcode += "____rest SEGMENT PARA 'CODE'";
        MASMcode += MASMcode_NewRest;
        MASMcode += "____rest ENDS";
    }

    MASMcode += "\r\n\r\n";
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
        string fileName = "test_result";
        // ====== ML ======
        string mlCommand = "C:\\masm32\\bin\\ml.exe /c /coff " + fileName + ".asm";

        // --- BASE ---
        string imageBase; ConvertNumberToHexString (imageBase, baseData.OptionalHeader.ImageBase);
        imageBase.resize (imageBase.size()-1);
        imageBase = "0x" + imageBase;

        // --- SECTION ALIGN ---
        string sectionAlignment; ConvertNumberToHexString (sectionAlignment, baseData.OptionalHeader.SectionAlignment);
        sectionAlignment.resize (sectionAlignment.size()-1);
        sectionAlignment = "0x" + sectionAlignment;

        // --- RAW DATA ALIGN ---
        string fileAlignment; ConvertNumberToHexString (fileAlignment, baseData.OptionalHeader.FileAlignment);
        fileAlignment.resize (fileAlignment.size()-1);
        fileAlignment = "0x" + fileAlignment;

        // ====== LINK ======
        string linkCommand  = "C:\\masm32\\bin\\link.exe " + fileName + ".obj" + ' '
                            + "/subsystem:windows /map /pdb:test_result_PDB"
                            + " /base:"      + imageBase;
                            + " /align:"     + sectionAlignment;
                            + " /filealign:" + fileAlignment;

        // ====== DEBUG INFO ======
        cout << "imageBase        = " << imageBase          << endl;
        cout << "sectionAlignment = " << sectionAlignment   << endl;
        cout << "fileAlignment    = " << fileAlignment      << endl;

        // ====== REALISE ======
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

    while (*i=='0')                                         //skip zero
        i++;

    if (IsAlphabetic(*i))                                   //letter must be preceded by a number
        i--;

    destination += i;                                       //write
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
