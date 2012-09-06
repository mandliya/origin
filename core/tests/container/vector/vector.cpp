// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#include <iostream>

#include <origin/vector.hpp>
#include <origin/container/testing.hpp>

using namespace origin;
using namespace origin::testing;

int main()
{
  using V = vector<int>;

  // Static properties of lists.
  static_assert(Random_access_iterator<V::iterator>(), "");
  static_assert(Random_access_iterator<V::const_iterator>(), "");

  // Prototype values
  V v1 {1, 2, 3, 4, 5};
  V v2 {1, 2, 3};

  // Container tests
  using C = container<V>;
  C::check_default_ctor();
  
  C::check_copy_ctor(v1);
  C::check_copy_assign(v1, v2);
  
  C::check_move_ctor(v1);
  C::check_move_assign(v1, v2);
  
  C::check_init_list_ctor({1, 2, 3, 4, 5});
  C::check_init_list_assign({1, 2, 3, 4, 5}, v2);

  C::check_swap(v1, v2);
  C::check_clear(v1);

  // Sequence tests
  using S = sequence<V>;
  S::check_resize_grow(v1);
  S::check_resize_shrink(v1);

  S::check_insert(v1);
  S::check_erase(v1);

  S::check_insert(v1);
  S::check_erase(v1);

  S::check_push_back(v1);
  S::check_pop_back(v1);
}
