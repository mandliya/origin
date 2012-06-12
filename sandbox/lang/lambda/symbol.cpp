
#include <cassert>

#include "symbol.hpp"

using namespace std;


// Symbol operations
String symbol_name(Symbol::Kind k);
bool is_punctuation(Symbol::Kind k);
bool is_identifier(Symbol::Kind k);



// Returns the name of the symbol as a string (e.g., "dot" or "identifier").
String 
Symbol::name() const
{
  switch (kind) {
    case Symbol::Eof: return "eof";
    case Symbol::Error: return "error";

    case Symbol::Lparen: return "lparen";
    case Symbol::Rparen: return "rparen";
    case Symbol::Backslash: return "backslash";
    case Symbol::Dot: return "dot";
    case Symbol::Semicolon: return "semicolon";

    case Symbol::Eval: return "eval";
    case Symbol::Let: return "let";

    case Symbol::Identifier: return "identifier";
  }
}

// Returns true if this symbol does not represnt a symbol in the language.
// This the case when the symbol is eof or error.
bool
Symbol::is_invalid() const
{
  return kind == Eof || kind == Error;
}

// Returns true if the token is an identifier.
bool 
Symbol::is_identifier() const
{
  return kind == Symbol::Identifier;
}

