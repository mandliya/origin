
#ifndef PARSER_HPP
#define PARSER_HPP

#include <stack>

#include "lexer.hpp"
#include "syntax.hpp"
#include "context.hpp"

// Parser
//
// The parser is responsible for parsing the untyped lambda expression syntax.
// The grammar follows:
//
//    program := nothing 
//             | statements
//
// Statements:
//
//    statements := statement 
//                | statement statements
//
//    statement := decl-statement
//               | eval-statement
//
//    eval-declaration := variable = expression;
//
//    eval-statement := expression;
//
// A declaration binds the variable name in the global context. In those
// respects, it could be thought of as a top-level let expression. An evaluation
// evaluates the expression in the current context, writing the results to
// the output register.
//
// Expressions in the language are:
//
//    exprexssion := abstraction 
//                 | application
//
//    abstraction := \ variable . expression
//
//    application := primary application 
//                 | primary
//
//    primary := (expression) 
//             | variable
//
//    var := identifier
//
// In this language, an expression corresponds to a Term in the AST. This will
// not be true for all lambda calculus-based languages. 
//
// Note that declarations
//
// NOTE: I think I see how to implement tentative parsing. If we copy the
// current lex state (position, location, etc.), then we could restore that
// at a later time. I think that this is going to be closely related to the
// recursive lexing/parsing discussed in the lexer header.
class Parser
{
public:
  Parser(Context& cxt, Lexer& lex);

  bool operator()();

  // Parsing infrastructure
  Token consume();
  Token match(Symbol::Kind kind);

  // Grammar
  void parse_program();

  // Statements
  Statement* parse_statement();
  Definition* parse_definition();
  Evaluation* parse_evaluation();

  // Expression
  Term* parse_expression();
  Term* parse_nested();
  Term* parse_primary();
  Term* parse_application();
  Abstraction* parse_abstraction();
  Variable* parse_variable();

  Context& context() { return cxt; }

private:
  Lexer& lex;
  Context& cxt;
  Token tok;
};

#endif
