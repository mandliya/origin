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
    class matrix_ref;


  //////////////////////////////////////////////////////////////////////////////
  // Operations
  //
  // The following function objects extend the usual set found in the 
  // <functional> library and provide abstractions over assignment and 
  // arithmetic compound assignment operators.
  //
  // TODO: These should probably be moved into the functional module.

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
  // Support algorithms for implementing some matrix operations.
  //
  // TODO: These should be moved into the algorithms module.


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
      using value_type = T;

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
      matrix_shape(std::initializer_list<value_type> list)
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


      // Returns the rank of the shape.
      static constexpr value_type rank() { return N; }

      // Returns the number of sub-matrices contained in the nth dimension. If 
      // a specific dimension is not given, then returns the number of rows.
      value_type extent(std::size_t i = 0) const 
      { 
        assert(i < N);
        return exts[i]; 
      }

      // Returns the total number of elements contained in a submatrix of the
      // ith dimension. When i == 0, this is the total number of elements in
      // the matrix. When i == 1, this is the total number of elementsm in
      // a row the matrix, etc. If i is not given, the function returns the
      // total number of elements in the table.
      value_type size(std::size_t i = 0) const
      {
        assert(i < N);
        return elems[i];
      }

      // Returns the shape of a row described by this shape.
      //
      // TODO: Is there a better name for this function?
      matrix_shape<value_type, N - 1> inner() const 
      { 
        static_assert(N > 1, "");
        return {exts + 1, exts + N}; 
      }


      // Data accessors

      // Returns a pointer to the array of extents.
      const value_type* extents() const { return exts; }

      // Returns a pointer to the array of sizes.
      const value_type* sizes() const { return elems; }


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
  // Note that we do not compare sizes since they are computed from the extents
  // of the shape (that array exists only as an optimization).
  //
  // TODO: This definition not allow us to compare shapes of different rank.
  // The operation is not defined. Should it be? Obviously, it would just 
  // be false. The implication is that each shape type is part of a local
  // group.
  template <typename T, std::size_t N>
    inline bool
    operator==(const matrix_shape<T, N>& a, const matrix_shape<T, N>& b)
    {
      for (std::size_t i = 0; i < N; ++i)
        if (a.extent(i) != b.extent(i))
          return false;
      return true;
    }

  template <typename T, std::size_t N>
    inline bool
    operator!=(const matrix_shape<T, N>& a, const matrix_shape<T, N>& b)
    {
      return !(a == b);
    }


  namespace matrix_traits
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
  
  } // namespace matrix_traits


  // An alias to the reference-to-element type of the Matrix, M. The type
  // denoted by this alias depends on whehter or not M is const. For example:
  //
  //    using M = matrix<T, N>;
  //    Matrix_reference_type<M>       // is T&
  //    Matrix_reference_type<const M> // is const T&
  //
  // Note that M can be any type modeling the Matrix concept. If M does not
  // satisfy the requirements of the Matrix concept, then the resulting type
  // indicates substitution failure.
  template <typename M>
    using Matrix_reference = 
      If<Const<M>(), Associated_const_reference<M>, Associated_reference<M>>;


  // An alias to the pointer-to-element type of the matrix, M. The type
  // denoted by this alias depends on whehter or not M is const. For example:
  //
  //    using M = matrix<T, N>;
  //    Matrix_pointer_type<M>       // is T*
  //    Matrix_pointer_type<const M> // is const T*
  //
  // Note that M can be any type modeling the Matrix concept. If M does not
  // satisfy the requirements of the Matrix concept, then the resulting type
  // indicates substitution failure.
  template <typename M>
    using Matrix_pointer =
      If<Const<M>(), Associated_const_pointer<M>, Associated_pointer<M>>;


  // An alias to the reference-to-row type of the matrix, M. The type denoted
  // by this alias depends on the const-ness and rank of M. When the rank of
  // M is 1, this type is the same as the matrice's reference-to-eleemnt type.
  // Otherwise, the type is an instance of the matrix_ref class template.
  // Examples of usage and results are:
  //
  //    using M1 = matrix<T, 1>;
  //    using M2 = matrix<T, 2>;
  //    Matrix_row_reference
  //
  // Note that M can be any type modeling the Matrix concept. If M does not
  // satisfy the requirements of the Matrix concept, then the resulting type
  // indicates substitution failure.
  template <typename M>
    using Matrix_row_reference =
      If<Const<M>(),
        typename matrix_traits::get_associated_row_reference<M>::type,
        typename matrix_traits::get_associated_const_row_reference<M>::type>;



  // An alias to the shape type associated with a matrix.
  template <typename M>
    using Matrix_shape_type = typename matrix_traits::get_shape_type<M>::type;


  // Returns true if T is a matrix.
  //
  // FIXME: A matrix is substantially more complex than this. Finish defining
  // and implementing the concept.
  template <typename M>
    constexpr bool Matrix()
    {
      return Subst_succeeded<Matrix_shape_type<M>>();
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
    template <typename M, std::size_t N>
      using Row_type = If<(N == 0), Matrix_reference<M>, matrix_ref<M, N>>;

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
  template <typename T, std::size_t N, typename Store = std::vector<T>>
    class matrix
    {
      using this_type           = matrix<T, N, Store>;
      using store_type          = Store;
    public:
      using allocator_type      = typename store_type::allocator_type;
      using value_type          = typename store_type::value_type;
      using reference           = typename store_type::reference;
      using const_reference     = typename store_type::const_reference;
      using pointer             = typename store_type::pointer;
      using const_pointer       = typename store_type::const_pointer;
      using size_type           = typename store_type::size_type;
      using difference_type     = typename store_type::difference_type;

      using shape_type          = matrix_shape<size_type, N>;
      using row_reference       = matrix_impl::Row_type<this_type, N - 1>;
      using const_row_reference = matrix_impl::Row_type<const this_type, N - 1>;

      using iterator            = typename store_type::iterator;
      using const_iterator      = typename store_type::const_iterator;


      // Default construction
      explicit matrix(const allocator_type& a = {})
        : dims(), elems()
      { }


      // Move semantics
      // Moving a matrix transfers the underlying state of the object x into
      // this object, leaving x empty.
      matrix(matrix&& x, const allocator_type& alloc = {})
        : dims(std::move(x.dims)), elems(std::move(x.elems), alloc)
      { }
      
      matrix& operator=(matrix&& x)
      {
        dims = std::move(x.dims);
        elems = std::move(x.elems);
        return *this;
      }


      // Copy semantics
      matrix(matrix const& x, const allocator_type& alloc = {})
        : dims(x.dims), elems(x.elems, alloc)
      { }

      matrix& operator=(matrix const& x) 
      { 
        dims = x.dims;
        elems = x.elems;
        return *this;
      }


      // Conversion from matrix_ref
      matrix(const matrix_ref<this_type, N>& x)
        : dims(x.shape()), elems(x.begin(), x.end())
      { }

      matrix& operator=(const matrix_ref<this_type, N>& x)
      {
        dims = x.shape();
        elems.assign(x.begin(), x.end());
      }


      // Construct a matrix of the specified shape.
      explicit
      matrix(const shape_type& shape, 
             const value_type& value = {},
             const allocator_type& alloc = {})
        : dims(shape), elems(shape.size(), value, alloc)
      { }

      // Refinement of the shape constructor for 1-D matrices. This allows
      // intialization over a single integer value.
      template <typename Int>
        explicit 
        matrix(Int n,
               const value_type& value = {},
               const allocator_type& alloc = {},
               matrix_impl::Requires_1D<Int, N>* = {})
          : dims(size_type(n)), elems(n, value, alloc)
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
        : dims(shape), elems(list, alloc)
      {
        assert(dims.size() == elems.size());
      }



      // Returns the shape of the matrix.
      const shape_type& shape() const { return dims; }

      // Returns the rank of the matrix.
      static constexpr size_type rank() { return N; }

      // Returns the extent of the matrix in the nth dimension.
      size_type extent(size_type n) const { return dims.extent(n); }

      // Returns the total number of elements contained in the matrix.
      size_type size() const { return elems.size(); }


      // Element access
      template <typename... Args>
        reference operator()(Args... args);
      template <typename... Args>
        const_reference operator()(Args... args) const;


      // Row access
      row_reference       operator[](size_type n);
      const_row_reference operator[](size_type n) const;


      // Data access
      pointer       data()       { return elems.data(); }
      const_pointer data() const { return elems.data(); }


      // Vector addition
      template <typename M>
        Requires<Matrix<M>(), matrix&> operator+=(const M& x);
      template <typename M>
        Requires<Matrix<M>(), matrix&> operator-=(const M& x);

      // Scalar addition
      matrix& operator=(const value_type& value);
      matrix& operator+=(const value_type& value);
      matrix& operator-=(const value_type& value);

      // Scalar multiplication
      matrix& operator*=(const value_type& value);
      matrix& operator/=(const value_type& value);
      matrix& operator%=(const value_type& value);


      // Iterators
      iterator begin() { return elems.begin(); }
      iterator end()   { return elems.end(); }

      const_iterator begin() const { return elems.begin(); }
      const_iterator end() const   { return elems.end(); }

      private:
        shape_type dims;
        store_type elems;
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
        return *(data() + off);
      }

  template <typename T, std::size_t N, typename S>
    template <typename... Args>
      inline auto
      matrix<T, N, S>::operator()(Args... args) const -> const_reference
      {
        size_type off = matrix_impl::offset(dims.sizes(), size_type(args)...);
        return *(data() + off);
      }


  namespace matrix_impl
  {
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


  template <typename T, std::size_t N, typename S>
    inline auto
    matrix<T, N, S>::operator[](size_type n) -> row_reference
    {
      return matrix_impl::row(*this, *this, n);
    }


  template <typename T, std::size_t N, typename S>
    inline auto
    matrix<T, N, S>::operator[](size_type n) const -> const_row_reference
    {
      return matrix_impl::row(*this, *this, n);
    }


  // Matrix addition
  // One matrix can be added to another (elementwise) only when they have the
  // same shape.
  template <typename T, std::size_t N, typename S>
    template <typename M>
      inline Requires<Matrix<M>(), matrix<T, N, S>&>
      matrix<T, N, S>::operator+=(const M& x)
      {
        static_assert(rank() == M::rank(), "");
        assert(shape() == x.shape());
        assign_elements(begin(), end(), x.begin(), plus_assign<value_type>{});
        return *this;
      }

  template <typename T, std::size_t N, typename S>
    template <typename M>
      inline Requires<Matrix<M>(), matrix<T, N, S>&>
      matrix<T, N, S>::operator-=(const M& x)
      {
        static_assert(rank() == M::rank(), "");
        assert(shape() == x.shape());
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
  // Matrix ref
  //
  // A matrix ref is a reference to a contiguous sub-matrix with a specified
  // shape. The matrix ref does not own its elements.
  //
  // This is closely related to the notion of a slice or a submatrix except
  // that all elements are contiguously allocated and define a complete matrix.
  // Indexing into a matrix row does not require strides or spans to move from
  // one element to the next.
  //
  // The matrix row class is a Matrix except that assignment writes through
  // (not to) the matrix reference to the referenced data.
  //
  // Template parameters:
  //    M -- The type of matrix into which this class ultimately refers. Note
  //         that M is an instane of matrix, not matrix_ref. Also, M may have
  //         the type const T, indicating that the all elements returned by
  //         this refrence class are also const.
  //    N -- The number of dimensions of this sub-matrix. N must be less than
  //         or equal to M::rank().
  template <typename M, std::size_t N>
    class matrix_ref
    {
      using this_type = matrix_ref<M, N>;
      using base_type = Remove_const<M>;
    public:
      using matrix_type         = M;
      using allocator_type      = typename base_type::allocator_type;
      using value_type          = typename base_type::value_type;
      using reference           = Matrix_reference<M>;
      using const_reference     = typename base_type::const_reference;
      using pointer             = Matrix_pointer<M>;
      using const_pointer       = typename base_type::const_pointer;
      using size_type           = typename base_type::size_type;
      using difference_type     = typename base_type::difference_type;

      using shape_type          = matrix_shape<size_type, N>;
      using row_reference       = matrix_impl::Row_type<M, N - 1>;
      using const_row_reference = matrix_impl::Row_type<const M, N - 1>;

      using iterator            = pointer;
      using const_iterator      = const_pointer;


      // Move and copy semantics
      // Are implicit. Moves are are the same copies, and copies are shallow.
      // This class does not implemnt value semantics.


      // Initialize the submatrix with the given shape.
      matrix_ref(matrix_type& m, const shape_type& s, pointer p)
        : mat(m), dims(s), ptr(p)
      { }


      // Returns a reference to the originating matrix.
      const matrix_type& base() const { return mat; }

      // Returns the allocator of the originating matrix.
      allocator_type get_allocator() const { return mat.get_allocator(); }

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
      row_reference       operator[](size_type n);
      const_row_reference operator[](size_type n) const;


      // Data access
      pointer       data()       { return ptr; }
      const_pointer data() const { return ptr; }


      // Matrix addition
      template <typename M2>
        Requires<Matrix<M2>(), matrix_ref&> operator+=(const M2& x);
      template <typename M2>
        Requires<Matrix<M2>(), matrix_ref&> operator-=(const M2& x);

      // Scalar addition
      matrix_ref& operator=(const value_type& value);
      matrix_ref& operator+=(const value_type& value);
      matrix_ref& operator-=(const value_type& value);

      // Scalar multiplication
      matrix_ref& operator*=(const value_type& value);
      matrix_ref& operator/=(const value_type& value);
      matrix_ref& operator%=(const value_type& value);

      // Iterators
      iterator begin() { return ptr; }
      iterator end()   { return ptr + dims.elements(); }

      const_iterator begin() const { return ptr; }
      const_iterator end() const   { return ptr + dims.elements(); }

    private:
      matrix_type mat;  // A reference to the originating matrix
      shape_type dims;  // The shape of the matrix
      pointer ptr;      // The 1st element in the matrix
    };


  // FIXME: Write type requirements.
  template <typename T, std::size_t N>
    template <typename... Args>
      inline auto
      matrix_ref<T, N>::operator()(Args... args) -> reference
      {
        size_type off = matrix_impl::offset(dims.sizes(), size_type(args)...);
        return *(ptr + off);
      }

  template <typename T, std::size_t N>
    template <typename... Args>
      inline auto
      matrix_ref<T, N>::operator()(Args... args) const -> const_reference
      {
        size_type off = matrix_impl::offset(dims.sizes(), size_type(args)...);
        return *(ptr + off);
      }

  template <typename T, std::size_t N>
    inline auto
    matrix_ref<T, N>::operator[](size_type n) -> row_reference
    {
      return matrix_impl::row(mat, *this, n);
    }

  template <typename T, std::size_t N>
    inline auto
    matrix_ref<T, N>::operator[](size_type n) const -> const_row_reference
    {
      return matrix_impl::row(mat, *this, n);
    }


  // Scalar addition
  template <typename T, std::size_t N>
    inline matrix_ref<T, N>& 
    matrix_ref<T, N>::operator=(const value_type& value)  
    { 
      assign_value(begin(), end(), value, assign<value_type>{}); 
      return *this;
    }
      
  template <typename T, std::size_t N>
    inline matrix_ref<T, N>& 
    matrix_ref<T, N>::operator+=(const value_type& value) 
    { 
      assign_value(begin(), end(), value, plus_assign<value_type>{}); 
      return *this;
    }
      
  template <typename T, std::size_t N>
    inline matrix_ref<T, N>& 
    matrix_ref<T, N>::operator-=(value_type const& value) 
    { 
      assign_value(begin(), end(), value, minus_assign<value_type>{}); 
      return *this;
    }
  

  // Scalar multiplication
  template <typename T, std::size_t N>
    inline matrix_ref<T, N>& 
    matrix_ref<T, N>::operator*=(value_type const& value) 
    { 
      assign_value(begin(), end(), value, multiplies_assign<value_type>{}); 
      return *this;
    }

  template <typename T, std::size_t N>
    inline matrix_ref<T, N>& 
    matrix_ref<T, N>::operator/=(value_type const& value) 
    { 
      assign_value(begin(), end(), value, divides_assign<value_type>{}); 
      return *this;
    }
      
  // This operation is only defined when T is a model of Euclidean domain.
  template <typename T, std::size_t N>
    inline matrix_ref<T, N>& 
    matrix_ref<T, N>::operator%=(value_type const& value) 
    { 
      assign_value(begin(), end(), value, modulus_assign<value_type>{}); 
      return *this;
    }



  //////////////////////////////////////////////////////////////////////////////
  // Matrix operations
  //
  // The following operations are defined for all Matrix types.



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
  template <typename T, std::size_t N, typename S>
    inline matrix<T, N, S>
    operator+(const matrix<T, N, S>& a, const matrix<T, N, S>& b)
    {
      assert(a.dim() == b.dim());
      matrix<T, N, S> result = a;
      return result += b;
    }

  // Addition for matrix and matrix_ref.
  //
  // FIXME: If neither a nor b is a matrix, then are we assuming that the
  // default allocator will be used? Technically yes, but is that appropriate.
  // It seems like we might actually need to bury the references "original" 
  // allocator in the reference class.
  template <typename T, std::size_t N, typename S>
    inline matrix<T, N, S>
    operator+(const matrix<T, N, S>& a, const matrix_ref<T, N>& b)
    {
      assert(a.dim() == b.dim());
      matrix<T, N, S> result = a;
      return result += b;
    }

  template <typename T, std::size_t N, typename S>
    inline matrix<T, N, S>
    operator+(const matrix_ref<T, N>& a, const matrix<T, N, S>& b)
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
