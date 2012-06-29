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


int main()
{
  typedef matrix<int, 2> Matrix;
  
  Matrix m1;
  assert(m1.size() == 0);
  assert(m1.dim() == make_pair(0ul, 0ul));
  
  Matrix m2({3, 4});
  assert(m2.size() == 12);
  cout << m2.rows() << '\n';
  assert(m2.rows() == 3);
  assert(m2.cols() == 4);

  Matrix m3({3, 4}, 1);  
  cout << m3 << '\n';
  m3 += 2;
  cout << m3 << '\n';
  assert(( m3 == Matrix({3, 4}, 3) ));
  
  /*
  Matrix m4 = m3 + 3;
  assert(( m4 == Matrix(3, 6) ));

  Matrix m5 = m3 * 3;
  assert(( m5 == Matrix{9, 9, 9} ));
  
  Matrix m6 = {1, 2, 3, 4};
  assert(( m6 % 2 == Matrix{1, 0, 1, 0} ));
  */
}
