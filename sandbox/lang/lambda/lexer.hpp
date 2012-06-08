
#ifndef LEXER_HPP
#define LEXER_HPP

#include "symbol.hpp"
#include "symbol_table.hpp"
#include "token.hpp"

// Lexer
//
// The lexer class is responsible recognizing the terminals in a language.
// This generally includes punctation, keywords, numbers, and identifiers.
//
// TODO: Refactor this so that there's a basic core lexer that can be extended
// for different different languages. The goal is to not rewrite a new lexer
// for every different language from scratch.
//
// FIXME: How (or if) we handle recursive file parsing is going to be an 
// interesting problem. If recursive parsing is done lexically (as with the C
// preprocessor), then this class would have to implement a lexing stack (just
// the first and last pointers, and the current location), in addition to the
// glboal lexing state. If recursive parsing is syntax driven, then the parser
// would simply need to keep a stack of lexers.
struct lexer
{
  lexer(symbol_table* t, const lstring& buf);


  // Lex the next token out of the buffer, returning it.
  token operator()();


  // Observers

  // Returns the current location.
  location get_location() const { return loc; }

  // Returns the current line number.
  int get_line() const { return loc.line; }

  // Returns the current column.
  int get_column() const { return loc.column; }


  // Lexing support
  bool is_eof() const;
  bool is_char(char c) const;
  bool is_letter() const;
  bool is_digit() const;
  bool is_alphanumeric() const;
  bool is_identifier();


  // Comments
  // FIXME: Implement comment parsing! Are there comments in untyped lambda
  // calculus? Not really sure. I could make "--" to the EoL a comment.


  // Whitespace
  bool consume_ws();
  bool consume_horizontal_ws();
  bool consume_vertical_ws();

  // Token construction
  // These functions fix information for the current token prior to
  // returning it from the lex function. In particular, they bind the token
  // the current source location and register the lexed content with the
  // current symbol table.

  void make_eof();
  void make_punctuation(symbol_kind kind);
  void make_identifer(const lchar* first, const lchar* last);
  void make_error();


  symbol_table* table;  // The primary symbol table
  const lchar* first;   // An iterator to the first input character
  const lchar* last;    // An iterator past the last input character

  token tok;            // The current token
  location loc;         // The current location
};

#endif
