
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
//
// Invariants:
//    this->sym != nullptr
struct token
{
  token() 
  : loc{}, sym{nullptr}
    { }
  
  token(location l, symbol* sym) 
    : loc{l}, sym{nullptr} 
  { }

  // Return the location of the token.
  const location& locus() const { return loc; }

  // Returns the token's symbol kind.
  symbol_kind kind() const { return sym->kind; }

  // Returns true if the token is of the specified kind.
  bool is(symbol_kind k) { return kind() == k; }

  // Returns true ift he token is an identifier.
  bool is_identifier() const { return ::is_identifier(kind()); }

  // Returns a string containing the name of the symbol kind. This function is
  // primarily intended for debugging.
  std::string name() const { return sym->name(); }

  // Returns a string containing the spelling of the symbol.
  const std::string& spelling() const { return sym->spelling; }

  location loc;
  symbol* sym;
};


// Streaming
// Write information about the token to the output stream.
template <typename C, typename T>
  inline std::basic_ostream<C, T>& 
  operator<<(std::basic_ostream<C, T>& os, const token& tok)
  {
    os << '('; 
    os << tok.locus() << ": " << tok.name() << ": "  << tok.spelling();
    os << ')';
    return os;
  }


#endif
