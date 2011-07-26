// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_ITERATOR_CONCEPTS_HPP
#define ORIGIN_ITERATOR_CONCEPTS_HPP

#include <origin/utility/meta.hpp>

// Define a number of useful intrinsics

// Used to state concept requirements as part of a function signature, although
// it could actually be used in multiple places. 
//
// This expands to an empty sequence of tokens.
#define requires(...)

// FIXME: Consider a static_requires that actually asserts requirements.

// Used to derive or evaluate the performance complexity of an expression.
// This is used in conjunction with the O() intrinsic to state complexity
// requirements. For example: performance(advance(i, n)) == O(1). Note tht
// requirement can't actually be evaluated.
#define performance(...) false

// Used to state complexity requirements. See the performance intrinsice.
#define O(...) false

namespace origin
{
  // These are placeholders for deduced types in concept specfifications.
  struct decltype_type { };
  struct auto_type { };
  
  template<typename... Args> struct c_Same { };
  
#ifdef ORIGIN_HAS_TEMPLATE_ALIASES
  // The Same_type type function gives the type shared by its arguments. All
  // type arguments must be the same.
  template<Same... Args>
    using Same_type = Same<Args...>::type;
#endif
  
  // A sequence of types is Common if they share a common type. The common
  // type of a single type T is T. The common type of two types T and U is
  // defined by the result of conditional operator with using the expression
  // true ? declval<T>() : declval<U>(). The common type of a sequence of more
  // than two types is defined recursively.
  template<typename... Args> struct c_Common { };
  
  // FIXME: Disable signed/unsigned integral promotions using Common. They
  // aren't really. Ths might also mean that we need a new name for common
  // type (general type?)

#ifdef ORIGIN_HAS_TEMPLATE_ALIASES
  // The Common_type type function gives the common type of its arguments. The
  // common type is explicitly required to exist.
  template<Common... Args>
    using Common_type = Common<Args...>::type;
#endif

  // FIXME: These are "concepts light" for the time being.
  template<typename T> struct c_Regular { };
  template<typename T> struct c_Ordered : c_Regular<T> { };
  
  template<typename T> struct c_Integer;
  
  template<typename T, typename... Args> struct c_Procedure { };
  template<typename T, typename... Args> struct c_Function : c_Procedure<T, Args...> { };
  template<typename T, typename... Args> struct c_Operation : c_Function<T, Args...> { };
  template<typename T, typename... Args> struct c_Predicate : c_Function<T, Args...> { };
  template<typename T, typename... Args> struct c_Relation : c_Predicate<T, Args...> { };
  
  
}

#endif
