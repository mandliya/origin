// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>

#include <origin/utility/typestr.hpp>
#include <origin/matrix.hpp>
#include <origin/random.hpp>
#include <origin/geometry.hpp>
#include <origin/statistics.hpp>

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

  typedef matrix<double, 1> Vector;
  typedef vector<Vector> Data;
  
  Data data;
  
  // FIXME: Is there not a more graceful way to accomplish this? Probably.
  string line;
  while(getline(cin, line)) {
    // Read the line of data.
    vector<double> vec;
    stringstream str(line);
    double value;
    while(str >> value) {
      vec.push_back(value);
    }
    
    // Build the vector and put it in the data set.
    Vector v(vec.begin(), vec.end());
    data.push_back(move(v));
  }

  typedef euclidean_distance_of<Vector> Dist;

  // Generate large numbers of increasing large samples
  // NOTE: We don't pick samples of size 1 (nnd won't work), size 2 (measures
  // are symmetric), or size of the data set (only one way to sample the
  // data set).
  cout << "size nn_min nn_max nn_mean nn_sd c_min c_max c_mean c_sd\n";
  constexpr size_t iters = 1;
  for(size_t i = 3; i < data.size(); ++i) {
    for(size_t j = 0; j < iters; ++j) {
      // Choose a random sample of size i
      Data sample(i);
      random_sample(data.begin(), data.end(), sample.begin(), i, rng);

      // Compute nearest neighbor distances for each element in the sample,
      // and observe summary.
      vector<double> nnd(i);
      nearest_neighbor_distances(sample.begin(), sample.end(), nnd.begin(), Dist{});
      auto nn_mm = minmax_element(nnd.begin(), nnd.end());
      double nn_min = *nn_mm.first;
      double nn_max = *nn_mm.second;
      double nn_mean = arithmetic_mean(nnd.begin(), nnd.end());
      double nn_sd = sample_standard_deviation(nnd.begin(), nnd.end(), nn_mean);

      // Compute distance to center for each element in the sample and observe
      // summary statistics.
      vector<double> cd(i);
      Vector c = centroid(sample.begin(), sample.end());
      distance_to(sample.begin(), sample.end(), c, cd.begin(), Dist{});
      double c_mm = minmax_element(cd.begin(), cd.end());
      double c_min = *c_mm.first;
      double c_max = *c_mm.second;
      double c_mean = arithmetic_man(cd.begin(), cd.end());
      double c_sd = sample_standard_deviation(cd.begin(), cd.end(), c_mean);

      cout << i << " " 
           << nn_min << " " << nn_max << " " << nn_mean << " " << nn_sd 
           << c_min << " " << c_max << " " << c_mean << " " << c_sd 
           << "\n";
    }
  }
}
