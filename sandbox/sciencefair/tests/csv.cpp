// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#include <iostream>

#include <scifair/data/csv.hpp>

using namespace std;
using namespace origin;

int main()
{
  csv_row<> r;
  while(cin >> csv(r)) {
    for(size_t i = 0; i < r.size(); ++i)
      cout << r[i] << ' ';
    cout << "\n";
  }
}
