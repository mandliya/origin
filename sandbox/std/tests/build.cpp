// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#include <iostream>

#include <origin/memory.hpp>

using namespace origin;

int main()
{
  int *p = allocate<int>(get_default_allocator(), 5);
  deallocate<int>(get_default_allocator(), p);
}
