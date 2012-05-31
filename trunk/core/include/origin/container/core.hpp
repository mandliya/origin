// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_CONTAINER_CONTAINER_CORE_HPP
#define ORIGIN_CONTAINER_CONTAINER_CORE_HPP

#include <initializer_list>

#include <origin/iterator/core.hpp>
#include <origin/range/core.hpp>
#include <origin/algorithm.hpp>
#include <origin/memory.hpp>

namespace origin
{
  // Copy assign sequence
  //
  // The algorithm implements copy assignment for sequence types. First,
  // as many elements as possible are copied from the input to output range.
  // If all elements were copied int out, then the remaining elements of out
  // are erased. If not, the remaining are inserted at the end of the sequence.
  // The algorithm returns a reference to out, for convenience.

  template <typename S, typename I>
    inline S& 
    copy_assign_sequence(S& out, I first, I last)
    {
      auto p = bounded_copy(first, last, out.begin(), out.end());
      if (p.first == last)
        out.erase(p.second, out.end());
      else
        out.insert(out.end(), p.first, last);
      return out;
    }

  template <typename S, typename R>
    inline S& 
    copy_assign_sequence(S& out, const R& in)
    {
      static_assert(Input_range<R>(), "");
      return copy_assign_sequence(out, begin(in), end(in));
    }



  // Move assign sequence
  //
  // The move assign algorithm mvoes the elements of the input range in into
  // the elements of the output range out. 
  //
  // Complexity:
  //    The algorithm destroys exactly size(out) objects and moves exactly
  // size(in) objects. There should be at most 1 allocation.
  template <typename S, typename R>
    S& move_assign_sequence(S& out, R& in)
    {
      static_assert(Input_range<R>(), "");
      auto p = bounded_move(in, out);
      if (p.first == in.end())
        out.erase(p.second, out.end());
      else
        out.insert(out.end(), 
                   make_move_iterator(p.first), 
                   make_move_iterator(in.end()));
      return out;
    }



  // Copy assign sequence
  //
  // The algorithm implements fill assignment for sequence types.
  template <typename S>
    S& fill_assign_sequence(S& out, std::size_t n, const Value_type<S>& value)
    {
      auto p = bounded_fill_n(out.begin(), n, value);
      if (p.second == 0)
        out.erase(p.first, out.end());
      else
        out.insert(out.end(), p.second, value);
      return out;
    }

} // namespace origin

#endif
