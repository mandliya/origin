// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <algorithm>

#include <origin/container/vector.hpp>

using Clock = std::chrono::system_clock;

std::minstd_rand eng;

template <typename R>
  void print(const R& range)
  {
    for (auto x : range)
      std::cout << x << ' ';
    std::cout << '\n';
  }

// Returns the iterator in front of which value should be inserted. Note that
// I'm not using std::find to avoid implementation bias.
template <typename Vec, typename T>
  auto insert_at(Vec& v, const T& value) -> decltype(v.begin())
  {
    auto i = v.begin();
    auto end = v.end();
    while (i != end) {
      if (!(*i < value))
        return i;
      ++i;
    }
    return end;
  }

int x;

template <typename Vec>
  long test(std::size_t n)
  {
    std::uniform_int_distribution<> dist (0, n);
    auto start = Clock::now();

    /*
    Vec v;
    for (std::size_t i = 0; i < n; ++i) {
      int num = dist(eng);
      auto pos = insert_at(v, num);
      v.insert(pos, num);
    }
    */

    auto gen = std::bind(dist, eng);
    Vec v(n);
    std::generate(v.begin(), v.end(), gen);
    std::sort(v.begin(), v.end());

    auto stop = Clock::now();
    return (stop - start).count();
  }

int main() 
{
  using Origin = origin::vector<int>;
  using Std = std::vector<int>;

  // Run a test to make sure everything is fully initialized.
  eng.seed(time(0));
  test<Std>(5);


  // FIXME: There's a curious thing happening here that I'm going to call the
  // "second run" bias. Whatever test is run first seems to perform better
  // than its equivalent.
  for(int i = 10; i <= 100000; i *= 10) {
    double s = test<Std>(i);
    double o = test<Origin>(i);
    std::cout << o << '\t' << s << '\t' << o / s << '\n';
  }
}
