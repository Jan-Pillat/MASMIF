#ifndef _HPP_DebugLabelGenerator_
#define _HPP_DebugLabelGenerator_

#include <string>
#include <vector>
#include <algorithm>

#include "../utils/Declarations/DebugLabel.hpp"
#include "../utils/CppCore/include/StringUtils.hpp"
#include "../utils/CppCore/include/PEData.hpp"
#include "../utils/CppCore/include/FileData.hpp"

using std::string;
using std::vector;

class DebugLabelGenerator
{
public:
    DebugLabelGenerator (string& targetFileName, vector<DebugLabel>& gotDebugLabels, PEData& gotPEData);

    void GetFileName    ();
    void PrepareLabels  ();
    void SetNewPath     ();
    void SaveToFile     ();

private:
    vector <DebugLabel>&   debugLabels;
    string  path;
    string  newPath;
    string  fileName;
    string  finalFileData;
    PEData& base;
};

#endif
