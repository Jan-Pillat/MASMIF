#include "GetSectionIndex.hpp"

int GetSectionIndex (PEData& pe, DWORD rva) //rva - relative virtual address
{
    for (int i = 0;  i<pe.sections.size();   i++)
    {

        DWORD begin = pe.sections[i].header.VirtualAddress  +  pe.OptionalHeader.ImageBase;

        DWORD size  = pe.sections[i].header.Misc.VirtualSize;
        DWORD end   = begin + size;
        DWORD aligned = size % pe.OptionalHeader.SectionAlignment;
        if (aligned != 0)
            end += pe.OptionalHeader.SectionAlignment - aligned;

        if (rva >= begin  &&  rva < end)
            return i;
    }

    return -1;
}
