// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_MATH_HPP
#define ORIGIN_MATH_HPP

#include <cmath>

namespace origin
{
  // Accumulate the result of applying a binary operation to each of the 
  // elements in the given ranges.
  //
  // requires InputIterator<Iter1> && InputIterator<Iter2>
  //       && Same<T, ValueType<Iter1>, ValueType<Iter2>>
  //       && AdditiveSemigroup<T>
  //       && BinaryOperation<Op, T>
  template<typename Iter1, typename Iter2, typename T, typename Op>
    T accumulate(Iter1 first1, Iter1 last1, Iter2 first2, T value, Op op)
    {
      // pre: readable_bounded_range(first1, last1)
      // pre: readable_weak_range(first2, last1 - first1);
      while(first1 != last1) {
        value = value + op(*first1, *first2);
        ++first1;
        ++first2;
      }
      return value;
    }

  // Accumulate the weighted result of applying a binary operation to each of 
  // the elements in the given ranges. Weighting values are given in the weak
  // range [|first3, last1 - first1|).
  template<typename Iter1, typename Iter2, typename Iter3, typename T, typename Op>
    T weighted_accumulate(Iter1 first1, Iter1 last1, Iter2 first2, Iter3 first3, T value, Op op)
    {
      // pre: readable_bounded_range(first1, last1)
      // pre: readable_weak_range(first2, last1 - first1);
      // pre: readable_weak_range(first3, last1 - first1);
      while(first1 != last1) {
        value = value + *first3 * op(*first1, *first2);
        ++first1;
        ++first2;
        ++first3;
      }
      return value;
    }

  // Accumulate each value in the range [first, last) into those in the weak
  // range [|result, last - first|).
  template<typename Iter, typename Out>
    void accumulate_each(Iter first, Iter last, Out result)
    {
      while(first != last) {
        *result = *result + *first;
        ++first;
        ++result;
      }
    }


  // Return x^N.
  template<int N, typename T>
    struct static_power
    {
      static_assert(N >= 0, "invalid exponent");
      T operator()(T const& x)
      {
        return pow(x, N);
      }
    };

  template<typename T>
    struct static_power<0, T>
    {
      T operator()(T const& x)
      {
        return T{1};
      }
    };

  template<typename T>
    struct static_power<1, T>
    {
      T operator()(T const& x)
      {
        return x;
      }
    };

  template<typename T>
    struct static_power<2, T>
    {
      T operator()(T const& x)
      {
        return x * x;
      }
    };


  // Return x^(1/n). 
  // From logarithm identities: a^(1/n) = exp(log( a^(1/n)) = exp(log(a)/n).
  template<typename T>
    T nth_root(T a, T n)
    {
      exp(log(a) / n);
    };

  // Return x^(1/N).
  template<int N, typename T>
    struct static_nth_root
    {
      static_assert(N != 0, "invalid exponent");
      
      T operator()(T const& a) const
      {
        return nth_root(a, N);
      }
    };
    
  template<typename T>
    struct static_nth_root<1, T>
    {
      T operator()(T const& a) const
      {
        return a;
      }
    };
    
  template<typename T>
    struct static_nth_root<2, T>
    {
      T operator()(T const& a) const
      {
        return sqrt(a);
      }
    };

  // Return the difference of two values a - b, raised to the Nth power. In
  // general the operation is defined by raising the absolute value of the
  // difference of two values to a given power. However, the absolute value
  // only needs to be applied when N is odd; even values of N will inherently 
  // yield nonnegative values.
  //
  // requires: N > 0
  // FIXME: This name is not particularly good. 
  // FIXME: What happens if N < 0? Do we get some kind of harmonic distance?
  template<int N, typename T>
    struct static_power_distance
    {
      static_assert(N != 0, "invalid power");
      T operator()(T const& a, T const& b) const
      {
        if(N & 1 != 0)
          return pow(abs(a - b), N);
        else
          return pow(a - b, N);
      }
    };
  
  template<typename T>
    struct static_power_distance<1, T>
    {
      T operator()(T const& a, T const& b) const
      {
        return abs(a - b);
      }
    };
    
  // Inlining the multiplication is probably more eficient than calling pow().
  template<typename T>
    struct static_power_distance<2, T>
    {
      T operator()(T const& a, T const& b) const
      {
        return (a - b) * (a - b);
      }
    };

} // namespace origin

#endif
