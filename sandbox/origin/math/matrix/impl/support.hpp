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
  ////////////////////////////////////////////////////////////////////////////
  // Operations
  //
  // The following function objects extend the usual set found in the 
  // <functional> library and provide abstractions over assignment and 
  // arithmetic compound assignment operators.
  //
  // TODO: These should probably be moved into the functional module.
  ////////////////////////////////////////////////////////////////////////////

  template< typename T>
    struct assign
    {
      T& operator()(T& a, T const& b) const { return a = b; }
    };

  template <typename T>
    struct plus_assign
    {
      T& operator()(T& a, T const& b) const { return a += b; }
    };

  template <typename T>
    struct minus_assign
    {
      T& operator()(T& a, T const& b) const { return a -= b; }
    };

  template <typename T>
    struct multiplies_assign 
    {
      T& operator()(T& a, T const& b) const { return a *= b; }
    };

  template <typename T>
    struct divides_assign
    {
      T& operator()(T& a, T const& b) const { return a /= b; }
    };

  template <typename T>
    struct modulus_assign
    {
      T& operator()(T& a, T const& b) const { return a %= b; }
    };



  ////////////////////////////////////////////////////////////////////////////
  // Algorithms
  //
  // Support algorithms for implementing some matrix operations.
  //
  // TODO: These should be moved into the algorithms module.
  ////////////////////////////////////////////////////////////////////////////


  // Assign value
  //
  // The assign value algorithm calls op(*i, value) for each iterator i in the
  // range [first, last). Here, op typically performs an assignment.
  template <typename I, typename T, typename Op>
    void assign_value(I first, I last, const T& value, Op op)
    {
      while (first != last) {
        op(*first, value);
        ++first;
      }
    }


  // Assign elements
  //
  // The assign elements function calls op(*i, *j) for each corresponding
  // pair of iterators i and j in the ranges [first1, last1) and [first2, last).
  template <typename I1, typename I2, typename Op>
    void assign_elements(I1 first1, I1 last1, I2 first2, Op op)
    {
      while (first1 != last1) {
        op(*first1, *first2);
        ++first1;
      }
    }



  //////////////////////////////////////////////////////////////////////////////
  // Matrix Support
  //
  // The following features support the generic implementation of the matrix
  // and matrix_ref classes.
  //////////////////////////////////////////////////////////////////////////////


  // Determines the row type of a matrix. If N is 0, then the row type is
  // simply the scalar type T. Otherwise, the row type is a row matrix of
  // the given dimension.
  template <typename M, std::size_t N>
    using Row_type = If<(N == 0), Matrix_reference<M>, matrix_ref<M, N>>;



  // Offset
  //
  // Returns the address of the nth element in a table whose element counts
  // are given in the range [first, last). That is, each value in that range
  // denotes the number of contiguous elements in that table. 
  //
  // Note that these functions are defined over iterators, but they're
  // expected to be pointers.

  // This overload is specifically for 1D matrices, hence the single
  // index n. Note that size + 1 is PTE of the sizes array owned by the
  // calling Matrix.
  template <typename Size>
    inline Size
    offset(const Size* size, Size n)
    {
      assert(n < *size);
      return n;
    }

  // Here, {n, args...} are the indexes of the requested element. Note that
  // size + 1 + sizeof...(Args) is PTE of the sizes array owned by the shape 
  // of the calling matrix.
  template <typename Size, typename... Args>
    inline Size
    offset(const Size* size, Size n, Args... args)
    {
      assert(n * *(size + 1) < *size);
      ++size;
      return (n * *size) + offset(size, args...);
    }



  // Row
  //
  // Returns a reference to the nth row of the matrix m, with base being the
  // originating matrix.
  //
  // FIXME: It would be nice to use Row_reference<M> and Row_refernce<const M> 
  // in these algorithms.
  template <typename B, typename M>
    inline Requires<(M::rank() != 1), typename M::row_reference>
    row(B& base, M& m, Size_type<M> n)
    {
      return {base, m.shape().inner(), m.data() + n * m.shape().size(1)};
    }

  template <typename B, typename M>
    inline Requires<(M::rank() != 1), typename M::const_row_reference>
    row(const B& base, const M& m, Size_type<M> n)
    {
      return {base, m.shape().inner(), m.data() + n * m.shape().size(1)};
    }

  // Specializations for the case where N == 1
  template <typename B, typename M>
    inline Requires<(M::rank() == 1), typename M::row_reference>
    row(B& base, M& m, Size_type<M> n)
    {
      return *(m.data() + n);
    }

  template <typename B, typename M>
    inline Requires<(M::rank() == 1), typename M::const_row_reference>
    row(const B& base, const M& m, Size_type<M> n)
    {
      return *(m.data() + n);
    }

} // namespace matrix_impl

