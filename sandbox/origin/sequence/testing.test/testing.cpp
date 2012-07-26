// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.


#include <forward_list>
#include <iterator>
#include <iostream>
#include <list>
#include <vector>
#include <memory>

#include <origin/type/typestr.hpp>
#include <origin/sequence/concepts.hpp>

using namespace std;
using namespace origin;

int main()
{
  // FIXME: All of the iterator tests should be in iterator.test, not
  // testing.test. This directory is for testing actual test components.

  // An input iterator is input, but not output and not forward.
  using In = istream_iterator<int>;
  static_assert(Input_iterator<In>(), "");
  static_assert(!Output_iterator<In, int>(), "");
  static_assert(!Forward_iterator<In>(), "");

  // An output iterator is output but not readable.
  using Out = ostream_iterator<int>;
  static_assert(Output_iterator<Out, int>(), "");
  static_assert(!Readable<Out>(), "");

  // A forward iterator is input and forward but not bidirectional.
  using F = forward_list<int>::iterator;
  static_assert(Input_iterator<F>(), "");
  static_assert(Forward_iterator<F>(), "");
  static_assert(!Bidirectional_iterator<F>(), "");

  // A bidirectional iterator is input, forward, and bidi but not random.
  using L = list<int>::iterator;
  static_assert(Input_iterator<L>(), "");
  static_assert(Forward_iterator<L>(), "");
  static_assert(Bidirectional_iterator<L>(), "");
  static_assert(!Random_access_iterator<L>(), "");

  // A random access iterator is input, forward, bidi, and random.
  using V = vector<int>::iterator;
  static_assert(Input_iterator<V>(), "");
  static_assert(Forward_iterator<V>(), "");
  static_assert(Bidirectional_iterator<V>(), "");
  static_assert(Random_access_iterator<V>(), "");

}
