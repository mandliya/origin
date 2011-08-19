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
  // FIXME: Do farthest also? That might be interesting.

  // The concept of min and max are related to the notion of nearest. In min
  // and max, we're searching for an extreme value out of each. With nearest
  // (and probably farthest also), we're looking for an element that minimizes
  // or maximizes (extremizes) a property. We also generally have the case that
  // the oeration doesn't apply to an object with itself.
  //
  // How general can Distance be? Does it really need to be a metric?

  // Return an iterator to the object that is nearest to the given value
  // by distance.
  //
  // Note that pos not in the range [first, last).
  // FIXME: The Value parameter should be ValueType<Iter>.
  template<typename Iter, typename Value, typename Distance>
    Iter nearest_to(Iter first, Iter last, Value value, Distance dist)
    {
      // pre: readable_bounded_range(first, last) and valid(*pos)
      // pre: metric(dist) --- symmetric, triangle, identity of indisc. nonneg
      Iter near = first;
      auto min = dist(value, *first);
      advance(first, 1);
      while(first != last) {
        auto d = dist(value, *first);
        if(d < min) {
          near = first;
          min = d;
        }
        ++first;
      }
      return near;
    }

  // Find the nearest object in the range [first, last) to the object referred
  // to by mid. That object is not compared to itself.
  // ForwardIterator<Iter>
  template<typename Iter, typename Distance>
    Iter nearest(Iter first, Iter mid, Iter last, Distance dist)
    {
      // pre: readable_bounded_range(fi
      // pre: first <= mid < last
      // pre: metric(dist) --- symmetric, triangle, identity of indisc. nonneg
      if(first == mid) {
        return nearest_to(next(first), last, *mid, dist);
      } else if(next(mid) == last) {
        return nearest_to(first, mid, *mid, dist);
      } else {
        Iter i = nearest_to(first, mid, *mid, dist);
        Iter j = nearest_to(next(first), last, *mid, dist);
        return std::min({i, j}, [](Iter a, Iter b) { return *a < *b; });
      }
    }

  // Write the sequence of nearest neighbors 

  // FIXME: This can be inefficient because we're computing distances
  // redundantly. We could pre-compute a distance matrix and then use that
  // for lookups instead of continuing to call dist. That would be very
  // worthwhile for non-trivial value types or distance computatations.
  // A good question is: how much faster? How complex does ValueType and
  // Distance have to be before we start getting noticable performance
  // differences.
  
  // ForwardIterator<Iter>
  // WeaklyIncrementable<Out>
  // Writable<Iter, Out> 
  // Function<Distance, ValueType<Iter>, ValueType<Iter>>
  template<typename Iter, typename Out, typename Distance>
    void nearest_neighbors(Iter first, Iter last, Out result, Distance dist)
    {
      // pre: distance(first, last) > 1
      for(Iter i = first; i != last; ++i)
        *result++ = nearest(first, i, last, dist);
    }
  
  // ForwardIterator<Iter>
  // WeaklyIncrementable<Out>
  // Function<Distance, ValueType<Iter>, ValueType<Iter>>
  // Writable<ResultType<Distance, VaulueType<Iter>, ValueType<Iter>>
  template<typename Iter, typename Out, typename Distance>
    void nearest_neighbor_distances(Iter first, Iter last, Out result, Distance dist)
    {
      // pre: distance(first, last) > 1
      for(Iter i = first; i != last; ++i)
        *result++ = dist(*i, *nearest(first, i, last, dist));
    }

} // namespace origin

#endif
