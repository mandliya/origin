
#include <iostream>

#include "lexer.hpp"

using namespace std;

Lexer::Lexer(symbol_table& t, Istream& is)
  : table(t)
  , is(is)
  , tok()
  , loc(1, 1)
{ }

// Lex the next token out of the buffer, returning it.
Token 
Lexer::operator()()
{
  // Pre-lex the token, skipping any initial whitespace and/or comments,
  // advancing to the first non-ws character. Note that we may reach EoF in
  // the process.
  //
  // TODO: There may be some cases where emitting WS tokens are useful.
  // but that's future work. 
  consume_ws();

  // If we reached the end of the line, return an EoF token.
  if (is_eof()) {
    make_eof();
  } else {
    // Otherwise, match against the language's symbols.
    //
    // Keep a pointer to the start of the symbol so we can reconstruct the
    // string later.
    switch(peek()) {
      case '(':  make_punctuation(Symbol::Lparen);    advance(); break;
      case ')':  make_punctuation(Symbol::Rparen);    advance(); break;
      case '\\': make_punctuation(Symbol::Backslash); advance(); break;
      case '.':  make_punctuation(Symbol::Dot);       advance(); break;
      case ';':  make_punctuation(Symbol::Semicolon); advance(); break;
      case '=':  make_punctuation(Symbol::Equal);     advance(); break;
      default: {
        // NOTE: This will handle keywords gracefully since keywords are hashed
        // into the same symbol table. Note that we could be much more efficient
        // about doing this by building a state-machine to recognize keywords
        // explicitly rather than relying on the symbol table. It's just more
        // work.
        String id;
        if (match_identifier(id)) {
          make_identifer(id);
        } else {
          make_error();
          advance();
        }
      }
    }
  }
  return tok;
}

// Lexing support

// Returns true when we reach the end of file.
bool 
Lexer::is_eof() const { return is.eof(); }

// Returns true if the currenct character compares equal to c.
bool 
Lexer::is_char(char c) const { return !is_eof() && (peek() == c); }

// Returns true if the current character is a letter. 
bool 
Lexer::is_letter() const { return !is_eof() && isalpha(peek()); }

// Returns true if the current character is a digit.
bool 
Lexer::is_digit() const { return !is_eof() && isdigit(peek()); }

// Returns true if c is either a letter or digit. If it is, assign that
// character to c and advance.
bool 
Lexer::is_alphanumeric() const { return !is_eof() && isalnum(peek()); }

// An identifier (variable) in the untyped lambda calculus is a string
// of alphanumeric characters that cannot start with a digit. An underscore
// may be used in any position.
bool 
Lexer::match_identifier(String& id)
{
  if (is_letter() || is_char('_')) {
    id += get();
    while (is_alphanumeric() || is_char('_'))
      id += get();
    return true;
  }
  return false;
}


// Comments
//
// FIXME: Implement comment parsing! Are there comments in untyped lambda
// calculus? Not really sure. I could make "--" to the EoL a comment.


// Whitespace

// Consume whitespace and return true if any whitespace was consumed.
bool 
Lexer::consume_ws()
{
  bool result;
  while (!is_eof() && (consume_horizontal_ws() || consume_vertical_ws()))
    result = true;
  return result;
}

// Recognize and consume horizontal whitespace (' ' or tab).
bool 
Lexer::consume_horizontal_ws() 
{ 
  switch (peek()) {
    case ' ':
    case '\t':
    case '\v':
      ++loc.column;
      advance();
      return true;
    }
  return false;
}

// Recognize and consume vertical whitespace, incrementing the line
// count when we find some. If the line number is incremented, then reset
// the current column count to 1.
//
// FIXME: What about DOS or Windows EoF? How should I expect to handle a
// carriage return? Vertical newlines?
bool 
Lexer::consume_vertical_ws()
{
  switch(peek()) {
    case '\r':
    case '\n':
      ++loc.line;
      loc.column = 1;
      advance();
      return true;
    }
  return false;
}


// Token construction
// These functions fix information for the current token prior to
// returning it from the lex function. In particular, they bind the token
// the current source location and register the lexed content with the
// current symbol table.

// FIXME: Fnd 

// Return the eof token.
void 
Lexer::make_eof()
{
  tok.sym = table.get(Symbol::Eof);
}

// Return a punctuation token.
//
// FIXME: This will, in the future, require that we know the length of
// the punctuator. For now, all tokens are length 1, so its easy.
void 
Lexer::make_punctuation(Symbol::Kind kind)
{
  tok.loc = loc;
  tok.sym = table.get(kind);
  ++loc.column;
}

// Enter the given substring as an identifier in the table. If the identifier
// already exists, return the current string.
//
// Don't increment the 
void 
Lexer::make_identifer(const String& id)
{
  tok.loc = loc;
  tok.sym = table.put(Symbol::Identifier, id);
  loc.column += tok.spelling().size();
}

void 
Lexer::make_error()
{
  tok.loc = loc;
  tok.sym = table.get(Symbol::Error);
  ++loc.column;
}

