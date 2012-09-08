// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_TYPE_TRAITS_HPP
#  error This file cannot be included directly. Include type/traits.hpp.
#endif

namespace traits_impl
{
  struct subst_failure { };


  // Is same
  template <typename T, typename U> struct is_same       : False { };
  template <typename T>             struct is_same<T, T> : True { };

  // Are Same
  // Returns true if all of the types are the same, or if Args... is an
  // empty sequence of types. Evaluation is of type equality is performed
  // left-to-right, and the operation will not instantiate arguments after
  // a non-equal pair is found.
  template <typename... Args> struct are_same;

  // True for an empty sequence
  template <> struct are_same<> : True { };

  // For a single type, this is trivially true.
  template <typename T> struct are_same<T> : True { };

  // Recursively apply are_same (is_same) to T, U and Args...
  template <typename T, typename U, typename... Args>
    struct are_same<T, U, Args...>
      : Meta_boolean<is_same<T, U>{} && are_same<U, Args...>{}>
    { };

  // Adapt are_same to the Expand wrapper. 
  template <template <typename... Args> class Template, typename... Args>
    struct are_same<Expand<Template<Args...>>>
      : are_same<Args...>
    { };


  // Common type
  template <typename... Args>
    struct common_type;

  // The common type of a single type is obviously that type.
  template <typename T>
    struct common_type<T>
    {
      using type = T;
    };

  // The common type relation. This specialization is the primary mechanism
  // by which common type is defined, and by which it is extended. The default
  // definition of common type is based on the result type of the ?: operator.
  template <typename T, typename U>
    struct common_type<T, U>
    {
    private:
      template <typename X, typename Y>
        static auto check(X&& a, Y&& b) -> decltype(true ? a : b);

      static subst_failure check(...);

      using C = decltype(check(std::declval<T>(), std::declval<U>()));
    public:
      using type = typename std::remove_const<
        typename std::remove_reference<C>::type
      >::type;
    };

  // When the type arguments are the same, we can bypass the deduction on
  // the conditional operator. This should make compilation slightly faster
  // in those cases.
  template <typename T>
    struct common_type<T, T>
    {
      using type = typename std::remove_const<
        typename std::remove_reference<T>::type
      >::type;
    };

  // The recursive definition of common type simply applies the common type
  // to each pair of types in turn. The computation is similar to that of
  // max_elements where the "max" type is the common type of each consecutive
  // pair of elements.
  template <typename T, typename U, typename... Args>
    struct common_type<T, U, Args...>
      : common_type<typename common_type<T, U>::type, Args...>
    { };

  // Adaptation for expandable types
  template <template <typename...> class Template, typename... Args>
    struct common_type<Expand<Template<Args...>>>
      : origin::common_type<Args...>
    { };


  // Are Common
  template <typename... Ts>
    struct are_common
      : Meta_boolean<
          is_same<typename common_type<Ts...>::type, subst_failure>{}
        >
      { };



  ////////////////////////////////////////////////////////////////////////////
  // Metaprogramming support
  //
  // The type functions in this module provide limited support for basic
  // metaprogramming tasks. None of the metafunctions in this module require
  // SFINAE support. These are purely type functions.
  ////////////////////////////////////////////////////////////////////////////


  // Return the first type in the given sequence of arguments. This is
  // undefined when Args... is empty.
  template <typename... Args> struct front_type;

  template <typename T, typename... Args>
    struct front_type<T, Args...>
    { 
      using type = T; 
    };

  // Adaptation for expandable types
  template <template <typename...> class Template, typename... Args>
    struct front_type<Expand<Template<Args...>>>
      : front_type<Args...>
    { };



  // Returns the last type in a sequence of type arguments. This is
  // undefined when Args... is empty.
  template <typename... Args> struct back_type;

  template <typename T>
    struct back_type<T>
    { 
      using type = T;
    };

  template <typename T, typename... Args>
    struct back_type<T, Args...> : back_type<Args...>
    { };

  // Adapatation for expandable types.
  template <template <typename...> class Template, typename... Args>
    struct back_type<Expand<Template<Args...>>>
      : back_type<Args...>
    { };




  // The identity metafunction for types.
  //
  // TODO: Move this into the origin namespace?
  template <typename T>
    struct identity
    {
      using type = T;
    };

} // namespace type_impl
