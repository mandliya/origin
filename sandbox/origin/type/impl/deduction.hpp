// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_TYPE_CONCEPTS_HPP
#  error This file cannot be included directly. Include type/concepts.hpp.
#endif

namespace type_impl
{
  //////////////////////////////////////////////////////////////////////////////
  // Type deduction systems
  //
  // A deduction system is a set of traits and overloaded functions that
  // can be adapted to deduce an associated type. The implementations are
  // generally comprised of two different parts:
  //
  //     1. A function (deduction hook) that gives concept-specific deduction 
  //        rules. These functions can be overoaded and constrained for
  //        different sets of concepts.
  //     2. A type trait that looks for a nested name, delegating to the
  //        deduction hook if no such name is found.
  //
  // The general deduction mechanism first searches for a nested name, and
  // then delegates to the concept-specific rules.
  //
  // Note that the deduction hook depends on ADL to find overloads for different
  // conceptual domains. In particular, the default_t argument type requires 
  // that the origin namespace be searched for viable overloads.
  //////////////////////////////////////////////////////////////////////////////


  // Value type deduction

  // Type deduction hook.
  subst_failure deduce_value_type(...);

  // If T can be dereferenced, then the value type is the result of that
  // operation minus reference and const-qualifiers.
  template <typename T>
    auto deduce_value_type(default_t, const T&) 
      -> Requires<Has_dereference<T>(), Make_unqualified<Dereference_result<T>>>;


  // Deduce the value type associated with T. We first look for an  associated
  // member type, then default to the deduction overloads.
  template <typename T, bool = Has_associated_value_type<T>()>
    struct get_deduced_value_type;

  template <typename T>
    struct get_deduced_value_type<T, true>
    {
      using type = typename T::value_type;
    };

  template <typename T>
    struct get_deduced_value_type<T, false>
    {
      using type = decltype(deduce_value_type(default_t {}, std::declval<T>()));
    };


  // Size type deduction
  // 
  // FIXME: The rule for Size_type should be:
  //    1. If the member is associated, use that.
  //    2. If Difference_type is defined, use the unsigned version of that.
  //    3. Subst failure everywhere else.

  // Deduction hook.
  subst_failure deduce_size_type(...);


  // Safely deduce the associate size type of T. 
  template <typename T, bool = Has_associated_size_type<T>()>
    struct get_deduced_size_type;

  template <typename T>
    struct get_deduced_size_type<T, true>
    {
      using type = typename T::size_type;
    };

  template <typename T>
    struct get_deduced_size_type<T, false> 
    { 
      using type = decltype(deduce_size_type(default_t {}, std::declval<T>()));
    };



  // Difference type deduction

  // Type deduction hook. 
  subst_failure deduce_difference_type(...);

  // Pointers have difference type ptrdiff_t.
  template <typename T>
    std::ptrdiff_t deduce_difference_type(default_t, T*);


  // The difference type of integral types is the signed representation of
  // that type.
  //
  // FIXME: I think that this should be the signed version of the next larger
  // integer type (up to long long). However, since size_t and ptrdiff_t are
  // related in this same way, I'm not terribly concerned right now.
  template <typename T>
    auto deduce_difference_type(default_t, T) 
      -> Requires<Integer<T>(), Make_signed<T>>;

  // The difference type for floating point types is that type. This is an
  // identity function for floating point types.
  template <typename T>
    auto deduce_difference_type(default_t, T)
      -> Requires<Floating_point<T>(), T>;

  // We can provide a reasonable guess for all other incrementable 
  // user-defined types as ptrdiff_t. This is basically a default guess for
  // all iterator-like types.
  template <typename T>
    auto deduce_difference_type(default_t, T) 
      -> Requires<Class<T>() && Has_pre_increment<T>(), std::ptrdiff_t>;


  // Get the distance type associated with T.
  template <typename T, bool = Has_associated_difference_type<T>()>
    struct get_deduced_difference_type;

  template <typename T>
    struct get_deduced_difference_type<T, true>
    {
      using type = typename T::difference_type;
    };

  // Intercept array types so they don't decay when we try to deduce them.
  // It probably isn't entirely wrong to let the array decay to a pointer, but
  // let's be explicit about it.
  template <typename T, std::size_t N>
    struct get_deduced_difference_type<T[N], false>
    {
      using type = std::ptrdiff_t;
    };

  template <typename T>
    struct get_deduced_difference_type<T, false>
    {
      using type = 
        decltype(deduce_difference_type(default_t {}, std::declval<T>()));
    };

} // namespace type_impl
