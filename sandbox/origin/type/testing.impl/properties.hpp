// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_TYPE_TESTING_HPP
#  error This file cannot be included directly. Include type/testing.hpp.
#endif

namespace testing
{

  //////////////////////////////////////////////////////////////////////////////
  // Function Properties
  //
  // The following classes describe testable properties of relations and
  // operation. There are two kinds of testable properties:
  //
  //    - a logical formula quantified over the domain of a function, and
  //    - a set of properties.
  //
  // Every logical formula (axiom, property, proposition, etc.) is encapsulated
  // within a class. The class implements a test oracle that decides whether the
  // property holds for specific inputs.
  //
  // Every set of properties is also encapsulated within a class. Unlike the
  // properties above, these must be tested against sets of values. This is
  // done by testing the property over a quantified set of values (usually
  // randomly generated).




  //////////////////////////////////////////////////////////////////////////////
  // Check Equal
  //
  // Returns true if two values, a and b, compare equal.
  //
  // Unless specifically overloaded, this function compares for equuality using
  // the == operator, which implicitly requires that T is equality comparable.
  // Overloads of this function must implement an equivalence relation.
  //
  // Parameters:
  //    a, b -- The arguments being compared for equality.
  //
  // Returns:
  //    True when a and b compare equal.
  template <typename T>
    inline bool
    check_equal(const T& a, const T& b)
    {
      return a == b;
    }


  //////////////////////////////////////////////////////////////////////////////
  // Check Less
  //
  // Returns true if a value, a, is strictly ordered before the value, b.
  //
  // Unless specifically overloaded, this function compares for order using the
  // < operator, which implicitly requires that T is weakly ordered. Overloads
  // of this function must implement a strict weak order.
  //
  // Parameters:
  //    a, b -- The arguments being compared for order.
  //
  // Returns:
  //    True when a is strictly ordered before b.
  template <typename T>
    inline bool
    check_less(const T& a, const T& b)
    {
      return a == b;
    }


  //////////////////////////////////////////////////////////////////////////////
  // Logical Equivalence
  //
  // Two logical expressions e1 and e2 are logically equivalent if, for all
  // args, e1(args...) == e2(args...). By "logical" expressions, we mean
  // two expressions returning bool.
  //
  // Template Parameters:
  //    Args -- A list of arguments for the expressions e1 and e2.
  template <typename... Args>
    struct logical_equivalence
    {
      template <typename E1, typename E2>
        bool 
        operator()(E1 e1, E2 e2, Args&&... args) const
        {
          return 
            e1(std::forward<Args>(args)...) == e2(std::forward<Args>(args)...);
        }
    };


  //////////////////////////////////////////////////////////////////////////////
  // Common Type Equivalence
  //
  // The common type equivalence property states that any expression involving
  // arguments of different types, but sharing a common type, must be equivalent
  // to (have the same observable effects as) the same expression with all
  // arguments converted to the formal type. Formally stated any binary
  // expression e the common type equivalence property is, for all values a of
  // type T and all values b of type U where T and U share a common type C,
  // the following holds:
  //
  //    e(a, b) <=> e(C(a), C(b))
  //
  // The more general property holds for expressions of any arity.
  //
  // Template Parameters:
  //    T -- A type
  //    U -- A type
  //
  // TODO: Generalize the property for exprssions of arbitrary arity.
  template <typename T, typename U>
  struct common_type_equivalence : testable
    {
      using C = Common_type<T, U>;

      common_type_equivalence(context& cxt)
        : testable(cxt)
      { }

      template <typename E>
        bool
        operator()(E e, const T& a, const U& b) const
        {
          return check_equal(e(a, b), e(C(a), C(b)));
        }
    };



  //////////////////////////////////////////////////////////////////////////////
  // Common Type Symmetry
  //
  // Common type symmetry (or more precisely symmetric common type equivalence)
  // extends common type equivalence such that if e(a, b) is equivalent to
  // it's common form, then so is e(b, a).
  //
  // Template Parameters:
  //    T -- A type
  //    U -- A type
  template <typename T, typename U>
    struct common_type_symmetry : testable
    {
      using C = Common_type<T, U>;

      common_type_symmetry(context& cxt)
        : testable(cxt)
      { }

      template <typename E>
        bool
        operator()(E e, const T& a, const U& b) const
        {
          return check_equal(e(a, b), e(C(a), C(b)))
              && check_equal(e(b, a), e(C(b), C(a)));
        }
    };




  //////////////////////////////////////////////////////////////////////////////
  // Reflexive Property
  //
  // A relation is reflexive if r(a, a) is true for all a of the domain of r.
  struct reflexive : testable
  {
    reflexive(context& cxt);

    template <typename R, typename T>
      bool operator()(R r, const T& a) const {return r(a, a); }
  };



  //////////////////////////////////////////////////////////////////////////////
  // Irreflexive Property
  //
  // A relation is irreflexive if r(a, a) is false for all a of the domain of r. 
  // Equivalently, !r(a, a) is true for all a in the domain of r.
  struct irreflexive : testable
  {
    irreflexive(context& cxt);

