
#ifndef SYMBOL_HPP
#define SYMBOL_HPP

#include <functional>
#include <string>
#include <iosfwd>

////////////////////////////////////////////////////////////////////////////////
// Strings and Streams
//
// The following typedefs lift the underlying definition of strings and 
// characters from the lexer and parser. The entire system can be changed to
// support wide strings (or not) by changing the type of String.
//
// TODO: Refactor this to a separate header (system.hpp?).


// The String type describes the type of underlying character string used
// throughout the implementation.
//
// Note that changing this type to wstring will allow us to read utf16 encoded 
// input. This is, in a sense, an implied parameter of the program. 
using String = std::string;

// The type of characters contained by String.
using Char = typename String::value_type;

// Streams.
//
// FIXME: I'm going to have to replace cin/cout with functions that return
// references to Istream and Ostream.
using Istream = std::basic_istream<Char>;
using OStream = std::basic_ostream<Char>;


////////////////////////////////////////////////////////////////////////////////
// Symbols


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
class Symbol
{
public:
  // The symbol classifies the kinds of symbols found in the language. A symbol
  // kind may correspond to 0, 1, or many spellings. A symbol kind with a unique
  // spelling defines punctuation, operators, and reserved words. A symbol kind
  // multiple spellings is generally an identifier or number. Symbols with no
  // spellings are generally used internally, but may have value in other
  // applications.
  //
  // TODO: Add symbols for whitespace?
  enum Kind
  {
    // Miscellaneous
    Eof,          // End of file
    Error,        // Not a valid symbol

    // Punctuation and operators
    Lparen,       // )
    Rparen,       // (
    Backslash,    // '\'
    Dot,          // .
    Semicolon,    // ;
    Equal,        // =

    // Symbol classes
    Identifier    // [a-zA-Z_][a-zA-Z0-9_]
  };


  Symbol() = default;

  Symbol(Kind k, const String& s)
    : kind{k}, spelling{s}
  { }

  // Returns the name of the symbol (e.g., "dot", "backslash", "identifier").
  String name() const;


  // FIXME: There should be more useful operations than just this...
  bool is(Kind k) { return k == kind; }
  bool is_eof() const { return kind == Eof; }
  bool is_error() const { return kind == Error; }
  bool is_invalid() const;
  bool is_identifier() const;

  Kind kind;
  String spelling;
};



// Hashing support for symbols. A symbol is hashed as an int.
struct symbol_kind_hash
{
  std::size_t operator()(Symbol::Kind k) const
  {
    std::hash<int> h;
    return h((int)k);
  }
};




#endif
