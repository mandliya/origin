
#include <iostream>
#include <stdexcept>

#include "parser.hpp"

using namespace std;

parser::parser(lexer& lex)
  : lex(lex), tok{}
{ }

// Consume the current token, moving to the next.
void
parser::consume()
{
  tok = lex();
}

// The main entry point into the parser. Initialize the current token and
// parse a term: the basic entry point into an untyped lambda calculus
// program.
bool
parser::operator()()
{
  consume();
  parse_term();
}

// Parse a variable expression.
Variable*
parser::parse_variable()
{
  lstring var;
  if (tok.is_identifier())
    return unit.make_variable(tok.sym);
  else
    return nullptr;
}

// Parse an expression of the form:
//
//    \ <var> . <term>
Abstraction*
parser::parse_abstraction()
{
  if (tok.is(sym_backslash)) {
    consume();
    if (Variable* var = parse_variable()) {
      consume();
      if (tok.is(sym_dot)) {
        consume();
        if (Node *term = parse_term()) {
          consume();
          return unit.make_abstraction(var, term);
        } else {
          throw std::runtime_error("could not parse abstracted term in abstraction");
        }
      } else {
        throw std::runtime_error("expecing '.' after variable in abstraction");
      }
    } else {
      throw std::runtime_error("could not parse variable in abstraction");
    }
  } else {
    return nullptr;
  }
}


// Parse an expression of the form:
//
//    <term> <term>
Application*
parser::parse_application()
{
  if (Node* left = parse_term()) {
    if (Node* right = parse_term())
      return unit.make_application(left, right);
    else 
      throw std::runtime_error("could not parse argument to application");
  } else {
    return nullptr;
  }
}

// Parse an expression of the form:
//
//    ( <term> )
Node*
parser::parse_bracketed()
{
  if (tok.is(sym_lparen)) {
    consume();
    if (Node* expr = parse_term()) {
      if (tok.is(sym_rparen))
        return expr;
      else
        throw std::runtime_error("expecting ')' in bracketed term");
    } else {
      throw std::runtime_error("could not parse enclosed term");
    }
  } else {
    return nullptr;
  }
}

Node*
parser::parse_term()
{
  if (Node* e = parse_bracketed())
    return e;
  if (Abstraction* abs = parse_abstraction())
    return abs;
  if (Application* app = parse_application())
    return app;
  if (Variable* var = parse_variable())
    return var;
  else
    throw std::runtime_error("failure\n");
}

