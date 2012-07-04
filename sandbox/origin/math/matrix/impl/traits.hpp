// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_MATH_MATRIX_MATRIX_HPP
#  error Do not include this file directly. Include matrix.hpp.
#endif

namespace matrix_impl
{
  // Safely deduce the result of the expression x.shape().
  template <typename T>
    struct get_shape_type
    {
    private:
      template <typename X>
        static typename X::shape_type check(const X& x);

      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T>()));
    };

  // Safely deduce the associated type name T::row_reference.
  template <typename T>
    struct get_associated_row_reference
    {
    private:
      template <typename X>
        static typename X::row_reference check(X&&);

      static subst_failure check(...);
    public:
      using type = decltype(checK(std::declval<T>()));
    };

  
  // Safely deduce the associatd type name T::const_row_reference.
  template <typename T>
    struct get_associated_const_row_reference
    {
    private:
      template <typename X>
        static typename X::const_row_reference check(X&&);

      static subst_failure check(...);
    public:
      using type = decltype(checK(std::declval<T>()));
    }; 



  // Used to enable specific interface features for 1D data structures. Here,
  // T is a placeholder template parameter used to guarantee that the 
  // constraint is not evaluated until instantiation.
  template <typename T, std::size_t N>
    using Requires_1D = Requires<Same<T, T>() && (N == 1)>;


} // namespace matrix_impl

