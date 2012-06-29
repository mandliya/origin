// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_MATRIX1_HPP
#define ORIGIN_MATRIX1_HPP


namespace origin
{
  //////////////////////////////////////////////////////////////////////////////
  // Matrix (1D)
  //
  // The 1d matrix specialization implements a vector.
  template<typename T, typename Storage>
    class matrix<T, 1, Storage>
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
      using dimension_type  = size_type;

      using iterator        = typename base_type::iterator;
      using const_iterator  = typename base_type::const_iterator;

      // Constructs a vector with 0 elements.
      matrix(const allocator_type& alloc = {})
        : base(alloc)
      { }
      

      // Move semantics
      matrix(matrix&& x)
        : base(std::move(x.base))
      { }
      
      matrix& operator=(matrix&& x)
      {
        base = std::move(x.base);
        return base;
      }


      // Copy semantics
      matrix(matrix const& x)
        : base(x.base)
      { }
      
      matrix& operator=(matrix const& x) 
      { 
        base = x.base;
        return *this;
      }


      // Fill initialziation
      // Create an n-element vector, with each element initialized to the
      // given value or the default value if not given.
      explicit matrix(size_type n, 
                      const value_type& value = {}, 
                      const allocator_type& alloc = {})
        : base(n, value, alloc)
      { }
      

      // Range initialization
      template <typename I>
        matrix(I first, I last, const allocator_type& alloc = {})
          : base(first, last, alloc)
        { }

      // FIXME: Write a range constructor.
      
      matrix(std::initializer_list<value_type> list, 
             const allocator_type& alloc = {})
        : base(list, alloc)
      { }


      // Returns the matrix's allocator.
      allocator_type get_allocator() const { return base.get_allocator(); }
      
      // Returns the size of the vector.
      size_type size() const { return base.size(); }
      
      // Returns the dimensions of the vector. This is equivalent to its size.
      size_type dim() const { return base.size(); }

      // Data access
      // Returns a pointer to the underlying data.
      pointer       data()       { return base.data(); }
      const_pointer data() const { return base.data(); }
      

      // Element access
      // Returns the nth element of the vector.
      reference       operator[](size_type n)       { return get(n); }
      const_reference operator[](size_type n) const { return get(n); }
      
      reference       operator()(size_type n)       { return get(n); }
      const_reference operator()(size_type n) const { return get(n); }
      
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
    };
    

  template <typename T, typename S>
    inline matrix<T, 1, S>&
    matrix<T, 1, S>::operator+=(const matrix& x)
    {
      assign_elements(begin(), end(), x.begin(), plus_assign<value_type>{});
      return *this;
    }

  template <typename T, typename S>
    inline matrix<T, 1, S>&
    matrix<T, 1, S>::operator-=(const matrix& x)
    {
      assign_elements(begin(), end(), x.begin(), minus_assign<value_type>{});
      return *this;
    }

  template <typename T, typename S>
    inline matrix<T, 1, S>& 
    matrix<T, 1, S>::operator=(const value_type& value)  
    { 
      assign_value(begin(), end(), value, assign<value_type>{}); 
      return *this;
    }
      
  template <typename T, typename S>
    inline matrix<T, 1, S>& 
    matrix<T, 1, S>::operator+=(const value_type& value) 
    { 
      assign_value(begin(), end(), value, plus_assign<value_type>{}); 
      return *this;
    }
      
  template <typename T, typename S>
    inline matrix<T, 1, S>& 
    matrix<T, 1, S>::operator-=(value_type const& value) 
    { 
      assign_value(begin(), end(), value, minus_assign<value_type>{}); 
      return *this;
    }
  
  template <typename T, typename S>    
    inline matrix<T, 1, S>& 
    matrix<T, 1, S>::operator*=(value_type const& value) 
    { 
      assign_value(begin(), end(), value, multiplies_assign<value_type>{}); 
      return *this;
    }

  template <typename T, typename S>      
    inline matrix<T, 1, S>& 
    matrix<T, 1, S>::operator/=(value_type const& value) 
    { 
      assign_value(begin(), end(), value, divides_assign<value_type>{}); 
      return *this;
    }
      
  template <typename T, typename S>      
    inline matrix<T, 1, S>& 
    matrix<T, 1, S>::operator%=(value_type const& value) 
    { 
      assign_value(begin(), end(), value, modulus_assign<value_type>{}); 
      return *this;
    }



  // Output streamable
  // TODO: Can this be made general? Probably... Look at the 2D implementaiton.
  // It's virtually identical except in the use of m.size() vs. m.rows().
  template <typename C, typename T, typename U, typename S>
    std::basic_ostream<C, T>& 
    operator<<(std::basic_ostream<C, T>& os, const matrix<U, 1, S>& m)
    {
      os << '[';
      typename matrix<T, 1, S>::size_type i = 0;
      while (i < m.size() - 1) {
        os << m[i] << ' ';
        ++i;
      }
      os << m[i] << ']';
      return os;
    }

} // namespace origin

#endif
