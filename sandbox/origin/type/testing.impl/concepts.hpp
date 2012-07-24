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

  // Cross-type Equality
  //
  // The equality of two values of different types sharing a common type, C, is
  // equivalent to the same operation after converting both arguments to the
  // common type. The requirement applies symmetrically. Both must hold:
  //
  //    t == u <=> C(t) == C(u)
  //    u == t <=> C(u) == C(t)
  struct crosstype_equality : testable
  {
    crosstype_equality(context& cxt) : testable(cxt) { }

    template <typename T, typename U>
      bool 
      operator()(const T& t, const U& u) const
      {
        using C = Common_type<T, U>;
        return (t == u) == (C(t) == C(u))
            && (u == t) == (C(u) == C(t));
      }
  };


  // Cross-type Distintion 
  //
  // The distinction of two values of different types sharing a common type, C,
  // is equivalent to the same operation after converting both arguments to the
  // common type. The requirement applies symmetrically. Both must hold:
  //
  //    t != u <=> C(t) != C(u)
  //    u != t <=> C(u) != C(t)
  //
  struct crosstype_distinction : testable
  {
    crosstype_distinction(context& cxt) : testable(cxt) { }

    template <typename T, typename U>
      bool 
      operator()(const T& t, const U& u) const
      {
        using C = Common_type<T, U>;
        return (t != u) == (C(t) != C(u))
            && (u != t) == (C(u) != C(t));
      }
  };


  // Cross-type Less Than 
  //
  // The ordering of two values of different types sharing a common type, C, is
  // equivalent to the same operation after converting both arguments to the
  // common type. The requirement applies symmetrically. Both must hold:
  //
  //    t < u <=> C(t) < C(u)
  //    u < t <=> C(u) < C(t)
  struct crosstype_less : testable
  {
    crosstype_less(context& cxt) : testable(cxt) { }

    template <typename T, typename U>
      bool operator()(const T& t, const U& u) const
      {
        using C = Common_type<T, U>;
        return (t < u) == (C(t) < C(u))
            && (u < t) == (C(u) < C(t));
      }
  };


  // Cross-type Greater Than 
  //
  // The ordering of two values of different types sharing a common type, C, is
  // equivalent to the same operation after converting both arguments to the
  // common type. The requirement applies symmetrically. Both must hold:
  //
  //    t > u <=> C(t) > C(u)
  //    u > t <=> C(u) > C(t)
  struct crosstype_greater : testable
  {
    crosstype_greater(context& cxt) : testable(cxt) { }

    template <typename T, typename U>
      bool operator()(const T& t, const U& u) const
      {
        using C = Common_type<T, U>;
        return (t > u) == (C(t) > C(u))
            && (u > t) == (C(u) > C(t));
      }
  };


  // Cross-type Less Than or Equal To
  //
  // The ordering of two values of different types sharing a common type, C, is
  // equivalent to the same operation after converting both arguments to the
  // common type. The requirement applies symmetrically. Both must hold:
  //
  //    t <= u <=> C(t) <= C(u)
  //    u <= t <=> C(u) <= C(t)
  struct crosstype_less_equal : testable
  {
    crosstype_less_equal(context& cxt) : testable(cxt) { }

    template <typename T, typename U>
      bool operator()(const T& t, const U& u) const
      {
        using C = Common_type<T, U>;
        return (t <= u) == (C(t) <= C(u))
            && (u <= t) == (C(u) <= C(t));
      }
  };


  // Cross-type Greater Than or Equal To
  //
  // The ordering of two values of different types sharing a common type, C, is
  // equivalent to the same operation after converting both arguments to the
  // common type. The requirement applies symmetrically. Both must hold:
  //
  //    t >= u <=> C(t) >= C(u)
  //    u >= t <=> C(u) >= C(t)
  struct crosstype_greater_equal : testable
  {
    crosstype_greater_equal(context& cxt) : testable(cxt) { }

    template <typename T, typename U>
      bool operator()(const T& t, const U& u) const
      {
        using C = Common_type<T, U>;
        return (t >= u) == (C(t) >= C(u))
            && (u >= t) == (C(u) >= C(t));
      }
  };


  // Complement of Equality
  //
  // Distinction is the complement of equality. That is:
  //
  //    a != b <=> !(a == b).
  //
  struct complement_of_equality : testable
  {
    complement_of_equality(context& cxt) : testable(cxt) { }

    template <typename T>
      bool
      operator()(const T& a, const T& b) const
      {
        return (a != b) == !(a == b);
      }
  };


  // Complement of Less
  //
  // The complement of less is greater or equal. That is:
  //
  //    a > b <=> !(a < b).
  //
  struct complement_of_less : testable
  {
    complement_of_less(context& cxt) : testable(cxt) { }

    template <typename T>
      bool
      operator()(const T& a, const T& b) const
      {
        return (a >= b) == !(a < b);
      }
  };



  // Converse of Less
  //
  // The complement of less is greater. That is:
  //
  //    a > b <=> b < a
  //
  struct converse_of_less : testable
  {
    converse_of_less(context& cxt) : testable(cxt) { }

    template <typename T>
      bool
      operator()(const T& a, const T& b) const
      {
        return (a > b) == (b < a);
      }
  };


  // Complement of Converse of Less
  //
  // The complement of the converse of less is less or equal. That is:
  //
  //    a <= b <=> !(b < a)
  //
  struct complement_of_converse_of_less : testable
  {
    complement_of_converse_of_less(context& cxt) : testable(cxt) { }

    template <typename T>
      bool
      operator()(const T& a, const T& b) const
      {
        return (a <= b) == !(b < a);
      }
  };



  struct move_construction : testable
  {
    move_construction(context& cxt) : testable(cxt) { }

    template <typename T>
      bool
      operator()(const T& a, const T& b) const
      {
        return a == b ? check_equal(T {std::move(a)}, b) : true;
      }
  };

  struct move_assignment : testable
  {
    move_assignment(context& cxt) : testable(cxt) { }

    template <typename T>
      bool
      operator()(T a, const T& b, const T& c) const
      {
        // Sequence the assignment and comparison so that we don't induce
        // requirements on the result of assignment.
        return b == c ? (a = std::move(b), check_equal(a, c)) : true;
      }
  };

  struct copy_construction : testable
  {
    copy_construction(context& cxt) : testable(cxt) { }

    template <typename T>
      bool
      operator()(const T& x) const
      {
        return check_equal(T {x}, x);
      }
  };


  struct copy_assignment : testable
  {
    copy_assignment(context& cxt) : testable(cxt) { }

    template <typename T>
      bool
      operator()(T a, const T& b) const
      {
        // Sequence the assignment and comparison so that we don't induce
        // requirements on the result of assignment.
        return (a = b, check_equal(a, b));
      }
  };



  //////////////////////////////////////////////////////////////////////////////
  // Equality Comparable
  //
  // The equality comparable test can be used to evaluate the semantics of the
  // equality comparison relation. That is, the == operator must satisfy
  // the requirements of an equivalence relation, and the != operator must
  // be the negation of the equality operator.
  //
  // In actuality, the == operator must compare for value equality, but we have
  // no way to actually test that.
  template <typename T, typename U = T>
    struct equality_comparable : testable
    {
      static_assert(Equality_comparable<T, U>(), "");

      using C = Common_type<T, U>;

      equality_comparable(context& cxt)
        : testable(cxt)
        , eq_t(cxt), eq_u(cxt), eq_c(cxt), cross_eq(cxt), cross_df(cxt)
      { }

      template <typename Tv, typename Uv>
        void 
        operator()(Tv&& tvar, Uv&& uvar)
        {
          auto cvar = quantify_over<C>(this->cxt);
          operator()(tvar, uvar, cvar);
        }

      template <typename Tv, typename Uv, typename Cv>
        void 
        operator()(Tv&& tvar, Uv&& uvar, Cv&& cvar) const
        {
          quick_check(this->cxt, eq_t, tvar);
          quick_check(this->cxt, eq_u, uvar);
          quick_check(this->cxt, eq_c, cvar);

          // Create "quantified" variables for the given expressions.
          quick_check(this->cxt, cross_eq, tvar, uvar);
          quick_check(this->cxt, cross_df, tvar, uvar);
        }

      equality_comparable<T> eq_t;
      equality_comparable<U> eq_u;
      equality_comparable<C> eq_c;

      crosstype_equality     cross_eq;
      crosstype_distinction  cross_df;
  };


  // Specialization when testing for a single type.
  template <typename T>
    struct equality_comparable<T, T> : testable
    {
      static_assert(Equality_comparable<T>(), "");
      
      equality_comparable(context& cxt)
        : testable(cxt), eq(cxt), df(cxt)
      { }

      template <typename Tv>
        void 
        operator()(Tv&& tvar) const
        {
          auto eqvar = single(std::equal_to<T> {});
          quick_check(this->cxt, eq, eqvar, tvar);
          quick_check(this->cxt, df, tvar, tvar);
        }

      equivalence            eq;
      complement_of_equality df;
    };


  
  //////////////////////////////////////////////////////////////////////////////
  // Weakly Ordered
  //
  // The weakly ordered comparable test can be used to test the semantics of
  // standard relational operators <, >, <=, and >=.
  //
  //
  template <typename T, typename U = T>
    struct weakly_ordered : testable
    {
      static_assert(Weakly_ordered<T, U>(), "");
      
      using C = Common_type<T, U>;

      weakly_ordered(context& cxt)
        : testable(cxt)
        , ord_t(cxt), ord_u(cxt), ord_c(cxt)
        , cross_lt(cxt), cross_gt(cxt), cross_le(cxt), cross_ge(cxt)
      { }

      template <typename Tv, typename Uv>
        void 
        operator()(Tv&& tvar, Uv&& uvar) const
        {
          auto cvar = quantify_over<C>(this->cxt);
          operator()(tvar, uvar, cvar);
        }
      
      template <typename Tv, typename Uv, typename Cv>
        void 
        operator()(Tv&& tvar, Uv&& uvar, Cv&& cvar) const
        {
          quick_check(this->cxt, ord_t, tvar);
          quick_check(this->cxt, ord_u, uvar);
          quick_check(this->cxt, ord_c, cvar);

          quick_check(this->cxt, cross_lt, tvar, uvar);
          quick_check(this->cxt, cross_gt, tvar, uvar);
          quick_check(this->cxt, cross_le, tvar, uvar);
          quick_check(this->cxt, cross_ge, tvar, uvar);
        }

      weakly_ordered<T> ord_t;
      weakly_ordered<U> ord_u;
      weakly_ordered<C> ord_c;

      crosstype_less          cross_lt;
      crosstype_greater       cross_gt;
      crosstype_less_equal    cross_le;
      crosstype_greater_equal cross_ge;
    };


  // Specialization for the unary type.
  template <typename T>
    struct weakly_ordered<T, T> : testable
    {
      static_assert(Weakly_ordered<T>(), "");

      weakly_ordered(context& cxt)
        : testable(cxt), lt(cxt), gt(cxt), le(cxt), ge(cxt)
      { }

      template <typename Tv>
        void operator()(Tv&& tvar) const
        {
          auto ltvar = single(std::less<T> {});
          quick_check(this->cxt, lt, ltvar, tvar);
          quick_check(this->cxt, gt, tvar, tvar);
          quick_check(this->cxt, le, tvar, tvar);
          quick_check(this->cxt, ge, tvar, tvar);
        }

      strict_weak_ordering           lt;
      converse_of_less               gt;
      complement_of_converse_of_less le;
      complement_of_less             ge;
    };



  //////////////////////////////////////////////////////////////////////////////
  // Totally Ordered
  //
  // The totally ordered comparable test can be used to test the semantics of
  // standard relational operators <, >, <=, and >=, and their relationship
  // to ==.
  //
  template <typename T, typename U = T>
    struct totally_ordered : testable
    {
      static_assert(Totally_ordered<T, U>(), "");

      using C = Common_type<T, U>;

      totally_ordered(context& cxt)
        : testable(cxt)
        , ord_t(cxt), ord_u(cxt), ord_c(cxt)
        , cross_lt(cxt), cross_gt(cxt), cross_le(cxt), cross_ge(cxt)
      { };

      template <typename Tv, typename Uv>
        void
        operator()(Tv&& tvar, Uv&& uvar) const
        {
          auto cvar = quantify_over<C>(cxt);
          operator()(tvar, uvar, cvar);
        }

      template <typename Tv, typename Uv, typename Cv>
        void
        operator()(Tv&& tvar, Uv&& uvar, Cv&& cvar) const
        {
          quick_check(this->cxt, ord_t, tvar);
          quick_check(this->cxt, ord_u, uvar);
          quick_check(this->cxt, ord_c, cvar);

          quick_check(this->cxt, cross_lt, tvar, uvar);
          quick_check(this->cxt, cross_gt, tvar, uvar);
          quick_check(this->cxt, cross_le, tvar, uvar);
          quick_check(this->cxt, cross_ge, tvar, uvar);
        }

      totally_ordered<T> ord_t;
      totally_ordered<U> ord_u;
      totally_ordered<C> ord_c;

      crosstype_less          cross_lt;
      crosstype_greater       cross_gt;
      crosstype_less_equal    cross_le;
      crosstype_greater_equal cross_ge;      
    };
    
  template <typename T>
    struct totally_ordered<T, T> : testable
    {
      static_assert(Totally_ordered<T>(), "");

      totally_ordered(context)
        : testable(cxt), lt(cxt), gt(cxt), le(cxt), ge(cxt)
      { }

      template <typename Tv>
        void
        operator()(Tv&& tvar) const
        {
          auto ltvar = single(std::less<T> {});
          quick_check(this->cxt, lt, ltvar, tvar);
          quick_check(this->cxt, gt, tvar, tvar);
          quick_check(this->cxt, le, tvar, tvar);
          quick_check(this->cxt, ge, tvar, tvar);
        }

      strict_total_ordering          lt;
      converse_of_less               gt;
      complement_of_converse_of_less le;
      complement_of_less             ge;
    };



  //////////////////////////////////////////////////////////////////////////////
  // Move Constructuble
  //
  template <typename T>
    struct move_constructible : testable
    {
      move_constructible(context& cxt) 
        : testable(cxt), ctor(cxt)
      { }

      template <typename Tv>
        void
        operator()(Tv&& tvar) const
        {
          quick_check(this->cxt, ctor, tvar, tvar);
        }

      move_construction ctor;
    };


  //////////////////////////////////////////////////////////////////////////////
  // Movable
  //
  template <typename T>
    struct movable : testable
    {
      movable(context& cxt) 
        : testable(cxt), ctor(cxt), assign(cxt)
      { }

      template <typename Tv>
        void
        operator()(Tv&& tvar) const
        {
          quick_check(this->cxt, ctor, tvar, tvar);
          quick_check(this->cxt, assign, tvar, tvar, tvar);
        }

      move_construction ctor;
      move_assignment   assign;
    };


  //////////////////////////////////////////////////////////////////////////////
  // Copy Constructuble
  //
  template <typename T>
    struct copy_constructible : testable
    {
      copy_constructible(context& cxt) 
        : testable(cxt), move(cxt), ctor(cxt)
      { }

      template <typename Tv>
        void
        operator()(Tv&& tvar) const
        {
          quick_check(this->cxt, move, tvar);
          quick_check(this->cxt, ctor, tvar);
        }

      move_constructible<T> move;
      copy_construction ctor;
    };


  //////////////////////////////////////////////////////////////////////////////
  // Copyable
  //
  template <typename T>
    struct copyable : testable
    {
      copyable(context& cxt) 
        : testable(cxt), move(cxt), ctor(cxt), assign(cxt)
      { }

      template <typename Tv>
        void
        operator()(Tv&& tvar) const
        {
          quick_check(this->cxt, move, tvar);
          quick_check(this->cxt, ctor, tvar);
          quick_check(this->cxt, assign, tvar, tvar);
        }

      movable<T>        move;
      copy_construction ctor;
      copy_assignment   assign;
    };


  //////////////////////////////////////////////////////////////////////////////
  // Default Constructible
  //
  // The default value of a default constructible type is unique.
  //
  // Note that an object is only set tothe default value when an object is
  // default initialized, not simply default constructed. For example:
  //
  //    int x;    // x does not have the default value
  //    int x {}; // x is default-initialized to 0.
  //
  template <typename T>
    struct default_constructible : testable
    {
      static_assert(Default_constructible<T>(), "");

      default_constructible(context& cxt) 
        : testable(cxt)
      { }

      bool
      operator()() const
      {
        return check_equal(T{}, T{});
      }
    };


  //////////////////////////////////////////////////////////////////////////////
  // Semiregular
  //
  template <typename T>
    struct semiregular : testable
    {
      static_assert(Semiregular<T>(), "");

      semiregular(context& cxt)
        : testable(cxt), def(cxt), copy(cxt)
      { }

      template <typename Tv>
        void
        operator()(Tv&& tvar) const
        {
          quick_check(this->cxt, def);
          quick_check(this->cxt, copy, tvar);
        }

      default_constructible<T> def;
      copyable<T>              copy;
    };


  //////////////////////////////////////////////////////////////////////////////
  // Regular
  //
  template <typename T>
    struct regular : testable
    {
      static_assert(Regular<T>(), "");

      regular(context& cxt)
        : testable(cxt), semi(cxt), eq(cxt)
      { }

      template <typename Tv>
        void
        operator()(Tv&& tvar) const
        {
          quick_check(this->cxt, semi, tvar);
          quick_check(this->cxt, eq, tvar);
        }

      semiregular<T>         semi;
      equality_comparable<T> eq;
    };



  // TODO: Implement testing for function objects. 

  // Check that T is equality comparable.
  template <typename T>
    inline void 
    check_equality_comparable(context& cxt)
    {
      auto tvar = quantify_over<T>(cxt);
      quick_check(cxt, equality_comparable<T> {cxt}, tvar);
    }

  // Check that T and U satisfy the requirements of cross-type equality
  // comparison.
  template <typename T, typename U>
    inline void 
    check_equality_comparable(context& cxt)
    {
      auto tvar = quantify_over<T>(cxt);
      auto uvar = quantify_over<U>(cxt);
      quick_check(cxt, equality_comparable<T, U> {cxt}, tvar, uvar);
    }

  // Check that T is weakly ordered.
  template <typename T>
    inline void 
    check_weakly_ordered(context& cxt)
    {
      auto tvar = quantify_over<T>(cxt);
      quick_check(cxt, weakly_ordered<T> {cxt}, tvar);
    }

  // Check that T and U satisfy the requirements of cross-type weak ordering.
  template <typename T, typename U>
    inline void 
    check_weakly_ordered(context& cxt)
    {
      auto tvar = quantify_over<T>(cxt);
      auto uvar = quantify_over<U>(cxt);
      quick_check(cxt, weakly_ordered<T, U> {cxt}, tvar, uvar);
    }

  // Check that T is totally ordered.
  template <typename T>
    inline void 
    check_totally_ordered(context& cxt)
    {
      auto tvar = quantify_over<T>(cxt);
      quick_check(cxt, weakly_ordered<T> {cxt}, tvar);
    }

  // Check that T and U satisfy the requirements of cross-type total ordering.
  template <typename T, typename U>
    inline void 
    check_totally_ordered(context& cxt)
    {
      auto tvar = quantify_over<T>(cxt);
      auto uvar = quantify_over<U>(cxt);
      quick_check(cxt, weakly_ordered<T, U> {cxt}, tvar, uvar);
    }

  // Check that T is move constructible.
  template <typename T>
    inline void
    check_move_constructible(context& cxt)
    {
      auto tvar = quantify_over<T>(cxt);
      quick_check(cxt, move_constructible<T> {cxt}, tvar);
    }

  // Check that T is movable.
  template <typename T>
    inline void
    check_movable(context& cxt)
    {
      auto tvar = quantify_over<T>(cxt);
      quick_check(cxt, movable<T> {cxt}, tvar);
    }

  // Check that T is copy constructible.
  template <typename T>
    inline void
    check_copy_constructible(context& cxt)
    {
      auto tvar = quantify_over<T>(cxt);
      quick_check(cxt, copy_constructible<T> {cxt}, tvar);
    }

  // Check that T is copyable.
  template <typename T>
    inline void
    check_copyable(context& cxt)
    {
      auto tvar = quantify_over<T>(cxt);
      quick_check(cxt, copyable<T> {cxt}, tvar);
    }

  // Check that T is default constructible.
  template <typename T>
    inline void
    check_default_constructible(context& cxt)
    {
      quick_check(cxt, default_constructible<T> {cxt});
    }

  // Check that T is semiregular.
  template <typename T>
    inline void 
    check_semiregular(context& cxt)
    {
      auto tvar = quantify_over<T>(cxt);
      quick_check(cxt, semiregular<T> {cxt}, tvar);
    }

  // Check that T is regular.
  template <typename T>
    inline void 
    check_regular(context& cxt)
    {
      auto tvar = quantify_over<T>(cxt);
      quick_check(cxt, regular<T> {cxt}, tvar);
    }

} // namespace origin

