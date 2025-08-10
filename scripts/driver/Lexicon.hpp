#ifndef _HPP_Lexicon_
#define _HPP_Lexicon_

#include <string>
#include <unordered_set>
#include "Parser.hpp"

using std::string;
using std::unordered_set;

extern unordered_set <string> assemblyDeclarations;
extern unordered_set <string> assemblyCommands;
extern unordered_set <string> keywordsToAvoidByConverter;
extern unordered_set <string> asmRegisters;

#endif // _HPP_Lexicon_


