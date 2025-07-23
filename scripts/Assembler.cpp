#include "Assembler.hpp"
#include <iostream> //debug
using namespace std;


//======================================================
//======================================================
Assembler::Assembler (string& gotPath, vector<Thunk>& gotThunks, vector<Declaration>& gotDeclarations, PEData& gotPEData) :  projectPath(gotPath), declarations(gotDeclarations), thunks(gotThunks), baseData(gotPEData)
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
    cout << "Scan And Declare DLLs" << endl;

    // -- SAFEGUARD --
    if (baseData.data.IsEmpty())
        return;

    // -- MORE READABLE POINTER --
    char* pointer   = baseData.data.GetBeginPointer();

    DWORD importTableRva = baseData.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;

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
    cout << "Scan and declare thunks" << endl;
    for (int i = 0;  i<thunks.size();  i++)
    {
        char* thunkPointer      =   baseData.data.GetBeginPointer() + VaToOffset(thunks[i].address);
        DWORD currentAddress    =   thunks[i].address - thunkSize; //-thunkSize because loop uses +=thunkSize
        DWORD count             =   0;

        while ((unsigned char)*(thunkPointer) == 0xFF  &&  *(thunkPointer+1) == 0x25) //jmp dword ptr []
        {
            if (thunks[i].count > 0   &&   count++ >= thunks[i].count)
                break;

            Declaration thunkDeclaration;

            thunkDeclaration.type    = THUNK;
            thunkDeclaration.size    = thunkSize;
            thunkDeclaration.address = currentAddress+=thunkSize;

            thunkPointer += 2;

            DWORD pointerValue = *reinterpret_cast<DWORD*>(thunkPointer);

            thunkPointer += 4;

            for (int decl_i=0;  decl_i<declarations.size();  decl_i++)
            {

                if ( (declarations[decl_i].type == DLL)  &&  (declarations[decl_i].address == pointerValue) )
                {
                    thunkDeclaration.name = declarations[decl_i].name;
                    break;
                }
            }

            declarations.push_back (thunkDeclaration);
        }
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
        cout << "    autoInclude = false" << endl;
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

DWORD Assembler::GetOriginalSectionsSize ()
{
    cout << "  Get Original Sections Size\n";

    // ----- FIND FARTHEST SECTION END -----
    DWORD   end  =  baseData.sections[0].header.VirtualAddress  +  baseData.sections[0].header.Misc.VirtualSize;

    for (int i=1;  i<baseData.sections.size();  i++)
    {
        DWORD   foundEnd = baseData.sections[i].header.VirtualAddress  +  baseData.sections[i].header.Misc.VirtualSize;
        DWORD   align    = foundEnd % baseData.OptionalHeader.SectionAlignment;

        if (align != 0)
            foundEnd +=  baseData.OptionalHeader.SectionAlignment - align;

        if (end < foundEnd)
            end = foundEnd;
    }

    // ----- FIND SMALLEST SECTION BEGIN -----
    DWORD   begin =  baseData.sections[0].header.VirtualAddress;

    for (int i=1;  i<baseData.sections.size();  i++)
    {
        if (begin > baseData.sections[i].header.VirtualAddress)
            begin = baseData.sections[i].header.VirtualAddress;
    }

    return end-begin;
}

//------------------------------------------------------

void Assembler::WriteMASMCode ()
{
    // ---------- DEBUG INFO ----------
    cout << "Write MASM Code" << endl;

    // ---------- VARIABLES ----------
    int labelCount      = 0;
    int dllCount        = 0;
    int thunkCount      = 0;

    DWORD origin        = 0;

    // ---------- WRITE ----------
    for (int i = 0;  i<declarations.size();  i++)
    {
        // ---- DECLARATION SEGMENTS ARE FIRST ----
        if (declarations[i].type == DECLARATION)
        {
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
                *destination   += "ORG " + ConvertNumberToHexString (origin) + "\r\n";


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

    MASMcode += "\r\n\r\n";
    MASMcode += "ORG " + ConvertNumberToHexString (GetOriginalSectionsSize()-1);
    MASMcode += "\r\n";
    MASMcode += "____MAIN_FINISH:\r\n";
    MASMcode += "db 0CCh \r\n";
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

    SetCurrentDirectoryA    (&projectPath[0]);
    string asmResultPath    = projectPath + "\\result.asm";

    FileData fileData       (MASMcode);
    if (fileData.SaveTextFile   (asmResultPath))
    {
        // ====== ML ======
        string mlCommand = masmPath + "\\bin\\ml.exe /c /coff \"" + asmResultPath + "\"";

        // --- BASE ---
        string imageBase = ConvertNumberToHexString (baseData.OptionalHeader.ImageBase);
        imageBase.resize (imageBase.size()-1);
        imageBase = "0x" + imageBase;

        // --- SECTION ALIGN ---
        string sectionAlignment = ConvertNumberToHexString (baseData.OptionalHeader.SectionAlignment);
        sectionAlignment.resize (sectionAlignment.size()-1);
        sectionAlignment = "0x" + sectionAlignment;

        // --- RAW DATA ALIGN ---
        string fileAlignment = ConvertNumberToHexString (baseData.OptionalHeader.FileAlignment);
        fileAlignment.resize (fileAlignment.size()-1);
        fileAlignment = "0x" + fileAlignment;

        // ====== LINK ======
        string linkCommand  = masmPath + "\\bin\\link.exe \"" + projectPath + "\\result.obj\"" + ' '
                            + "/subsystem:windows /map /pdb:test_result_PDB"
                            + " /base:"         + imageBase
                            + " /align:"        + sectionAlignment
                            + " /filealign:"    + fileAlignment;
        cout << linkCommand << endl;

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
