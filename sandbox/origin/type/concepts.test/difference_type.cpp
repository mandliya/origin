// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#include <vector>
#include <string>

#include <origin/type/concepts.hpp>

using namespace std;
using namespace origin;

struct failure { };

int main()
{
  // Check adaptation for some integral types. It's the signed version of
  // the value.
  static_assert(Same<Difference_type<int>, int>(), "");
  static_assert(Same<Difference_type<unsigned int>, int>(), "");

  // For floating point types difference type is an identity operation.
  static_assert(Same<Difference_type<float>, float>(), "");
  static_assert(Same<Difference_type<double>, double>(), "");
  static_assert(Same<Difference_type<long double>, long double>(), "");

  // For pointers and arrays, it's ptrdiff_t
  static_assert(Same<Difference_type<int*>, ptrdiff_t>(), "");
  static_assert(Same<Difference_type<int const*>, ptrdiff_t>(), "");
  static_assert(Same<Difference_type<int[3]>, ptrdiff_t>(), "");

  
  // Containers...
  using Vec = vector<int>;
  static_assert(Has_difference_type<Vec>(), "");
  static_assert(Same<Difference_type<Vec>, ptrdiff_t>(), "");

  // ... and their iterators
  using Iter = Vec::iterator;
  static_assert(Has_difference_type<Iter>(), "");
  static_assert(Same<Difference_type<Iter>, ptrdiff_t>(), "");

  static_assert(!Has_difference_type<failure>(), "");
}

