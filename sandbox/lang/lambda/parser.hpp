
#ifndef PARSER_HPP
#define PARSER_HPP

#include "lexer.hpp"
#include "syntax.hpp"

// Paser
//
// The parser is responsible for parsing the untyped lambda expression syntax.
// The grammar follows:
//
//    t ::= x       // variable
//          \x.t    // abstraction
//          t t     // application
//          (e)     // bracketed expression
//
// For the sake of simplity, we assume that a variable can be any identifier
// of the for [a-zA-Z].
//
//
// NOTE: I think I see how to implement tentative parsing. If we copy the
// current lex state (position, location, etc.), then we could restore that
// at a later time. I think that this is going to be closely related to the
// recursive lexing/parsing discussed in the lexer header.
class parser
{
public:
  parser(lexer& lex);

  bool operator()();

  // Parsing infrastructure
  // FIXME: Make a function that compares and consumes at the same time.
  void consume();

  Variable* parse_variable();
  Abstraction* parse_abstraction();
  Application* parse_application();
  Node* parse_bracketed();
  Node* parse_term();

private:
  lexer& lex;
  token tok;

  Unit unit;
};

#endif
