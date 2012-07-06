// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_MATH_MATRIX_HPP
#  error Do not include this file directly. Include matrix/matrix.hpp.
#endif

#include <iosfwd>

namespace origin
{
  //////////////////////////////////////////////////////////////////////////////
  // Matrix Operations
  //
  // The following operations are defined for all Matrix types.
  //////////////////////////////////////////////////////////////////////////////



  // Returns the number of rows in a matrix with rank > 1. The number of rows
  // is the same as the extent in the 1st dimension.
  template <typename M, typename = Requires<Matrix<M>()>>
    inline Size_type<M> rows(const M& m)
    {
      static_assert(M::rank() > 0, "");
      return m.extent(0);
    }


  // Returns the number of columns in a matrix with rank > 1. This number of
  // columns is the same as the extent in the 2nd dimension.
  template <typename M, typename = Requires<Matrix<M>()>>
    inline Size_type<M> cols(const M& m)
    {
      static_assert(M::rank() > 0, "");
      return m.extent(1);
    }



  // Equality comparable
  // Two matrices compare equal when they have the same shape and elements.
  template <typename M>
    inline auto 
    operator==(const M& a, const M& b)
      -> Requires<Matrix<M>(), bool>
    { 
      return a.shape() == b.shape() 
          && std::equal(a.begin(), a.end(), b.begin());
    }
  
  template <typename M>
    inline auto 
    operator!=(const M& a, const M& b) -> Requires<Matrix<M>(), bool>
    {
      return !(a == b);
    }
      


  // FIXME: These arithmetic operations can't be implemented in the same
  // straightforward manner since the statement "M result = a" would not make
  // a copy of the elements of a; it would just copy the pointer. Basically,
  // we'd be trying to modify the first argument.
  //
  // We could assume that there's a corresponding matrix for a matrix_ref, but
  // if we want to be precise, we're going to have to parameterize the
  // matrix_ref over an allocator even though *no allocation is performed*!
  // Allocators as template parameters are poisonous.
  //
  // Furthermore, we would be returning a different type -- which leads me to
  // believe that these operators should not be defined for matrix_refs.


  // Matrix addition
  // Add or subtract the elements of a matrix.
  template <typename T, std::size_t N>
    inline matrix<T, N>
    operator+(const matrix<T, N>& a, const matrix<T, N>& b)
    {
      assert(a.dim() == b.dim());
      matrix<T, N> result = a;
      return result += b;
    }

  // Addition for matrix and matrix_ref.
  template <typename T, std::size_t N>
    inline matrix<T, N>
    operator+(const matrix<T, N>& a, const matrix_ref<T, N>& b)
    {
      assert(a.dim() == b.dim());
      matrix<T, N> result = a;
      return result += b;
    }

  template <typename T, std::size_t N>
    inline matrix<T, N>
    operator+(const matrix_ref<T, N>& a, const matrix<T, N>& b)
    {
      assert(a.dim() == b.dim());
      matrix<T, N> result = a;
      return result += b;
    }

  //  Addition of matrix_refs.
  //
  // NOTE: This operation is kind of funny because it is heterogeneous in its
  // result type. If we try to concept check Matrix<R, R> (where R is a matrix
  // ref type), we would normally be asking for an operation a homogeneous
  // operator+(R,R)->R. That's not what we have.
  //
  // In order to check this concept, we have to weaken the result type. The
  // C++0x concepts required that the result be convertible to the argument
  // types. That doesn't work here because matrix is not convertible to matrix
  // ref. It's the other way around.
  //
  // The correct way to check this is to say that the result type must share
  // a common type with the domain type. That is, if U is the result type of the
  // expression t + t (with t having type T), then Common<T, U> must be true.
  // That's it.

  template <typename T, std::size_t N>
    inline matrix<T, N>
    operator+(const matrix_ref<T, N>& a, const matrix_ref<T, N>& b)
    {
      assert(a.dim() == b.dim());
      matrix<T, N> result = a;
      return result += b;
    }


  // Matrix subtraction
  template <typename T, std::size_t N>
    inline matrix<T, N>
    operator-(const matrix<T, N>& a, const matrix<T, N>& b)
    {
      asssert(a.dim() == b.dim());
      matrix<T, N> result = a;
      return a -= b;
    }

  // Subtraction for matrix and matrix_ref.
  template <typename T, std::size_t N>
    inline matrix<T, N>
    operator-(const matrix<T, N>& a, const matrix_ref<T, N>& b)
    {
      assert(a.dim() == b.dim());
      matrix<T, N> result = a;
      return result -= b;
    }

  template <typename T, std::size_t N>
    inline matrix<T, N>
    operator-(const matrix_ref<T, N>& a, const matrix<T, N>& b)
    {
      assert(a.dim() == b.dim());
      matrix<T, N> result = a;
      return result -= b;
    }

  // Subtraction for matrix_refs.
  template <typename T, std::size_t N>
    inline matrix<T, N>
    operator-(const matrix_ref<T, N>& a, const matrix_ref<T, N>& b)
    {
      assert(a.dim() == b.dim());
      matrix<T, N> result = a;
      return result -= b;
    }



