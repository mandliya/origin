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
#include <array>
#include <functional>
#include <iosfwd>
#include <numeric>

#include <origin/traits.hpp>

namespace origin
{
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


  //////////////////////////////////////////////////////////////////////////////
  // Matrix Shape
  //
  // This class describes the shape of an array: its number of dimensions (or
  // rank) and the number of elements (extent) contained in each dimension.
  // The rank of the shape is specified by the template parameter N.
  //
  // The template parameter T is an Integer type describing the size of each
  // extent.
  template <typename T, std::size_t N>
    class matrix_shape
    {
    public:
      using value_type     = T;
      using iterator       = const std::size_t*;
      using const_iterator = const std::size_t*;


      // Construct an "empty" matrix shape such that a matrix with this shape
      // contains 0 elements in each dimension.
      matrix_shape()
        : exts {}
      { }

      // Construct a matrix shape with the extents given by the sequence of
      // arguments.
      //
      // FIXME: Write type requirements (must be the same as T? Convertible?).
      template <typename... Exts>
        matrix_shape(Exts... es)
          : exts{es...}
        {
          static_assert(sizeof...(Exts) == N, "");
        }

      // Construct a matrix shape with the extents given in the specified
      // initializer list. The size of the list must be the same as the rank
      // of the shape (N).
      matrix_shape(std::initializer_list<T> list)
      {
        assert(list.size() == N);
        std::copy(list.begin(), list.end(), exts);
      }

      // Returns the number of dimensions representd by these extents.
      static constexpr std::size_t rank() { return N; }

      // Returns the number of elements contained in the nth dimension. If a
      // specific dimension is not given, then returns the number of elements
      // contained in the first dimension.
      T extent(std::size_t i = 0) const 
      { 
        assert(i < N);
        return exts[i]; 
      }

      // Computes the number of elements contained within an array of this
      // shape. This operation requires exactly N - 1 multiplications.
      T elements() const
      {
        using mul = std::multiplies<T>;
        return std::accumulate(exts, exts + N, std::size_t{1}, mul{});
      }

      // Iterators
      iterator begin() { return exts; }
      iterator end()   { return exts + N; }

      const_iterator begin() const { return exts; }
      const_iterator end() const   { return exts + N; }

    private:
      T exts[N];
    };


  // Equality comparable
  // Two shapes compare equal when they have the same rank and extents.
  //
  // TODO: This does not allow us to compare shapes of different rank. Should
  // we be able to do that? Should it just return false?
  template <typename T, std::size_t N>
    inline bool
    operator==(const matrix_shape<T, N>& a, const matrix_shape<T, N>& b)
    {
      return std::equal(a.begin(), a.end(), b.begin());
    }

  template <typename T, std::size_t N>
    inline bool
    operator!=(const matrix_shape<T, N>& a, const matrix_shape<T, N>& b)
    {
      return !(a == b);
    }


  // Some infrastructure
  namespace matrix_impl
  {
    // Used to enable specific interface features for 1D data structures. Here,
    // T is a placeholder template parameter used to guarantee that the 
    // constraint is not evaluated until instantiation.
    template <typename T, std::size_t N>
      using Requires_1D = Requires<Same<T, T>() && (N == 1)>;
  
  } // namespace matrix_impl




  //////////////////////////////////////////////////////////////////////////////
  // Matrix
  //
  // The matrix template specifies an implementaiton of an N-dimensional
  // matrix parameterized over some value type T.
  //
  // The matrix is also parameterized over an underlying Storage type which
  // is responsible for the allocation and ownership of the underlying value
  // types. Storage is required to be a Random_access_container. 
  template <typename T, std::size_t N, typename Storage = std::vector<T>>
    class matrix
    {
      using storage_type = Storage;
    public:
      using allocator_type  = typename storage_type::allocator_type;
      using value_type      = T;
      using reference       = T&;
      using const_reference = const T&;
      using pointer         = typename storage_type::pointer;
      using const_pointer   = typename storage_type::const_pointer;
      using size_type       = typename storage_type::size_type;
      using difference_type = typename storage_type::difference_type;
      using shape_type      = matrix_shape<size_type, N>;

      using iterator        = typename storage_type::iterator;
      using const_iterator  = typename storage_type::const_iterator;


    // Default construction
    matrix(const allocator_type& a = {})
      : dims(), data()
    { }


    // Move semantics
    matrix(matrix&& x, const allocator_type& alloc = {})
      : dims(std::move(x.dims)), data(std::move(x.data), alloc)
    { }
    
    matrix& operator=(matrix&& x)
    {
      dims = std::move(x.dims);
      data = std::move(x.data);
      return *this;
    }


    // Copy semantics
    matrix(matrix const& x, const allocator_type& alloc = {})
      : dims(x.dims), data(x.data, alloc)
    { }

    matrix& operator=(matrix const& x) 
    { 
      dims = x.dims;
      data = x.data;
      return *this;
    }


    // Construct a matrix of the specified shape.
    explicit
    matrix(const shape_type& shape, 
           const value_type& value = {},
           const allocator_type& alloc = {})
      : dims(shape), data(shape.elements(), value, alloc)
    { }

