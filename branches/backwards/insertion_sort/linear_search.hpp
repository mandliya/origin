// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_LINEAR_SEARCH_HPP
#define ORIGIN_LINEAR_SEARCH_HPP

#include <functional>

namespace origin
{
  // NOTE: The linear variants of these algorithms accept input iterators!

  /**
   * Return the lower bound of the sorted sequence [f, l) using a linear search
   * and the given ordering function.
   */
  template<typename Iter, typename T, typename Comp>
  inline Iter linear_lower_bound(Iter first, Iter last, T const& x, Comp comp)
  {
    while(first != last && comp(*first, x))
      ++first;
    return first;
  }

  /**
   * Return the lower bound of the sorted sequence [f, l) using a linear search.
   */
  template<typename Iter, typename T>
  inline Iter linear_lower_bound(Iter first, Iter last, T const& x)
  { return linear_lower_bound(first, last, x, std::less<T>()); }

  // NOTE: The runtime of the upper bound functions is |linear_lower_bound| + m
  // where m is the number of objects in [f, l) that are greater than or equal
  // to the value at the lower bound.

  /**
   * Return the upper bound a the sorted sequence [f, l) using a linear search
   * using the given ordering function.
   */
  template<typename Iter, typename T, typename Comp>
  inline Iter linear_upper_bound(Iter first, Iter last, T const& x, Comp comp)
  {
    // Find the lower bound first and then find the first position where
    // *f != x, using the same relational operator of course and assuming that
    // we aren't already at the end of the range.
    // NOTE: If the comparison function is not homogenous, then the comparator
    // needs to provide symmetric overloads for the first and second arguments.
    first = linear_lower_bound(first, last, x, comp);
    while(first != last && !comp(x, *first))
      ++first;
    return first;
  }

  /**
   * Return the upper bound a the sorted sequence [f, l) using a linear search.
   */
  template<typename Iter, typename T>
  inline Iter linear_upper_bound(Iter first, Iter last, T const& x)
  { return linear_upper_bound(first, last, x, std::less<T>()); }

  // NOTE: Unlike the previous algorithms equal_range requires forward iterators
  // since we have to cache the lower bound.
  /**
   * Return an iterator denoting the set of values that are equivalent to the
   * given value using the given ordering function.
   */
  template<typename Iter, typename T, typename Comp>
  inline std::pair<Iter, Iter>
  linear_equal_range(Iter first, Iter last, T const& x, Comp comp)
  {
    // This is virtually identical to the upper bound algorithm except that
    // we have to store the lower bound for the return value.
    Iter lower = lower_bound(first, last, x, comp);
    Iter upper = lower;
    while(upper != last && !comp(x, *upper))
      ++upper;
    return std::make_pair(lower, upper);
  }

  /**
   * Return an iterator denoting the set of values that equivalent to the given
   * value.
   */
  template<typename Iter, typename T>
  inline std::pair<Iter, Iter>
  linear_equal_range(Iter first, Iter last, T const& x)
  { return linear_equal_range(first, last, x, std::less<T>()); }


  /**
   * Return true if the given value can be found in the sequence [first, last)
   * sorted according to given ordering function.
   */
  template<typename Iter, typename T, typename Comp>
  inline bool
  linear_search(Iter first, Iter last, T const& x, Comp comp)
  {
    std::pair<Iter, Iter> rng = linear_equal_range(first, last, x, comp);
    return rng.first != rng.second;
  }

  /**
   * Return true if the given value can be found in the sorted sequence
   * [first, last).
   */
  template<typename Iter, typename T>
  inline bool
  linear_search(Iter first, Iter last, T const& x)
  { return linear_search(first, last, x, std::less<T>()); }

} // namespace origin

#endif
