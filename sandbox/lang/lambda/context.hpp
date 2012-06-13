
#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <list>

#include "syntax.hpp"
#include "identifier_table.hpp"

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

// The stmt factory provides an interface for accessing....
struct stmt_factory
{
  Definition_impl* make_definition(Variable* var, Term* def);
  Evaluation_impl* make_evaluation(Term* term);

  std::list<Definition_impl> decls;
  std::list<Evaluation_impl> evals;
};


// The Context class provides the primary interface for constructing and
// maintianing an AST. It provides factories for allocating nodes and accessing
// language intrinsics.
class Context : public term_factory
              , public stmt_factory
{
  using Definition_map = identifier_table<Definition>;
public:
  // Returns the program embodied in the context.
  Program*       program()       { return &prog; }
  const Program* program() const { return &prog; }


  // Returns the definition associated with the symbol, or none.
  Definition* find_term(Symbol* sym) const;

  // Register the definition with its defining symbol..
  void define_term(Definition* def);

private:
  Program_impl prog;
  Definition_map map;
};

#endif