    template <typename R, typename T>
      bool operator()(R r, const T& a) const { return !r(a, a); }
  };
 


  //////////////////////////////////////////////////////////////////////////////
  // Symmetric Property
  //
  // A relation is symmetric if r(a, b) => r(b, a) for all a and b in the
  // domain of r.
  struct symmetric : testable
  {
    symmetric(context& cxt);

    template <typename R, typename T>
      bool 
      operator()(R r, const T& a, const T& b) const 
      { 
        return r(a, b) ? r(b, a) : true; 
      }
  };


  //////////////////////////////////////////////////////////////////////////////
  // Asymmetric Property
  //
  // A relation is asymmetric if r(a, b) => !r(b, a) for all a and b in the
  // domain of r.
  struct asymmetric : testable
  {
    asymmetric(context& cxt);

    template <typename R, typename T>
      bool
      operator()(R r, const T& a, const T& b) const
      {
        return r(a, b) ? !r(b, a) : true;
      }
  };


  
  //////////////////////////////////////////////////////////////////////////////
  // Antisymmetric Property
  //
  // A relation is antisymmetric if r(a, b) && r(b, a) => a == b for all a and b
  // in the domain of r.
  struct antisymmetric : testable
  {
    antisymmetric(context& cxt);

    template <typename R, typename T>
      bool
      operator()(R r, const T& a, const T& b) const
      {
        return r(a, b) && r(b, a) ? a == b : true;
      }
  };



  //////////////////////////////////////////////////////////////////////////////
  // Transitive Property
  //
  // A relation is transitive if r(a, b) && r(b, c) => r(a, c) for all a, b,
  // and c in the domain of r.
  struct transitive : testable
  {
    transitive(context& cxt);

    template <typename R, typename T>
      bool
      operator()(R r, const T& a, const T& b, const T& c) const
      {
        return r(a, b) && r(b, c) ? r(a, c) : true;
      }
  };



  //////////////////////////////////////////////////////////////////////////////
  // Trichotomy Law
  //
  // A relation, r, obeys the trichotomy law if exactly one of the following
  // holds:
  //
  //    r(a, b)
  //    !r(a, b)
  //    a == b
  //
  // for all a and b in the domain of r.
  struct trichotomous : testable
  {
    trichotomous(context& cxt);

    template <typename R, typename T>
      bool 
      operator()(R r, const T& a, const T& b) const
      {
        if (a == b)         // Equality is the same as equivalence
          return !r(a, b) && !r(b, a);
        else if (r(a, b))   // Asymmetric property
          return !r(b, a);
        else                // Exactly one must hold.
          return r(b, a);
      }
  };

    

  //////////////////////////////////////////////////////////////////////////////
  // Equivalence Relation
  //
  // An equivalence relation is a relation that is reflexive, symmetric, and 
  // transitive.
  struct equivalence : testable
  {
    equivalence(context& cxt);

    template <typename R, typename T>
      void
      operator()(R&& rvar, T&& tvar) const
      {
        quick_check(cxt, refl, rvar, tvar);
        quick_check(cxt, sym, rvar, tvar, tvar);
        quick_check(cxt, trans, rvar, tvar, tvar, tvar);
      }

    reflexive refl;
    symmetric sym;
    transitive trans;
  };


  //////////////////////////////////////////////////////////////////////////////
  // Strict Ordering Relation
  //
  // An ordering relation is irreflexive, asymmetric, and transitive. Without
  // further qualification a strict ordering is assumed to be a partial
  // ordering.
  struct strict_ordering : testable
  {
    strict_ordering(context& cxt);

    template <typename R, typename T>
      void
      operator()(R&& rvar, T&& tvar) const
      {
        quick_check(cxt, irrefl, rvar, tvar);
        quick_check(cxt, asym, rvar, tvar, tvar);
        quick_check(cxt, trans, rvar, tvar, tvar, tvar);
      }

    irreflexive irrefl;
    asymmetric asym;
    transitive trans;
  };


  //////////////////////////////////////////////////////////////////////////////
  // Strict Weak Ordering Relation
  //
  // A strict weak ordering is a strict ordering relation, r, where the
  // symmetric complement, !r(a, b) && !r(b, a), is an equivalence.
  struct strict_weak_ordering : testable
  {
    strict_weak_ordering(context& cxt);

    template <typename R, typename T>
      void
      operator()(R&& rvar, T&& tvar) const
      {
        // Derive the symmetric complement of r and quantify over it. I
        // wouldn't need to do this if I had a symmetric_complement function.
        using U = Result_of<T()>;
        auto r = rvar();
        auto sc = [r](const U& a, const U& b) { return !r(a, b) && !r(b, a); }; 
        auto svar = single(sc);
        
        quick_check(cxt, ord, rvar, tvar);
        quick_check(cxt, eq, svar, tvar);
      }

    strict_ordering ord;
    equivalence eq;
  };


