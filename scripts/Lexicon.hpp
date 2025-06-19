#ifndef _HPP_Lexicon_
#define _HPP_Lexicon_

#include <unordered_map>

#include "Parser/Section.hpp"
#include "Parser/Segment.hpp"
#include "Parser/Variable.hpp"

#include "Parser/Keyword.hpp"
#include "Parser/Punctator.hpp"

using std::unordered_map;
using std::string;

extern unordered_map <string, keyword> sectionKeywords;
extern unordered_map <string, keyword> segmentKeywords;
extern unordered_map <string, keyword> variableKeywords;
extern unordered_map <string, keyword> generalKeywords;

extern unordered_map <char, punctator> punctators;


#endif //_HPP_Lexicon_
