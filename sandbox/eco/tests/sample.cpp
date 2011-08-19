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


// Return the euclidean distance between two numbers.
template<typename T>
  T euclidean_distance(T const& a, T const& b)
  {
    return abs(a - b);
  }

// Return the euclidean distance between two n-vectors
template<typename T, typename Alloc>
  double euclidean_distance(vector<T, Alloc> const& a, vector<T, Alloc> const& b)
  {
    assert(( a.size() == b.size() ));
    double di = 0.0;
    for(size_t i = 0; i < a.size(); ++i) {
      di += (a[i] - b[i]) * (a[i] - b[i]);
    }
    return sqrt(di);
  }
  
template<typename T>
  struct euclidean_distance_of
  {
    double operator()(T const& a, T const& b) const
    {
      return euclidean_distance(a, b);
    }
  };

int main()
{
  minstd_rand rng;
  rng.seed(time(0));

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
  Vector near(data.size());
  nearest_neighbor_distances(data.begin(), data.end(), near.begin(), Dist{});
  print(near);
  
  /*
  // Generate large numbers of increasing large samples
  constexpr size_t iters = 1000;
  for(size_t i = 2; i < v.size(); ++i) {
    for(size_t j = 0; j < iters; ++j) {
      vector<int> s(i);
      random_sample(v.begin(), v.end(), s.begin(), i, rng);
      print(s);
    }
  }
  */
}
