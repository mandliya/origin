// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_SEQUENCE_ALGORITHM_HPP
#define ORIGIN_SEQUENCE_ALGORITHM_HPP

#include <algorithm>

#include "concepts.hpp"

namespace origin
{
  //////////////////////////////////////////////////////////////////////////////
  // Quantifiers
  //
  //////////////////////////////////////////////////////////////////////////////

  template <typename R, typename P>
    inline bool
    all_of(const R& range, P pred)
    {
      using std::begin;
      using std::end;
      return std::all_of(begin(range), end(range), pred);
    }

  template <typename R, typename P>
    inline bool
    any_of(const R& range, P pred)
    {
      using std::begin;
      using std::end;
      return std::any_of(begin(range), end(range), pred);
    }

  template <typename R, typename P>
    inline bool
    none_of(const R& range, P pred)
    {
      using std::begin;
      using std::end;
      return std::none_of(begin(range), end(range), pred);
    }


  //////////////////////////////////////////////////////////////////////////////
  // For Each                                                       alg.for_each
  //
  template <typename R, typename F>
    inline F
    for_each(R&& range, F f)
    {
      using std::begin;
      using std::end;
      for_each(begin(range), end(range), f);
    }



  //////////////////////////////////////////////////////////////////////////////
  // Find
  //
  //////////////////////////////////////////////////////////////////////////////

  template <typename R, typename T>
    inline Iterator_of<R> 
    find(R&& range, const T& value)
    {
      using std::begin;
      using std::end;
      return std::find(begin(range), end(range), value);
    }


  template <typename R, typename P>
    inline Iterator_of<R>
    find_if(R&& range, P pred)
    {
      using std::begin;
      using std::end;
      return std::find_if(begin(range), end(range), pred);
    }

  template <typename R, typename P>
    inline Iterator_of<R>
    find_if_not(R&& range, P pred)
    {
      using std::begin;
      using std::end;
      return std::find_if_not(begin(range), end(range), pred);
    }


  //////////////////////////////////////////////////////////////////////////////
  // Find End
  //
  // NOTE: This should be called search_end or search_last. It's not really
  // a find algorithm.
  template <typename R1, typename R2>
    inline Iterator_of<R1>
    find_end(R1&& range1, const R2& range2)
    {
      using std::begin;
      using std::end;
      return std::find_end(begin(range1), end(range1),
                           begin(range2), end(range2));
    }

  template <typename R1, typename R2, typename C>
    inline Iterator_of<R1>
    find_end(R1&& range1, const R2& range2, C comp)
    {
      using std::begin;
      using std::end;
      return std::find_end(begin(range1), end(range1),
                           begin(range2), end(range2), comp);
    }


  //////////////////////////////////////////////////////////////////////////////
  // Adjacent Find
  //
  // NOTE: This shold be called find_adjacent -- a much better name for it.
  template <typename R>
    inline Iterator_of<R>
    adjacent_find(R&& range)
    {
      using std::begin;
      using std::end;
      return std::adjacent_find(begin(range), end(range));
    }

  template <typename R, typename C>
    inline Iterator_of<R>
    adjacent_find(R&& range, C comp)
    {
      using std::begin;
      using std::end;
      return std::adjacent_find(begin(range), end(range), comp);
    }


  //////////////////////////////////////////////////////////////////////////////
  // Count
  //
  //////////////////////////////////////////////////////////////////////////////

  template <typename R, typename T>
    inline Difference_type<R>
    count(const R& range, const T& value)
    {
      using std::begin;
      using std::end;
      return std::count(begin(range), end(range), value);
    }

  template <typename R, typename P>
    inline Difference_type<R>
    count_if(const R& range, P pred)
    {
      using std::begin;
      using std::end;
      return std::count_if(begin(range), end(range), pred);
    }


  //////////////////////////////////////////////////////////////////////////////
  // Mismatch
  //
  // NOTE: This is named range_mismatch to avoid overload ambiguities with
  // std::mismatch.
  template <typename R1, typename R2>
    inline std::pair<Iterator_of<R1>, Iterator_of<R2>>
    range_mismatch(R1&& range1, R2&& range2)
    {
      using std::begin;
      using std::end;
      return std::mismatch(begin(range1), end(range1), begin(range2));
    }

  template <typename R1, typename R2, typename C>
    inline std::pair<Iterator_of<R1>, Iterator_of<R2>>
    range_mismatch(R1&& range1, R2&& range2, C comp)
    {
      using std::begin;
      using std::end;
      return std::mismatch(begin(range1), end(range1), begin(range2), comp);
    }


  //////////////////////////////////////////////////////////////////////////////
  // Equal
  //

  template <typename R1, typename R2>
    inline bool
    range_equal(const R1& range1, const R2& range2)
    {
      using std::begin;
      using std::end;
      return std::equal(begin(range1), end(range1), begin(range2));
    }

  template <typename R1, typename R2, typename C>
    inline bool
    range_equal(const R1& range1, const R2& range2, C comp)
    {
      using std::begin;
      using std::end;
      return std::equal(begin(range1), end(range1), begin(range2), comp);
    }


  //////////////////////////////////////////////////////////////////////////////
  // Is Permutation
  //

  template <typename R1, typename R2>
    inline bool 
    range_is_permutation(const R1& range1, const R2& range2)
    {
      using std::begin;
      using std::end;
      return std::is_permutation(begin(range1), end(range1), begin(range2));
    }

  template <typename R1, typename R2, typename C>
    inline bool 
    range_is_permutation(const R1& range1, const R2& range2, C comp)
    {
      using std::begin;
      using std::end;
      return std::is_permutation(begin(range1), end(range1), 
                                 begin(range2), 
                                 comp);
    }


  //////////////////////////////////////////////////////////////////////////////
  // Search
  //
  template <typename R1, typename R2>
    Iterator_of<R1>
    search(R1&& range1, const R2& range2)
    {
      using std::begin;
      using std::end;
      return std::search(begin(range1), end(range1), 
                         begin(range2), end(range2));
    }

  template <typename R1, typename R2, typename C>
    Iterator_of<R1>
    search(R1&& range1, const R2& range2, C comp)
    {
      using std::begin;
      using std::end;
      return std::search(begin(range1), end(range1), 
                         begin(range2), end(range2));
    }


  //////////////////////////////////////////////////////////////////////////////
  // Search N
  //
  template <typename R, typename T>
    Iterator_of<R>
    search_n(R&& range, Difference_type<R> n, const T& value)
    {
      using std::begin;
      using std::end;
      return std::search_n(begin(range), end(range), n, value);
    }

  template <typename R, typename T, typename C>
    Iterator_of<R>
    search_n(R&& range, Difference_type<R> n, const T& value, C comp)
    {
      using std::begin;
      using std::end;
      return std::search_n(begin(range), end(range), n, value, comp);
    }


} // namespace origin

#endif
