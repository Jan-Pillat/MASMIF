#ifndef _HPP_ContentAnalyser_
#define _HPP_ContentAnalyser_

#include <string>
#include <vector>
#include "Lexer.hpp"
#include "Lexicon.hpp"
#include "../utils/Declarations/Token.hpp"

using std::string;
using std::vector;

class ContentAnalyser
{
private:
//! ---------- PRIVATE VARIABLES ----------
    const string* content;

    string publications = "";

    vector <Token> tokens;
    size_t   iterator   = 0;
    Token*   gotToken;
    Token*   nextToken;


//! ---------- PRIVATE METHODS ----------
    void LexContent                 ();
    void FindAndWritePublications   ();

    bool GetToken                   ();
    bool ShowNextToken              ();

public:
//! ---------- PUBLIC METHODS ----------
    string  GetPublications ();

    void    SetContent      (const string& content);
    void    SetAndLexContent(const string& content);

//! ---------- CONSTRUCTORS ----------
    ContentAnalyser (const string& content) { SetContent(content); }
};

#endif
