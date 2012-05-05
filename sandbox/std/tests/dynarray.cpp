// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#include <cassert>

#include <origin/dynarray.hpp>

using namespace origin;

int main()
{
  dynarray_base<int> b1;
  assert(b1.size() == 1);

  dynarray_base<int> b2(10);
  assert(b2.size() == 2);
}
