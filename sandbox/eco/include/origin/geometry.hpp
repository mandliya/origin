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
  // NOTE: Points in [first, last) should define a metric space. Could use
  // metric_space_traits to get the distance type.
  
  // Returns a pair<Iter, D> where D is probably a real number.
  // FIXME: I'm pretty sure that D is real since distance metrics are defined
  // in terms of real numbers. Right?
  template<typename Iter, typename Distance>
    auto nearest_neighbor(Iter first, Iter last, Distance dist)
      -> decltype(std::make_pair(first, dist(first, first)))
    {
      // pre: distance(first, last) > 1
      Iter near = next(first);
      auto min = distance(first, near);
      for(Iter i = next(near); i != last; ++i) {
        auto d = distance(first, i);
        if(d < min) {
          near = i;
          min = d;
        }
      }
      return {near, min};
    }

  template<typename Iter, typename Out, typename Distance>
    void nearest_neighbors(Iter first, Iter last, Out result, Distance dist)
    {
      // pre: distance(first, last) > 1
      while(next(first) != last) {
        auto p = nearest_neighbor(first, last, dist);
        *result = p.second;
        ++first;
        ++result;
      }
    }
    
} // namespace origin

#endif
