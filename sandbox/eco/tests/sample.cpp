// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <algorithm>

#include <origin/random.hpp>
#include <origin/geometry.hpp>

using namespace std;
using namespace origin;

template<typename Iter>
  void print(Iter first, Iter last)
  {
    for( ; first != last; ++first)
      std::cout << *first << ' ';
    std::cout << '\n';
  }

template<typename R>
  void print(R const& r)
  {
    print(begin(r), end(r));
  }



int main()
{
  minstd_rand rng;
  rng.seed(time(0));

  // A sample vector.
  vector<int> v = {0, 1, 2, 3, 4, 5, 6, 7, 8};
  
  // Generate large numbers of increasing large samples
  constexpr size_t iters = 1000;
  for(size_t i = 2; i < v.size(); ++i) {
    for(size_t j = 0; j < iters; ++j) {
      vector<int> s(i);
      random_sample(v.begin(), v.end(), s.begin(), i, rng);
      
      print(s);
    }
  }
  
}
