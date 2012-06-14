
#include <iostream>
#include <stdexcept>

#include "parser.hpp"
#include "sexpr.hpp"

using namespace std;


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
      cxt.program()->add_statement(stmt);
    else
      break;
  }
}

// statement := declaration | evaluation
Statement*
Parser::parse_statement()
{
  if (tok.is(Symbol::Identifier)) {
    Token id = consume();
    if (tok.is(Symbol::Equal))
      return parse_definition(id);
    else
      return parse_evaluation(id);
  } else {
    return parse_evaluation();
  }

  /*
  Token la = lex(1);
  if (la.is(Symbol::Equal))
    return parse_definition();
  else
    return parse_evaluation();
  */
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

// Parse an evaluation statement that starts with the given id. This means
// we could either be parsing a variable or an application.
Evaluation* 
Parser::parse_evaluation(Token id)
{
  Term* left = cxt.make_variable(id.sym);
  Term* term = parse_application(left);
  return cxt.make_evaluation(term);
}

// Try to parse a definition.
Definition*
Parser::parse_definition(Token id)
{
  // Make sure we don't re-define terms.
  if (Definition* def = cxt.find_term(id.sym)) {
    // FIXME: Emit the location of the last definition.
    throw runtime_error("symbol '" + tok.spelling() + "'' already defined");
  }

  Variable* var = cxt.make_variable(id.sym);
  if (Token eq = match(Symbol::Equal)) {
    if (Term* term = parse_expression()) {
      if (Token semi = match(Symbol::Semicolon)) {
        Definition* def = cxt.make_definition(var, term);
        cxt.define_term(def);
        return def;
      } else {
        throw std::runtime_error("expecting ';' after expression");
      }
    } else {
      throw std::runtime_error("expecting expression after '='");
    }
  } else {
    throw std::runtime_error("expecting '=' after variable");
  }

  // This should not be a reachable location.
  throw runtime_error("internal compiler error");
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
    return parse_nested();
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
  return parse_application(left);
}

// A helper function for parsing the remainder of a sequence of applications.
// The left-most term has already been parsed.
Term*
Parser::parse_application(Term* left)
{
  while (1) {
    if (Term* right = parse_primary())
      left = cxt.make_application(left, right);
    else
      break;
  }
  return left;
}

Term*
Parser::parse_nested()
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

