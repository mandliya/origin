// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#include <iostream>

#include <origin/type/traits.hpp>

using namespace std;
using namespace origin;

void f() { }

struct ftor
{
  void operator()() const { }
};

int main()
{
  using F = decltype(f);
  static_assert(Function_type<F>(), "");

  // Note that F* is callable (a pointer to f). F is not callable because
  // it is not an object.
  static_assert(Same<Result_of<F*()>, void>(), "");

  // A function pointer does not have function type; it is a pointer.
  static_assert(!Function_type<decltype(&f)>(), "");

  // A functor does not have function type (it has class type).
  static_assert(!Function_type<ftor>(), "");

  static_assert(Same<Result_of<ftor()>, void>(), "");
}
