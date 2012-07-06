
// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_MATH_MATRIX_HPP
#  error Do not include this file directly. Include matrix/matrix.hpp.
#endif

namespace origin
{
  //////////////////////////////////////////////////////////////////////////////
  // Matrix Reference
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
      using reference           = Reference_of<M>;
      using const_reference     = typename base_type::const_reference;
      using pointer             = Pointer_of<M>;
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


  // FIXME: Implement matrix addition!


  // Scalar addition
  template <typename T, std::size_t N>
    inline matrix_ref<T, N>& 
    matrix_ref<T, N>::operator=(const value_type& value)  
    { 
      matrix_impl::assign_value(
        begin(), end(), value, matrix_impl::assign<value_type>{}); 
      return *this;
    }
      
  template <typename T, std::size_t N>
    inline matrix_ref<T, N>& 
    matrix_ref<T, N>::operator+=(const value_type& value) 
    { 
      matrix_impl::assign_value(
        begin(), end(), value, matrix_impl::plus_assign<value_type>{}); 
      return *this;
    }
      
  template <typename T, std::size_t N>
    inline matrix_ref<T, N>& 
    matrix_ref<T, N>::operator-=(value_type const& value) 
    { 
      matrix_impl::assign_value(
        begin(), end(), value, matrix_impl::minus_assign<value_type>{}); 
      return *this;
    }
  

  // Scalar multiplication
  template <typename T, std::size_t N>
    inline matrix_ref<T, N>& 
    matrix_ref<T, N>::operator*=(value_type const& value) 
    { 
      matrix_impl::assign_value(
        begin(), end(), value, matrix_impl::multiplies_assign<value_type>{}); 
      return *this;
    }

  template <typename T, std::size_t N>
    inline matrix_ref<T, N>& 
    matrix_ref<T, N>::operator/=(value_type const& value) 
    { 
      matrix_impl::assign_value(
        begin(), end(), value, matrix_impl::divides_assign<value_type>{}); 
      return *this;
    }
      
  // This operation is only defined when T is a model of Euclidean domain.
  template <typename T, std::size_t N>
    inline matrix_ref<T, N>& 
    matrix_ref<T, N>::operator%=(value_type const& value) 
    { 
      matrix_impl::assign_value(
        begin(), end(), value, matrix_impl::modulus_assign<value_type>{}); 
      return *this;
    }

} // namespace origin

