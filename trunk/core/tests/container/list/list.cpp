// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#include <iostream>

#include <origin/list.hpp>

using namespace origin;

template <typename R>
  void print(const R& range)
  {
    for (auto x : range)
      std::cout << x << ' ';
    std::cout << '\n';
  }


template <typename L>
  void test_default()
  {
    L l;
    assert(l.empty());
  }

template <typename L>
  void test_push_back()
  {
    L l;
    l.push_back(5);
    assert(!l.empty());
    assert(l.size() == 1);
    print(l);
    l.push_back(10);
    assert(l.size() == 2);
    print(l);
    l.push_back(15);
    assert(l.size() == 3);
    print(l);
  }

int main()
{
  list<int> l;
  test_default<list<int>>();

  test_push_back<list<int>>();
}
