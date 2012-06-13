
#include "context.hpp"

template <typename List, typename... Args>
  auto emplace_node(List& list, Args&&... args) -> decltype(&list.back())
  {
    list.emplace_back(std::forward<Args>(args)...);
    return &list.back();
  }

Variable_impl*
term_factory::make_variable(Symbol* sym)
{
  return emplace_node(var, sym);
}

Abstraction_impl*
term_factory::make_abstraction(Variable* var, Term* term)
{
  return emplace_node(abs, var, term);
}

Application_impl*
term_factory::make_application(Term* left, Term* right)
{
  return emplace_node(app, left, right);
}

Definition_impl*
stmt_factory::make_definition(Variable* var, Term* def)
{
  return emplace_node(decls, var, def);
}

Evaluation_impl*
stmt_factory::make_evaluation(Term* term)
{
  return emplace_node(evals, term);
}


////////////////////////////////////////////////////////////////////////////////

Definition*
Context::find_term(Symbol* sym) const
{
  return map.find(sym);
}

void
Context::define_term(Definition* def)
{
  map.define(def->var()->symbol(), def);
}
