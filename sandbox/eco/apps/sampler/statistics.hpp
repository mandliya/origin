// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_STATISTICS_HPP
#define ORIGIN_STATISTICS_HPP

#include <algorithm>
#include <functional>

#include "math.hpp"

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
      typedef typename std::iterator_traits<Iter>::value_type Value;
      typedef typename std::iterator_traits<Iter>::difference_type Distance;

      // NOTE: We shouldn't use accumulate(first, last) / distance(last - first)
      // since two loops is slower than one, and it prohibits the use of input
      // iterators. This applies for the geometric mean also.
      Distance n = 0;
      Value sum = 0;
      while(first != last) {
        sum += *first;
        ++first;
        ++n;
      }
      return sum / n;
    }

  // Compute the geometric mean of the values in [first, last).
  // NOTE: This can also be computed as the sum of logarithms of each element
  // and then exponentiating the result.
  // TODO: If the the range [first, last) denotes measures of a growth rate,
  // we can compute the result in constant time as (an / a0) ^ (1/n) where 0 
  // and n are the 0th and nth elements in [first, last). 
  template<typename Iter>
    typename std::iterator_traits<Iter>::value_type
    geometric_mean(Iter first, Iter last)
    {
      // pre: for(Iter i : range(first, last)) *i >= 0;
      typedef typename std::iterator_traits<Iter>::value_type Value;
      typedef typename std::iterator_traits<Iter>::difference_type Distance;
      Distance n = 0;
      Value prod = 0;
      while(first != last) {
        prod *= *first;
        ++first;
        ++n;
      }
      return nth_root(prod);
    }

  // Compute the harmonic mean of the values in [first, last).
  template<typename Iter>
    typename std::iterator_traits<Iter>::value_type
    harmonic_mean(Iter first, Iter last)
    {
      typedef typename std::iterator_traits<Iter>::value_type Value;
      typedef typename std::iterator_traits<Iter>::difference_type Distance;
      Distance n = 0;
      Value sum = 0;
      while(first != last) {
        sum += Value(1) / *first;
        ++first;
        --n;
      }
      return n / sum;
    }
  
  // Compute the quadratic mean (aka root mean square or RMS) of the values
  // in the range [first, last). This is not a "classic" mean.
  template<typename Iter>
    typename std::iterator_traits<Iter>::value_type
    quadratic_mean(Iter first, Iter last)
    {
      typedef typename std::iterator_traits<Iter>::value_type Value;
      typedef typename std::iterator_traits<Iter>::difference_type Distance;

      Distance n = 0;
      Value sum = 0;
      while(first != last) {
        sum += *first * *first;
        ++first;
        --n;
      }
      return sqrt(sum / n);
    }

  // Compute the sum of squares. This is the sum of the difference between
  // each element in the sample, population, or distribution and the mean.
  // Return a pair containing the result and the number of elements in the
  // data set.
  template<typename Iter, typename T>
    std::pair<T, typename std::iterator_traits<Iter>::difference_type>
    sum_of_squares(Iter first, Iter last, T mean)
    {
      typedef typename std::iterator_traits<Iter>::value_type Value;
      typedef typename std::iterator_traits<Iter>::difference_type Distance;
      typedef static_power_distance<2, Value> Diff;

      Diff diff;
      Distance n = 0;
      Value sum = 0;
      while(first != last) {
        sum += diff(*first, mean);
        ++first;
        ++n;
      }
      return std::make_pair(sum, n);
    }

  // TODO: Real population variance is defined over a probability distribution,
  // It does not include a 1/n. More importantly, it doesn't generate a result,
  // it generates a function. That is, given a 

  // Compute the population variance
  template<typename Iter, typename T>
    T population_variance(Iter first, Iter last, T mean)
    {
      typedef typename std::iterator_traits<Iter>::difference_type Distance;
      std::pair<T, Distance> p = sum_of_squares(first, last, mean);
      return p.first / p.second;
    }

  // Note that most spreadsheets and math programs define variance (or VAR)
  // to be the sample variance.
  template<typename Iter, typename T>
    T sample_variance(Iter first, Iter last, T mean)
    {
      typedef typename std::iterator_traits<Iter>::difference_type Distance;
      std::pair<T, Distance> p = sum_of_squares(first, last, mean);
      return p.first / (p.second - 1);
    }

  // Return the population standard deviation.
  template<typename Iter, typename T>
    T population_standard_deviation(Iter first, Iter last, T mean)
    {
      return std::sqrt(population_variance(first, last, mean));
    }

  // Return the sample standard deviation.
  template<typename Iter, typename T>
    T sample_standard_deviation(Iter first, Iter last, T mean)
    {
      return std::sqrt(sample_variance(first, last, mean));
    }

} // namespace origin

#endif
