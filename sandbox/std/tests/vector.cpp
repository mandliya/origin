// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#include <cassert>
#include <iostream>
#include <vector>

#include <origin/utility.hpp>
#include <origin/container/vector.hpp>

using namespace origin;


template <typename R>
  void print(const R& range)
  {
    for (auto x : range)
      std::cout << x << ' ';
    std::cout << '\n';
  }


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
    assert((c == {3, 3, 3, 3, 3}));
  }

template <typename C>
  void test_init_list()
  {
    C c {1, 2, 3, 4, 5};
    assert((c == {1, 2, 3, 4, 5}));
  };


template <typename C>
  void test_move_init()
  {
    C a {1, 2, 3};
    C b = std::move(a);
    assert(a.empty());
    assert(a.capacity() == 0);
    assert((b == {1, 2, 3}));
  }

template <typename C>
  void test_move_assign()
  {
    C a {1, 2, 3};
    C b;
    b = std::move(a);
    assert(a.empty());
    assert((b == {1, 2, 3}));
  }

template <typename C>
  void test_copy_init()
  {
    C a {1, 2, 3};
    C b {a};
    assert(a == b);
  }

template <typename C>
  void test_copy_assign()
  {
    C a {1, 2, 3};
    C b;
    b = {a};
    assert(a.size() == b.size());
    assert(a == b);
  }

template <typename C>
  void test_iter_init()
  {
    auto list = {1, 2, 3, 4};
    C c(list.begin(), list.end());
    assert(c == list);
  }

template <typename C>
  void test_range_init()
  {
    vector<short> a {1, 2, 3};
    C c {a};
    assert(c == a);
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


// FIXME: It would be nice if I actually asserted these properties.
template <typename V>
  void test_insert()
  {
    V v1 {1, 2, 3};
    v1.reserve(10);

    v1.insert(v1.end(), 4);
    assert((v1 == {1, 2, 3, 4}));;

    v1.insert(v1.begin() + 1, -1);
    assert((v1 == {1, -1, 2, 3, 4}));

    v1.insert(v1.begin() + 3, 2, -2);
    assert((v1 == {1, -1, 2, -2, -2, 3, 4}));

    v1.insert(v1.end(), 5, -5);
    assert((v1 == {1, -1, 2, -2, -2, 3, 4, -5, -5, -5, -5, -5}));
  }

template <typename V>
  void test_erase()
  {
    V v {1, 2, 3, 4, 5};
    
    auto i1 = v.erase(v.begin());
    assert(*i1 == 2);
    print(v);

    auto i2 = v.erase(v.end() - 1);
    assert(i2 == v.end());
    print(v);

    auto i3 = v.erase(v.begin() + 1);
    assert(*i3 == 4);
    print(v);
  }

template <typename V>
  void test_erase_range()
  {
    V v {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    auto i = v.erase(v.begin() + 2, v.end() - 2);
    print(v);
    assert(*i == 9);
  }


int main() 
{
  using V = vector<int>;

  {
    // Insert at the end of an empty vector (without resizing).
    vector<int> v;
    v.reserve(1024);
    v.insert(v.end(), {1, 2, 3, 4, 5});
  }

  {
    // Insert at the front of an empty vector (without resizing).
    vector<int> v;
    v.reserve(1024);
    v.insert(v.begin(), {1, 2, 3});
    print(v);
  }

  {
    // Insert more elemnts at the front of a vector than there are in
    // the remainder of the vector (without resizing).
    vector<int> v;
    v.reserve(1024);
    v.insert(v.end(), {1, 2, 3});
    v.insert(v.begin(), {-1, -2, -3, -4,-5});
    print(v);
  }

  test_default_init<V>();
  test_fill<V>();
  test_init_list<V>();

  test_move_init<V>();
  test_move_assign<V>();
  test_copy_init<V>();
  test_copy_assign<V>();

  test_iter_init<V>();
  test_range_init<V>();

  test_reserve<V>();
  test_resize<V>();

  test_push_pop_back<V>();
  test_insert<V>();
  test_erase<V>();
  test_erase_range<V>();
}
