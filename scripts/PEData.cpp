#include "PEData.hpp"

PEData::PEData ()
{

}

PEData::PEData (const char* path)
{
    LoadPEFile(path);
}

bool PEData::LoadPEFile (const char* path)
{
    // ---------- LOAD FILE ----------
    data.LoadBinaryFile (path);

    if (data.IsEmpty())
        return false;

    // ---------- DOS HEADER ----------
    DOSHeader       = *reinterpret_cast<IMAGE_DOS_HEADER*>(data.GetBeginPointer());

    if (DOSHeader.e_magic != IMAGE_DOS_SIGNATURE)
        return false;

    // ---------- DOS PROGRAM ----------
    size_t dosProgramSize = DOSHeader.e_lfanew     - sizeof(IMAGE_DOS_HEADER);
    char* dosProgramBegin = data.GetBeginPointer() + sizeof(IMAGE_DOS_HEADER);

    DOSProgram.SetData( dosProgramBegin , dosProgramSize );

    // ---------- PE SIGNATURE ----------
    Signature       = *reinterpret_cast<ULONG*>(data.GetBeginPointer() + DOSHeader.e_lfanew);

    if (Signature != IMAGE_NT_SIGNATURE)
        return false;

    // ---------- PE HEADERS ----------
    FileHeader      = *reinterpret_cast<IMAGE_FILE_HEADER*>      (data.GetBeginPointer() + DOSHeader.e_lfanew + sizeof(Signature));
    OptionalHeader  = *reinterpret_cast<IMAGE_OPTIONAL_HEADER32*>(data.GetBeginPointer() + DOSHeader.e_lfanew + sizeof(Signature) + sizeof(FileHeader));

    IMAGE_SECTION_HEADER*   sectionHeaders  =   reinterpret_cast<IMAGE_SECTION_HEADER*>(data.GetBeginPointer() + DOSHeader.e_lfanew + sizeof(IMAGE_NT_HEADERS32));

    for (int i=0;  i<FileHeader.NumberOfSections;  i++)
    {
        sections.emplace_back(&sectionHeaders[i], data.GetBeginPointer()+sectionHeaders[i].PointerToRawData);
    }

    return true;
}
