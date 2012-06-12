
#ifndef SYNTAX_HPP
#define SYNTAX_HPP

#include <vector>
#include <list>

#include "location.hpp"
#include "symbol.hpp"

struct Visitor;

// There are several layers of the AST design:
//
// Common infrastructure -- a set of classes expected to be common to all
// languages or a family of languages. This inclues the node interface and
// associated helper classes.
//
// Language interaface -- a set of node classes that conform to the set of
// terms defined by the language. These are purely interface classess.
//
// Language implementation -- a set of node classes that implement the
// language interface, often by deriving and implementing a set of interfaces.
//
// Note that the Unit class is both infrastructure and language implementaiton
// since it combines factories for the different implementations.


// The node class is the base class of the abstract syntax tree. This class
// describes the abstract interface for all AST nodes: a method of traversing
// the children of the class (begin, end), and access to the specific kind of
// node.
struct Node
{
  enum Kind
  {
    // Terms
    Variable_node,
    Abstraction_node,
    Application_node,

    // Statements
    Declaration_node,
    Evaluation_node,
  };

  Node(Kind k) : kind{k} { }
  virtual ~Node() { }

  // Returns the a range of child node pointers.  
  virtual Node* const* begin() const = 0;
  virtual Node* const* end() const = 0;

  virtual void accept(Visitor& vis) = 0;

  Kind kind;
  Location loc;
};


// Returns a pointer type the node U, typed as the node T.
template <typename T, typename U>
  inline T* as(U* node)
  {
    return dynamic_cast<T*>(node);
  }

template <typename T, typename U>
  inline const T* as(const U* node)
  {
    return dynamic_cast<const T*>(node);
  }



// The nullary node defines the base interface for all nullary nodes. 
//
// Note that the class contains an empty array so that the n->nodes member
// will always be a valid expression.
template <typename Base>
  struct Nullary_node : Base
  {
    Nullary_node() 
      : Base()
    { }

    virtual Node* const* begin() const { return nodes; }
    virtual Node* const* end() const { return nodes; }

    Node* nodes[0];
  };


// The unary node class defines the base interface for all unary nodes. The
// child node may be accessed using the child() function.
template <typename Base>
  struct Unary_node : Base
  {
    Unary_node(Node* child)
      : Base(), nodes{child}
    { }

    Node* child() const { return nodes[0]; }

    Node* first() const { return nodes[0]; }

    virtual Node* const* begin() const { return nodes; }
    virtual Node* const* end() const { return nodes + 1; }


    Node* nodes[1];
  };


// The binary node class defines the basic interface for all binary nodes.
// The left and right operands of the node are accessible using the left()
// and right() member functions.
template <typename Base>
  struct Binary_node : Base
  {
    Binary_node(Node* left, Node* right)
      : Base(), nodes{left, right}
    { }

    Node* left() const { return nodes[0]; }
    Node* right() const { return nodes[1]; }

    Node* first() const { return nodes[0]; }
    Node* second() const { return nodes[1]; }

    virtual Node* const* begin() const { return nodes; }
    virtual Node* const* end() const { return nodes + 2; }

    Node* nodes[2];
  };


// A multi-node contains a sequence of nodes of the specified type.
template <typename Base, typename Node>
  struct Multi_node : Base
  {
    using Node_list = std::vector<Node*>;

    template <typename I>
      Multi_node(I first, I last)
        : Base(), nodes{first, last}
      { }

    template <typename R>
      Multi_node(const R& range)
        : Multi_node(std::begin(range), std::end(range))
      { }

    Multi_node(std::initializer_list<Node*> list)
      : Multi_node(list.begin(), list.end())
    { }

    virtual Node* const* begin() const { return nodes.data(); }
    virtual Node* const* end() const { return nodes.data() + nodes.size(); }

    Node_list nodes;
  };



////////////////////////////////////////////////////////////////////////////////
// Language interface

// Terms
// There are three terms in the untyped lambda calculus: variables, 
// abstractions, and applications.
//
// FIXME: The abstract class could be called Expression, not Term.


