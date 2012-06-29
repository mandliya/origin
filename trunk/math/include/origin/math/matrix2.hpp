// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_MATRIX2_HPP
#define ORIGIN_MATRIX2_HPP


namespace origin
{

  //////////////////////////////////////////////////////////////////////////////
  // Matrix (2D)
  //
  // The 2d matrix specialization implements a matrix.
  template<typename T, typename Storage>
    class matrix<T, 2, Storage>
    {
      using base_type = Storage;
    public:
      using allocator_type  = typename base_type::allocator_type;
      using value_type      = T;
      using reference       = T&;
      using const_reference = const T&;
      using pointer         = typename base_type::pointer;
      using const_pointer   = typename base_type::const_pointer;
      using size_type       = typename base_type::size_type;
      using difference_type = typename base_type::difference_type;

      // TODO: Make this an actual class that allows comparison to an 
      // initializer list (i.e., m.dim() == {2, 3}).
      using dimension_type  = std::pair<size_type, size_type>;

      using slice           = matrix_slice<T, 1>;
      using const_slice     = matrix_slice<const T, 1>;

      // TODO: We should be able to make a submatrix slice. What would the
      // the name of that type be? How do we do it?

      using iterator        = typename base_type::iterator;
      using const_iterator  = typename base_type::const_iterator;

      // Constructs a vector with 0 elements.
      matrix(const allocator_type& alloc = {})
        : base(alloc), dims()
      { }
      

      // Move semantics
      matrix(matrix&& x)
        : base(std::move(x.base)), dims(x.dims())
      { }
      
      matrix& operator=(matrix&& x)
      {
        base = std::move(x.base);
        return base;
      }


      // Copy semantics
      matrix(matrix const& x)
        : base(x.base), dims(x.dims)
      { }
      
      matrix& operator=(matrix const& x) 
      { 
        base = x.base;
        return *this;
      }


      // Fill initialziation
      // Construct a vector with the given dimensions (as a pair of numbers)
      // with each element initialized to the given value.
      explicit matrix(const dimension_type& dim, 
                      const value_type& value = {}, 
                      const allocator_type& alloc = {})
        : base(dim.first * dim.second, value, alloc), dims(dim)
      { }
      

      // TODO: Write range constructors, but we have to be aware of the fact
      // that we're not just initializing a sequence. We're initializing a
      // 2D object. Could ranges of ranges, for example.


      // Returns the matrix's allocator.
      allocator_type get_allocator() const { return base.get_allocator(); }
      
      // Returns the size of the vector.
      size_type size() const { return base.size(); }
      
      // Returns the dimensions of the matrix.
      dimension_type dim() const { return dims; }
      size_type rows() const { return dims.first; }
      size_type cols() const { return dims.second; }

      // Data access
      // Returns a pointer to the underlying data.
      pointer       data()       { return base.data(); }
      const_pointer data() const { return base.data(); }
      

      // Element access
      // Returns the nth element of the vector.
      slice       operator[](size_type n)       { return row(n); }
      const_slice operator[](size_type n) const { return row(n); }
      
      reference       operator()(size_type n)       { return get(n); }
      const_reference operator()(size_type n) const { return get(n); }

      // Row access
      slice       row(size_type n);
      const_slice row(size_type n) const;

      // Column access
      slice       col(size_type n);
      const_slice col(size_type n) const;
      
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


      // Operations
      void swap(matrix& x) { base.swap(x); }

      // Iterators
      iterator begin() { return base.begin(); }
      iterator end()   { return base.end(); }

      const_iterator begin() const { return base.begin(); }
      const_iterator end() const   { return base.end(); }

      // TODO: Provide iterator and range access to rows and columns. This
      // is going to require a some thought. It would be easy to parameterize
      // the iterator over this matrix type, but we'd run into some problems
      // with incomplete types (couldn't use size_type, etc.). An alternative
      // would be to separate the matrix "core" from the interface and then
      // parameterize the iterator over that core.
      
