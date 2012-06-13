
#ifndef IDENTIFIER_TABLE_HPP
#define IDENTIFIER_TABLE_HPP

#include <unordered_map>

class Symbol;
class Node;

// The identifier table contains a mapping of symbols to a nodes. This class
// implements a unique mapping between symbols and nodes. Use 
// chained_identifier_table to associate multiple definitions with the same
// name.
//
// Note that T is a node type (not a pointer!).
template <typename T>
  class identifier_table
  {
    using Identifier_map = std::unordered_map<Symbol*, T*>;
  public:

    // Associate the symbol sym with the given node. Returns false if the
    // symbol is already associated with a node.
    bool define(Symbol* sym, T* node);

    // Return the node associated with the given symbol or nullptr if no such
    // exists.
    T* find(Symbol* s) const;

  private:
    Identifier_map idents;
  };


template <typename T>
  inline bool 
  identifier_table<T>::define(Symbol* sym, T* node)
  {
    auto p = idents.insert({sym, node});
    return p.second;
  }

template <typename T>
  inline T*
  identifier_table<T>::find(Symbol* sym) const
  {
    auto i = idents.find(sym);
    return i == idents.end() ? nullptr : i->second;
  }


#endif