  // Scalar addition
  // Adding a scalar to a matrix adds that value to each element in the matrix.
  // The following operations are supported:
  //
  //    a + n
  //    n + a
  //
  template <typename T, std::size_t N>
    inline matrix<T, N> 
    operator+(const matrix<T, N>& a, const T& n)
    {
      matrix<T, N> result = a;
      return result += n;
    }

  template <typename T, std::size_t N>
    inline matrix<T, N> 
    operator+(const T& n, const matrix<T, N>& a)
    {
      matrix<T, N> result = a;
      return result += n;
    }

  // Scalar subtraction
  // A scalar can be subtracted from a matrix. It is equivalent to adding the
  // negation of the scalar. That is:
  //
  //    a - n <=> a + -n;
  //
  // It is not possible to subtract a matrix from a scalar.
  template <typename T, std::size_t N>
    inline matrix<T, N> 
    operator-(const matrix<T, N>& a, const T& n)
    {
      matrix<T, N> result = a;
      return result += n;
    }

  // Scalar multiplication
  // Multiplying a matrix by a scalar multiplies each element by the scalar
  // value. The following operations are supported:
  //
  //    a * n
  //    n * a
  //
  template <typename T, std::size_t N>
    inline matrix<T, N>
    operator*(const matrix<T, N>& a, const T& n)
    {
      matrix<T, N> result = a;
      return result *= n;
    }

  template <typename T, std::size_t N>
    inline matrix<T, N>
    operator*(const T& n, const matrix<T, N>& a)
    {
      matrix<T, N> result = a;
      return result *= n;
    }

  // Scalar division
  // A matrix can be divided by a scalar value. It is equivalent to multiplying
  // the matrix by the reciprocal of the scalar.
  //
  //    a / n <=> a * 1/n
  //
  // It is not possible to divide a scalar by a matrix.
  template <typename T, std::size_t N>
    inline matrix<T, N>
    operator/(const matrix<T, N>& a, const T& n)
    {
      matrix<T, N> result = a;
      return result /= n;
    }

  // Scalar modulus
  // Returns a matrix containing the remainder of each element divided by the
  // given scalar value.
  //
  // This operation is only available when T is an Integer type.
  template <typename T, std::size_t N>
    inline matrix<T, N>
    operator%(const matrix<T, N>& a, const T& n)
    {
      matrix<T, N> result = a;
      return result %= n;
    }


  // Declarations
  template <typename T>
    matrix<T, 2> 
    matrix_product(const matrix<T, 2>& a, const matrix<T, 2>& b);

  template <typename T>
    matrix<T, 2> 
    hadamard_product(const matrix<T, 2>& a, const matrix<T, 2>& b);


  // Matrix mulitplication
  // Two 2D matrices a (m x p) and b (p x n) can be multiplied, resulting in a
  // matrix c (m x n). Note that the "inner" dimension of the operands must
  // be the same.
  template <typename T>
    inline matrix<T, 2>
    operator*(const matrix<T, 2>& a, const matrix<T, 2>& b) 
    {
      return matrix_product(a, b);
    }


  // Streaming
  template <typename C, typename T, typename M>
    inline Requires<Matrix<M>(), std::basic_ostream<C, T>&>
    operator<<(std::basic_ostream<C, T>& os, const M& m)
    {
      os << '[';
      for (std::size_t i = 0; i < rows(m); ++i) {
        os << m[i];
        if (i + 1 != rows(m))
          os << ',';
      }
      os << ']';
      return os;
    }


  //////////////////////////////////////////////////////////////////////////////
  // Matrix Multiplication
  //
  // The following algorithms implement various forms of matrix products.


  // Matrix Product
  //
  // The usual meaning of the operation.
  //
  // FIXME: Write in terms of slices when we get them.
  template <typename T>
    matrix<T, 2>
    matrix_product(const matrix<T, 2>& a, const matrix<T, 2>& b) 
    {
      using Size = typename matrix<T, 2>::size_type;

      assert(cols(a) == rows(b));

      matrix<T, 2> result{{rows(a), cols(b)}, 0};
      for (Size i = 0; i != rows(a); ++i) {
        for (Size j = 0; j < cols(b); ++j) {
          for (Size k = 0; k < rows(b); ++k) {
            result(i, j) += a(i, k) * b(k, j);
          }
        }
      }
      return result;
    }

  
  // Hadamard Product
  //
  // The hadamard product can be easly generalized to N-dimensional matrices 
  // since the operation is performed elementwise. The operands only need the
  // same shape.
  template <typename T, std::size_t N>
    matrix<T, N>
    hadamard_product(const matrix<T, N>& a, const matrix<T, N>& b) 
    {
      assert(a.shape() == b.shape());
      matrix<T, N> result = a;
      matrix_impl::assign_elements(
        a.begin(), a.end(), b.begin(), matrix_impl::multiplies_assign<T>{}); 
      return result;
    }
} // namespace origin

