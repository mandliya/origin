// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_GEOMETRY_HPP
#define ORIGIN_GEOMETRY_HPP

#include <cmath>
#include <algorithm>
#include <functional>
#include <random>
#include <vector>

namespace origin
{
  // FIXME: Vector/matrix traits should be in a more central location.
  template<typename T>
    struct is_vector : std::false_type 
    { };
    
  template<typename T, typename Alloc>
    struct is_vector<std::vector<T, Alloc>> : std::true_type
    { };

  // Return the difference of two values a - b, raised to the Nth power.
  // FIXME: This name is not particularly good. 
  template<int N, typename T>
    struct power_distance
    {
      static_assert(N != 0, "invalid power");
      T operator()(T const& a, T const& b) const
      {
        return pow(a - b, N);
      }
    };
  
  template<typename T>
    struct power_distance<1, T>
    {
      T operator()(T const& a, T const& b) const
      {
        return abs(a - b);
      }
    };
    
  template<typename T>
    struct power_distance<2, T>
    {
      T operator()(T const& a, T const& b) const
      {
        return (a - b) * (a - b);
      }
    };

  // Compute the distance, raised to the Nth power between each pair of 
  // iterators the given ranges.
  template<int N, typename Iter1, typename Iter2>
    typename std::iterator_traits<Iter1>::value_type
    pairwise_power_distance(Iter1 first1, Iter1 last1, Iter2 first2)
    {
      typedef typename std::iterator_traits<Iter1>::value_type Value;
      typedef power_distance<N, Value> Dist;
      
      Dist dist;
      Value sum = 0;
      while(first1 != last1) {
        sum = sum + dist(*first1, *first2);
        ++first1;
        ++first2;
      }
      return sum;
    }
    
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
    
  template<int N, typename Iter1, typename Iter2>
    typename std::iterator_traits<Iter1>::value_type
    minkowski_distance(Iter1 first1, Iter1 last1, Iter2 first2)
    {
      typedef typename std::iterator_traits<Iter1>::value_type Value;
      typedef static_nth_root<N, Value> Root;
      
      Root root;
      return root(pairwise_power_distance(first1, last1, first2));
    }
    
  // Return the Manhattan distance between two arithmetic types. This is the
  // same as the absolute value of their difference (and the Euclidean 
  // distance).
  template<typename T>
    inline typename std::enable_if<std::is_arithmetic<T>::value, T>::type
    manhattan_distance(T const& a, T const& b)
    {
      return abs(a - b);
    }
    
  // Return the Manhattan distance betwen two vectors.
  template<typename T>
    typename std::enable_if<is_vector<T>::value, typename T::value_type>::type
    manattan_distance(T const& a, T const& b)
    {
      typedef typename T::value_type Value;
      typedef typename T::size_type Size;
 
      assert(( a.size() == b.size() ));
      return pairwise_power_distance<1>(a.begin(), a.end(), b.begin());
    }

  // Distance measures (i.e., Metric functions)
  // Return the euclidean distance between two numeric types. Numeric types are
  // 
  // FIXME: This should be generalized to any OrderedRing. Note that numeric 
  // vectors are not ordered rings because they don't support v * v; they have
  // scalar multiplication.
  template<typename T>
    inline typename std::enable_if<std::is_arithmetic<T>::value, T>::type
    euclidean_distance(T const& a, T const& b)
    {
      return abs(a - b);
    }

  // Return the euclidean distance between two Real-valued vectors of the
  // same dimension.
  // FIXME: The result type must be the vector's value type. 
  // FIXME: This will probably fail to compile because of the naked value_type.
  template<typename T>
    typename std::enable_if<is_vector<T>::value, typename T::value_type>::type
    euclidean_distance(T const& a, T const& b)
    {
      typedef typename T::value_type Value;
      typedef typename T::size_type Size;
 
      assert(( a.size() == b.size() ));
      
      // FIXME: Write an iterative version of the algorithm. I might be able
      // to do this efficiently and generally by generating Minkowski distances
      // and then specializing to unroll the common cases.
      return minkowski_distance<2>(a.begin(), a.end(), b.begin());
//       Value sum = pairwise_power_distance<2>(a.begin(), a.end(), b.begin());
//       return sqrt(sum);
//       Value d = 0;
//       for(Size i = 0; i < a.size(); ++i) {
//         d = d + (a[i] - b[i]) * (a[i] - b[i]);
//       }
//       return sqrt(d);
    }
  
template<typename T>
  struct euclidean_distance_of
  {
    auto operator()(T const& a, T const& b) const
      -> decltype(euclidean_distance(a, b))
    {
      return euclidean_distance(a, b);
    }
  };
  
  // NOTE: The definitions of the euclidean and manhatten distances are
  // unified by Minkowski distances. Minkowski applies is an exponent applied
  // to each summand, the reciprocal of which is the exponent of the entire
  // sum. Is it possible to parameterize these algorithms over the exponent?
  // Yes, but the method by which it might be done probably requiers some
  // serious programming (e.g., specializing the computation of each summand
  // on its exponent).
  // Are there algorithms for cubing that are known to be faster than squaring?
  // For different types? What about cube root vs. square root?
  
  // FIXME: Do farthest also? That might be interesting.

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
