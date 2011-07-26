// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#include <iostream>
#include <type_traits>

#include <origin/utility/typestr.hpp>

struct A { };
struct B { };
struct C : A, B { };  // !Common<A, B> (obviously)

struct X { };
struct Y : X { };
struct Z : X { };     // !Common<Y, Z> (common base != common type)

struct Z2 : Y { };

// Common type (as a relation) is not generally an equivalence relation. It is
// more generally a tolerance relation (possibly dependency?). It is reflexive
// and symmetric, but not transitive.
//
// For some sets of related types, common type does denote an equivalence
// relation. Specifically, if there is a unique sequence of conversions from 
// one type to the other, then the notion of common type is an equivalence 
// relation.
// Cases where this occurs are:
//    1. Promotions of built in numeric types.
//    2. Conversions between derived and base classes in the same inheritance
//       path. Two classes sharing a common base only share a common type if
//       one is a base of the other.
//    3. Explicitly defined conversions and operations between types. An 
//       example is the chrono duration class, which uses common_type to 
//       describe duration implicit promotions.
//
// Why is this important? If two types share a common type, then we can 
// consider an operation with one type to be equivalent to any operation 
// involving both types. This is generally achieved through implicit conversion
// of one argument to the type of the other.

// There can be cases where the common type of two types is not one the two
// arguments, but this usually requires a programmer to specialize common_type.

int main()
{
  using std::cout;
  using std::declval;
  using std::common_type;
  using origin::typestr;
  
  typedef common_type<A, C>::type T1;
  typedef common_type<B, C>::type T2;
//   typedef common_type<A, B>::type T2;
  cout << typestr<T1>() << "\n";
  cout << typestr<T2>() << "\n";
  
//   typedef common_type<Y, Z>::type T3;
//   cout << typestr<T3>() << "\n";
  
  cout << typestr<common_type<X, Y>::type>() << '\n';
  cout << typestr<common_type<Y, Z2>::type>() << '\n';
  cout << typestr<common_type<X, Z2>::type>() << '\n';
  
  cout << typestr<common_type<char, int>::type>() << '\n';
  cout << typestr<common_type<int, double>::type>() << '\n';
  cout << typestr<common_type<char, double>::type>() << "\n";
}
