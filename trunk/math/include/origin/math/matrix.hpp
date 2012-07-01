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
#include <origin/concepts.hpp>

namespace origin
{
  template <typename T, std::size_t N, typename S>
    class matrix;

  template <typename T, std::size_t N>
    class matrix_ptr;


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


  // Compute the product of the elements in the range [first, last) starting
  // with the initial value. If no initial value is given, the value 1 is
  // used by default.
  template <typename I, typename T>
    T product(I first, I last, const T& value = 1)
    {
      return std::accumulate(first, last, value, std::multiplies<T>{});
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
          count();
        }

      // Construct a matrix shape with the extents given in the specified
      // initializer list. The size of the list must be the same as the rank
      // of the shape (N).
      matrix_shape(std::initializer_list<T> list)
      {
        assert(list.size() == N);
        std::copy(list.begin(), list.end(), exts);
        count();
      }

      // Construct a matrix over the extents given in the range [first, last).
      // The size of the [first, last) must be the same as the rank of the
      // shape (N).
      template <typename I>
        matrix_shape(I first, I last)
        {
          assert(std::distance(first, last) == N);
          std::copy(first, last, exts);
          count();
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
      //
      // TODO: This probably isn't the right name for this function, but 
      // neither is size() (that relates to iterators).
      T elements() const { return elems[0]; }


      // Returns the shape of a row described by this shape.
      matrix_shape<T, N - 1> inner() const 
      { 
        static_assert(N > 1, "");
        return {exts + 1, exts + N}; 
      }

      // Underlying data
      const T* extents() const { return exts; }
      const T* sizes() const { return elems; }

      // Iterators
      iterator begin() { return exts; }
      iterator end()   { return exts + N; }

      const_iterator begin() const { return exts; }
      const_iterator end() const   { return exts + N; }

    private:
      void count();

    private:
      T exts[N];    // The extents of each dimension
      T elems[N];   // Number of elements stored in each sub-table.
    };


  // Pre-compute the number of elements for each sub-table described by the
  // shape. Note that N must not be 0.
  template <typename T, std::size_t N>
    inline void 
    matrix_shape<T, N>::count()
    {
      if (N == 0)
        return;
      T i = N - 1;
      elems[i] = exts[i];
      if (N == 1)
        return;      
      do {
        --i;
        elems[i] = exts[i] * elems[i + 1];
      } while (i != 0);
    }


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

    // Determines the row type of a matrix. If N is 0, then the row type is
    // simply the scalar type T. Otherwise, the row type is a row matrix of
    // the given dimension.
    template <typename T, std::size_t N>
      using Row_type = If<(N == 0), T, matrix_ptr<T, N>>;
  
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
      using row_type        = matrix_impl::Row_type<T, N - 1>;
      using const_row_type  = matrix_impl::Row_type<const T, N - 1>;

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
    static constexpr size_type rank() { return N; }

    // Returns the extent of the matrix in the nth dimension.
    size_type extent(size_type n) const { return dims.extent(n); }

    // Returns the total number of elements contained in the matrix.
    size_type size() const { return data.size(); }



    // Element access
    template <typename... Args>
      reference operator()(Args... args);
    template <typename... Args>
      const_reference operator()(Args... args) const;

