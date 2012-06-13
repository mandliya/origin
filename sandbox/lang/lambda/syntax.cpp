
#include <cassert>

#include "syntax.hpp"


void
Variable::accept(Visitor& vis)
{
  vis.visit_variable(this);
}

void
Abstraction::accept(Visitor& vis)
{
  vis.visit_abstraction(this);
}

void
Application::accept(Visitor& vis)
{
  vis.visit_application(this);
}

void
Definition::accept(Visitor& vis)
{
  vis.visit_definition(this);
}

void
Evaluation::accept(Visitor& vis)
{
  vis.visit_evaluation(this);
}

void
Program::accept(Visitor& vis)
{
  vis.visit_program(this);
}


////////////////////////////////////////////////////////////////////////////////

void
Visitor::visit_node(Node* node)
{
  if (Statement* stmt = as<Statement>(node))
    visit_statement(stmt);
  if (Term* term = as<Term>(node))
    visit_term(term);
  if (Program* prog = as<Program>(node))
    visit_program(prog);
}

void
Visitor::visit_program(Program* prog)
{ }

void
Visitor::visit_statement(Statement* stmt) 
{
  switch (stmt->kind) {
    case Node::Definition_node:
      visit_definition(as<Definition>(stmt));
      break;
    case Node::Evaluation_node:
      visit_evaluation(as<Evaluation>(stmt));
      break;
    default:
      assert(false);
  }
}

void
Visitor::visit_definition(Definition*)
{ }

void
Visitor::visit_evaluation(Evaluation*)
{ }

void
Visitor::visit_term(Term* term)
{
  switch (term->kind) {
    case Node::Variable_node:
      visit_variable(as<Variable>(term));
      break;
    case Node::Abstraction_node:
      visit_abstraction(as<Abstraction>(term));
      break;
    case Node::Application_node:
      visit_application(as<Application>(term));
      break;
    default:
      assert(false);
  }
}

void
Visitor::visit_variable(Variable* var)
{ }

void
Visitor::visit_abstraction(Abstraction* abs)
{ }

void
Visitor::visit_application(Application* app)
{ }
