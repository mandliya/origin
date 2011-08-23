// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_ALGORITHM_HPP
#define ORIGIN_ALGORITHM_HPP

#include <cmath>

namespace origin
{
  template<typename Iter, typename Func>
    void for_all_combinations(Iter first, Iter last, Func f)
    {
      while(first != last) {
        for(Iter i = first; i != last; ++i)
          f(*first, *i);
        ++first;
      }
    }

  // FIXME: Refactor this so we don't compare in every loop. Use the strategy
  // found in nearest_neighbor.
  template<typename Iter, typename Func>
    void for_all_strict_combinations(Iter first, Iter last, Func f)
    {
      while(first != last) {
        for(Iter i = first; i != last; ++i) {
          if(first != i)
            f(*first, *i);
        }
        ++first;
      }
    }

} // namespace origin

#endif
