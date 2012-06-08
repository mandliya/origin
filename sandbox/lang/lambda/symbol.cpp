
#include <cassert>

#include "symbol.hpp"

using namespace std;


// Returns the name of the symbol as a string (e.g., "dot" or "identifier").
lstring symbol_name(symbol_kind k)
{
  switch(k) {
    case sym_eof: return "eof";
    case sym_error: return "error";

    case sym_lparen: return "lparen";
    case sym_rparen: return "rparen";
    case sym_backslash: return "backslash";
    case sym_dot: return "dot";

    case sym_identifier: return "identifier";
  }
}


// Returns true if the symbol is a punctuation token.
bool is_punctuation(symbol_kind k)
{
  return k >= sym_lparen && k <= sym_dot;
}

// Returns true if the token is an identifier token.
bool is_identifier(symbol_kind k)
{
  return k == sym_identifier;
}



