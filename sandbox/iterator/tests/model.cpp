// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#include <iostream>

#include <origin/utility/typestr.hpp>
#include <origin/iterator/algorithm.hpp>

struct obj 
{
  int num;
  obj(int n) 
    : num{n} 
  { }
  
  obj(obj const& x) 
    : num{x.num} 
  { std::cout << "copy ctor\n"; }
  
  obj(obj&& x) 
    : num{std::move(x.num)} 
  { 
    x.num = 0;
    std::cout << "move ctor\n"; 
  }
  
  obj& operator=(int n) { num = n; return *this; }
  
  obj& operator=(obj const& x) 
  { 
    std::cout << "copy assign\n";
    num = x.num; 
    return *this; 
  }

  obj& operator=(obj&& x) 
  { 
    std::cout << "move assign\n";
    num = std::move(x.num); 
    x.num = 0;
    return *this; 
  }
};

// Reference binding tests

obj& val(obj& x)
{
  return x;
}

obj const& cval(obj const& x)
{
  return x;
}

// NOTE: If I pass by value, then the copy occurs on input and an additional
// move constructor is called for the return (because I'm returing a temporary).
// Passing by cref eliminates the additionl move constructor.
inline obj copy(obj const& x)
{
  return x;
}

void f(obj&& x) 
{ 
  obj y = std::move(x);
}

int main()
{
  using std::cout;
  using namespace origin;
  obj objs[] = {1, 2, 3};

  int x;
  int* p = &x;
  int const* cp = &x;
//   cout << typestr(get(p)) << "\n";
//   cout << typestr(get(cp)) << "\n";
  cout << typestr<reference<decltype(p)>::type>() << "\n";
  cout << typestr<value_type<decltype(p)>::type>() << "\n";
  cout << typestr<reference<decltype(cp)>::type>() << "\n";
  cout << typestr<value_type<decltype(cp)>::type>() << "\n";
  
  cout << "--\n";
  cout <<  typestr(next(p, 5)) << "\n";
  cout <<  typestr(get(p)) << "\n";
  cout <<  typestr(get(next(p, 5))) << "\n";

  // Check signed/unsigned comparison?
  cout << has_less<int, unsigned>::value << "\n";

  /*
//   give(&objs[1], take(&objs[0]));
//   *&objs[0] = std::move(*&objs[1]);
  
  cout << typestr<decltype(get((int*)0))>() << "\n";
  cout << typestr<value_type<int*>::type>() << "\n";
  
  // Yup. you can move an lvalue reference. Makes sense.
  f(std::move(objs[0]));        cout << "--\n";
  f(std::move(val(objs[0])));   cout << "--\n";
  f(std::move(copy(objs[0])));  cout << "--\n";
  
  // Can't move a const object. Big surprise.
  // f(std::move(cval(objs[0])));
  
  cout << typestr<value_type<int*>::type>() << "\n";
  cout << typestr<value_type<int const*>::type>() << "\n";
  
  
  {
    std::cout << "--\n";
    int x = 0;
    int const* cp = &x;
    cout << typestr<decltype(take(cp))>() << '\n';
//     f(std::move(x));
  }
  */

}
