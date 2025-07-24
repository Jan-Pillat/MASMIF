#include "Assembler.hpp"
#include <iostream> //debug
using namespace std;


//======================================================
//======================================================
Assembler::Assembler (string gotPath, PEData& gotPEData) :  projectPath(gotPath), baseData(gotPEData)
{
    cout << "Assembler Init" << endl;

    if (baseData.data.IsEmpty())
    {
        cout << "(STOP) - base data is empty" << endl;
        return;
    }

    InvokeMASM  ();
}

//======================================================
//======================================================

void Assembler::InvokeMASM ()
{
    cout << "Invoke MASM" << endl;

    SetCurrentDirectoryA    (&projectPath[0]);
    string asmResultPath    = projectPath + "\\result.asm";

    // ====== ML ======
    string mlCommand   = masmPath + "\\bin\\ml.exe /c /coff \"" + asmResultPath + "\"";

    // ====== LINK ======
    string imageBase        = ConvertNumberToHexString0x (baseData.OptionalHeader.ImageBase);
    string sectionAlignment = ConvertNumberToHexString0x (baseData.OptionalHeader.SectionAlignment);
    string fileAlignment    = ConvertNumberToHexString0x (baseData.OptionalHeader.FileAlignment);

    string linkCommand  = masmPath + "\\bin\\link.exe \"" + projectPath + "\\result.obj\"" + ' '
                        + "/subsystem:windows /map /pdb:test_result_PDB"
                        + " /base:"         + imageBase
                        + " /align:"        + sectionAlignment
                        + " /filealign:"    + fileAlignment;

    // ====== REALISE ======
    system (&mlCommand[0]);
    system (&linkCommand[0]);
}


