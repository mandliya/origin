
#include <iostream>

#include "sexpr.hpp"

using namespace std;

Sexpr_printer::Sexpr_printer(ostream& os)
  : os(os)
{ }

void 
Sexpr_printer::visit_declaration(Declaration* decl)
{
  os << '(' << "decl" << ' ';
  visit_variable(decl->var());
  os << ' ';
  visit_term(decl->def());
  os << ')' << '\n';
}

void 
Sexpr_printer::visit_evaluation(Evaluation* eval)
{
  os << '(' << "eval" << ' ';
  visit_term(eval->term());
  os << ')' << '\n';
}

void 
Sexpr_printer::visit_variable(Variable* var)
{
  os << var->name();
}

void 
Sexpr_printer::visit_abstraction(Abstraction* abs)
{
  os << '(' << "lambda" << ' ';
  visit_variable(abs->var());
  os << ' ';
  visit_term(abs->term());
  os << ')';
}

void 
Sexpr_printer::visit_application(Application* app)
{
  os << '(';
  visit_term(app->func());
  os << ' ';
  visit_term(app->arg());
  os << ')';
}

ostream&
operator<<(ostream& os, const sexpr& x)
{
  Sexpr_printer p(os);
  p.visit_node(x.node);
  return os;
}
