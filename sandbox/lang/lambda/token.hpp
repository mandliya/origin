
#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <iosfwd>
#include <string>

#include "location.hpp"
#include "symbol.hpp"


// Token
//
// The token class represents an instannce of a token in the language. Each
// token stores the parsed kind, its source location and a reference to its
// text in a symbol table.
struct Token
{
  Token() 
  : loc{}, sym{nullptr}
    { }
  
  Token(Location l, Symbol* sym) 
    : loc{l}, sym{nullptr} 
  { }

  // Returns true if the token represents a symbol in the language.
  explicit operator bool() const
  {
    return sym != nullptr && !sym->is_eof() && !sym->is_error();
  }

  // Return the location of the token.
  const Location& locus() const { return loc; }


  // Returns a string containing the name of the symbol kind. This function is
  // primarily intended for debugging.
  String name() const { return sym->name(); }

  // Returns the token's symbol kind.
  Symbol::Kind kind() const { return sym->kind; }

  // Queries
  bool is(Symbol::Kind k) { return sym->is(k); }
  bool is_identifier() const { return sym->is_identifier(); }


  // Returns a string containing the spelling of the symbol.
  const String& spelling() const { return sym->spelling; }


  Location loc;
  Symbol* sym;
};


// Streaming
// Write information about the token to the output stream.
template <typename C, typename T>
  inline std::basic_ostream<C, T>& 
  operator<<(std::basic_ostream<C, T>& os, const Token& tok)
  {
    os << '('; 
    os << tok.locus() << ": " << tok.name() << ": "  << tok.spelling();
    os << ')';
    return os;
  }


#endif
