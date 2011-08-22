// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>

#include <origin/utility/typestr.hpp>
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
  
// TODO: Build a library of distance functions for various kinds of 
// abstractions: Numbers, Vectors, Tuples, others?


int main()
{
  minstd_rand rng;
//   rng.seed(time(0));

  typedef vector<double> Vector;
  typedef vector<Vector> Data;
  
  Data data;
  
  // FIXME: Is there not a more graceful way to accomplish this? Probably.
  string line;
  while(getline(cin, line)) {
    Vector v;
    stringstream str(line);
    double value;
    while(str >> value) {
      v.push_back(value);
    }
    data.push_back(move(v));
  }

  typedef euclidean_distance_of<Vector> Dist;

  // Generate large numbers of increasing large samples
  constexpr size_t iters = 1;
  for(size_t i = 3; i < data.size(); ++i) {
    for(size_t j = 0; j < iters; ++j) {
      // Choose a random sample of size i
      Data sample(i);
      random_sample(data.begin(), data.end(), sample.begin(), i, rng);

      // Compute nearest neighbor distances for each element in the sample.
      Vector nnd(i);
      nearest_neighbor_distances(sample.begin(), sample.end(), nnd.begin(), Dist{});
      // print(nnd);
      
      // FIXME: Do we compute summary statistics for each sample and then
      // compute summary stats for all of those? Maybe it's just better to
      // dump summary stats of NND for each sample and let the user figure
      // out what to do with it.
      
      // Compute summary statistics on nnd.
      auto mm = minmax_element(nnd.begin(), nnd.end());
      double min = *mm.first;
      double max = *mm.second;
      cout << min << " " << max << "\n";
      
      // FIXME: Implement mean and stddev.
    }
  }
}
