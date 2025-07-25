#ifndef _HPP_DebugLabelGenerator_
#define _HPP_DebugLabelGenerator_

#include <string>
#include <vector>

#include "../utils/Declarations/Declaration.hpp"
#include "../utils/CppCore/include/StringUtils.hpp"
#include "../utils/CppCore/include/PEData.hpp"
#include "../utils/CppCore/include/FileData.hpp"

using std::string;
using std::vector;

class DebugLabelGenerator
{
public:
    DebugLabelGenerator (string& targetFileName, vector<Declaration>& gotDeclarations, PEData& gotPEData);

    void GetFileName    ();
    void PrepareLabels  ();
    void SetNewPath     ();
    void SaveToFile     ();

private:
    vector <Declaration>&   declarations;
    string  path;
    string  newPath;
    string  fileName;
    string  finalFileData;
    PEData& base;
};

#endif
