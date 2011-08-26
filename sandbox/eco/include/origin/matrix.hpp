// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_MATRIX_HPP
#define ORIGIN_MATRIX_HPP

#include <cassert>

#include <origin/dynarray.hpp>

namespace origin
{
  // A matrix is an N-dimensional array.
  //
  // The underlying storage is dynamically allocatoed.
  template<typename T, int N>  class matrix;
  
  // A 1d matrix is a vector.
  template<typename T>
    class matrix<T, 1>
    {
      typedef dynarray<T> array_type;
    public:
      typedef typename array_type::value_type value_type; // scalar type
      typedef typename array_type::reference reference;
      typedef typename array_type::size_type size_type;
      
      typedef typename array_type::iterator iterator;
      typedef typename array_type::const_iterator const_iterator;
      
      matrix()
        : data_{ }
      { }
      
      matrix(size_type n, value_type const& value = value_type{})
        : data_(n, value)
      { }
      
      template<typename Iter>
        matrix(Iter first, Iter last)
          : data_(first, last)
        { }
        
      matrix(std::initializer_list<value_type> list)
        : data_(list)
      { }
      
      size_type size() const { return data_.size(); }
      
      iterator begin() { return data_.begin(); }
      iterator end()   { return data_.end(); }
      const_iterator begin() const { return data_.begin(); }
      const_iterator end() const   { return data_.end(); }

      // Matrix addition
      matrix& operator+=(matrix const& x);
      matrix& operator-=(matrix const& x);
      
      // Scalar multiplication
      matrix& operator*=(value_type const& x);
      matrix& operator/=(value_type const& x);
      
    private:
      array_type data_;
    };
    
  template<typename T>
    auto matrix<T, 1>::operator+=(matrix const& x) -> matrix&
    {
      assert(( size() == x.size() ));
      auto i = begin();
      auto j = x.begin();
      for( ; i != end(); ++i, ++j) {
        *i += *j;
      }
      return *this;
    }

  template<typename T>
    auto matrix<T, 1>::operator-=(matrix const& x) -> matrix&
    {
      assert(( size() == x.size() ));
      auto i = begin();
      auto j = x.begin();
      for( ; i != end(); ++i, ++j) {
        *i -= *j;
      }
      return *this;
    }
 
  template<typename T>
    auto matrix<T, 1>::operator*=(value_type const& s) -> matrix&
    {
      for(iterator i = begin(); i != end(); ++i)
        *i *= s;
      return *this;
    }

  template<typename T>
    auto matrix<T, 1>::operator/=(value_type const& s) -> matrix&
    {
      assert(( s != value_type{0} ));
      for(iterator i = begin(); i != end(); ++i)
        *i /= s;
      return *this;
    }

  // Generic matrix operations
  // FIXME: Generalize for cases two types T and U where T and U are common?
  template<typename T, int N>
    matrix<T, N> operator+(matrix<T, N> const& a, matrix<T, N> const& b)
    {
      matrix<T, N> result{a};
      return std::move(a += b);
    }

  template<typename T, int N>
    matrix<T, N> operator-(matrix<T, N> const& a, matrix<T, N> const& b)
    {
      matrix<T, N> result{a};
      return std::move(a -= b);
    }

  template<typename T, int N>
    matrix<T, N> operator*(matrix<T, N> const& a, T s)
    {
      matrix<T, N> result{a};
      return std::move(a *= s);
    }

  // FIXME: Why can't use T as a second parameter?
  template<typename T, int N>
    matrix<T, N> operator/(matrix<T, N> const& a, typename matrix<T, N>::value_type s)
    {
      matrix<T, N> result{a};
      return std::move(result /= s);
    }

} // namespace origin

#endif
