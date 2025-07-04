
#include "Injector.hpp"

#include <iostream> //Debug

using namespace std;

//======================================================
//======================================================

Injector::Injector   (string& gotPath,      PEData& gotPEData,  PEData& gotResultData,  vector<SectionToCopy>& gotSectionsToCopy,  vector<RawDataToCopy>& gotRawDataToCopy)
                   :  targetPath (gotPath), base   (gotPEData), result (gotResultData),        sectionsToCopy (gotSectionsToCopy),        rawDataToCopy  (gotRawDataToCopy)
{
    IncludeNewSections      ();
    MergeSections           ();
    CorrectRawDataOffsets   ();
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
{
    cout << "MergeSections\n";

}

//======================================================
//======================================================


void Injector::CorrectRawDataOffsets ()
{
    cout << "CorrectRawDataOffsets\n";

}

//======================================================
//======================================================


void Injector::RewriteRawData ()
{
    cout << "RewriteRawData\n";

}

//======================================================
//======================================================


void Injector::Inject ()
{
    cout << "Inject\n";

    BinaryData finalData;

    //finalData.AppendData();
}
