// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_RANGE_CONCEPTS_HPP
#define ORIGIN_RANGE_CONCEPTS_HPP

#include <origin/iterator/concepts.hpp>

namespace origin
{
// Include traits
#include "impl/traits.hpp"

  //////////////////////////////////////////////////////////////////////////////
  // Range Traits                                                   range.traits
  //
  // The begin and end result aliases deduces the type of the iterators i and j
  // in the following program:
  //
  //    using std::begin;
  //    using std::end;
  //    auto i = begin(r);
  //    auto j = end(r);
  //
  // Note that the type referred to by these alias may depends on type of R.
  // If R is const qualified, then the resulting iterator may be a const
  // iterator.


  // Refers to the result of the expression std::begin(r) or end(r).
  template <typename R>
    using Begin_result = typename range_impl::get_begin_result<R>::type;

  // Returns true if either std::begin(r) or begin(r) is a valid expression.
  template <typename R>
    constexpr bool Has_begin()
    {
      return Subst_succeeded<R>();
    }


  // Refers to the result of the expressoin std::end(r) or end(r).
  template <typename R>
    using End_result = typename range_impl::get_end_result<R>::type;

  // Returns true if either std::end(r) or end(r) is a valid expression.
  template <typename R>
    constexpr bool Has_end()
    {
      return Subst_succeeded<R>();
    }



  //////////////////////////////////////////////////////////////////////////////
  // Iterator Of                                               range.iterator_of
  //
  // The iterator type of a range is the type returned by both begin(r) and 
  // end(r) for some range(r). Note that the type referred to this alias
  // may be different for R and const R. However, the alias is always defined
  // for both.
  //
  // Aliases:
  //    The type returned by both begin and end operations of a range.
  template <typename R>
    using Iterator_of = Begin_result<R>;



  //////////////////////////////////////////////////////////////////////////////
  // Range Concepts                                               range.concepts
  //
  // A range is type that exposes an underlying iterator that is accessible
  // through the begin and end operations. 

  template <typename R>
    constexpr bool Range()
    {
      return Has_begin<R>()
          && Has_end<R>()
          && Same<Begin_result<R>, End_result<R>>()
          && Iterator<Iterator_of<R>>();
    }


} // namespace origin

#endif
