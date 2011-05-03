// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef HEAP_SORT_HPP
#define HEAP_SORT_HPP

#include <cassert>
#include <algorithm>
#include <vector>
#include <random>

/**
 * Check the heap order on a copy of the given heap. This is done by comparing
 * each popped element to the previous using the heaps value comparison function
 */
template<typename Heap>
  void check_heap_order(Heap h)
  {
    // Empty heaps are valid.
    if(h.empty())
      return;
    

    // Compare each subsequent pair of elements popped from the heap. They
    // must not violate the order property of the heap.
    auto comp = h.value_comp();
    auto x = h.top();
    h.pop();
    while(!h.empty()) {
      auto y = h.top();
      h.pop();
      assert(( !comp(x, y) ));
      x = y;
    }
  }

/**
 * Check the validity of the (non-mutable) Heap implementation.
 */
template<template<typename...> class Heap, typename Eng>
  void check_heap(Eng& eng)
  {
    std::uniform_int_distribution<> dist{0, 100};
    auto var = bind(dist, eng);

    Heap<int> h;
    for(int i = 0; i < 100; ++i)
      h.push(var());
    check_heap_order(h);
  }



/**
 * Verify the behavior or a mutable heap. Here, the heap indirectly orders 
 * elements in a vector. The vector and heap are initialized, and then random 
 * elements are modified, and the heap updated.
 */
template<template<typename...> class Heap, typename Gen, typename Engine>
  void check_mutable_heap(Gen& var, Engine& eng)
  {
    typedef std::vector<int> Vector;
    typedef Vector::iterator Iter;
    typedef Vector::size_type Size;
    
    // The vector holds values to be modified and we will be comparing
    // them indirectly.
    Vector v(10);
    auto cmp = [&v](Size m, Size n) { return v[m] < v[n]; };

    // Generate an initial distribution of random data.
    std::generate<Iter, Gen&>(v.begin(), v.end(), var);

    // The mutable heap orders references to modifiable data by pointers. The
    // heap order is determined indirectly.
    typedef decltype(cmp) Comp;
    Heap<size_t, Comp> h{cmp};
    
    // Initialize the heap with references to the initialized data. Make sure
    // that we're 
    for(Size i = 0; i < v.size(); ++i)
      h.push(i);
    // check_heap_sorted(h);
    assert(( is_heap(h.data().begin(), h.data().end(), cmp) ));

    // Modify values and then check to make sure we're in sorted order.
    std::uniform_int_distribution<std::size_t> indexes{0, v.size() - 1};
    auto index = bind(indexes, eng);
    for(int i = 0; i < 1000; ++i) {
      // Randomly modify an element in the vector.
      size_t n = index();
      v[n] = var();
      
      // And then update the heap (and check the results).
//       std::cout << "Ha: "; print(h.data()); std::cout << "\n";
      h.update(n);
//       std::cout << "Hb: "; print(h.data()); std::cout << "\n~~~~~~~~~\n";
      assert(( is_heap(h.data().begin(), h.data().end(), cmp) ));
//       check_heap_sorted(v, h);
    }
  }

#endif