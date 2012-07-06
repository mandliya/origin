// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_MATRIX_SHAPE_HPP
#define ORIGIN_MATRIX_SHAPE_HPP

#include <algorithm>

#include <origin/type/traits.hpp>

namespace origin
{
  //////////////////////////////////////////////////////////////////////////////
  // Matrix Shape
  //
  // The shape class describes the shape of an array: its number of dimensions 
  // (or order) and the number of elements (extent) contained in each dimension.
  // The order of the shape is specified by the template parameter N.
  //
  // NOTE: The traditonal C/C++ name for number of dimensions is traditionally
  // called its rank. However, that name has special meaning in the context
  // of linear algebra, so we use the name "order" instead.
  template <typename T, std::size_t N>
    class matrix_shape
    {
      static_assert(Integer<T>(), "");
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
      // initializer list. The size of the list must be the same as the order
      // of the shape (N).
      matrix_shape(std::initializer_list<value_type> list)
      {
        assert(list.size() == N);
        std::copy(list.begin(), list.end(), exts);
        count();
      }

      // Construct a matrix over the extents given in the range [first, last).
      // The size of the [first, last) must be the same as the order of the
      // shape (N).
      template <typename I>
        matrix_shape(I first, I last)
        {
          assert(std::distance(first, last) == N);
          std::copy(first, last, exts);
          count();
        }


      // Returns the order of the shape.
      static constexpr value_type order() { return N; }

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
  // Two shapes compare equal when they have the same order and extents.
  //
  // Note that we do not compare sizes since they are computed from the extents
  // of the shape (that array exists only as an optimization).
  //
  // TODO: This definition not allow us to compare shapes of different order.
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

} // namespace origin


#endif
