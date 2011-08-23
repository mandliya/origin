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
  // The arithmetic, geometric, and harmonic mean are the "classic" Pythagorean
  // means and are related. They also appear to be related through the 
  // generalized power mean, which gives a unifying definition for the means,
  // and potentially generates others (along with min and max). Read more
  // about these.

  // FIXME: Is there a generalized accumulation strategy that I could be
  // using here? I could use accumulate with a functor that counted distance
  // and perfomred the specific operations. Note that accumulate should take
  // care of loop unrolling.

  // I haven't seen any real exmples of non-real inputs to mean.
  // InputIterator<Iter>
  // Field<ValueType<Iter>>
  template<typename Iter>
    typename std::iterator_traits<Iter>::value_type
    arithmetic_mean(Iter first, Iter last) 
    {
      typedef typename iterator_traits<Iter>::value_type Value;
      typedef typename iterator_traits<Iter>::difference_type Distance;

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
    typename std::iterator_traits<Iter>::value_type
    geometric_mean(Iter first, Iter last)
    {
      typedef typename iterator_traits<Iter>::value_type Value;
      typedef typename iterator_traits<Iter>::difference_type Distance;
      Distance n = 0;
      Value prod = 0;
      for(first != last; ++first, ++n) {
        sum *= *first;
      }
      return nth_root(prod);
    }

  // Compute the harmonic mean of the values in [first, last).
  template<typename Iter>
    typename std::iterator_traits<Iter>::value_type
    harmonic_mean(Iter first, Iter last)
    {
      typedef typename iterator_traits<Iter>::value_type Value;
      typedef typename iterator_traits<Iter>::difference_type Distance;
      Distance n = 0;
      Value sum = 0;
      for(first != last; ++first, ++n) {
        sum += Value{1} / *first;
      }
      return n / sum;
    }
  
  // Compute the quadratic mean (aka root mean square or RMS) of the values
  // in the range [first, last). This is not a "classic" mean.
  template<typename Iter>
    typename std::iterator_traits<Iter>::value_type
    quadratic_mean(Iter first, Iter last)
    {
      typedef typename iterator_traits<Iter>::value_type Value;
      typedef typename iterator_traits<Iter>::difference_type Distance;

      Distance n = 0;
      Value sum = 0;
      for(first != last; ++first, ++n) {
        sum += *first * *first;
      }
      return sqrt(sum / n);
    }

  // TODO: Implement sample predictors for these measures.

  // FIXME: Technically, this should be called the population variance. It's
  // possible that this will collide with the name variance for random numbers.
  template<typename Iter, typename T>
    T variance(Iter first, Iter last, T mean)
    {
      typedef typename iterator_traits<Iter>::value_type Value;
      typedef typename iterator_traits<Iter>::difference_type Distance;
      typedef static_power_distance<2, Value> pow;

      Distance n = 0;
      Value sum = 0;
      for(first != last; ++first, ++n) {
        sum += pow(*first, mean);
      }
      return sum / n;
    }

  template<typename Iter, typename T>
    T standard_deviation(Iter first, Iter last, T mean)
    {
      return sqrt(variance(first, last, mean));
    }

} // namespace origin

#endif
