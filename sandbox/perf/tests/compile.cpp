// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#include <cstdlib>
#include <iostream>

#include <origin/perf.hpp>

using namespace std;
using namespace origin;

int main()
{
  perf::timer<std::chrono::system_clock> timer;
  ::sleep(2);
}
