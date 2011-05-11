// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#include <cstdlib>
#include <iostream>

#include <origin/utility/typestr.hpp>
#include <origin/perf.hpp>

using namespace std;
using namespace origin;


int main()
{
  perf::system_timing ts;
  
  {
    ORIGIN_SYS_TIMER(ts, test);
    ::sleep(1);
  }
  
  ts.print(cout);
}
