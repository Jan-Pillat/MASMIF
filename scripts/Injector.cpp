
#include "Injector.hpp"

#include <iostream> //Debug

using namespace std;

//!======================================================
//!======================================================

Injector::Injector   (string& gotPath,      PEData& gotPEData,  PEData& gotResultData,  vector<SectionToCopy>& gotSectionsToCopy,  vector<RawDataToCopy>& gotRawDataToCopy, vector<Merge>& gotMerges)
                   :  targetPath (gotPath), base   (gotPEData), result (gotResultData),        sectionsToCopy (gotSectionsToCopy),        rawDataToCopy  (gotRawDataToCopy),       merges (gotMerges)
{
    IncludeNewSections      ();
    RoundVirtualSize        ();
    MergeSections           ();
    RewriteRawData          ();
    RoundRawDataSize        ();
    Inject                  ();
}

//!======================================================
//!======================================================

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

//!======================================================
//!======================================================

void Injector::MergeSections ()
{
    cout << "MergeSections\n";

    for (int currentMerge = 0;  currentMerge < merges.size();  currentMerge++)
    {
        cout << "    merge nr. " << currentMerge<< endl;
        cout << "    first  name      = " << merges[currentMerge].first << endl;
        cout << "    first  name size = " << merges[currentMerge].first.size() << endl;
        cout << "    second name      = " << merges[currentMerge].second << endl;
        cout << "    second name size = " << merges[currentMerge].second.size() << endl;

        size_t  firstSectionIndex  = -1;
        size_t  secondSectionIndex = -1;

        //! ---------- FIND SECTION INDXES ----------
        for (int i = 0;  i < base.sections.size();  i++)
        {
            cout << "        memcmp (first)  = " << memcmp(&base.sections[i].header.Name, &merges[currentMerge].first[0], IMAGE_SIZEOF_SHORT_NAME) << endl;

            if ( merges[currentMerge].first.size() <= IMAGE_SIZEOF_SHORT_NAME
            &&   memcmp(&base.sections[i].header.Name, &merges[currentMerge].first[0], merges[currentMerge].first.size()) == 0)
            {
                firstSectionIndex = i;
            }

            if ( merges[currentMerge].second.size() <= IMAGE_SIZEOF_SHORT_NAME
            &&   memcmp(&base.sections[i].header.Name, &merges[currentMerge].second[0], merges[currentMerge].second.size()) == 0)
            {
                secondSectionIndex = i;
            }

            if (firstSectionIndex != -1  &&  secondSectionIndex != -1)
                break;
        }

        //! ---------- SKIP IF SECTION NOT FOUND ----------
        if (firstSectionIndex == -1  ||  secondSectionIndex == -1)
        {
            if (firstSectionIndex == -1)
                cout << "    first section NOT FOUND!" << endl;
            if (secondSectionIndex == -1)
                cout << "    second section NOT FOUND!" << endl;
            continue;
        }
        else
        {
            cout << "    firstSectionIndex  = " << firstSectionIndex  << endl;
            cout << "    secondSectionIndex = " << secondSectionIndex << endl;
        }

        //! ---------- SET FAR AND NEAR SECTION ----------
        size_t  furtherSectionIndex;
        size_t  closerSectionIndex;

        if (base.sections[firstSectionIndex].header.VirtualAddress > base.sections[secondSectionIndex].header.VirtualAddress)
        {
            furtherSectionIndex = firstSectionIndex;
            closerSectionIndex  = secondSectionIndex;
        }
        else
        {
            furtherSectionIndex = secondSectionIndex;
            closerSectionIndex  = firstSectionIndex;
        }

        //! ---------- CALCULATE SIZE ----------
        size_t virtualSize  = base.sections[furtherSectionIndex].header.VirtualAddress
                            - base.sections[closerSectionIndex ].header.VirtualAddress
                            + base.sections[furtherSectionIndex].header.Misc.VirtualSize;

        size_t rawDataSize  = base.sections[furtherSectionIndex].header.VirtualAddress
                            - base.sections[closerSectionIndex ].header.VirtualAddress;
                            //+ base.sections[furtherSectionIndex].header.SizeOfRawData;  <--- 'Append rawData' add this

        //! ---------- RESIZE AND APPEND RAW DATA ----------
        base.sections[closerSectionIndex].rawData.Resize(rawDataSize);
        base.sections[closerSectionIndex].rawData.AppendData(base.sections[furtherSectionIndex].rawData);
        base.sections[firstSectionIndex ].rawData = base.sections[closerSectionIndex].rawData;

        base.sections[firstSectionIndex].header.SizeOfRawData    = base.sections[firstSectionIndex].rawData.GetLength();
        base.sections[firstSectionIndex].header.VirtualAddress   = base.sections[closerSectionIndex].header.VirtualAddress;
        base.sections[firstSectionIndex].header.Misc.VirtualSize = virtualSize;

        //! ---------- REMOVE SECTION ----------
        base.sections.erase (base.sections.begin() + secondSectionIndex);
        base.FileHeader.NumberOfSections -= 1;
    }

    cout << "    END" << endl;
}

//!======================================================
//!======================================================


void Injector::RoundVirtualSize ()
{
    cout << "RoundVirtualSize\n";

    for (int i = 0;  i<base.sections.size();  i++)
    {
        DWORD size  = base.sections[i].header.Misc.VirtualSize;
        DWORD aligned = size % base.OptionalHeader.SectionAlignment;
        if (aligned != 0)
            base.sections[i].header.Misc.VirtualSize  +=  base.OptionalHeader.SectionAlignment - aligned;
    }
}

//!======================================================
//!======================================================


