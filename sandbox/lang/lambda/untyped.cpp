
#include <cassert>
#include <string>
#include <iostream>
#include <sstream>
#include <unordered_map>

#include "location.hpp"
#include "token.hpp"

using namespace std;


// Language symbol table
//
// The language table stores the spelling of keyword and punctuation token in
// the source language.
//
// FIXME: It is technically redundant to store the token kind in a map since
// the symbol stores that also. A more space efficient solution would be to
// store this in an unordered set and then provide lookups on just the token
// key type. Unfortunately, this is not easy to do with the STL. I would need
// a custom lookup function for token kinds.
class language_table
{
  using map_type = unordered_map<token_kind, symbol, token_kind_hash>;
public:
  // Get the symbol associated with the given token kind.
  symbol* get(token_kind kind) const
  {
    auto i = map.find(kind);
    if (i != map.end())
      return const_cast<symbol*>(&((*i).second));
    else
      return nullptr;
  }

  // Set the spelling associated with the given token kind.
  symbol *put(token_kind kind, string str)
  {
    return &(map[kind] = symbol{kind, str});
  }

private:
  map_type map;
};



// Symbol table
//
// The symbol table provides a unified mechanism for accessing data about
// symbols in the program. Symbols are either accessed by their token id
// (deferring to the language table above) or their spellings.
class symbol_table
{
  using identifier_table = unordered_map<string, symbol>; 
public:
  
  symbol_table(language_table& t)
    : lang(t)
  { }

  // Read access to the language table.
  symbol* get(token_kind kind) const { return lang.get(kind); }

  // Get the symbol associated with the given string. 
  symbol *get(const string& str) const
  {
    auto i = ident.find(str);
    if (i != ident.end())
      return const_cast<symbol*>(&((*i).second));
    else 
      return nullptr;
  }

  // Get the symbol associated with the given range of characters.
  template <typename I>
    symbol* get(I first, I last) const 
    { 
      return get(string(first, last));
    }

  // Insert the given symbol into the table.
  symbol *put(token_kind kind, const string& str)
  {
    assert(is_identifier(kind));
    return &(ident[str] = symbol{kind, str});
  }

  // Insert the given symbol into the table.
  template <typename I>
  symbol *put(token_kind kind, I first, I last)
  {
    return put(kind, string(first, last));
  }

  language_table& lang;
  identifier_table ident;
};



// Lexer
//
// The lexer class is responsible recognizing the terminals in a language.
// This generally includes punctation, keywords, numbers, and identifiers.
//
// TODO: Count columns in addition to line numbers, and build support for
// files and source locations.
//
// TODO: Refactor this so that there's a basic core lexer that can be extended
// for different different languages. The goal is to not rewrite a new lexer
// for every different language from scratch.
struct lexer
{
  using Iter = string::iterator;
  using Table = symbol_table;

  lexer(Table* t, Iter f, Iter l)
    : table{t}, first{f}, last{l}, tok{}, loc{1, 1}
  { }

  // Lex the next token out of the buffer, returning it.
  //
  // FIXME: Tokens should be relatively small.
  token lex()
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
      Iter start = first; 
      switch(*first) {
      case '(':
        make_punctuation(tok_lparen);
        break;
      case ')':
        make_punctuation(tok_rparen);
        break;
      case '\\':
        make_punctuation(tok_backslash);
        break;
      case '.':
        make_punctuation(tok_dot);
        break;
      default: {
        if (is_identifier())
          make_identifer(start, first);
        else
          make_error();
      }
      }
    }

    return tok;
  }


  // Observers

  // Returns the current location.
  location get_location() const { return loc; }

  // Returns the current line number.
  int get_line() const { return loc.line; }

  // Returns the current column.
  int get_column() const { return loc.column; }



  // Lexing support

  // Returns true when we reach the end of file.
  bool is_eof() const { return first == last; }

  // Returns true if the currenct character compares equal to c.
  bool is_char(char c) const { return !is_eof() && (*first == c); }

  // Returns true if the current character is a letter. 
  bool is_letter() const { return !is_eof() && isalpha(*first); }

  // Returns true if the current character is a digit.
  bool is_digit() const { return !is_eof() && isdigit(*first); }
  
  // Returns true if c is either a letter or digit. If it is, assign that
  // character to c and advance.
  bool is_alphanumeric() const { return !is_eof() && isalnum(*first); }

  // An identifier (variable) in the untyped lambda calculus is a string
  // of alphanumeric characters that cannot start with a digit. An underscore
  // may be used in any position.
  bool is_identifier()
  {
    if (is_letter() || is_char('_')) {
      ++first;
      while (is_alphanumeric() || is_char('_'))
        ++first;
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
  bool consume_ws()
  {
    bool result;
    while (!is_eof() && (consume_horizontal_ws() || consume_vertical_ws()))
      result = true;
    return result;
  }

  // Recognize and consume horizontal whitespace (' ' or tab).
  bool consume_horizontal_ws() 
  { 
    switch (*first) {
    case ' ':
    case '\t':
      ++loc.column;
      ++first;
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
  //
  // FIXME: Sometimes I need to know about the end of line!
  bool consume_vertical_ws()
  {
    switch(*first) {
    case '\r':
    case '\n':
    case '\v':
      ++loc.line;
      loc.column = 1;
      ++first;
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
  void make_eof()
  {
    tok.sym = nullptr;
  }

  // Return a punctuation token.
  //
  // FIXME: This will, in the future, require that we know the length of
  // the punctuator. For now, all tokens are length 1, so its easy.
  void make_punctuation(token_kind kind)
  {
    tok.loc = loc;
    tok.sym = table->get(kind);
    ++first;
    ++loc.column;
  }

  // Enter the given substring as an identifier in the table. If the identifier
  // already exists, return the current string.
  void make_identifer(Iter first, Iter last)
  {
    tok.loc = loc;
    tok.sym = table->put(tok_identifier, first, last);
    first += tok.get_spelling().size();
    loc.column += tok.get_spelling().size();
  }

  void make_error()
  {
    tok.loc = loc;
    tok.sym = table->get(first, first + 1);
    ++first;
    ++loc.column;
  }


  Table* table;   // The current symbol table
  Iter first;     // An iterator to the first input character
  Iter last;      // An iterator past the last input character

  token tok;      // The current token
  location loc;   // The current location
};


/*



int main()
{
  // Crete tokens for built-in types.
  language_table lang;
  lang.put(tok_lparen, "(");
  lang.put(tok_rparen, ")");
  lang.put(tok_backslash, "\\");
  lang.put(tok_dot, ".");

  // Create a global symbol table.
  symbol_table sym{lang};

  string buf;
  for (string line; getline(cin, line); ) {
    buf += line + '\n';
  }

  lexer l{&sym, buf.begin(), buf.end()};
  while (1) {
    token tok = l.lex();
    if (!tok.sym)
      break;
    else
      std::cout << tok << '\n';
  }

  /*
  parser p(buf);
  p.parse_variable();
  */
}
