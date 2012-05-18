// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#include <iostream>

#include <origin/container/vector.hpp>

#include "perf.hpp"
#include "test.hpp"

using namespace origin;

int main()
{
  // vector<int> v;
  // for(int i = 0; i < 10; ++i) {
  //   v.push_back(i);
  // }
  
  performance_tester env;
  exec<vector<int>>(env);
}
