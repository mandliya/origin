// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

#include "util.hpp"

#include <origin/utility/typestr.hpp>
#include <origin/matrix.hpp>
#include <origin/random.hpp>
#include <origin/geometry.hpp>
#include <origin/statistics.hpp>

#include <boost/lexical_cast.hpp>


using namespace std;
using namespace origin;

int main(int argc, char **argv)
{
  minstd_rand rng;
  rng.seed(time(0));

  typedef vector<double> Vector;
  typedef vector<Vector> Data;

  // Parse some command line options.
  string wfile;
  bool replace = true;
  size_t iters = 10;
  for(int i = 1; i < argc; ++i) {
    if(argv[i] == string{"-samples"}) {
      iters = boost::lexical_cast<size_t>(argv[++i]);
    } 
    else if(argv[i] == string{"-weight"}) {
      wfile = argv[++i];
    } else {
      cerr << "command line usage error\n";
    }
  }
  
  // Read the population matrix
  Data data;
  read_matrix(std::cin, data);
  
  // Read weighting vector if given. Otherwise, the weighting will be all 1's.
  Vector w;
  if(!wfile.empty()) {
    ifstream win(wfile);
    read_vector(win, w);
  }

  // The weighted distance function.
  auto dist = [&w](Vector const& a, Vector const& b) -> double {
    return weighted_euclidean_distance(a, b, w);
  };

  cout << "size min max mean sd\n";
  cout.precision(4);

  // Generate large numbers of increasing large samples
  // NOTE: Don't pick samples of size < 3. Data isn't very meaningful.
  for(size_t i = 3; i < data.size(); ++i) {
    for(size_t j = 0; j < iters; ++j) {
      // Choose a random sample of size i
      Data sample(i);
      random_sample(data.begin(), data.end(), sample.begin(), i, rng);
      
      // Compute nearest neighbor distances for each element in the sample,
      // and observe summary.
      Vector nnd(i);
      nearest_neighbor_distances(sample.begin(), sample.end(), nnd.begin(), dist);
      auto mm = minmax_element(nnd.begin(), nnd.end());
      double min = *mm.first;
      double max = *mm.second;
      double mean = arithmetic_mean(nnd.begin(), nnd.end());
      double sd = sample_standard_deviation(nnd.begin(), nnd.end(), mean);

      cout << i << " " << min << " " << max << " " << mean << " " << sd << "\n";
    }
  }
}
