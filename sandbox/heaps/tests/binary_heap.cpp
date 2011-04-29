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
  void print(Seq const& seq, bool nl = true)
  {
    for(auto i : seq)
      cout << i << " ";
    if(nl)
      cout << "\n";
  }

int main()
{
  default_random_engine eng(time(0));
  uniform_int_distribution<> dist{0, 100};
  auto var = bind(dist, eng);

//   check_non_mutable_heap<binary_heap<int, greater<int>>>(var);
//   check_non_mutable_heap<mutable_binary_heap<int, greater<int>>>(var);

  // Check the mutable version.
  {
    typedef vector<int>::iterator Iter;
    typedef decltype(var) Gen;

    vector<int> v(10);

    // Generate initial random data.
    generate<Iter, Gen&>(v.begin(), v.end(), var);

    // Construct the initial heap. This is going to be a set of integers
    // referring to indices in the original vector.
    auto less = [&v](size_t a, size_t b) { return v[a] < v[b]; };
    mutable_binary_heap<size_t, decltype(less)> h{less};
    for(size_t i = 0; i < v.size(); ++i) 
      h.push(i);
    
      
    // Modify values
    uniform_int_distribution<size_t> idist{0, v.size() - 1};
    auto index = bind(idist, eng);
    for(int i = 0; i < 10; ++i) {
      int n = index();
      print(h.data(), false); cout << " | "; print(v);
      v[n] = var();
      h.update(n);
      print(h.data(), false); cout << " | "; print(v);
      cout << "~~~~~~~~\n";

      // Create a copy of the heap and make sure that it's sorted by copying
      // its results out to a new vector.
      /*
      mutable_binary_heap<size_t, decltype(less)> h2 = h;
      vector<int> v2(v.size());
      for(size_t i = 0; i < v.size(); ++i, h2.pop())
        v2[i] = v[h2.top()];
      print(v2);
      assert(( is_sorted(v2.begin(), v2.end(), greater<int>{}) ));
      */
    }
  }
}
