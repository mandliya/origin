// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_CONCEPTS_OPERATOR_HPP
#define ORIGIN_CONCEPTS_OPERATOR_HPP

#include <origin/concepts/impl.hpp>

namespace origin
{
  /**
   * @class Equal<T, U>
   * @class Equal<T>
   *
   * Determine if two types T and U can be compared for equality using
   * operator==. This concept provides a default implementation of operator!=
   * for all such types.
   *
   * Note that when T and U are the same, the concept implies additional
   * semantic requirements.
   */
  //@{
  // Thje general comparison.
  template<typename T, typename U>
  struct Equal
  {
    Equal()
    { auto p = constraints; }

    static void constraints(T x, U y)
    {
      Boolean<decltype(x == y)>{};
      Boolean<decltype(y == x)>{};
    }

    typedef std::tuple<
      has_equal<T, U>, has_equal<U, T>
    > requirements;
    typedef concept_check<requirements> type;
    static constexpr bool value = type::value;
  };

  // The single type comparison
  template <typename T>
  struct Equal<T, T>
  {
    Equal()
    { auto p = constraints; }

    static void constraints(T x, T y)
    { Boolean<decltype(x == y)>{}; }

    typedef std::tuple<
      has_equal<T, T>
    > requirements;
    typedef concept_check<requirements> type;
    static constexpr bool value = type::value;
  };

  /** @name Defaults */
  //@{
  /**
   * @fn operator!=(x, y)
   * Provide a default implementation of operator!= for all x and y with type
   * T and U respectively such that Equal<T, U> is a valid model.
   */
  //@{
  template<typename T, typename U>
  auto operator!=(T const& x, U const& y)
    -> typename std::enable_if<Equal<T, U>::value, decltype(!(x == y))>::type
  { return !(x == y); }

  template<typename T>
  auto operator!=(T const& x, T const& y)
    -> typename std::enable_if<Equal<T>::value, decltype(!(x == y))>::type
  { return !(x == y); }
  //@}
  //@}

  /** @name Axioms */
  //@{
  // FIXME: This cannot be true for volatile T, but could be for const
  // volatile? Not sure...
  //
  // FIXME: Implement type constraints on these axioms.
  /**
   * Two objects that are in actuality the same object, are equivalent. Said
   * otherwise, an object is always the same as itself.
   */
  template<typename T>
  auto Equal_Identity(T const& x, T const& y)
    -> decltype(implies(std::addressof(x) == std::addressof(y), x == y))
  { return implies(std::addressof(x) == std::addressof(y), x == y); }

  /**
   * The equality comparison operator is reflexive.
   */
  template<typename T>
  auto Equal_Reflexive(T x)
    -> decltype(x == x)
  { return x == x; }

  /**
   * The equality comparison operatopr is symmetric.
   */
  template<typename T>
  auto Equal_Symmetric(T x, T y)
    -> decltype(implies(x == y, y == x))
  { return implies(x == y, y == x); }

  /**
   * The equality comparison operator is transitive.
   */
  template<typename T>
  auto Equal_Transitive(T x, T y, T z)
    -> decltype(implies(x == y && y == z, x == z))
  { return implies(x == y && y == z, x == z); }

  /**
   * Connect the equality operator to the inequality operator. If x == y, then
   * it is not the cast that x != y.
   */
  template<typename T>
  auto Equal_Not_Equal(T x, T y)
    -> decltype(implies(x == y, !!(x == y)))
  { return implies(x == y, !(x != y)); }
  //@}
  //@}

  /**
   * @ingroup concepts
   * @class Ordered<T, U>
   * @class Ordered<T>
   *
   * Determine if two types T and U can be ordered using operator<. This
   * concept provides a default implementation of the operator>, operator<=,
   * and operator>= for all valid models.
   *
   * Note that when T and U are the same, the concept implies additional
   * semantic requirements.
   */
  //@{
  // The general requiremetn
  template<typename T, typename U>
  struct Ordered
  {
    Ordered()
    { auto p = constraints; }

    static void constraints(T x, T y)
    {
      Boolean<decltype(x < y)>{};
      Boolean<decltype(y < x)>{};
    }

    typedef std::tuple<
      has_less<T, U>,
      Boolean<typename deduce_less<T, U>::type>,
      has_less<U, T>,
      Boolean<typename deduce_less<U, T>::type>
    > requirements;
    typedef concept_check<requirements> type;
    static constexpr bool value = type::value;
  };

  // The unary variant
  template<typename T>
  struct Ordered<T, T>
  {
    Ordered()
    { auto p = constraints; }

    static void constraints(T x, T y)
    { Boolean<decltype(x < y)>{}; }

    typedef std::tuple<
      has_less<T, T>,
      Boolean<typename deduce_less<T, T>::type>
    > requirements;
    typedef concept_check<requirements> type;
    static constexpr bool value = type::value;
  };

  /** @name Defaults */
  //@{
  /**
   * @fn operator>(x, y)
   * Provide a default implementation of operator> for all x and y of type T
   * and U such that Ordered<T, U> is a valid model.
   */
  template<typename T, typename U>
  auto operator>(T const& x, U const& y)
    -> typename std::enable_if<Ordered<T, U>::value, decltype(y < x)>::type
  { return y < x; }

  template<typename T>
  auto operator>(T const& x, T const& y)
    -> typename std::enable_if<Ordered<T>::value, decltype(y < x)>::type
  { return y < x; }

  /**
   * @fn operator<=(x, y)
   * Provide a default implementation of operator<= for all x and y of type T
   * and U such that Ordered<T, U> is a valid model.
   */
  template<typename T, typename U>
  auto operator<=(T const& x, U const& y)
    -> typename std::enable_if<Ordered<T, U>::value, decltype(!(x < y))>::type
  { return !(x < y); }

  template<typename T>
  auto operator<=(T const& x, T const& y)
    -> typename std::enable_if<Ordered<T>::value, decltype(!(x < y))>::type
  { return !(x < y); }

  /**
   * @fn operator>=(x, y)
   * Provide a default implementation of operator>= for all x and y of type T
   * and U such that Ordered<T, U> is a valid model.
   */
  template<typename T, typename U>
  auto operator<=(T const& x, U const& y)
    -> typename std::enable_if<Ordered<T, U>::value, decltype(!(y < x))>::type
  { return !(x < y); }

  template<typename T>
  auto operator<=(T const& x, T const& y)
    -> typename std::enable_if<Ordered<T>::value, decltype(!(y < x))>::type
  { return !(y < x); }
  //@}

  /** @name Axioms */
  //@{
  template<typename T>
  auto Ordered_Irreflexive(T x)
    -> decltype(!(x < x))
  { return !(x < x); }

  // FIXME: Requires Boolean<T>
  template<typename T>
  auto Ordered_Antisymmetric(T x, T y)
    -> decltype(implies(x < y, !(y < x)))
  { return implies(x < y, !(y < x)); }

  //  FIXME: Requires Boolean<T>
  template<typename T>
  auto Ordered_Transitive(T x, T y, T z)
    -> decltype(implies(x < y && y < z, x == z))
  { return implies(x < y && y < z, x == z); }
  //@}
  //@}


} // namespace origin

#endif
