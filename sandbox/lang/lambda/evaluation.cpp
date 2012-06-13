
#include <cassert>
#include <iostream>

#include "evaluation.hpp"
#include "sexpr.hpp"



// Implements the substitution rule:
//
//    [x -> s]t
//
// where x is a variable name, s is the replacement term, and t is the term
// in which the substitution occurs. The specific rules are:
//
//    [x -> s]x        = s
//    [x -> s]y        = y                       if y != x
//    [x -> s](\y. t1) = { \y.t1                 if y = x
//                         \y. [x->s]t1          if y != x and y not in FV(s)
//    [x -> s](t1 t2)  = ([x -> s]t1 [x ->s]t2)
//
// Note that 's' is the substituted term.
struct Subst_visitor : Visitor
{
  Subst_visitor(Variable* x, Term* s)
    : var{x}, subst{s}, result{nullptr}
  { }

  Term* operator()(Term* term);

  void visit_variable(Variable* var);

  Variable* var;
  Term* subst;
  Term* result;
};

Term* 
Subst_visitor::operator()(Term* term)
{
  visit_term(term);
  return result;
}


// Implements the subsituttion:
//
//    [x -> s]x        = s
//    [x -> s]y        = y    if y != x
//
// We preserve the term if the variable does not match.
void 
Subst_visitor::visit_variable(Variable* term)
{
  if (term->symbol() == var->symbol())
    result = subst;
  else
    result = term;
}

// Substitute the term subst for var in the given term.
Term* substitute(Variable* var, Term* subst, Term* term)
{
  Subst_visitor vis(var, subst);
  return vis(term);
}


////////////////////////////////////////////////////////////////////////////////
// Operational semantics
//
// Each algorithm implements a reduction that is used by one or more evaluation
// strategies. 
// 
// Reductions are parameterized over a reduce function that can be used to
// implement small- or large-step reductions.


// Return a reduction of the variable. Typically, this is just the name of
// the variable. However, we allow substitution from pre-defined values to
// occur. That is, if var is mapped to a term in the context, then we reduce
// to the term.
inline Term*
reduce_var(Context& cxt, Variable* var)
{
  if (Definition* def = cxt.find_term(var->symbol()))
    return def->def();
  else
    return var;
}


// Returns a reduction on the first term (the function) of an application.
//
//    t1 --> t
// --------------- E-App-Func
// t1 t2 --> t t2
//
// If t1 can be reduced to t, then reduce it and return a new application
// (t t2). Otherwise, t1 is normal and we cannot apply the rule.
template <typename Reduce>
  inline Term* 
  reduce_app_func(Context& cxt, Application* app, Reduce reduce)
  {
    Term* t1 = app->func();
    Term* t2 = app->arg();

    Term* t = reduce(cxt, t1);
    if (t != t1)
      return cxt.make_application(t, t2);
    else
      return app;
  }


// Returns a reduction on the 2nd term of the function.
//
//    t2 --> t
// -------------- E-App-Arg
// t1 t2 --> t1 t
//
// If t can be reduced to u, then return a new application (t1 t). 
template <typename Reduce>
  inline Term*
  reduce_app_arg(Context& cxt, Application* app, Reduce reduce)
  {
    Term* v = app->func();
    Term* t = app->arg();
    
    Term* u = reduce(cxt, t);
    if (u != t)
      return cxt.make_application(v, u);
    else
      return app;
  }


// Reduces the application of a value to an abstraction by substituting the
// value v everywhere x occurs in the term t.
//
// ----------------------- E-App-Abs
// (\x. t) v --> [x -> v]t
//
// When the the function is an abstraction and and the 2nd argument is
// irriducible, perform a substitution. Note that this will not reduce any
// subterms of the expression.
template <typename Reduce>
  Term*
  reduce_app_abs(Context& cxt, Application* app, Reduce reduce)
  {
    Term* f = app->func();
    Term* a = app->arg();

    if (Abstraction* abs = as<Abstraction>(f)) {
      Term* subst = substitute(abs->var(), a, abs->term());
      return reduce(cxt, subst);
    } else {
      return app;
    }
  }


// Reduces an abstraction by reducing the term over which it is abstracted.
//
//     t --> u
// --------------- Abs-Term
// \x. t --> \x. u
//
// If the term t can be reduced to, then return a new abstraction of the
// variable x over the term u.
template <typename Reduce>
  inline Term*
  reduce_abs_term(Context& cxt, Abstraction* abs, Reduce reduce)
  {
    Variable* x = abs->var();
    Term* t = abs->term();
    Term* u = reduce(cxt, t);
    if (u != t) 
      return cxt.make_abstraction(x, u);
    else
      return abs;
  }