void Injector::RoundRawDataSize ()
{
    cout << "CorrectRawDataOffsets\n";

    for (int i = 0;  i<base.sections.size();  i++)
    {
        DWORD aligned = base.sections[i].rawData.GetLength() % base.OptionalHeader.FileAlignment;
        if (aligned != 0)
        {
            size_t increase = base.OptionalHeader.FileAlignment-aligned;
            base.sections[i].rawData.ResizeBy(increase);
        }

        base.sections[i].header.SizeOfRawData = base.sections[i].rawData.GetLength();
    }
}

//======================================================
//======================================================

int GetSectionIndex (PEData& pe, DWORD rva) //rva - relative virtual address
{
    cout << "--GetSectionIndex--\n";

    for (int i = 0;  i<pe.sections.size();   i++)
    {
        cout << "    section ";
        cout.write((char*)&pe.sections[i].header.Name[0], 8);
        cout << endl;

        DWORD begin = pe.sections[i].header.VirtualAddress  +  pe.OptionalHeader.ImageBase;

        DWORD size  = pe.sections[i].header.Misc.VirtualSize;
        DWORD end   = begin + size;
        DWORD aligned = size % pe.OptionalHeader.SectionAlignment;
        if (aligned != 0)
            end += pe.OptionalHeader.SectionAlignment - aligned;

        cout << hex;
        cout << "    ImageBase = 0x" << pe.OptionalHeader.ImageBase << endl;
        cout << "    begin = 0x" << begin << endl;
        cout << "    end   = 0x" << end   << endl;
        cout << "    size  = 0x" << size  << endl;
        cout << "    rva   = 0x" << rva   << endl;
        cout << dec;

        if (rva >= begin  &&  rva < end)
            return i;

        cout << "    Is bad" << endl;
    }
    return -1;
}

int RvaToOffset (PEData& pe, DWORD rva) //rva - relative virtual address
{
    cout << "--RvaToOffset--\n";

    for (int i = 0;  i<pe.sections.size();   i++)
    {
        cout << "    section ";
        cout.write((char*)&pe.sections[i].header.Name[0], 8);
        cout << endl;

        DWORD begin = pe.sections[i].header.VirtualAddress  +  pe.OptionalHeader.ImageBase;

        DWORD size  = pe.sections[i].header.Misc.VirtualSize;
        DWORD end   = begin + size;
        DWORD aligned = size % pe.OptionalHeader.SectionAlignment;
        if (aligned != 0)
            end += pe.OptionalHeader.SectionAlignment - aligned;

        cout << hex;
        cout << "    ImageBase = 0x" << pe.OptionalHeader.ImageBase << endl;
        cout << "    begin = 0x" << begin << endl;
        //cout << "    end   = 0x" << end   << endl;
        cout << "    size  = 0x" << size  << endl;
        cout << "    rva   = 0x" << rva   << endl;
        cout << dec;

        if (rva >= begin  &&  rva < end)
            return rva - begin;

        cout << "    Is bad" << endl;
    }
    return -1;
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

                // ---------- TARGET OFFSET ----------
                int     targetRawDataOffset   =  RvaToOffset(base,   rawDataToCopy[i].virtualAddress);

                if (targetRawDataOffset == -1)
                {
                    cout << "  (STOP) targetRawDataOffset = -1" << endl;
                    continue;
                }
                else
                {
                    cout << "  (CONTINUE) targetRawDataOffset != -1" << endl;
                    int difference = (targetRawDataOffset + rawDataToCopy[i].size) - base.sections[targetSectionIndex].rawData.Length();
                    if (difference > 0)
                    {
                        base.sections[targetSectionIndex].rawData.ResizeBy (difference);
                    }
                }

                // ---------- SOURCE OFFSET ----------
                int     sourceRawDataOffset   =  RvaToOffset(result,   rawDataToCopy[i].virtualAddress);
                if (sourceRawDataOffset == -1)
                {
                    cout << "  (STOP) sourceRawDataOffset = -1" << endl;
                    continue;
                }
                else
                    cout << "  (CONTINUE) sourceRawDataOffset != -1" << endl;

                // ---------- POINTERS ----------
                char*   sourceRawDataPointer  =  result.sections[sourceSectionIndex].rawData.GetBeginPointer() + sourceRawDataOffset;
                char*   targetRawDataPointer  =  base.sections[targetSectionIndex].rawData.GetBeginPointer()   + targetRawDataOffset;

                // ---------- WRITE ----------
                memcpy (targetRawDataPointer, sourceRawDataPointer, rawDataToCopy[i].size);
            }
            else cout << "  (STOP) Bad target index!" << endl;
        }
        else cout << "  (STOP) Bad source index!" << endl;
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

    size_t offsetToSectionHeaders = finalData.GetLength();

    // ---- APPEND SECTION HEADERS ----
    for (int i=0;  i<base.sections.size();  i++)
        finalData.Append(base.sections[i].header);

    // ---- FILE ALIGNMENT ----
    DWORD aligned = finalData.GetLength() % base.OptionalHeader.FileAlignment;
    if (aligned != 0)
    {
        size_t increase = base.OptionalHeader.FileAlignment-aligned;
        finalData.ResizeBy(increase);
    }

    // ---- APPEND RAW DATA ----
    for (int i=0;  i<base.sections.size();  i++)
    {
        IMAGE_SECTION_HEADER* sectionHeaderBegin =  reinterpret_cast<IMAGE_SECTION_HEADER*>(finalData.GetBeginPointer() + offsetToSectionHeaders);
        sectionHeaderBegin[i].PointerToRawData   =  finalData.GetLength();
        finalData.AppendData(base.sections[i].rawData);
    }

    finalData.SaveBinaryFile(targetPath);
}
