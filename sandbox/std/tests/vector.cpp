// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#include <cassert>
#include <iostream>
#include <type_traits>

#include <origin/utility.hpp>
#include <origin/container/vector.hpp>

using namespace origin;

template <typename C>
  void test_default_init()
  {
    C c;
    assert(c.empty());
    assert(c.size() == 0);
    assert(c.capacity() == 0);
  }

template <typename C>
  void test_fill()
  {
    C c(5, 3);
    for (auto x : c)
      std::cout << x << ' ';
    std::cout << '\n';
  }

template <typename C>
  void test_init_list()
  {
    C c {1, 2, 3, 4, 5};
    for (auto x : c)
      std::cout << x << ' ';
    std::cout << '\n';
  };


template <typename C>
  void test_move_init()
  {
    C a {1, 2, 3};
    C b = std::move(a);
    assert(a.empty());
    assert(a.capacity() == 0);

    for (auto x : b)
      std::cout << x << ' ';
    std::cout << '\n';
  }

template <typename C>
  void test_move_assign()
  {
    C a {1, 2, 3};
    C b;
    b = std::move(a);
    assert(a.empty());
    assert(a.capacity() == 0);

    for (auto x : b)
      std::cout << x << ' ';
    std::cout << '\n';
  }

template <typename C>
  void test_copy_init()
  {
    C a {1, 2, 3};
    C b {a};
    assert(a.size() == b.size());
    assert(a[0] == b[0]);
    assert(a[1] == b[1]);
    assert(a[2] == b[2]);
  }

template <typename C>
  void test_copy_assign()
  {
    C a {1, 2, 3};
    C b;
    b = {a};
    assert(a.size() == b.size());
    assert(a[0] == b[0]);
    assert(a[1] == b[1]);
    assert(a[2] == b[2]);
  }


template <typename V>
  void test_reserve()
  {
    V v;
    v.reserve(20);
    assert(v.capacity() == 20);
    assert(v.size() == 0);

    // Don't shrink capacity.
    v.reserve(10);
    assert(v.capacity() == 20);
    assert(v.size() == 0);
  }

template <typename V>
  void test_resize()
  {
    V v;
    v.resize(3, 1);
    assert(v.size() == 3);
    for (auto x : v)
      std::cout << x << ' ';
    std::cout << '\n';
    
    v.resize(6, 2);
    assert(v.size() == 6);
    for (auto x : v)
      std::cout << x << ' ';
    std::cout << '\n';

    v.resize(2);
    assert(v.size() == 2);
    for (auto x : v)
      std::cout << x << ' ';
    std::cout << '\n';
  }

template <typename V>
  void test_push_pop_back()
  {
    V v;
    for (int i = 0; i < 5; ++i)
      v.push_back(i);
    assert(v.size() == 5);
    assert(v.capacity() > 5);

    for (int i = 0; i < 5; ++i) {
      std::cout << v.back() << ' ';
      v.pop_back();
    }
    std::cout << '\n';
    assert(v.empty());
    assert(v.capacity() > 5);
  }

int main() 
{
  using V = vector<int>;

  test_default_init<V>();
  test_fill<V>();
  test_init_list<V>();

  test_move_init<V>();
  test_move_assign<V>();
  test_copy_init<V>();
  test_copy_assign<V>();

  test_reserve<V>();
  test_resize<V>();

  test_push_pop_back<V>();
}
