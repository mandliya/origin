// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#include <cassert>
#include <iostream>
#include <string>

#include <origin/traits.hpp>

using namespace origin;

int main()
{
  assert(( Common<int, char>() && Same<Common_type<int, char>, int>() ));
  assert(( Common<int, double>() && Same<Common_type<int, double>, double>() ));
  assert(( !Common<int, std::ostream>() ));

  // FIXME: Write more common type tests.
  
  // FIXME: This is known to fail.
  // assert(( Common<duration<int, ratio<3, 5>>, duration<int, ratio<5, 7>>>() ));
}