    // Row access
    row_type       operator[](size_type n);
    const_row_type operator[](size_type n) const;

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
        size_type off = matrix_impl::offset(dims.sizes(), size_type(args)...);
        return *(data.data() + off);
      }

  template <typename T, std::size_t N, typename S>
    template <typename... Args>
      inline auto
      matrix<T, N, S>::operator()(Args... args) const -> const_reference
      {
        size_type off = matrix_impl::offset(dims.sizes(), size_type(args)...);
        return *(data.data() + off);
      }


  namespace matrix_impl
  {
    // Returs the address the "row" corresponding to the given index. If N == 1
    // then the resulting "row" is just a scalar value.
    //
    // TODO: This feels inelegant. Maybe there should be a better way to
    // implement it.
    template <std::size_t N, typename Result>
      struct get_row
      {
        template <typename T, typename Shape>
          Result operator()(T* ptr, const Shape& shape, std::size_t n)
          {
            std::size_t off = n * shape.sizes()[1];
            return {shape.inner(), ptr + off};
          }
      };

    template <typename Result>
      struct get_row<1, Result>
      {
        template <typename T, typename Shape>
          Result operator()(T* ptr, const Shape& shape, std::size_t n)
          {
            return *(ptr + n);
          }
      };

    template <std::size_t N, typename Result, typename T, typename Shape>
      inline Result
      row(T* ptr, const Shape& shape, std::size_t n)
      {
        get_row<N, Result> f;
        return f(ptr, shape, n);
      }

  } // namespace matrix_impl


  template <typename T, std::size_t N, typename S>
    inline auto
    matrix<T, N, S>::operator[](size_type n) -> row_type
    {
      return matrix_impl::row<N, row_type>(data.data(), dims, n);
    }

  template <typename T, std::size_t N, typename S>
    inline auto
    matrix<T, N, S>::operator[](size_type n) const -> const_row_type
    {
      return matrix_impl::row<N, const_row_type>(data.data(), dims, n);
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



  //////////////////////////////////////////////////////////////////////////////
  // Matrix ptr
  //
  // A matrix ptr wraps a contiguously allocated region of memory, imbuing it
  // with a specified shape. The matrix row does not own its elements.
  //
  // This is closely related to the notion of a slice or a submatrix except
  // that all elements are contiguously allocated and define a complete matrix.
  // Indexing into a matrix row does not require strides or spans to move from
  // one element to the next.
  //
  // The matrix row class is a Matrix; it has the same interface as the matrix
  // class.
  //
  // TOOD: There is a disagreement between the pointer/size types here, and
  // those derived from the allocator in the matrix class.
  template <typename T, std::size_t N>
    class matrix_ptr
    {
    public:
      using value_type      = T;
      using reference       = T&;
      using const_reference = const T&;
      using pointer         = T*;
      using const_pointer   = const T*;
      using size_type       = std::size_t;
      using difference_type = std::ptrdiff_t;

      using shape_type      = matrix_shape<size_type, N>;
      using row_type        = matrix_impl::Row_type<T, N - 1>;
      using const_row_type  = matrix_impl::Row_type<const T, N - 1>;

      using iterator        = T*;
      using const_iterator  = const T*;

    // Initialize the submatrix with the given shape.
    matrix_ptr(const shape_type& s, pointer f)
      : ptr(f), dims(s)
    { }


    // Returns the shape of the matrix.
    const shape_type& shape() const { return dims; }

    // Returns the rank of the matrix
    static constexpr size_type rank() { return N; }

    // Returns the extent of the matrix in the nth dimension.
    size_type extent(size_type n) const { return dims.extent(n); }

    // Returns the total number of elements contained in the matrix.
    size_type size() const { return dims.elements(); }


    // Element access
    template <typename... Args>
      reference operator()(Args... args);
    template <typename... Args>
      const_reference operator()(Args... args) const;

    // Row access
    row_type       operator[](size_type n);
    const_row_type operator[](size_type n) const;


    // Iterators
    iterator begin() { return ptr; }
    iterator end()   { return ptr + dims.elements(); }

    const_iterator begin() const { return ptr; }
    const_iterator end() const   { return ptr + dims.elements(); }

    private:
      pointer ptr;      // The 1st element in the matrix
      shape_type dims;  // The shape of the matrix
    };


  // FIXME: Write type requirements.
  template <typename T, std::size_t N>
    template <typename... Args>
      inline auto
      matrix_ptr<T, N>::operator()(Args... args) -> reference
      {
        size_type off = matrix_impl::offset(dims.sizes(), size_type(args)...);
        return *(ptr + off);
      }

  template <typename T, std::size_t N>
    template <typename... Args>
      inline auto
      matrix_ptr<T, N>::operator()(Args... args) const -> const_reference
      {
        size_type off = matrix_impl::offset(dims.sizes(), size_type(args)...);
        return *(ptr + off);
      }

  template <typename T, std::size_t N>
    inline auto
    matrix_ptr<T, N>::operator[](size_type n) -> row_type
    {
      return matrix_impl::row<N, row_type>(ptr, dims, n);
    }

  template <typename T, std::size_t N>
    inline auto
    matrix_ptr<T, N>::operator[](size_type n) const -> const_row_type
    {
      return matrix_impl::row<N, const_row_type>(ptr, dims, n);
    }



  //////////////////////////////////////////////////////////////////////////////
  // Matrix operations
  //
  // The following operations are defined for all Matrix types.

  namespace matrix_impl
  {
    // Safely deduce the result of the expression x.shape().
    template <typename T>
      struct get_shape_result
      {
      private:
        template <typename X>
          static auto check(const X& x) -> decltype(x.shape());

        static subst_failure check(...);
      public:
        using type = decltype(check(std::declval<T>()));
      };

    template <typename T>
      using Shape_type = typename get_shape_result<T>::type;
  };


  // Returns true if T is a matrix.
  //
  // FIXME: A matrix is substantially more complex than this. Finish defining
  // and implementing the concept.
  template <typename T>
    constexpr bool Matrix()
    {
      return Subst_succeeded<matrix_impl::Shape_type<T>>();
    }



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
  // We could assume that there's a corresponding matrix for a matrix_ptr, but
  // if we want to be precise, we're going to have to parameterize the
  // matrix_ptr over an allocator even though *no allocation is performed*!
  // Allocators as template parameters are poisonous.
  //
  // Furthermore, we would be returning a different type -- which leads me to
  // believe that these operators should not be defined for matrix_ptrs.


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
