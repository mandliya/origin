// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_MATRIX_SLICE_HPP
#define ORIGIN_MATRIX_SLICE_HPP

namespace origin
{
  //////////////////////////////////////////////////////////////////////////////
  // Matrix Slice
  //
  // A matrix slice provides access to a slice of an N-dimensional matrix.
  //
  // Note that if T is const, then the resulting slice is a const slice. That
  // is, changes cannot be made to the matrix through the returned reference.
  template <typename T, std::size_t N>
    class matrix_slice;


  //////////////////////////////////////////////////////////////////////////////
  // Matrix Slice (1D)
  //
  // A 1D matrix slice is a vector in a 2D matrix.
  template <typename T>
    class matrix_slice<T, 1>
    {
    public:
      using value_type      = T;
      using reference       = T&;
      using const_reference = const T&;
      using pointer         = T*;
      using const_pointer   = const T*;
      using size_type       = std::size_t;
      using difference_type = std::ptrdiff_t;

      matrix_slice(pointer f, pointer l, size_type s)
        : first(f), last(l), stride(s)
      { }

      size_type size() const { return (last - first) / stride; }

      // Element access
      reference       operator[](size_type n)       { return get(n); }
      const_reference operator[](size_type n) const { return get(n); }

      reference       operator()(size_type n)       { return get(n); }
      const_reference operator()(size_type n) const { return get(n); }

    private:
      reference get(size_type n)
      {
        assert(first + n * stride < last);
        return *(first + n * stride);
      }

      const_reference get(size_type n) const
      {
        assert(first + n * stride < last);
        return *(first + n * stride);
      }

    private:
      pointer first;
      pointer last;
      size_type stride;
    };


  // Output streamable
  // TODO: This implementation is virtually identical to the matrix<T, 1, S>
  // implementation. Concepts would be nice, here...
  template <typename C, typename T, typename U>
    inline std::basic_ostream<C, T>&
    operator<<(std::basic_ostream<C, T>& os, const matrix_slice<U, 1>& m)
    {
      os << '[';
      typename matrix<T, 1>::size_type i = 0;
      while (i < m.size() - 1) {
        os << m[i] << ' ';
        ++i;
      }
      os << m[i] << ']';
      return os;    
    }

} // namespace origin

#endif
