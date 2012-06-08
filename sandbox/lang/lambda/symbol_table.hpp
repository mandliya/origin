
#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <string>
#include <unordered_map>

#include "symbol.hpp"

// Symbol table
//
// The symbol table stores unique instances of symbols in the program text.
// Here, each symbol represents an occurrence of a string and its 
// classification. The table maintains two associations: one mapping symbol
// kinds to their symbols (for punctuation and keywords), and one mapping
// strings to symbols (identifiers and numbers).
//
// Note that this is not the same kind of symbol table that is conventionally
// used in parsing. This table provides a uniquing facility for lexical symbols
// in a language. 
//
// TODO: The name of this class amy change.
//
// FIXME: It is redundant to store both the string or token and the symbol in
// the map since the symbol also stores the text of the string. We really need a
// custom version of a hash table for this application (basically a modified
// unordered_set).
class symbol_table
{
  // FIXME: The only reason I'm storing symbol* and not symbol is so that I
  // can multimap symbol kinds and strings to the underlying symbol. That is,
  // I want to lookup e.g., "(" as either a sym_lparen or the string "(", and
  // I want the symbol corresponding to either lookup to be the same. 

  using Kind_map = std::unordered_map<symbol_kind, symbol*, symbol_kind_hash>;
  using String_map = std::unordered_map<lstring, symbol*>;
public:

  // Get the symbol associated with the given token kind.
  symbol* get(symbol_kind kind);
  symbol* get(symbol_kind kind) const;


  // Get the symbol associated with the given string. 
  symbol* get(const lstring& str);
  symbol* get(const lstring& str) const;

  // Create a symbol for the given symbol kind. There must not already be a
  // a symbol associated with this symbol kind.
  symbol *put(symbol_kind kind, const lstring str);


  // Get the symbol associated with the given range of characters.
  symbol* get(const lchar* first, const lchar* last);
  symbol* get(const lchar* first, const lchar* last) const;

  // Insert the character range [first, last) into the table.
  symbol* put(symbol_kind kind, const lchar* first, const lchar* last);

private:
  Kind_map kinds;
  String_map strings;
};

#endif