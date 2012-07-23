// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#include <iostream>
#include <string>

#include <origin/type/testing.hpp>

using namespace std;
using namespace origin;
using namespace origin::testing;

int main()
{
  context cxt;

  // Check relation properties
  std::equal_to<int> eq;
  check_reflexive(cxt, eq);
  check_symmetric(cxt, eq);
  check_transitive(cxt, eq);
  check_equivalence(cxt, eq);

  std::less<int> lt;
  check_irreflexive(cxt, lt);
  check_asymmetric(cxt, lt);
  check_antisymmetric(cxt, lt);
  check_strict_ordering(cxt, lt);
  check_strict_weak_ordering(cxt, lt);
  check_strict_total_ordering(cxt, lt);


  // Concepts

  // Equality comparable
  check_equality_comparable<short>(cxt);
  check_equality_comparable<int, long>(cxt);
  // check_equality_comparable<string, const char*>(cxt);  

  // Weakly ordered 
  check_weakly_ordered<int>(cxt);
  check_weakly_ordered<int, long>(cxt);
  // check_weakly_ordered<string, const char*>(cxt);

  // Totally ordered
  check_totally_ordered<int>(cxt);
  check_totally_ordered<int, long>(cxt);
  // check_totally_ordered<string, const char*>(cxt);


  // Move
  check_move_constructible<int>(cxt);
  check_movable<int>(cxt);

  // Copy
  check_copy_constructible<int>(cxt);
  check_copyable<int>(cxt);

  // Default constructible.
  check_default_constructible<int>(cxt);


  // Semiregular and regular
  check_semiregular<int>(cxt);
  check_regular<int>(cxt);


  return cxt.failures();
}
