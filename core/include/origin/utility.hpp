// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_UTILITY_HPP
#define ORIGIN_UTILITY_HPP

#include <cassert>

// The assume macro is used in conjunction with assert. It is used to state
// a preconditon that is not intended to be evaluated or even compiled. This is 
// useful for documenting uncheckable preconditions or those that would induce
// complex dependencies, etc.
#define assume(X)

#include <origin/utility/typestr.hpp>

#endif
