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
  void test_insert()
  {
    L l;
    l.insert(l.end(), 1);
    print(l);
    auto i = l.insert(l.end(), 2);
    l.insert(l.begin(), 0);
    print(l);
    l.insert(i, -2);
    print(l);
  }

template <typename L>
  void test_push_back()
  {
    L l;
    for (int i : range(5))
      l.push_back(i);
    print(l);
  }

template <typename L>
  void test_push_front()
  {
    L l;
    for (int i : range(5))
      l.push_front(i);
    print(l);
  }

template <typename L>
  void test_erase()
  {
    // Erase the 1st e elements
    L l{1, 2, 3, 4, 5};
      l.erase(l.begin());
    print(l);
  }

template <typename L>
  void test_erase_range()
  {
    L l{1, 2, 3, 4, 5};
    l.erase(l.begin(), l.end());
    assert(l.empty());
  }


int main()
{
  using L = list<int>;
  test_default<L>();

  test_insert<L>();
  test_push_back<L>();
  test_push_front<L>();

  // test_erase<L>();
  test_erase_range<L>();
}
