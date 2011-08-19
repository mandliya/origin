// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_RANDOM_HPP
#define ORIGIN_RANDOM_HPP

#include <functional>
#include <random>

namespace origin
{
  // FIXME: Consider implementing Vitter's algorithms X, Y, and Z. Z is pretty
  // complicated.

  // Copy the first n elements from [first, first + n) into [result, result + n).
  // Return first + n.
  template<typename Iter, typename Size, typename Out>
    Iter initialize_reservoir(Iter first, Size n, Out result)
    {
      copy_n(first, n, result);
      advance(first, n);
      return first;
    }


  // FIXME: Write a program to ensure that samples are actually uniform. 
  // Basically. Just generate a million samples and observe the distribution
  // frequency for each value.
  
  // Reservoir sampling :
  // Jeffrey Vitter, Random Sampling with a Reservoir. ACM Transactions on
  // Mathematical software.
  // http://www.cs.umd.edu/~samir/498/vitter.pdf
  //
  // FIXME: Replace Size with DistanceType<Iter>
  // InputIterator<Iter>
  // RandomAccessIterator<Out>
  // UniformRandomNumberGenerator<Gen>
  template<typename Iter, typename Size, typename Out, typename Gen>
    void random_sample(Iter first, Iter last, Out out, Size n, Gen&& gen)
    {
      // pre: readable_bounded_range(first, last)
      // pre: mutable_weak_range(out, n)
      typedef std::uniform_int_distribution<Size> Dist;
      typedef typename Dist::param_type Param;
      
      // Start by filling the reservoir and advancing first.
      first = initialize_reservoir(first, n, out);

      // Sample the remaining values in [first + n, last) by selecting a random
      // number r in the range [0, k], and, if r < n, replace it. k increase
      // with each iteration, making the probability smaller. Note that for
      // random access iterators, k = i - first (assuming we increment i and
      // not first).
      Dist dist;
      Size k = n;
      while(first != last) {
        Size r = dist(gen, Param{0, k});
        if(r < n)
          *(out + r) = *first;
        ++first;
        ++k;
      }
    }
    
} // namespace origin

#endif
