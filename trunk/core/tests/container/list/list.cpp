// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#include <iostream>

#include <origin/list.hpp>
#include <origin/container/testing.hpp>

using namespace origin;
using namespace origin::testing;

template <typename R>
  void print(const R& range)
  {
    for (auto x : range)
      std::cout << x << ' ';
    std::cout << '\n';
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

  // Static properties of lists.
  static_assert(Bidirectional_iterator<L::iterator>(), "");
  static_assert(Bidirectional_iterator<L::const_iterator>(), "");

  // Prototype values
  L l1 {1, 2, 3, 4, 5};
  L l2 {1, 2, 3};

  // Container tests
  using C = container<L>;
  C::check_default_ctor();
  
  C::check_copy_ctor(l1);
  C::check_copy_assign(l1, l2);
  
  C::check_move_ctor(l1);
  C::check_move_assign(l1, l2);
  
  C::check_init_list_ctor({1, 2, 3, 4, 5});
  C::check_init_list_assign({1, 2, 3, 4, 5}, l2);

  C::check_swap(l1, l2);
  C::check_clear(l1);

  // Sequence tests
  using S = sequence<L>;
  S::check_insert(l1);
  S::check_erase(l1);

  // TODO: Write tests for push/pop
}
