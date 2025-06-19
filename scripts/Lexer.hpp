#ifndef _HPP_Lexer_
#define _HPP_Lexer_

#include <unordered_map>
#include <iostream>
#include "Token.hpp"
#include "Lexicon.hpp"
#include "../CppCore/include/FileData.hpp"

// ---------- VARIABLES ----------
extern char token_i;        //indeks to ostatniego wczytanego znaku znalezionego tokenu
extern char token[];        //Wczytany token

extern enum TokenType lastTokenType;

extern char* currentPos;
extern int  currentLine;

extern punctator  foundPunctator;

extern unordered_map <string, keyword>*   currentKeywords;


// ---------- FUNCTIONS ----------
bool TryToLoadToken (char* src);
bool Tokenize       (char* src);
void PrintTokens    ();
void TokenizeFile   (string path);

#endif //_HPP_Lexer_