  //////////////////////////////////////////////////////////////////////////////
  // Strict Total Ordering Relation
  //
  // A strict total ordering is a special case of a strict weak ordering where
  // the symmetric complement is the same as equality. Another way of saying
  // this is that the relation is transitive and trichotomous.
  struct strict_total_ordering : testable
  {
    strict_total_ordering(context& cxt);

    template <typename R, typename T>
      void
      operator()(R&& rvar, T&& tvar) const
      {
        quick_check(cxt, trans, rvar, tvar, tvar, tvar);
        quick_check(cxt, tri, rvar, tvar, tvar);
      }

    transitive trans;
    trichotomous tri;
  };



  //////////////////////////////////////////////////////////////////////////////
  // Testing functions


  // Check that T and U satisfy the common type equivalence property for the
  // expression e.
  template <typename T, typename U, typename E>
    void 
    check_common_type_equivalence(context& cxt, E e)
    {
      auto evar = single(e);
      auto tvar = quantify_over<T>(cxt);
      auto uvar = quantify_over<U>(cxt);
      quick_check(cxt, common_type_equivalence<T, U> {cxt}, evar, tvar, uvar);
    }

  // Check that T and U satisfy the common type symmetry property for the
  // exopression e.
  template <typename T, typename U, typename E>
    void 
    check_common_type_symmetry(context& cxt, E e)
    {
      auto evar = single(e);
      auto tvar = quantify_over<T>(cxt);
      auto uvar = quantify_over<U>(cxt);
      quick_check(cxt, common_type_symmetry<T, U> {cxt}, evar, tvar, uvar);
    }

  // Check that r, having domain T, is reflexive.
  template <typename R>
    void 
    check_reflexive(context& cxt, R r)
    {
      using D = Domain<R>;
      auto rvar = single(r);
      auto tvar = quantify_over<D>(cxt);
      quick_check(cxt, reflexive {cxt}, rvar, tvar);
    }

  // Check that r, having domain T, is irreflexive.
  template <typename R>
    void 
    check_irreflexive(context& cxt, R r)
    {
      auto rvar = single(r);
      auto tvar = quantify_over<Domain<R>>(cxt);
      quick_check(cxt, irreflexive {cxt}, rvar, tvar);
    }

  // Check that r, having domain T, is symmetric.
  template <typename R>
    void 
    check_symmetric(context& cxt, R r)
    {
      auto rvar = single(r);
      auto tvar = quantify_over<Domain<R>>(cxt);
      quick_check(cxt, symmetric {cxt}, rvar, tvar, tvar);
    }

  // Check that r, having domain T, is asymmetric.
  template <typename R>
    void 
    check_asymmetric(context& cxt, R r)
    {
      auto rvar = single(r);
      auto tvar = quantify_over<Domain<R>>(cxt);
      quick_check(cxt, asymmetric {cxt}, rvar, tvar, tvar);
    }

  // Check that r, having domain T, is antisymmetric.
  template <typename R>
    void 
    check_antisymmetric(context& cxt, R r)
    {
      auto rvar = single(r);
      auto tvar = quantify_over<Domain<R>>(cxt);
      quick_check(cxt, antisymmetric {cxt}, rvar, tvar, tvar);
    }

  // Check that r, having domain T, is transitive.
  template <typename R>
    void 
    check_transitive(context& cxt, R r)
    {
      auto rvar = single(r);
      auto tvar = quantify_over<Domain<R>>(cxt);
      quick_check(cxt, transitive {cxt}, rvar, tvar, tvar, tvar);
    }

  // Check that r, having domain T, is trichotomous.
  template <typename R>
    void 
    check_trichotomous(context& cxt, R r)
    {
      auto rvar = single(r);
      auto tvar = quantify_over<Domain<R>>(cxt);
      quick_check(cxt, trichotomous {cxt}, rvar, tvar);
    }

  // Check that r, having domain T, is an equivalence relation.
  template <typename R>
    void 
    check_equivalence(context& cxt, R r)
    {
      auto rvar = single(r);
      auto tvar = quantify_over<Domain<R>>(cxt);
      quick_check(cxt, equivalence {cxt}, rvar, tvar);
    }

  // Check that r, having domain T, is a strict ordering relation.
  template <typename R>
    void 
    check_strict_ordering(context& cxt, R r)
    {
      auto rvar = single(r);
      auto tvar = quantify_over<Domain<R>>(cxt);
      quick_check(cxt, strict_ordering {cxt}, rvar, tvar);
    }

  // Check that r, having domain T, is a strict weak ordering relation.
  template <typename R>
    void 
    check_strict_weak_ordering(context& cxt, R r)
    {
      auto rvar = single(r);
      auto tvar = quantify_over<Domain<R>>(cxt);
      quick_check(cxt, strict_weak_ordering {cxt}, rvar, tvar);
    }

  // Check that r, having domain T, is a strict total ordering relation.
  template <typename R>
    void 
    check_strict_total_ordering(context& cxt, R r)
    {
      auto rvar = single(r);
      auto tvar = quantify_over<Domain<R>>(cxt);
      quick_check(cxt, strict_total_ordering {cxt}, rvar, tvar);
    }


} // namespace testing
