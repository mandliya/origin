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


int main() 
{
  {
    allocator alloc;
    int *a = allocate<int>(alloc, 10);
    for(int i = 0; i < 10; ++i)
      std::cout << a[i] << ' ';
    std::cout << '\n';
    deallocate(alloc, a, 10);
  }
}