// All nodes in the language are terms.
struct Term : Node 
{
  Term(Kind kind) : Node(kind) { }
};

// A variable is a term that refers to a name in the environment.
struct Variable : Term
{
  Variable() : Term(Variable_node) { }

  virtual void accept(Visitor& vis);

  virtual const String& name() const = 0;
};

// An application (function application) applies a function (the first argument)
// to an argument (the second argument).
struct Application : Term
{
  Application() : Term(Application_node) { }

  virtual Term* func() const = 0;
  virtual Term* arg() const = 0;

  virtual void accept(Visitor& vis);
};


// An abstraction (lambda experssion)...
struct Abstraction : Term
{
  Abstraction() : Term(Abstraction_node) { }

  virtual void accept(Visitor& vis);

  virtual Variable* var() const = 0;
  virtual Term* term() const = 0;
};



// Statements
// Statements are distinct from terms in the current design since they trivially
// manipulate the context. We do not currently have formal semantics associated 
// with them.

// The base statement class
struct Statement : Node
{
  Statement(Node::Kind kind) : Node(kind) { }
};


// A declaration associates a variable with a given term in the program.
//
// This might more appropriately be called a Definition.
struct Declaration : Statement
{
  Declaration() : Statement(Declaration_node) { }

  virtual void accept(Visitor& vis);

  virtual Variable* var() const = 0;
  virtual Term* def() const = 0;
};

// An evaluation represents an evaluation request.
struct Evaluation : Statement
{
  Evaluation() : Statement(Evaluation_node) { }

  virtual void accept(Visitor& vis);

  virtual Term* term() const = 0;
};


////////////////////////////////////////////////////////////////////////////////
// Language implementation

// FIXME: For some reason, the design using templates is giving me problems
// allocating objects -- the abstract methods aren't being linked together
// correctly.


// A variable is a leaf node that refers to a name.
//
// FIXME: that we're just referring to the underlying symbols since we don't
// have any more advanced kinds of naming. Eventually, I think we'll have a
// Name hierarchy like IPR. If we had a Name abstraction, we could also rewrite
// the lexical name with a De Bruijn name also. 
struct Variable_impl : Nullary_node<Variable>
{
  Variable_impl(Symbol* sym)
    : Nullary_node<Variable>() , sym{sym}
  { }

  virtual const String& name() const { return sym->spelling; }

  Symbol* sym;
};


// An abstraction...
struct Abstraction_impl : Binary_node<Abstraction>
{
  Abstraction_impl(Variable* var, Term* term)
    : Binary_node<Abstraction>(var, term)
  { }

  virtual Variable* var() const { return static_cast<Variable*>(nodes[0]); }
  virtual Term* term() const { return static_cast<Term*>(nodes[1]); }
};


// Application...
struct Application_impl : Binary_node<Application>
{
  Application_impl(Term* left, Term* right)
    : Binary_node<Application>(left, right)
  { }

  virtual Term* func() const { return static_cast<Term*>(nodes[0]); }
  virtual Term* arg() const { return static_cast<Term*>(nodes[1]); }
};


// Declaration
struct Declaration_impl : Binary_node<Declaration>
{
  Declaration_impl(Variable* var, Term* term)
    : Binary_node<Declaration>(var, term)
  { }

  virtual Variable* var() const { return as<Variable>(first()); }
  virtual Term* def() const { return as<Term>(second()); }
};

// Evaluation
struct Evaluation_impl : Unary_node<Evaluation>
{
  Evaluation_impl(Term* term)
    : Unary_node<Evaluation>(term)
  { }

  virtual Term* term() const { return as<Term>(first()); }
};


////////////////////////////////////////////////////////////////////////////////


struct Visitor
{
  virtual void visit_node(Node* node);

  virtual void visit_statement(Statement* stmt);
  virtual void visit_declaration(Declaration* decl) ;
  virtual void visit_evaluation(Evaluation* eval);

  virtual void visit_term(Term* term);
  virtual void visit_variable(Variable* var);
  virtual void visit_abstraction(Abstraction* abs);
  virtual void visit_application(Application* app);
};



#endif
