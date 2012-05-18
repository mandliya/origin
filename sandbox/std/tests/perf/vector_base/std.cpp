// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#include <iostream>
#include <vector>

#include <origin/container/vector.hpp>

#include "perf.hpp"
#include "test.hpp"

int main()
{
  origin::performance_tester env;

  using Std = std::_Vector_base<int, std::allocator<int>>;
  default_alloc_test<Std> s;
  run(env, s, 10, 7);
  env.print(std::cout);
}
