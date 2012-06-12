
#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <list>

#include "syntax.hpp"

// The node factory is responsible for the allocation of all nodes. Each 
// different kind of node is maintained in a list of nodes.
struct term_factory
{
  Variable_impl* make_variable(Symbol* sym);
  Abstraction_impl* make_abstraction(Variable* var, Term* term);
  Application_impl* make_application(Term* left, Term* right);

  std::list<Variable_impl> var;
  std::list<Abstraction_impl> abs;
  std::list<Application_impl> app;
};

struct stmt_factory
{
  Declaration_impl* make_declaration(Variable* var, Term* def);
  Evaluation_impl* make_evaluation(Term* term);

  std::list<Declaration_impl> decls;
  std::list<Evaluation_impl> evals;
};


// The Context class provides the primary interface for constructing and
// maintianing an AST. It provides factories for allocating nodes and accessing
// language intrinsics.
//
// FIXME: Currently, the context owns the list of statements. This is probably
// not correct; there should be a Program multi-node that owns the list of
class Context : public term_factory
              , public stmt_factory
{
public:
  using Statement_list = std::list<Statement*>;

  // Add a statement.
  void add_statement(Statement* s) { stmts.push_back(s); }

  Statement_list stmts;
};

#endif