    // Refinement of the shape constructor for 1-D matrices. This allows
    // intialization over a single integer value.
    template <typename Int>
      explicit 
      matrix(Int n,
             const value_type& value = {},
             const allocator_type& alloc = {},
             matrix_impl::Requires_1D<Int, N>* = {})
        : dims(size_type(n)), data(n, value, alloc)
      { }


    // Range initialization
    //
    // TODO: Can we make this work for arbitrary levels of nesting for the
    // initializer list? Probably, but we'll do that later.
    //
    // FIXME: Provide iterator-based operators too.
    matrix(const shape_type& shape,
           std::initializer_list<value_type> list,
           const allocator_type& alloc = {})
      : dims(shape), data(list, alloc)
    {
      assert(dims.elements() == data.size());
    }


    // Returns the shape of the matrix.
    const shape_type& shape() const { return dims; }

    // Returns the rank of the matrix
    size_type rank() const { return dims.rank(); }

    size_type extent(size_type n) const { return dims.extent(n); }

    // Returns the total number of elements contained in the matrix.
    size_type size() const { return data.size(); }



    // Element access
    template <typename... Args>
      reference operator()(Args... args);
    template <typename... Args>
      const_reference operator()(Args... args) const;

      // Vector addition
      matrix& operator+=(const matrix& x);
      matrix& operator-=(const matrix& x);

      // Scalar addition
      matrix& operator=(const value_type& value);
      matrix& operator+=(const value_type& value);
      matrix& operator-=(const value_type& value);

      // Scalar multiplication
      matrix& operator*=(const value_type& value);
      matrix& operator/=(const value_type& value);
      matrix& operator%=(const value_type& value);


    // Iterators
    iterator begin() { return data.begin(); }
    iterator end()   { return data.end(); }

    const_iterator begin() const { return data.begin(); }
    const_iterator end() const   { return data.end(); }

    private:
      shape_type dims;
      storage_type data;
    };


  namespace matrix_impl
  {
    // The range [first, last) denotes the sequence of dimensions. The size of
    // this range corresponds to the extent for which we are requesting an offset.
    // The distance also corresponds to the the number of index parameters
    // being requested.
    template <typename T, typename I>
      inline T* 
      address(T* data, I first, I last, std::size_t n)
      {
        assert(first + 1 == last);
        assert(n < *first);
        return data + n;
      }

    template <typename T, typename I, typename... Args>
      inline T* 
      address(T* data, I first, I last, std::size_t n, Args... args)
      {
        // The number of indexes (1 + args) must match the number of extents.
        assert(first + (1 + sizeof...(Args)) == last);

        // The index must not exceed the extent.
        assert(n < *first);

        // Compute the size of the submatrix being addressed.
        // TODO: This is an extraordinarily redundand computation. We should
        // problaby pre-compute and cache this so we don't have keep re-computing
        // these values.
        using mul = std::multiplies<std::size_t>;
        std::size_t sz = std::accumulate(first + 1, last, std::size_t(1), mul{});

        // Move to the required address, and compute the next address for the
        // remaining indexes.
        return address(data + (n * sz), first + 1, last, args...);
      }

  } // namespace matrix_impl


  // Element access
  //
  // FIXME: Write type constraints on Args.
  template <typename T, std::size_t N, typename S>
    template <typename... Args>
      inline auto
      matrix<T, N, S>::operator()(Args... args) -> reference
      {
        // Explicitly convert to size_type so that all of the argument types
        // will be the same when calling address. That will make it easy to
        // write optimizations against common cases later on.
        return *matrix_impl::address(
          data.data(), dims.begin(), dims.end(), size_type(args)...);
      }

  template <typename T, std::size_t N, typename S>
    template <typename... Args>
      inline auto
      matrix<T, N, S>::operator()(Args... args) const -> const_reference
      {
        return *matrix_impl::address(
          data.data(), dims.begin(), dims.end(), size_type(args)...);
      }


  // Matrix addition
  // One matrix can be added to another (elementwise) only when they have the
  // same shape.
  template <typename T, std::size_t N, typename S>
    inline matrix<T, N, S>&
    matrix<T, N, S>::operator+=(const matrix& x)
    {
      assert(dims == x.dims);
      assign_elements(begin(), end(), x.begin(), plus_assign<value_type>{});
      return *this;
    }

  template <typename T, std::size_t N, typename S>
    inline matrix<T, N, S>&
    matrix<T, N, S>::operator-=(const matrix& x)
    {
      assert(dims == x.dims);
      assign_elements(begin(), end(), x.begin(), minus_assign<value_type>{});
      return *this;
    }


  // Scalar addition
  // A scalar can be added to a matrix, adding that value to each element of
  // the matrix.
  template <typename T, std::size_t N, typename S>
    inline matrix<T, N, S>& 
    matrix<T, N, S>::operator=(const value_type& value)  
    { 
      assign_value(begin(), end(), value, assign<value_type>{}); 
      return *this;
    }
      
