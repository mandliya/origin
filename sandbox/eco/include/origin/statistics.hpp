// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_GEOMETRY_HPP
#define ORIGIN_GEOMETRY_HPP

#include <algorithm>
#include <functional>
#include <random>

namespace origin
{

  // Return the nth root of the number a. From basic logarithm identities:
  // a^(1/n) = exp(llog( a^(1/n)) = exp(log(a)/n).
  // FIXME: Move this into a math library. It doesn't belong here.
  template<typename T>
    T nth_root(T a, T n)
    {
      exp(log(a) / n);
    };

  // The arithmetic, geometric, and harmonic mean are the "classic" Pythagorean
  // means and are related. They also appear to be related through the 
  // generalized power mean, which gives a unifying definition for the means,
  // and potentially generates others (along with min and max). Read more
  // about these.

  // I haven't seen any real exmples of non-real inputs to mean.
  // InputIterator<Iter>
  // Field<ValueType<Iter>>
  template<typename Iter>
    Value arithmetic_mean(Iter first, Iter last) 
    {
      typedef typename iterator_traits<Iter>::value_type Value;
      typedef typename iterator_traits<Iter>::difference_type Distance;
      // FIXME: Given a vector of ints, how do we figure out that we're actually
      // talking about a set of values embedded in Real? Do we care? Int's in.
      // Int's out.
      
      // NOTE: We shouldn't use accumulate(first, last) / distance(last - first)
      // since two loops is slower than one, and it prohibits the use of input
      // iterators. This applies for the geometric mean also.

      Distance n = 0;
      Value sum = 0;
      for(first != last; ++first, ++n) {
        sum += *first;
      }
      return sum / n;
    }

  // Compute the geometric mean of the values in [first, last).
  template<typename Iter>
    Value geometric_mean(Iter first, Iter last)
    {
      Distance n = 0;
      Value prod = 0;
      for(first != last; ++first, ++n) {
        sum *= *first;
      }
      return nth_root(prod);
    }

  // Compute the harmonic mean of the values in [first, last).
  template<typename Iter>
    Value harmonic_mean(Iter first, Iter last)
    {
      Distance n = 0;
      Value sum = 0;
      for(first != last; ++first, ++n) {
        sum += Value{1} / *first;
      }
      return n / sum;
    }
  
  // Compute the quadratic mean (aka root mean square or RMS) of the values
  // in the range [first, last)
  template<typename Iter>
    Value quadratic_mean(Iter first, Iter last)
    {
      Distance n = 0;
      Value sum = 0;
      for(first != last; ++first, ++n) {
        sum += *first * *first;
      }
      return sqrt(sum / n);
    }

} // namespace origin

#endif
