
#include <iostream>
#include <stdexcept>

#include "parser.hpp"
#include "sexpr.hpp"

using namespace std;


// Helper functions
namespace 
{
  // Returns true if the given token indicates the start of a term: a variable,
  // left paren, or backslash.
  inline bool
  starts_term(Token& tok)
  {
    return tok.is(Symbol::Identifier) 
        || tok.is(Symbol::Lparen) 
        || tok.is(Symbol::Backslash);
  }
}

////////////////////////////////////////////////////////////////////////////////
// Parser

// TODO: Write a match function that tests the current token for some property
// and a) advances if the property is satisifed, but b) returns a copy of the
// initial token.

Parser::Parser(Context& cxt, Lexer& lex)
  : lex(lex), cxt(cxt), tok{}
{ }

// Consume the current token, moving to the next.
Token
Parser::consume()
{
  Token tmp{tok};
  tok = lex();
  return tmp;
}

// If the current token matches the given symbol kind, then consume the
// current token, returning a copy of it. If the symbol does not match, return
// an empty token.
Token
Parser::match(Symbol::Kind kind)
{
  if (tok.is(kind))
    return consume();
  else
    return {};
}


// The main entry point into the parser. Initialize the current token and
// parse a term: the basic entry point into an untyped lambda calculus
// program.
bool
Parser::operator()()
{
  // Move to the first token.
  consume();

  // Try to parse a program.
  try {
    parse_program();
  } catch(runtime_error& err) {
    std::cout << "error: " << err.what() << '\n';
    return false;
  }
  return true;
}


// program := nothing | expression
void
Parser::parse_program()
{
  while (tok) {
    if (Statement* stmt = parse_statement())
      cxt.add_statement(stmt);
    else
      break;
  }
}

// statement := declaration | evaluation
Statement*
Parser::parse_statement()
{
  Token la = lex(1);
  if (la.is(Symbol::Equal))
    return parse_declaration();
  else
    return parse_evaluation();
}

Evaluation* 
Parser::parse_evaluation()
{
  if (Term* term = parse_expression()) {
    if (Token semi = match(Symbol::Semicolon))
      return cxt.make_evaluation(term);
    else
      throw std::runtime_error("expecting ';' after expression");
  } else {
    return nullptr;
  }
}

Declaration*
Parser::parse_declaration()
{
  if (Variable* var = parse_variable()) {
    if (Token eq = match(Symbol::Equal)) {
      if (Term* term = parse_expression()) {
        if (Token semi = match(Symbol::Semicolon))
          return cxt.make_declaration(var, term);
        else
          throw std::runtime_error("expecting ';' after expression");
      } else {
        throw std::runtime_error("expecting expression after '='");
      }
    } else {
      throw std::runtime_error("expecting '=' after variable");
    }
  } else {
    return nullptr;
  }
}


// expression := abstraction | application
Term*
Parser::parse_expression()
{
  if (Term* abs = parse_abstraction())
    return abs;
  if (Term* app = parse_application())
    return app;
}

// primary := (expression) | variable
Term*
Parser::parse_primary()
{
  if (tok.is(Symbol::Lparen))
    return parse_compound();
  if (tok.is_identifier())
    return parse_variable();
  else
    return nullptr;
}

// abstraction := \variable.expression
Abstraction*
Parser::parse_abstraction()
{
  if (Token slash = match(Symbol::Backslash)) {
    if (Variable* var = parse_variable()) {
      if (Token dot = match(Symbol::Dot)) {
        if (Term* expr = parse_expression()) {
          return cxt.make_abstraction(var, expr);
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

// application := application primary | primary
//
// Note that this production may result in either an Application or a Term.
Term*
Parser::parse_application()
{
  Term* left = parse_primary();
  while (1) {
    if (Term* right = parse_primary())
      left = cxt.make_application(left, right);
    else
      break;
  }
  return left;
}


Term*
Parser::parse_compound()
{
  // FIXME: I think that we've already compared for the lparen by the time
  // we get to this point. I should have a primitive called "expect" that
  // asserts the property, and returns the token.
  if (Token left = match(Symbol::Lparen)) {
    if (Term* term = parse_expression()) {
      if (Token right = match(Symbol::Rparen))
        return term;
      else
        throw std::runtime_error("expecting ')' after expression");
    } else {
      throw std::runtime_error("expecting expression after ')'");
    }
  } else {
    return nullptr;
  }
}

// Parse a variable expression.
Variable*
Parser::parse_variable()
{
  // FIXME: See comments for parse_compound.
  if (Token id = match(Symbol::Identifier))
    return cxt.make_variable(id.sym);
  else
    return nullptr;
}
