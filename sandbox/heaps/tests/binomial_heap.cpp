// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#include <origin/heaps/binomial_heap.hpp>

#include "check_heap.hpp"

using namespace std;
using namespace origin;

int main()
{
  default_random_engine eng;
  check_heap<binomial_heap>(eng);
}
