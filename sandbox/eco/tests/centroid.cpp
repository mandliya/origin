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

#include <boost/lexical_cast.hpp>

#include "util.hpp"

using namespace std;
using namespace origin;

int main(int argc, char **argv)
{
  minstd_rand rng;
  rng.seed(time(0));

  typedef vector<double> Vector;
  typedef vector<Vector> Data;
  
  // Parse some command line options.
  bool replace = true;
  size_t iters = 10;
  for(int i = 1; i < argc; ++i) {
    if(argv[i] == string{"-samples"}) {
      ++i;
      iters = boost::lexical_cast<size_t>(argv[i]);
    }
    else {
      cerr << "command line usage error\n";
    }
  }

  Data data;
  read_matrix(std::cin, data);

  cout << "size min max mean sd\n";
  cout.precision(4);

  // Capture the euclidean distance.
  auto dist = [](Vector const& a, Vector const& b) {
    return euclidean_distance(a, b);
  };

  // Generate large numbers of increasing large samples
  // NOTE: Don't pick samples of size < 3. For values < 3, stddev is 0 (or
  // very close to it).
  for(size_t i = 3; i < data.size(); ++i) {
    for(size_t j = 0; j < iters; ++j) {
      // Choose a random sample of size i
      Data sample(i);
      random_sample(data.begin(), data.end(), sample.begin(), i, rng);

      // Compute distance to center for each element in the sample and observe
      // summary statistics.
      vector<double> cd(i);
      Vector c = centroid(sample.begin(), sample.end());
      distance_to(sample.begin(), sample.end(), c, cd.begin(), dist);
      auto mm = minmax_element(cd.begin(), cd.end());
      double min = *mm.first;
      double max = *mm.second;
      double mean = arithmetic_mean(cd.begin(), cd.end());
      double sd = sample_standard_deviation(cd.begin(), cd.end(), mean);

      cout << i << " " << min << " " << max << " " << mean << " " << sd << "\n";
    }
  }
}
