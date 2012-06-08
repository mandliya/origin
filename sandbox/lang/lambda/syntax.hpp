
#ifndef SYNTAX_HPP
#define SYNTAX_HPP

#include <list>

#include "symbol.hpp"


// The node class is the base class of the abstract syntax tree. This class
// describes the abstract interface for all AST nodes: a method of traversing
// the children of the class (begin, end), and access to the specific kind of
// node.
struct Node
{
  enum Kind
  {
    unit,
    variable,
    abstraction,
    application
  };

  Node(Kind k) : kind{k} { }
  virtual ~Node() { }

  // Returns the a range of child node pointers.  
  virtual Node* const* begin() const = 0;
  virtual Node* const* end() const = 0;

  Kind kind;
};


// The nullary node defines the base interface for all nullary nodes. 
//
// Note that the class contains an empty array so that the n->nodes member
// will always be a valid expression.
template <Node::Kind K>
  struct Nullary_node : Node
  {
    Nullary_node()
      : Node{K}
    { }

    virtual Node* const* begin() const { return nodes; }
    virtual Node* const* end() const { return nodes; }

    Node* nodes[0];
  };


// The unary node class defines the base interface for all unary nodes. The
// child node may be accessed using the child() function.
template <Node::Kind K>
  struct Unary_node : Node
  {
    Unary_node(Node* child)
      : Node{K}, nodes {child}
    { }

    Node* child() const { return nodes[0]; }

    virtual Node* const* begin() const { return nodes; }
    virtual Node* const* end() const { return nodes + 1; }

    Node* nodes[1];
  };


// The binary node class defines the basic interface for all binary nodes.
// The left and right operands of the node are accessible using the left()
// and right() member functions.
template <Node::Kind K>
  struct Binary_node : Node
  {
    Binary_node(Node* left, Node* right)
      : Node{K}, nodes {left, right}
    { }

    Node* left() const { return nodes[0]; }
    Node* right() const { return nodes[1]; }

    virtual Node* const* begin() const { return nodes; }
    virtual Node* const* end() const { return nodes + 2; }

    Node* nodes[2];
  };


// A variable is a leaf node that refers to a name.
//
// FIXME: that we're just referring to the underlying symbols since we don't
// have any more advanced kinds of naming. Eventually, I think we'll have a
// Name hierarchy like IPR. If we had a Name abstraction, we could also rewrite
// the lexical name with a De Bruijn name also. 
struct Variable 
  : Nullary_node<Node::variable>
{
  Variable(symbol* name)
    : Nullary_node<Node::variable>{}, name{name}
  { }

  symbol* name;
};


// An abstraction is a term parameterized by a variable, written "\x.t" where
// x is the variable abstracted over the term t.
//
// Note that var is technically introduced into scope for the duration of
// the lambda expression. If we were looking at the associatd scope of the
// abstraction, it would only include var.
struct Abstraction 
  : Binary_node<Node::abstraction>
{
  Abstraction(Variable* var, Node* term)
    : Binary_node<Node::abstraction>{var, term}
  { }

  Variable* var() const { return static_cast<Variable*>(nodes[0]); }
  Node* term() const { return nodes[1]; }
};


// An application of two tems is writen "t t" and corresponds to function
// application.
using Application = Binary_node<Node::application>;



// The node factory is responsible for the allocation of all nodes. Each 
// different kind of node is maintained in a list of nodes.
struct node_factory
{
  Variable* make_variable(symbol* sym);
  Abstraction* make_abstraction(Variable* var, Node* term);
  Application* make_application(Node* left, Node* right);

  std::list<Variable> var;
  std::list<Abstraction> abs;
  std::list<Application> app;
};

// The unit node represents an entire program (the program node), and provides
// factory interfaces for the creation of all other node types.
class Unit 
  : public Unary_node<Node::unit>
  , public node_factory
{
  using Node_base = Unary_node<Node::unit>;
public:
  Unit()
    : Unary_node<Node::unit>{nullptr}
  { }

  Node* program() const { return nodes[0]; }
  void set_program(Node* node) { nodes[0] = node; }
};

#endif
