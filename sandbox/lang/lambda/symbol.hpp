
#ifndef SYMBOL_HPP
#define SYMBOL_HPP

#include <functional>
#include <string>

// The lstring type describes the type of underlying character string used
// throughout the implementation.
//
// Note that changing this type to wstring will allow us to read utf16 encoded 
// input. This is, in a sense, an implied parameter of the program. 
using lstring = std::string;

// The type of characters contained by String.
using lchar = typename std::string::value_type;


// The symbol classifies the kinds of symbols found in the language. A symbol
// kind may correspond to 0, 1, or many spellings. A symbol kind with a unique
// spelling defines punctuation, operators, and reserved words. A symbol kind
// multiple spellings is generally an identifier or number. Symbols with no
// spellings are generally used internally, but may have value in other
// applications.
//
// TODO: Add symbols for whitespace?
enum symbol_kind
{
  // Miscellaneous
  sym_eof,          // End of file
  sym_error,        // Not a valid symbol
  sym_comment,      // A comment

  // Punctuation and operators
  sym_lparen,       // )
  sym_rparen,       // (
  sym_backslash,    // '\'
  sym_dot,          // .

  // Keywords

  // Symbol classes
  sym_identifier    // [a-zA-Z_][a-zA-Z0-9_]
};


// Symbol operations.
lstring symbol_name(symbol_kind k);
bool is_punctuation(symbol_kind k);
bool is_identifier(symbol_kind k);


// Hashing support for symbols. A symbol is hashed as an int.
struct symbol_kind_hash
{
  std::size_t operator()(symbol_kind k) const
  {
    std::hash<int> h;
    return h((int)k);
  }
};




// Symbol
//
// A symbol table should return symbols (not strings). A symbol is represents
// the set of all tokens of the same kind and spelling in a program. For
// example, there is only one '\' symbol (in the mathematical sense), but there
// will be many '\' tokens in a program.
//
// TODO: If the symbol is numeric, it might be worthwhile to store its literal
// value or provide methods that compute its literal value from the stored
// string.
struct symbol
{
  symbol() = default;

  symbol(symbol_kind k, const lstring& s)
    : kind{k}, spelling{s}
  { }

  // Returns the name of the symbol (e.g., "dot", "backslash", "identifier").
  lstring name() const { return ::symbol_name(kind); }

  
  // Tests for pseudo symbols
  bool is_eof() const { return kind == sym_eof; }
  bool is_error() const { return kind == sym_error; }

  // Test for punctuator and operator classes.
  bool is_punctuation() const { return ::is_punctuation(kind); }
  bool is_lparen() const { return kind == sym_lparen; }
  bool is_rparen() const { return kind == sym_rparen; }
  bool is_backslash() const { return kind == sym_backslash; }
  bool is_dot() const { return kind == sym_dot; }

  // Test for identifers and numbers
  bool is_identifier() const { return ::is_identifier(kind); }

  symbol_kind kind;
  lstring spelling;
};




#endif