  template <typename T, std::size_t N, typename S>
    inline matrix<T, N, S>& 
    matrix<T, N, S>::operator+=(const value_type& value) 
    { 
      assign_value(begin(), end(), value, plus_assign<value_type>{}); 
      return *this;
    }
      
  template <typename T, std::size_t N, typename S>
    inline matrix<T, N, S>& 
    matrix<T, N, S>::operator-=(value_type const& value) 
    { 
      assign_value(begin(), end(), value, minus_assign<value_type>{}); 
      return *this;
    }
  

  // Scalar multiplication
  // A matrix can be multiplied by a scalar, scaling each element by that
  // value.
  template <typename T, std::size_t N, typename S>
    inline matrix<T, N, S>& 
    matrix<T, N, S>::operator*=(value_type const& value) 
    { 
      assign_value(begin(), end(), value, multiplies_assign<value_type>{}); 
      return *this;
    }

  template <typename T, std::size_t N, typename S>
    inline matrix<T, N, S>& 
    matrix<T, N, S>::operator/=(value_type const& value) 
    { 
      assign_value(begin(), end(), value, divides_assign<value_type>{}); 
      return *this;
    }
      
  // This operation is only defined when T is a model of Euclidean domain.
  template <typename T, std::size_t N, typename S>
    inline matrix<T, N, S>& 
    matrix<T, N, S>::operator%=(value_type const& value) 
    { 
      assign_value(begin(), end(), value, modulus_assign<value_type>{}); 
      return *this;
    }


  // Helper functions


  // Returns the number of rows in a matrix with rank > 1. The number of rows
  // is the same as the extent in the 1st dimension.
  template <typename T, std::size_t N, typename S>
    inline typename matrix<T, N, S>::size_type
    rows(const matrix<T, N, S>& m)
    {
      static_assert(N > 1, "");
      return m.extent(0);
    }

  // Returns the number of columns in a matrix with rank > 1. This number of
  // columns is the same as the extent in the 2nd dimension.
  template <typename T, std::size_t N, typename S>
    inline typename matrix<T, N, S>::size_type
    cols(const matrix<T, N, S>& m)
    {
      static_assert(N > 1, "");
      return m.extent(1);
    }


  // Equality comparable
  // Two matrices compare equal when they have the same shape and elements.
  template <typename T, std::size_t N, typename S>
    inline bool 
    operator==(const matrix<T, N, S>& a, const matrix<T, N, S>& b)
    { 
      return a.shape() == b.shape() 
          && std::equal(a.begin(), a.end(), b.begin());
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


  // Declarations
  template <typename T, typename S>
    matrix<T, 2, S> 
    matrix_product(const matrix<T, 2, S>& a, const matrix<T, 2, S>& b);

  template <typename T, typename S>
    matrix<T, 2, S> 
    hadamard_product(const matrix<T, 2, S>& a, const matrix<T, 2, S>& b);


  // Matrix mulitplication
  // Two 2D matrices a (m x p) and b (p x n) can be multiplied, resulting in a
  // matrix c (m x n). Note that the "inner" dimension of the operands must
  // be the same.
  template <typename T, typename S>
    inline matrix<T, 2, S>
    operator*(const matrix<T, 2, S>& a, const matrix<T, 2, S>& b) 
    {
      return matrix_product(a, b);
    }


  // Streaming
  //
  // FIXME: Make this general. We need slicing to make it work elegantly.
  template <typename C, typename T, typename U, typename S>
    inline std::basic_ostream<C, T>&
    operator<<(std::basic_ostream<C, T>& os, const matrix<U, 1, S>& m)
    {
      os << '[';
      for (auto i = m.begin(); i != m.end(); ++i) {
        os << *i;
        if (next(i) != m.end())
          os << ',' << ' ';
      }
      os << ']';
      return os;
    }

  template <typename C, typename T, typename U, typename S>
    inline std::basic_ostream<C, T>&
    operator<<(std::basic_ostream<C, T>& os, const matrix<U, 2, S>& m)
    {
      os << '[';
      for (std::size_t i = 0; i < rows(m); ++i) {
        os << '[';
        for (std::size_t j = 0; j < cols(m); ++j) {
          os << m(i, j);
          if (j + 1 != cols(m))
            os << ',' << ' ';
        }
        os << ']';
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
  template <typename T, typename S>
    matrix<T, 2, S>
    matrix_product(const matrix<T, 2, S>& a, const matrix<T, 2, S>& b) 
    {
      using Size = typename matrix<T, 2, S>::size_type;

      assert(cols(a) == rows(b));

      matrix<T, 2, S> result{{rows(a), cols(b)}, 0};
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
  template <typename T, std::size_t N, typename S>
    matrix<T, N, S>
    hadamard_product(const matrix<T, N, S>& a, const matrix<T, N, S>& b) 
    {
      assert(a.shape() == b.shape());
      matrix<T, N, S> result = a;
      assign_elements(a.begin(), a.end(), b.begin(), multiplies_assign<T>{}); 
      return result;
    }

} // namespace origin


#endif
