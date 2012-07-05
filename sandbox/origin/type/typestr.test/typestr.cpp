// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#include <cassert>
#include <iostream>

#include <origin/type/typestr.hpp>

using namespace std;
using namespace origin;

// Compre the result of typestr<T> to the text with which it is written,
// returning true when the two are the same. For example:
//
//    CHECK_TYPESTR(const int) // returns true
//
// Note that the result of the operation is sensitive to the spelling of T. In
// the example above, passing the argument as "int const" would cause the
// evaluation to base.
#define CHECK_TYPESTR(T) assert(typestr<T>() == #T)


template <typename T>
  void f(T&& x)
  {
    cout << typestr<T>() << '\n';
    // cout << type_impl::to_string(typeid(x)) << '\n';
  }

int main()
{
  CHECK_TYPESTR(int);
  CHECK_TYPESTR(int);
  CHECK_TYPESTR(const int);
  CHECK_TYPESTR(volatile int);
  CHECK_TYPESTR(const volatile int);
  CHECK_TYPESTR(int&);

  // The C Preprocessor can't expand this correctly.
  assert(typestr<int&&>() == "int&&");
  
  CHECK_TYPESTR(int*);
  CHECK_TYPESTR(int**);
  CHECK_TYPESTR(int***);

  CHECK_TYPESTR(const int*);
  CHECK_TYPESTR(const int* const);
  CHECK_TYPESTR(const int* const* const);
  CHECK_TYPESTR(const int*);
  CHECK_TYPESTR(const int* volatile);
  CHECK_TYPESTR(const int* const volatile);

  CHECK_TYPESTR(int(int, int));
  CHECK_TYPESTR(int(int&, int));
  CHECK_TYPESTR(int(*)(int, bool));

  int a[3];
  const int b[3] {0, 0, 0};
  f(a);
  f(b);

  // TODO: Check member pointer types, too. I'm pretty sure I'm going to have
  // to write specializations on those.


  // Variadic arguments won't expand correctly for the CHECK_TYPESTR macro.
  assert((typestr<int, char, short>() == "int, char, short"));
  assert((typestr<const int&, char*>() == "const int&, char*"));


  // TODO: Test the use of typest with object arguments.
}
