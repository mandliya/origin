// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#include <ctime>
#include <random>
#include <iostream>

#include <stream/stream.hpp>

using namespace std;
using namespace origin;

int main()
{
  minstd_rand prng (time(0));
  uniform_int_distribution<> dist {0, 100};
  auto gen = bind(dist, prng);


  igenstream<decltype(gen)> gs(gen);
  for (int i = 0; i < 10; ++i) {
    int n;
    gs >> n;
    cout << n << '\n';
  }
    
}
