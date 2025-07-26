#ifndef _HPP_driver_
#define _HPP_driver_

#include <string>
#include <vector>

#include "../Globals.hpp"
#include "../utils/GetPath.hpp"
#include "../utils/CppCore/include/PEData.hpp"
#include "../utils/CppCore/include/ConsoleUtils.hpp"

#include "../utils/Declarations/Declaration.hpp"
#include "../utils/Declarations/DebugLabel.hpp"
#include "../utils/Declarations/Token.hpp"
#include "../utils/Declarations/Thunk.hpp"
#include "../utils/Declarations/Merge.hpp"
#include "../utils/Declarations/Map.hpp"

#include "Preprocessor.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
#include "CodeGenerator.hpp"
#include "AsmConverter.hpp"
#include "MapParser.hpp"
#include "BuildDriver.hpp"
#include "BinaryPatcher.hpp"
#include "DebugLabelGenerator.hpp"

using std::string;
using std::vector;



class Driver
{
public:

    Driver  ();
    Driver  (const string& scriptPath);

    void    Run (const string& scriptPath);

private:

    void        Run             ();

    void        LoadBase        ();
    void        LoadScript      ();
    void        DoPreprocessing ();
    void        DoLexing        ();
    void        DoParsing       ();
    void        GenerateCode    ();
    void        ConvertCode     ();
    void        SaveCode        ();
    void        Build           ();
    void        LoadMap         ();
    void        LexMap          ();
    void        ParseMap        ();
    void        LoadResult      ();
    void        LoadTargetPath  ();
    void        Patch           ();
    void        GenerateLabels  ();
    void        SuccessInfo     ();

    void        ClearMemory     ();
    void        PrintTokens     (vector <Token>& tokens);


    FileData    script;
    FileData    mapData;
    PEData      base;
    PEData      result;
    string      MASMcode;
    string      targetPath;
    string      scriptPath;

    vector  <Token>         tokens;
    vector  <Merge>         merges;
    vector  <Thunk>         thunks;
    vector  <Declaration>   declarations;

    vector  <Token>         mapTokens;
    vector  <SectionToCopy> sectionsToCopy;
    vector  <RawDataToCopy> rawDataToCopy;
    vector  <DebugLabel>    debugLabels;
};

#endif
