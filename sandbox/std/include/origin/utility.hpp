// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_UTILITY_HPP
#define ORIGIN_UTILITY_HPP

#include <origin/utility/typestr.hpp>



// The assume macro is used in conjunction with assert. It is used to state
// a preconditon that is not intended to be evaluated or even compiled. This is 
// useful for documenting uncheckable preconditions or those that would induce
// complex dependencies, etc.
#define assume(X)



namespace origin
{
  // Default type (utility)  
  // The default_t type is a tag class used to indicate the selection of a
  // default value. This is only used to support class template specialization.
  struct default_t { };

  

  // Unspecified type (utility)
  // The unspecified_t type is a tag class used to indicate that an argument
  // for a template parameter has not been specified.
  struct unspecified_t { };

  

  // Empty type (utility)
  // The empty type is an empty, trivial type that is meant to be used as a
  // placeholder for unspecified types in containers or other data structures.
  struct empty_t { };


} // namespace origin

#endif
