// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_GEOMETRY_HPP
#define ORIGIN_GEOMETRY_HPP

#include <cassert>
#include <cstdlib>
#include <numeric>

#include "math.hpp"
#include "statistics.hpp"

namespace origin
{
  // Return (x + y)^n
  template<int N, typename T>
    struct static_power_sum
    {
      T operator()(T const& x, T const& y) const
      {
        static_power<N, T> power;
        return power(x + y);
      }
    };
  
  template<typename T>
    struct static_power_sum<0, T>
    {
      T operator()(T const& x, T const& y) const { return T(1); }
    };

  template<typename T>
    struct static_power_sum<1, T>
    {
      T operator()(T const& x, T const& y) const { return x + y; }
    };


  // Return (x - y)^n
  template<int N, typename T>
    struct static_power_difference
    {
      T operator()(T const& x, T const& y) const
      {
        static_power<N, T> power;
        return power(x - y);
      }
    };

  template<typename T>
    struct static_power_difference<0, T>
    {
      T operator()(T const& x, T const& y) const { return T(1); }
    };

  template<typename T>
    struct static_power_difference<1, T>
    {
      T operator()(T const& x, T const& y) const { return x - y; }
    };


  // Returns |x - y|^n. 
  template<int N, typename T>
    struct static_absolute_power_difference
    { 
      T operator()(T const& a, T const& b) const
      {
        static_power<N, T> power;
        return power(std::abs(a - b));
      }
    };

  template<typename T>
    struct static_absolute_power_difference<0, T>
    {
      T operator()(T const& a, T const& b) const { return T(1); }
    };

  template<typename T>
    struct static_absolute_power_difference<1, T>
    {
      T operator()(T const& a, T const& b) const { return std::abs(a - b); }
    };

  // Compute the Minkowski distance of order N on the vectors given by the
  // ranges in [first1, last1), and the weak range [first2, last1 - first1).
  //
  // requires N > 0 && InputIterator<Iter1> && InputIterator<Iter2>
  //       && Same<T, ValueType<Iter1>, ValueType<Iter2>>
  //       && Field<T>
  //
  // FIXME: Field<T> might be too strong. What are the actual requirements
  // imposed by the nth_root algorithm?
  template<int N, typename Iter1, typename Iter2>
    inline typename std::iterator_traits<Iter1>::value_type
    minkowski_distance(Iter1 first1, Iter1 last1, Iter2 first2)
    {
      // pre: readable_bounded_range(first1, last1)
      // pre: readable_weak_range(frist2, last1 - first1)
      typedef typename std::iterator_traits<Iter1>::value_type Value;
      static_power_difference<N, Value> diff;
      static_nth_root<N, Value> root;
      return root(accumulate(first1, last1, first2, Value(0), diff));
    }

  // requires Vector<Vec>
  template<int N, typename Vec>
    inline typename Vec::value_type 
    minkowski_distance(Vec const& a, Vec const& b)
    {
      // pre: a.size() == b.size()
      assert(( a.size() == b.size() ));

      return operator()(a.begin(), a.end(), b.begin());
    }

  // Compute the weighted Minkowski distance, where weights are given in the
  // weak range [|first3, last1 - first1|).
  template<int N, typename Iter1, typename Iter2, typename Iter3>
    inline typename std::iterator_traits<Iter1>::value_type
    weighted_minkowski_distance(Iter1 first1, Iter1 last1, Iter2 first2, Iter3 first3)
    {
      // pre: readable_bounded_range(first1, last1)
      // pre: readable_weak_range(frist2, last1 - first1)
      // pre: readable_weak_range(frist3, last1 - first1)
      typedef typename std::iterator_traits<Iter1>::value_type Value;

      static_absolute_power_difference<N, Value> diff;
      static_nth_root<N, Value> root;
      return root(weighted_accumulate(first1, last1, first2, first3, Value(0), diff));
    }

  // Compute the weighted Minkowski distance on the two vectors.  Weights are
  // given in the 3rd vector argument.
  template<int N, typename Vec>
    inline typename Vec::value_type 
    weighted_minkowski_distance(Vec const& a, Vec const& b, Vec const& w)
    {
      // pre: a.size() == b.size() && b.size() == w.size()
      assert(( a.size() == b.size() ));
      assert(( b.size() == w.size() ));

      return weighted_minkowski_distance(a.begin(), a.end(), b.begin(), w.begin());
    }


  // Compute the Manhattan distance between the given vectors.
  template<typename Iter1, typename Iter2>
    inline typename std::iterator_traits<Iter1>::value_type
    manhattan_distance(Iter1 first1, Iter1 last1, Iter2 first2)
    {
      return minkowski_distance<1>(first1, last1, first2);
    }
    
  // Return the Manhattan distance the given vectors.
  template<typename Vec>
    inline typename Vec::value_type 
    manhattan_distance(Vec const& a, Vec const& b)
    {
      assert(( a.size() == b.size() ));
      return manhattan_distance(a.begin(), a.end(), b.begin());
    }

  // Return the weighted Manhattan distance of the given ranges. Weights are
  // given in the weak range [|first3, last1 - first1|).
  template<typename Iter1, typename Iter2, typename Iter3>
    inline typename std::iterator_traits<Iter1>::value_type
    weighted_manhattan_distance(Iter1 first1, Iter1 last1, Iter2 first2, Iter3 first3)
    {
      return weighted_minkowski_distance<1>(first1, last1, first2, first3);
    }
    
