// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_RANDOM_HPP
#define ORIGIN_RANDOM_HPP

#include <algorithm>
#include <functional>

#include <boost/random/uniform_int.hpp>

namespace origin
{
  // This is a replacement for std::variate_generator, which captures the
  // generator by value. That causes selections to be replayed over and over.
  // This might be replaceable reference_wrapper, but for some reason, couldn't
  // get it to work.
  template<typename Dist, typename Gen>
    class random_var
    {
    public:
      typedef typename Dist::result_type result_type;

      random_var(Dist const& dist, Gen& gen)
        : dist(dist), gen(gen)
      { }

      result_type operator()()
      {
        return dist(gen);
      }
      
    private:
      Dist dist;
      Gen& gen;
    };
  
  // Bind the distribution and uniform random number generator together to
  // create a random variable.
  template<typename Dist, typename Gen>
    inline random_var<Dist, Gen> bind_random_var(Dist const& dist, Gen& gen)
    {
      return random_var<Dist, Gen>(dist, gen);
    }

  // FIXME: Consider implementing Vitter's algorithms X, Y, and Z. Z is pretty
  // complicated.

  // Copy the first n elements from [first, first + n) into [result, result + n).
  // Return first + n.
  template<typename Iter, typename Size, typename Out>
    Iter initialize_reservoir(Iter first, Size n, Out result)
    {
      Iter last = first;
      std::advance(last, n);
      std::copy(first, last, result);
      return last;
    }


  // Reservoir sampling :
  // Jeffrey Vitter, Random Sampling with a Reservoir. ACM Transactions on
  // Mathematical software.
  // http://www.cs.umd.edu/~samir/498/vitter.pdf
  //
  // FIXME: Replace Size with DistanceType<Iter>
  // FIXME: It's possible that the reservoir may be larger than the the
  // population. This seems to be a reasonable limitation.
  //
  // InputIterator<Iter>
  // RandomAccessIterator<Out>
  // UniformRandomNumberGenerator<Gen>
  template<typename Iter, typename Size, typename Out, typename Gen>
    Out random_sample(Iter first, Iter last, Out result, Size n, Gen& gen)
    {
      // pre: readable_bounded_range(first, last)
      // pre: mutable_weak_range(result, n)
      typedef boost::uniform_int<Size> Dist;
      typedef typename Dist::param_type Param;
      
      // Start by filling the reservoir and advancing first.
      first = initialize_reservoir(first, n, result);

      // Sample the remaining values in [first + n, last) by selecting a random
      // number r in the range [0, k], and, if r < n, replace it. k increase
      // with each iteration, making the probability smaller. Note that for
      // random access iterators, k = i - first (assuming we increment i and
      // not first).
      Dist dist;
      Size k = n;
      while(first != last) {
        Size r = dist(gen, Param(0, k));
        if(r < n)
          *(result + r) = *first;
        ++first;
        ++k;
      }
      return result;
    }


  // Choose n randomly selected elements from the sequence of objects in 
  // [first, last) such that each element has an equal probability of being
  // selected. Elements are replaced after selection.
  //
  // Technically, this is called random sampling with replacement, but we use
  // the name "selection" to differentiate it from random sampling without
  // replacement. Here, the algorithm makes n random selections.
  //
  // An algorithm for stream-base random sampling with replacement is given in:
  //
  // Park et al, "Reservoir-based Random Sampling with Replacement from Data
  // Stream", 
  //
  // FIXME: Replace Size with DistanceType<Iter>
  //
  // requires RandomAccessIterator<Iter>
  //       && WeaklyIncrementable<Out>
  //       && UniformRandomNumberGenerator<Gen>
  template<typename Iter, typename Size, typename Out, typename Gen>
    Out random_selection(Iter first, Iter last, Out result, Size n, Gen& gen)
    {
      typedef boost::uniform_int<Size> Dist;
      typedef random_var<Dist, Gen> Var;
      // pre: readable_range(first, last)
      // pre: mutable_range(out, n)
      // pre: n <= last - first
      
      Dist dist(0, last - first - 1);
      Var rand = bind_random_var(dist, gen);
      while(n != 0) {
        *result = *(first + rand());
        ++result;
        --n;
      }
      return result;
    }

} // namespace origin

#endif
