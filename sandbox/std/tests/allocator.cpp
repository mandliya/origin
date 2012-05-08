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
#include <origin/memory.hpp>

using namespace origin;


// FIXME: Move the pointer tests into a separate facility.

template <typename Ptr>
  void test_convert_to_void()
  {
    using Void_ptr = Rebind_pointer<Ptr, void>;
    Ptr p = nullptr;
    Void_ptr vp = p;
    assert(p == vp);
    assert(vp == p);
    assert(p == nullptr);
    assert(vp == nullptr);
  }

template <typename Ptr>
  void test_convert_to_const()
  {
    using Const_ptr = Rebind_pointer<Ptr, const Element_type<Ptr>>;
    Ptr p = nullptr;
    Const_ptr cp = p;
    assert(p == cp);
    assert(cp == p);
    assert(p == nullptr);
    assert(cp == nullptr);
  }

struct base { };
struct derived : base { };

void test_convert_to_base()
{
  normal_ptr<derived> p = nullptr;
  normal_ptr<base> q = p;
  assert(p == q);
  assert(p == nullptr);
  assert(q == nullptr);

  normal_ptr<const base> cq = p;
  assert(p == cq);
  assert(cq == nullptr);
}



int main() 
{
  // {
  //   allocator alloc;
  //   int *a = allocate<int>(alloc, 10);
  //   for(int i = 0; i < 10; ++i)
  //     std::cout << a[i] << ' ';
  //   std::cout << '\n';
  //   deallocate(alloc, a, 10);
  // }

  {
    test_convert_to_void<int*>();
    test_convert_to_const<int*>();

    test_convert_to_void<normal_ptr<int>>();
    test_convert_to_const<normal_ptr<int>>();

    test_convert_to_base();
  }

  {
    normal_allocator alloc;
    auto p = allocate<int>(alloc, 3);
    std::cout << *p << '\n';
    deallocate(alloc, p, 3);
  }
}
