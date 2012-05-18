// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_ALGORITHM_HPP
#define ORIGIN_ALGORITHM_HPP

#include <origin/concepts.hpp>
#include <origin/memory/allocation.hpp>

namespace origin
{

  template <typename I1, typename I2>
    bool lexicographical_equal(I1 first1, I1 last1, I2 first2)
    {
      while (first1 != last1) {
        if (*first1 != *first2)
          return false;
        ++first1;
        ++first2;
      }
      return true;
    }


  template <typename I1, typename I2>
    bool lexicographical_less(I1 first1, I1 last1, I2 first2, I2 last2)
    {
      while (first1 != last1) {
        if (*first1 < *first2)
          return true;
        if (*first2 < *first1)
          return false;
        ++first1;
        ++first2;
      }
      return first1 == last1 && first2 != last2;
    }



  // Copy

  template <typename I, typename O>
    inline void copy_step(I& i, O& o)
    {
      *o = *i;
      ++o;
      ++i;
    }

  template <typename I, typename O>
    inline O copy(I first, I last, O result)
    {
      while (first != last)
        copy_step(first, result);
      return result;

    }

  template <typename T>
    inline auto copy(const T* first, const T* last, T* result) 
      -> Requires<Trivial<T>(), T*>
    {
      std::size_t n = last - first;
      std::memcpy(result, first, n * sizeof(T));
      return result + n;
    }  

  template <typename T>
    inline auto copy(T* first, T* last, T* result) 
      -> Requires<Trivial<T>(), T*> 
    {
      std::size_t n = last - first;
      std::memcpy(result, first, n * sizeof(T));
      return result + n;
    }  



  // Copy n

  template <typename I, typename O>
    inline O copy_n(I first, std::ptrdiff_t n, O result)
    {
      while (n != 0) {
        copy_step(first, result);
        --n;
      }
    }

  template <typename T>
    inline auto copy_n(const T* first, std::ptrdiff_t n, T* result) 
      -> Requires<Trivial<T>(), T*>
    {
      std::memcpy(result, first, n * sizeof(T));
      return result + n;
    }  

  template <typename T>
    inline auto copy_n(T* first, std::ptrdiff_t n, T* result) 
      -> Requires<Trivial<T>(), T*> 
    {
      std::memcpy(result, first, n * sizeof(T));
      return result + n;
    }  



  // Move

  template <typename I, typename O>
    inline void move_step(I& i, O& o)
    {
      *o = std::move(*i);
      ++i;
      ++o;
    }

  template <typename I, typename O>
    inline O move(I first, I last, O result)
    {
      while (first != last)
        move_step(first, result);
      return result;
    }

  template <typename T>
    inline auto move(const T* first, const T* last, T* result)
      -> Requires<Trivial<T>(), T*>
    {
      std::ptrdiff_t n = last - first;
      std::memmove(result, first, n * sizeof(T));
      return result + n;
    }

  template <typename T>
    inline auto move(T* first, T* last, T* result)
      -> Requires<Trivial<T>(), T*>
    {
      std::ptrdiff_t n = last - first;
      std::memmove(result, first, n * sizeof(T));
      return result + n;
    }



  // Move backward

  // FIXME: Refactor to use backwards step.
  template <typename I, typename O>
    inline O move_backward(I first, I last, O result)
    {
      while (first != last) {
        --last;
        --result;
        *result = std::move(*last);
      }
      return result;
    }

  template <typename T>
    inline auto move_backward(const T* first, const T* last, T* result)
      -> Requires<Trivial<T>(), T*>
    {
      std::ptrdiff_t n = last - first;
      result -= n;
      std::memmove(result, first, n * sizeof(T));
      return result;
    }

  template <typename T>
    inline auto move_backward(T* first, T* last, T* result)
      -> Requires<Trivial<T>(), T*>
    {
      std::ptrdiff_t n = last - first;
      result -= n;
      std::memmove(result, first, n * sizeof(T));
      return result;
    }



  // Fill

  template <typename I, typename T>
    inline void fill_step(I& i, const T& value)
    {
      *i = value;
      ++i;
    }

  template <typename I, typename T>
    inline void fill(I first, I last, const T& value)
    {
      while (first != last)
        fill_step(first, value);
    }

  template <typename T>
    inline auto fill(T* first, T* last, const T& value)
      -> Requires<Trivial<T>() && sizeof(T) == 1, void>
    {
      std::memset(first, value, last - first);
    }



  // Generate

  template <typename I, typename Gen>
    inline void generate_step(I& i, Gen gen)
    {
      *i = gen();
      ++i;
    }

  template <typename I, typename Gen>
    inline Gen generate(I first, I last, Gen gen)
    {
      while (first != last)
        generate_step(first, gen);
      return gen;
    }
}

#endif
