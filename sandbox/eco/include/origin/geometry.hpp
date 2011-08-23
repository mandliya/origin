// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_GEOMETRY_HPP
#define ORIGIN_GEOMETRY_HPP

#include <origin/math.hpp>

namespace origin
{
  // Compute the distance, raised to the Nth power between each pair of 
  // iterators the given ranges. The exponent is given as an explicit template
  // argument.
  template<int N, typename Iter1, typename Iter2>
    typename std::iterator_traits<Iter1>::value_type
    power_distance(Iter1 first1, Iter1 last1, Iter2 first2)
    {
      typedef typename std::iterator_traits<Iter1>::value_type Value;
      typedef static_power_distance<N, Value> Dist;

      Dist dist;
      Value sum = 0;
      while(first1 != last1) {
        sum = sum + dist(*first1, *first2);
        ++first1;
        ++first2;
      }
      return sum;
    }
    
  // Compute the Minkowski distance of order N on the vectors given by the
  // ranges in [first1, last1), and [first2, first2 + (last1 - first1)). The
  // order N is given as an explicit template argument.
  template<int N, typename Iter1, typename Iter2>
    typename std::iterator_traits<Iter1>::value_type
    minkowski_distance(Iter1 first1, Iter1 last1, Iter2 first2)
    {
      typedef typename std::iterator_traits<Iter1>::value_type Value;
      typedef static_nth_root<N, Value> Root;
      
      Root root;
      return root(power_distance<N>(first1, last1, first2));
    }
    
  template<int N, typename T, typename T>
    typename T::value_type minkowski_distance(T const& a, T const& b)
    {
      assert(( a.size() == b.size() ));
      return minkowski_distance(a.begin(), a.end(), b.begin());
    }
    
  // FIXME: Write the non-static minkowski distance, allowing the order to
  // vary.


  // Compute the Manhattan distance between the given vectors.
  template<typename Iter1, typename Iter2>
    typename std::iterator_traits<Iter1>::value_type
    static manhattan_distance(Iter1 first1, Iter1 last1, Iter2 first2)
    {
      return minkowski_distance<1>(first1, last1, first2);
    }
    
  // Return the Manhattan distance the given vectors.
  template<typename T>
    typename T::value_type manattan_distance(T const& a, T const& b)
    {
      assert(( a.size() == b.size() ));
      return manhattan_distance(a.begin(), a.end(), b.begin());
    }
    
    
  // Return the Euclidean distance between the given vectors.
  template<typename Iter1, typename Iter2>
    typename std::iterator_traits<Iter1>::value_type
    euclidean_distance(Iter1 first1, Iter1 last1, Iter2 first2)
    {
      return minkowski_distance<2>(first1, last1, first2);
    }

  // Return the euclidean distance between the two vectors.
  template<typename T>
    typename T::value_type euclidean_distance(T const& a, T const& b)
    {
      assert(( a.size() == b.size() ));
      return euclidean_distance(a.begin(), a.end(), b.begin());
    }
  
  // Function wrappers for distance functions.
  template<typename T>
    struct minkowski_distance_of
    {
      auto operator()(T const& a, T const& b) const
        -> decltype(minkowski_distance(a, b))
      {
        return minkowski_distance(a, b);
      }
    };

  template<typename T>
    struct manhattan_distance_of
    {
      auto operator()(T const& a, T const& b) const
        -> decltype(manhattan_distance(a, b))
      {
        return manhattan_distance(a, b);
      }
    };

  template<typename T>
    struct euclidean_distance_of
    {
      auto operator()(T const& a, T const& b) const
        -> decltype(euclidean_distance(a, b))
      {
        return euclidean_distance(a, b);
      }
    };
  
  
  // TODO: Implement farthest neighbor.
  // TODO: Implement k-nearest neightbors.

  // The concept of min and max are related to the notion of nearest. In min
  // and max, we're searching for an extreme value out of each. With nearest
  // (and probably farthest also), we're looking for an element that minimizes
  // or maximizes (extremizes) a property. We also generally have the case that
  // the oeration doesn't apply to an object with itself.
  
  // A Metric is a binary function TxT->R that satisfies the following
  // properties:
  //  - symmetry:       d(x, y) <=> d(y, x)
  //  - nonnegativity:  d(x, y) >= 0
  //  - identity:       d(x, y) == 0 => x == y
  //  - triange:        d(x, z) <= d(x, y) + d(y, z)


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

  // TODO: This algorithm implements a general looping strategy for all 
  // combinations.

  // Find the nearest object in the range [first, last) to the object referred
  // to by mid. That object is not compared to itself.
  // ForwardIterator<Iter>
  template<typename Iter, typename Distance>
    Iter nearest(Iter first, Iter mid, Iter last, Distance dist)
    {
      // pre: readable_bounded_range(first, last) and last - first > 1
      // pre: first <= mid < last
      // pre: metric(dist) --- symmetric, triangle, identity of indisc. nonneg
      
      // NOTE: This implementation avoids repeated comparisons of iterator
      // positions in the loop. We go from last-first comparisons to 2.
      if(first == mid) {
        return nearest_to(next(first), last, *mid, dist);
      } else if(next(mid) == last) {
        return nearest_to(first, mid, *mid, dist);
      } else {
        Iter i = nearest_to(first, mid, *mid, dist);
        Iter j = nearest_to(next(mid), last, *mid, dist);
        return std::min(i, j, [](Iter a, Iter b) { return *a < *b; });
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
      for(Iter i = first; i != last; ++i) {
        *result++ = nearest(first, i, last, dist);
      }
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
    
  // TODO: We can replace repeated calls to dist() with a distance matrix.

} // namespace origin

#endif
