
#ifndef PRINT_SEXPR_HPP
#define PRINT_SEXPR_HPP

#include <iosfwd>

#include "syntax.hpp"

struct Sexpr_printer : public Visitor
{
  Sexpr_printer(std::ostream& os);

  virtual void visit_definition(Definition* decl);
  virtual void visit_evaluation(Evaluation* eval);

  virtual void visit_variable(Variable* var);
  virtual void visit_abstraction(Abstraction* abs);
  virtual void visit_application(Application* app);

  std::ostream& os;
};


// Streaming support for sexprs.
struct sexpr
{
  sexpr(Node* n) : node(n) { }

  Node* node;
};

std::ostream& operator<<(std::ostream& os, const sexpr& x);

#endif
