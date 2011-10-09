// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <numeric>

#include <origin/random.hpp>

using namespace std;
using namespace origin;

minstd_rand rng;  // Global random number generator.

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

template<typename Vector, typename Sample, typename Size>
  void sample(Vector const& v, Sample& s, Size n)
  {
    random_sample(v.begin(), v.end(), s.begin(), n, rng);
  }

template<typename Sample, typename Hist>
  void count(Sample const& s, Hist& h)
  {
    for(auto const& x : s)
      ++h[x];
  }

template<typename Vector, typename Hist, typename Size>
  void check_sample(Vector const& v, Hist& h, Size n)
  {
    vector<int> s(n);
    sample(v, s, n);
    count(s, h);
  }

int main()
{
  rng.seed(time(0));

  // A sample vector for testing.
  vector<int> v = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  
  // Generate large numbers of increasing large samples
  constexpr size_t iters = 1000;
  for(size_t i = 2; i < v.size(); ++i) {
    
    // Compute a bunch of i-samples and record how many times each value is
    // included in the sample.
    vector<int> h(10, 0);
    for(size_t j = 0; j < iters; ++j) {
      check_sample(v, h, i);
    }
    
    // How often did we see each value? Should be close to i*iters / 10.
    print(h);
  }
}
