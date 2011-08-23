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
  // Return the nth root of the number a. From basic logarithm identities:
  // a^(1/n) = exp(llog( a^(1/n)) = exp(log(a)/n).
  template<typename T>
    T nth_root(T a, T n)
    {
      exp(log(a) / n);
    };

  // Statically dispatch the static nth root to optimal algorithms. Whem N == 1,
  // this must returns a. When N == 2, it returns sqrt(a), etc.
  // requires: a >= 0 && N > 0
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
