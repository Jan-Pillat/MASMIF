#ifndef _HPP_DbgLabeler_
#define _HPP_DbgLabeler_

#include <string>
#include <vector>
#include "Declarations.hpp"
#include "utils/ConvertNumberToHexString.hpp"
#include "PEData.hpp"
#include "../CppCore/include/FileData.hpp"

using std::string;
using std::vector;

class DbgLabeler
{
public:
    DbgLabeler (string& targetFileName, vector<Declaration>& gotDeclarations, PEData& gotPEData);

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
