// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef HEAP_SORT_HPP
#define HEAP_SORT_HPP

#include <algorithm>
#include <functional>

/**
 * Run heap sort building and then rewriting the original sequence by
 * popping all elements in the heap.
 */
template<typename Heap, typename Vector>
  void heap_sort(Heap& h, Vector& v)
  {
    // Create a max heap by default (uses std::less).
    for(auto const& i : v)
      h.push(i);

    for(size_t i = 0; i < v.size(); ++i, h.pop())
      v[i] = h.top();
  }

/**
 * Run heap sort building and then rewriting the original sequence by
 * popping all elements in the heap. Check that the heap is, in fact sorted.
 */
template<typename Heap, typename Vector>
  void check_heap_sort(Vector& v)
  {
    Heap h;
    heap_sort(h, v);
    assert(( std::is_sorted(v.begin(), v.end(), std::less<int>{}) ));
  }


template<typename Heap, typename Gen>
  void check_non_mutable_heap(Gen var)
  {
    typedef std::vector<int>::iterator Iter;
    std::vector<int> v(1000);
    for(int i = 0; i < 100; ++i) {
      std::generate<Iter, Gen&>(v.begin(), v.end(), var);
      check_heap_sort<Heap>(v);
    }
  }

#endif