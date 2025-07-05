
#include "Injector.hpp"

#include <iostream> //Debug

using namespace std;

//======================================================
//======================================================

Injector::Injector   (string& gotPath,      PEData& gotPEData,  PEData& gotResultData,  vector<SectionToCopy>& gotSectionsToCopy,  vector<RawDataToCopy>& gotRawDataToCopy, vector<Merge>& gotMerges)
                   :  targetPath (gotPath), base   (gotPEData), result (gotResultData),        sectionsToCopy (gotSectionsToCopy),        rawDataToCopy  (gotRawDataToCopy),       merges (gotMerges)
{
    IncludeNewSections      ();
    MergeSections           ();
    //CorrectVirtualSize      ();
    RewriteRawData          ();
    CorrectRawDataSize      ();
    Inject                  ();
}

//======================================================
//======================================================

void Injector::IncludeNewSections ()
{
    cout << "IncludeNewSections\n";

    if (sectionsToCopy.size() <= 1)
        return;


    for (int i = 0;  i<sectionsToCopy.size();  i++)
    {
        cout << "  Section nr." << i << "; name = " << sectionsToCopy[i].name << endl;

        IMAGE_SECTION_HEADER    header;

        header.Misc.VirtualSize     = result.sections[i].header.Misc.VirtualSize;
        header.VirtualAddress       = result.sections[i].header.VirtualAddress;
        header.SizeOfRawData        = result.sections[i].header.SizeOfRawData;
        header.PointerToRawData     = 0;
        header.PointerToRelocations = 0;
        header.PointerToLinenumbers = 0;
        header.NumberOfRelocations  = 0;
        header.NumberOfLinenumbers  = 0;

        //! ---------- CODE ----------
        if (sectionsToCopy[i].name == "____code")
        {
            header.Characteristics      = IMAGE_SCN_CNT_CODE | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_EXECUTE;

            memcpy (header.Name, ".newCode", 8);

            base.FileHeader.NumberOfSections += 1;
            base.sections.emplace_back (&header, result.sections[i].rawData.GetBeginPointer());
        }
        //! ---------- DATA ----------
        else if (sectionsToCopy[i].name == "____data")
        {
            header.Characteristics      = IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE;

            memcpy (header.Name, ".newData", 8);

            base.FileHeader.NumberOfSections += 1;
            base.sections.emplace_back (&header, result.sections[i].rawData.GetBeginPointer());
        }
        //! ---------- OTHER ----------
        else if (sectionsToCopy[i].name == "____othe")
        {
            header.Characteristics      = IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE | IMAGE_SCN_CNT_CODE | IMAGE_SCN_MEM_EXECUTE;

            memcpy (header.Name, ".newRest", 8);

            base.FileHeader.NumberOfSections += 1;
            base.sections.emplace_back (&header, result.sections[i].rawData.GetBeginPointer());
        }
    }
}

//======================================================
//======================================================

void Injector::MergeSections ()
{/*
    cout << "MergeSections\n";

    for (i = 0;  i < merges.size();  i++)
    {
        if (merges[i].first ==

        for (int iOfBase = 0;  iOfBase<base.sections.size();  iOfBase++)
        {
        }

        DWORD baseSectionBegin = base.sections[].header.VirtualAddress;
        DWORD baseSectionEnd   = baseSectionEnd + base.sections[].header.VirtualSize;

        for (int iOfBase = 0;  iOfBase<base.sections.size();  iOfBase++)
        {
            if (base.sections[iOfBase].header.VritualAddress)
        }

        DWORD otherSectionBegin=

    }
*/}

//======================================================
//======================================================


void Injector::CorrectVirtualSize ()
{
    cout << "CorrectVirtualSize\n";

    for (int i = 0;  i<base.sections.size();  i++)
    {
        DWORD size  = base.sections[i].header.Misc.VirtualSize;
        DWORD aligned = size % base.OptionalHeader.SectionAlignment;
        if (aligned != 0)
            base.sections[i].header.Misc.VirtualSize  +=  base.OptionalHeader.SectionAlignment - aligned;
    }
}

//======================================================
//======================================================


void Injector::CorrectRawDataSize ()
{
    cout << "CorrectRawDataOffsets\n";

    for (int i = 0;  i<base.sections.size();  i++)
        base.sections[i].header.SizeOfRawData = base.sections[i].rawData.GetLength();
}

//======================================================
//======================================================

int GetSectionIndex (PEData& pe, DWORD rva) //rva - relative virtual address
{
    for (int i = 0;  i<pe.sections.size();   i++)
    {
        cout << "section ";
        cout.write((char*)&pe.sections[i].header.Name[0], 8);
        cout << endl;

        DWORD begin = pe.sections[i].header.VirtualAddress  +  pe.OptionalHeader.ImageBase;

        DWORD size  = pe.sections[i].header.Misc.VirtualSize;
        DWORD end   = begin + size;
        DWORD aligned = size % pe.OptionalHeader.SectionAlignment;
        if (aligned != 0)
            end += pe.OptionalHeader.SectionAlignment - aligned;

        cout << hex;
        cout << "  ImageBase = 0x" << pe.OptionalHeader.ImageBase << endl;
        cout << "  begin = 0x" << begin << endl;
        cout << "  end   = 0x" << end   << endl;
        cout << "  size  = 0x" << size  << endl;
        cout << "  rva   = 0x" << rva   << endl;
        cout << dec;
        cout << "  rva   = " << rva   << endl;

        if (rva >= begin  &&  rva < end)
            return i;

        cout << "  Is bad" << endl;
    }
    return -1;
}

