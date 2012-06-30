// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#include <iostream>

#include <origin/math/matrix.hpp>

using namespace std;
using namespace origin;

template <typename Matrix>
  void init(Matrix& m)
  {
    int n = 0;
    for (auto& x : m)
      x = n++;
  }

int main()
{
  matrix_shape<size_t, 1> s1{5ul};
  assert(s1.rank() == 1);
  assert(s1.extent(0) == 5);
  assert(s1.elements() == 5);

  matrix_shape<size_t, 2> s2 {3ul, 4ul};
  assert(s2.rank() == 2);
  assert(s2.extent(0) == 3);
  assert(s2.extent(1) == 4);
  assert(s2.elements() == 12);

  assert(s1 == s1);
  assert((s1 != matrix_shape<size_t, 1>{3ul}));


  // Test a 1D matrix.
  matrix<int, 1> m1 {5};
  assert(m1.rank() == 1);
  assert(m1.size() == 5);

  // Initialize m1 to the value:
  // 0 1 2 3 4
  init(m1);
  for (int i = 0; i < 5; ++i)
    assert(m1(i) == i);


  // Test a 2D matrix
  matrix<int, 2> m2 {{3, 4}};
  assert(m2.rank() == 2);
  assert(m2.size() == 12);

  // Put increasing values into m1 s.t we have the following matrix:
  // 0 1 2 3
  // 4 5 6 7
  // 8 9 a b
  init(m2);
  for (int i = 0, n = 0; i < 3; ++i) {
    for (int j = 0; j < 4; ++j, ++n)
      assert(m2(i, j) == n);
  }


  // This fails: can't construct a 2d matrix with 3 extents!
  // matrix<int, 2> m {{3, 4, 5}};


  matrix<int, 3> m3{{3, 4, 2}};
  assert(m3.rank() == 3);
  assert(m3.size() == 24);

  // 3x4x2
  // The elements of the "z" axis is written above and behind elements at
  // each row and colmn. Note that letters are used to replace numbers greater
  // than 10 (a == 10, b == 11, etc.).
  //  1  3  5  7
  // 0  2  4  6
  //
  //  9  b  d  f
  // 8  a  c  e
  //
  //  h  j  l  n
  // g  i  k  m 
  init(m3);
  for (int i = 0, n = 0; i < 3; ++i) {
    for (int j = 0; j < 4; ++j) {
      for (int k = 0; k < 2; ++k, ++n)
        assert(m3(i, j, k) == n);
    }
  }


  // Test matrix multiplication
  {
    matrix<int, 2> a {{2, 3}, {
      1, 0, -2,
      0, 3, -1 
    }};
    
    matrix<int, 2> b {{3, 2}, {
       0,  3,
      -2, -1,
       0,  4 
     }};

    auto c = a * b;
    cout << c << '\n';
  }

  /*
  typedef matrix<int, 1> Matrix;
  
  Matrix m1;
  assert(m1.size() == 0);
  assert(m1.dim() == 0);
  
  Matrix m2(3);
  cout << m2 << '\n';
  
  Matrix m3(3, 1);
  cout << m3 << '\n';
  
  m3 += 2;
  cout << m3 << '\n';
  assert(m3 == Matrix(3, 3));

  Matrix m4 = m3 + 3;
  cout << m4 << '\n';
  assert(m4 == Matrix(3, 6));

  Matrix m5 = m3 * 3;
  cout << m5 << '\n';
  assert((m5 == Matrix{9, 9, 9}));
  
  Matrix m6 = {1, 2, 3, 4};
  cout << m6 << '\n';
  cout << m6 % 2 << '\n';
  assert((m6 % 2 == Matrix{1, 0, 1, 0}));
  */
}
