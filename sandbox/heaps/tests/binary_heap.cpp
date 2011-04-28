// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

// The purpose of this file is to simply ensure that all header files in the
// heaps library compile without warnings or errors.

#include <cassert>
#include <vector>
#include <queue>
#include <functional>
#include <algorithm>
#include <random>
#include <iostream>

#include <origin/heaps/binary_heap.hpp>

#include "heap_sort.hpp"

using namespace std;
using namespace origin;

template<typename Seq>
  void print(Seq const& seq)
  {
    for(auto i : seq)
      cout << i << " ";
    cout << "\n";
  }

int main()
{
  default_random_engine eng(time(0));
  uniform_int_distribution<> dist{0, 100};
  auto var = bind(dist, eng);

  check_non_mutable_heap<priority_queue<int, vector<int>, greater<int>>>(var);


  check_non_mutable_heap<binary_heap<int, greater<int>>>(var);


  check_non_mutable_heap<mutable_binary_heap<int, greater<int>>>(var);

  // Check the mutable version.
  /*
  {
    typedef vector<int>::iterator Iter;
    typedef decltype(var) Gen;

    vector<int> v(10);
    for(int i = 0; i < 1000; ++i) {
      generate<Iter, Gen&>(v.begin(), v.end(), var);

      print(v);
      mutable_binary_heap<int> h;
      heap_sort(h, v);
      print(v);
      assert(( is_sorted(v.begin(), v.end(), greater<int>{}) ));
    }
  }
  */
}