  // Return the weighted Manhattan distance of the given ranges. Weights are
  // given in the vector w.
  template<typename Vec>
    inline typename Vec::value_type
    weighted_manhattan_distance(Vec const& a, Vec const& b, Vec const& w)
    {
      assert(( a.size() == b.size() ));
      assert(( b.sise() == w.size() ));
      return weighted_manhattan_distance(a.begin(), b.begin(), w.begin());
    }
    
  // Return the Euclidean distance between the given vectors.
  template<typename Iter1, typename Iter2>
    inline typename std::iterator_traits<Iter1>::value_type
    euclidean_distance(Iter1 first1, Iter1 last1, Iter2 first2)
    {
      return minkowski_distance<2>(first1, last1, first2);
    }

  // Return the euclidean distance between the two vectors.
  template<typename Vec>
    inline typename Vec::value_type 
    euclidean_distance(Vec const& a, Vec const& b)
    {
      assert(( a.size() == b.size() ));
      return euclidean_distance(a.begin(), a.end(), b.begin());
    }

  // Return the weighted Euclidean distance of the given ranges. Weights are
  // given in the weak range [|first3, last1 - first1|).
  template<typename Iter1, typename Iter2, typename Iter3>
    inline typename std::iterator_traits<Iter1>::value_type
    weighted_euclidean_distance(Iter1 first1, Iter1 last1, Iter2 first2, Iter3 first3)
    {
      return weighted_minkowski_distance<2>(first1, last1, first2, first3);
    }
    
  // Return the weighted Euclidean distance of the given ranges. Weights are
  // given in the vector w.
  template<typename Vec>
    inline typename Vec::value_type
    weighted_euclidean_distance(Vec const& a, Vec const& b, Vec const& w)
    {
      assert(( a.size() == b.size() ));
      assert(( b.size() == w.size() ));
      return weighted_euclidean_distance(a.begin(), a.end(), b.begin(), w.begin());
    }

  // FIXME: The algorithm maximizes the an operation on two sequences of
  // values. What is the most general form of this algorithm. Nearest neighbor
  // does something similar.
  template<typename Iter1, typename Iter2>
    typename std::iterator_traits<Iter1>::value_type
    chebyshev_distance(Iter1 first1, Iter1 last1, Iter2 first2)
    {
      typedef typename std::iterator_traits<Iter1>::value_type Value;
      Value dist = std::abs(*first1++, *first2++);
      while(first1 != last1) {
        dist = std::max(std::abs(*first1 - *first2), dist);
        ++first1;
        ++first2;
      }
      return dist;
    }

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

      // FIXME: This shouldn't be double. It sohuld be the result of the
      // function, but since this is a special build, I don't care too much.
      double min = dist(value, *first);
      advance(first, 1);
      while(first != last) {
        double d = dist(value, *first);
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
      // positions in the loop. We go from last-first comparisons per iteration
      // to 2.
      if(mid == first) {
        return nearest_to(origin::next(first), last, *mid, dist);
      } else if(next(mid) == last) {
        return nearest_to(first, mid, *mid, dist);
      } else {
        Iter i = nearest_to(first, mid, *mid, dist);
        Iter j = nearest_to(next(mid), last, *mid, dist);
        return dist(*i, *mid) < dist(*j, *mid) ? i : j;
      }
    }

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

  // TODO: Consider replacing repeated calls to dist() with a distance matrix.
  
  // Compute the distance from each vector in the range [first, last) to point,
  // writing the value to result. Distance is computed using the dist function.
  //
  // requires InputIterator<Iter>
  //       && DistanceMetric<Distance, ValueType<Iter>>
  //       && Writable<Out, ResultType<Distance>>
  //       && Same<ValueType<Iter>, Value> 
  //
  // FIXME: Replace Value with ValueType<Iter>
  template<typename Iter, typename Value, typename Out, typename Distance>
    void distance_to(Iter first, Iter last, Value const& point, Out result, Distance dist)
    {
      while(first != last) {
        *result = dist(*first, point);
        ++first;
        ++result;
      }
    }

  // Return a vector that represents the centroid of the given sequence of
  // points. The centroid is computed as the "arithmetic mean" of the points,
  // if arithmetic mean were generalized to vector spaces.
  //
  // Note that the algorithm only requires vectors, not vector spaces. The
  // vectors referenced by the given iterators are not required to provide any
  // mathematical operators.
  //
  // requires Iterator<Iter>
  //      &&  Vector<ValueType<Iter>>
  template<typename Iter>
    typename std::iterator_traits<Iter>::value_type
    centroid(Iter first, Iter last)
    {
      // pre: for(Iter i : range(first + 1, last)) (*i).size() == (*first).size()
      typedef typename std::iterator_traits<Iter>::value_type Vector;
      typedef typename std::iterator_traits<Iter>::difference_type Distance;
      typedef typename Vector::value_type Value;

      // FIXME: Should have size(*first).
      Distance m = first->size();
      Vector result(m, 0);

      // Add each coefficient into the result.
      Distance n = 0;
      while(first != last) {
        accumulate_each(first->begin(), first->end(), result.begin());
        ++first;
        ++n;
      }
      
      // Divide each coefficient by the number of elements.
      for(typename Vector::iterator i = result.begin(); i != result.end(); ++i)
        *i /= double(n);
      return result;
    }

} // namespace origin

#endif