    private:
      reference get(size_type n)
      {
        assert(n < size());
        return data()[n];
      }

      const_reference get(size_type n) const
      {
        assert(n < size());
        return data()[n];
      }

    private:
      base_type base;
      dimension_type dims;
    };
    

  template <typename T, typename S>
    inline auto
    matrix<T, 2, S>::row(size_type n) -> slice
    {
      assert(n < rows());
      return {data() + n * cols(), data() + (n + 1) * cols(), 1};
    }

  template <typename T, typename S>
    inline auto
    matrix<T, 2, S>::row(size_type n) const -> const_slice
    {
      assert(n < rows());
      return {data() + n * cols(), data() + (n + 1) * cols(), 1};
    }

  template <typename T, typename S>
    inline auto
    matrix<T, 2, S>::col(size_type n) -> slice
    {
      assert(n < cols());
      return {data() + n, data() + size() + n, cols()};
    }

  template <typename T, typename S>
    inline auto
    matrix<T, 2, S>::col(size_type n) const -> const_slice
    {
      assert(n < cols());
      return {data() + n, data() + size() + n, cols()};
    }

  template <typename T, typename S>
    inline matrix<T, 2, S>&
    matrix<T, 2, S>::operator+=(const matrix& x)
    {
      assign_elements(begin(), end(), x.begin(), plus_assign<value_type>{});
      return *this;
    }

  template <typename T, typename S>
    inline matrix<T, 2, S>&
    matrix<T, 2, S>::operator-=(const matrix& x)
    {
      assign_elements(begin(), end(), x.begin(), minus_assign<value_type>{});
      return *this;
    }

  template <typename T, typename S>
    inline matrix<T, 2, S>& 
    matrix<T, 2, S>::operator=(const value_type& value)  
    { 
      assign_value(begin(), end(), value, assign<value_type>{}); 
      return *this;
    }
      
  template <typename T, typename S>
    inline matrix<T, 2, S>& 
    matrix<T, 2, S>::operator+=(const value_type& value) 
    { 
      assign_value(begin(), end(), value, plus_assign<value_type>{}); 
      return *this;
    }
      
  template <typename T, typename S>
    inline matrix<T, 2, S>& 
    matrix<T, 2, S>::operator-=(value_type const& value) 
    { 
      assign_value(begin(), end(), value, minus_assign<value_type>{}); 
      return *this;
    }
  
  template <typename T, typename S>    
    inline matrix<T, 2, S>& 
    matrix<T, 2, S>::operator*=(value_type const& value) 
    { 
      assign_value(begin(), end(), value, multiplies_assign<value_type>{}); 
      return *this;
    }

  template <typename T, typename S>      
    inline matrix<T, 2, S>& 
    matrix<T, 2, S>::operator/=(value_type const& value) 
    { 
      assign_value(begin(), end(), value, divides_assign<value_type>{}); 
      return *this;
    }
      
  template <typename T, typename S>      
    inline matrix<T, 2, S>& 
    matrix<T, 2, S>::operator%=(value_type const& value) 
    { 
      assign_value(begin(), end(), value, modulus_assign<value_type>{}); 
      return *this;
    }


  // Matrix multiplication
  // Two matrices can be multiplied when...

  template <typename T, typename S>
    inline matrix<T, 2, S>
    operator*(const matrix<T, 2, S>& a, const matrix<T, 2, S>& b)
    {
      return {};
    }


  // Output streamable
  // TODO: Can this be made general? Maybe...
  template <typename C, typename T, typename U, typename S>
    std::basic_ostream<C, T>& 
    operator<<(std::basic_ostream<C, T>& os, const matrix<U, 2, S>& m)
    {
      os << '[';
      typename matrix<T, 2, S>::size_type i = 0;
      while (i < m.rows() - 1) {
        os << m[i] << ',';
        ++i;
      }
      os << m[i] << ']';
      return os;
    }

} // namespace origin

#endif