DWORD RvaToOffset (PEData& pe, DWORD rva) //rva - relative virtual address
{
    for (int i = 0;  i<pe.sections.size();   i++)
    {
        DWORD begin = pe.sections[i].header.VirtualAddress;
        DWORD size  = pe.sections[i].header.Misc.VirtualSize;

        if (rva >= begin  &&  rva < begin+size)
            return rva - begin;
    }
    return 0;
}

//------------------------------------------------------

void Injector::RewriteRawData ()
{
    cout << "RewriteRawData\n";

    for (int i = 0;  i<rawDataToCopy.size();  i++)
    {
        int sourceSectionIndex = GetSectionIndex(result, rawDataToCopy[i].virtualAddress);
        if (sourceSectionIndex > -1)
        {
            int targetSectionIndex = GetSectionIndex(base, rawDataToCopy[i].virtualAddress);
            if (targetSectionIndex > -1)
            {
                cout << "Start rewrite (";
                cout.write((char*)&result.sections[sourceSectionIndex].header.Name[0], 8);
                cout << " -> ";
                cout.write((char*)&base.sections[targetSectionIndex].header.Name[0], 8);
                cout << ")" << endl;

                char*   sourceRawDataPointer  =  result.sections[sourceSectionIndex].rawData.GetBeginPointer() + RvaToOffset(result, rawDataToCopy[i].virtualAddress);
                char*   targetRawDataPointer  =  base.sections[targetSectionIndex].rawData.GetBeginPointer()   + RvaToOffset(base,   rawDataToCopy[i].virtualAddress);

                /*
                //!!!!!!!!! SEKCJE BÊD¥ WYRÓWNYWAÆ SWÓJ ROZMIAR Z T¥ JEDN¥ GIGANTYCZN¥ !!!!!!!!!!!!!!!!!
                //!!!!!!!!! SEKCJE BÊD¥ WYRÓWNYWAÆ SWÓJ ROZMIAR Z T¥ JEDN¥ GIGANTYCZN¥ !!!!!!!!!!!!!!!!!
                //!!!!!!!!! SEKCJE BÊD¥ WYRÓWNYWAÆ SWÓJ ROZMIAR Z T¥ JEDN¥ GIGANTYCZN¥ !!!!!!!!!!!!!!!!!
                //!!!!!!!!! SEKCJE BÊD¥ WYRÓWNYWAÆ SWÓJ ROZMIAR Z T¥ JEDN¥ GIGANTYCZN¥ !!!!!!!!!!!!!!!!!
                size_t  sourceSize            =  base.sections[sourceSectionIndex].rawData.Length();
                size_t  targetSize            =  result.sections[targetSectionIndex].rawData.Length();

                if (targetSize>sourceSize)
                {
                    base.sections[sourceSectionIndex].rawData.ResizeBy(targetSize-sourceSize);
                    base.sections[sourceSectionIndex].header.SizeOfRawData = targetSize;
                }*/

                memcpy (targetRawDataPointer, sourceRawDataPointer, rawDataToCopy[i].size);
            }
            else cout << "Bad target index!" << endl;
        }
        else cout << "Bad source index!" << endl;
    }
}

//======================================================
//======================================================


void Injector::Inject ()
{
    cout << "Inject\n";

    FileData finalData;

    finalData.AppendData((char*)(&base.DOSHeader), sizeof(IMAGE_DOS_HEADER));
    finalData.AppendData(base.DOSProgram);
    finalData.Append    (base.Signature);
    finalData.Append    (base.FileHeader);
    finalData.Append    (base.OptionalHeader);

    IMAGE_SECTION_HEADER* sectionHeaderBegin = reinterpret_cast<IMAGE_SECTION_HEADER*>(finalData.GetBeginPointer());

    cout << "Append section headers" << endl;
    for (int i=0;  i<base.sections.size();  i++)
        finalData.Append(base.sections[i].header);


    cout << "Append section raw data" << endl;
    for (int i=0;  i<base.sections.size();  i++)
    {
        DWORD aligned = finalData.GetLength() % base.OptionalHeader.FileAlignment;
        if (aligned != 0)
        {
            size_t increase = base.OptionalHeader.FileAlignment-aligned;
            finalData.ResizeBy(increase);
        }

        sectionHeaderBegin[i].PointerToRawData = finalData.GetLength();

        finalData.AppendData(base.sections[i].rawData);
    }

    finalData.SaveBinaryFile(targetPath);
}
