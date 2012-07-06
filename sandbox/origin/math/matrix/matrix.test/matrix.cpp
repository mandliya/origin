// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#include <origin/math/matrix/matrix.hpp>


template <typename T>
  auto h(T x) -> decltype(*x) { return *x; }

template <typename T>
  auto g(T x) -> decltype(h(x)) { return h(x); }

template <typename T>
  auto f(T x) -> decltype(g(x)) { return g(x); };

int main()
{
  f(0);
}
