
#include "syntax.hpp"


template <typename List, typename... Args>
  auto emplace_node(List& list, Args&&... args) -> decltype(&list.back())
  {
    list.emplace_back(std::forward<Args>(args)...);
    return &list.back();
  }

Variable*
node_factory::make_variable(symbol* sym)
{
  return emplace_node(var, sym);
}

Abstraction*
node_factory::make_abstraction(Variable* var, Node* term)
{
  return emplace_node(abs, var, term);
}

Application*
node_factory::make_application(Node* left, Node* right)
{
  return emplace_node(app, left, right);
}