// Reduce the given term using the given call-by-value. This computes the
// entire reduction.
template <typename Step>
  Term* call_by(Context& cxt, Term* term, Step step)
  {
    while (1) {
      Term* q = step(cxt, term);
      if (q == term)
        break;
      term = q;
    }
    return term;
  }


////////////////////////////////////////////////////////////////////////////////
// Call by value reduction

// Implements call-by-value reduction.
struct Value_reduction_visitor : Visitor
{
  Value_reduction_visitor(Context& cxt)
    : cxt(cxt), result{nullptr}
  { }


  void visit_variable(Variable* var);
  void visit_abstraction(Abstraction* abs);
  void visit_application(Application* app);

  Context& cxt;
  Term* result;
};


// If x is a variable in the environment that is mapped to a term t, then
// x reduces to t. Otherwise, x is irreducible.
void
Value_reduction_visitor::visit_variable(Variable* var)
{
  result = reduce_var(cxt, var);
}

void
Value_reduction_visitor::visit_abstraction(Abstraction* abs)
{
  result = abs;
}

// Match and reduce applications.
void
Value_reduction_visitor::visit_application(Application* app)
{
  // Incrementally reduce the first term.
  result = reduce_app_func(cxt, app, step_by_value);
  if (result != app)
    return;

  // Incrementally reduce the second term.
  result = reduce_app_arg(cxt, app, step_by_value);
  if (result != app)
    return;

  result = reduce_app_abs(cxt, app, step_by_value);
}


// Reduce the given term using the call-by-value strategy. This function
// performs only a single step in the reduction.
Term* step_by_value(Context& cxt, Term* term)
{
  Value_reduction_visitor vis(cxt);
  term->accept(vis);
  return vis.result;
}


// Reduce the given term using the call-by-value strategy. This computes the
// entire reduction.
Term* call_by_value(Context& cxt, Term* term)
{
  return call_by(cxt, term, step_by_value);
}


////////////////////////////////////////////////////////////////////////////////
// Call by name reduction

// The name reduction visitor implements call-by-name reduction.
struct Name_reduction_visitor : Visitor
{
  Name_reduction_visitor(Context& cxt) : cxt(cxt) { }

  void visit_variable(Variable* var);
  void visit_abstraction(Abstraction* abs);
  void visit_application(Application* app);

  Context& cxt;
  Term* result;
};


void
Name_reduction_visitor::visit_variable(Variable* var)
{
  result = reduce_var(cxt, var);
}

void
Name_reduction_visitor::visit_abstraction(Abstraction* abs)
{
  result = abs;
}

void
Name_reduction_visitor::visit_application(Application* app)
{
  // Reduce the function argument first.
  result = reduce_app_func(cxt, app, step_by_name);
  if (result != app)
    return;

  // Once the function is reduced, try to apply the argument.
  result = reduce_app_abs(cxt, app, step_by_name);
}


Term* step_by_name(Context& cxt, Term* term)
{
  Name_reduction_visitor vis(cxt);
  term->accept(vis);
  return vis.result;
}

Term* call_by_name(Context& cxt, Term* term)
{
  return call_by(cxt, term, step_by_name);
}


////////////////////////////////////////////////////////////////////////////////
// Normal order reduction

// The normal order visitor implements the normal order reduction.
struct Normal_order_reduction_visitor : Visitor
{
  Normal_order_reduction_visitor(Context& cxt) : cxt(cxt) { }

  void visit_variable(Variable* var);
  void visit_abstraction(Abstraction* abs);
  void visit_application(Application* app);

  Context& cxt;
  Term* result;
};

void
Normal_order_reduction_visitor::visit_variable(Variable* var)
{
  result = reduce_var(cxt, var);
}

// FIXME: This is broken.
void
Normal_order_reduction_visitor::visit_abstraction(Abstraction* abs)
{
  result = reduce_abs_term(cxt, abs, step_normal_order);
}

void
Normal_order_reduction_visitor::visit_application(Application* app)
{
  // Reduce the first term by name.
  result = reduce_app_func(cxt, app, step_by_name);
  if (result != app)
    return;

  // If the 2nd term can be reduced, do so.
  result = reduce_app_arg(cxt, app, step_normal_order);
  if (result != app)
    return;


  // Apply the arguments to the function. If the result is an abstraction,
  // perform the substitution. Otherwise, further reduce the term in normal
  // order.
  //
  // FIXME: This is related reduce_app_abs, and could probably be refactored
  // as a separate rule.
  Term* expr = app->func();
  if (is<Abstraction>(expr)) {
    result = reduce_app_abs(cxt, app, step_normal_order);
  } else {
    result = step_normal_order(cxt, expr);
  }
}


Term* step_normal_order(Context& cxt, Term* term)
{
  Normal_order_reduction_visitor v{cxt};
  term->accept(v);
  return v.result;
}

Term* reduce_normal_order(Context& cxt, Term* term)
{
  return call_by(cxt, term, step_normal_order);
}
