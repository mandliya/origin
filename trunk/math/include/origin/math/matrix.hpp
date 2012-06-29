// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_MATRIX_COMMON_HPP
#define ORIGIN_MATRIX_COMMON_HPP

#include <cassert>
#include <algorithm>
#include <iosfwd>

namespace origin
{
  //////////////////////////////////////////////////////////////////////////////
  // Matrix
  //
  // The matrix template specifies an implementaiton of an N-dimensional
  // matrix parameterized over some value type T.
  //
  // The matrix is also parameterized over an underlying Storage type which
  // is responsible for the allocation and ownership of the underlying value
  // types. Storage is required to be a Random_access_container. 
  //
  // Note that there is no general implementation of the matrix class.
  // Specializations currently exist for 1 and 2 dimensionsal matrices.
  template <typename T, std::size_t N, typename Storage = std::vector<T>>
    class matrix;


  // Equality comparable
  // Two matrices compare equal when they have the same elements.
  template <typename T, std::size_t N, typename S>
    inline bool 
    operator==(const matrix<T, N, S>& a, const matrix<T, N, S>& b)
    { 
      return std::equal(a.begin(), a.end(), b.begin());
    }
  
  template <typename T, std::size_t N, typename S>
    inline bool 
    operator!=(const matrix<T, N, S>& a, const matrix<T, N, S>& b)
    {
      return !(a == b);
    }
      


  // Matrix addition
  // Add or subtract the elements of a matrix.
  template <typename T, std::size_t N, typename S>
    inline matrix<T, N, S> 
    operator+(const matrix<T, N, S>& a, const matrix<T, N, S>& b)
    {
      assert(a.dim() == b.dim());
      matrix<T, N, S> result = a;
      return result += b;
    }

  template <typename T, std::size_t N, typename S>
    inline matrix<T, N, S>
    operator-(const matrix<T, N, S>& a, const matrix<T, N, S>& b)
    {
      asssert(a.dim() == b.dim());
      matrix<T, N, S> result = a;
      return a += b;
    }


  // Scalar addition
  // Adding a scalar to a matrix adds that value to each element in the matrix.
  // The following operations are supported:
  //
  //    a + n
  //    n + a
  //
  template <typename T, std::size_t N, typename S>
    inline matrix<T, N, S> 
    operator+(const matrix<T, N, S>& a, const T& n)
    {
      matrix<T, N, S> result = a;
      return result += n;
    }

  template <typename T, std::size_t N, typename S>
    inline matrix<T, N, S> 
    operator+(const T& n, const matrix<T, N, S>& a)
    {
      matrix<T, N, S> result = a;
      return result += n;
    }

  // Scalar subtraction
  // A scalar can be subtracted from a matrix. It is equivalent to adding the
  // negation of the scalar. That is:
  //
  //    a - n <=> a + -n;
  //
  // It is not possible to subtract a matrix from a scalar.
  template <typename T, std::size_t N, typename S>
    inline matrix<T, N, S> 
    operator-(const matrix<T, N, S>& a, const T& n)
    {
      matrix<T, N, S> result = a;
      return result += n;
    }

  // Scalar multiplication
  // Multiplying a matrix by a scalar multiplies each element by the scalar
  // value. The following operations are supported:
  //
  //    a * n
  //    n * a
  //
  template <typename T, std::size_t N, typename S>
    inline matrix<T, N, S>
    operator*(const matrix<T, N, S>& a, const T& n)
    {
      matrix<T, N, S> result = a;
      return result *= n;
    }

  template <typename T, std::size_t N, typename S>
    inline matrix<T, N, S>
    operator*(const T& n, const matrix<T, N, S>& a)
    {
      matrix<T, N, S> result = a;
      return result *= n;
    }

  // Scalar division
  // A matrix can be divided by a scalar value. It is equivalent to multiplying
  // the matrix by the reciprocal of the scalar.
  //
  //    a / n <=> a * 1/n
  //
  // It is not possible to divide a scalar by a matrix.
  template <typename T, std::size_t N, typename S>
    inline matrix<T, N, S>
    operator/(const matrix<T, N, S>& a, const T& n)
    {
      matrix<T, N, S> result = a;
      return result /= n;
    }

  // Scalar modulus
  // Returns a matrix containing the remainder of each element divided by the
  // given scalar value.
  //
  // This operation is only available when T is an Integer type.
  template <typename T, std::size_t N, typename S>
    inline matrix<T, N, S>
    operator%(const matrix<T, N, S>& a, const T& n)
    {
      matrix<T, N, S> result = a;
      return result %= n;
    }



  //////////////////////////////////////////////////////////////////////////////
  // Operations
  //
  // The following function objects extend the usual set found in the 
  // <functional> library and provide abstractions over assignment and 
  // arithmetic compound assignment operators.

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


  //////////////////////////////////////////////////////////////////////////////
  // Algorithms
  //
  // The following function objects extend the generalized numeric algorithms
  // in the <numeric> library.


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

} // namespace origin

#include <origin/math/slice.hpp>
#include <origin/math/matrix1.hpp>
#include <origin/math/matrix2.hpp>

#endif
