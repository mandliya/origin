// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#include <iostream>

#include <origin/utility/typestr.hpp>
#include <origin/iterator/algorithm.hpp>

template<typename Iter>
  void print(Iter f, Iter l)
  {
    using namespace origin;
    while(f != l) {
      std::cout << get(f) << ' ';
      f = next(f);
    }
    std::cout << '\n';
  }

using origin::typestr;

void test_find_first_of() 
{
  int data[] = {5, 1, 3, 7, 8};
  int search[] = {7, 8};

  auto i = origin::find_first_of(data, data + 5, search, search + 2);
  assert(( origin::distance(data, i) == 3));
}


int main()
{
  using namespace origin;

  int tmp[] = {5, 1, 3, 7, 8};
  int tmp2[] = {7, 8};
  stable_sort(tmp, tmp + 5);
  print(tmp, tmp + 5);
  
  find(tmp, tmp + 5, 1u);
  

  // Run actual tests
  test_find_first_of();
}
