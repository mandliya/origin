// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef UTIL_HPP
#define UTIL_HPP

#include <sstream>
#include <cctype>
#include <string>
#include <vector>

template<typename Iter>
  void print(Iter first, Iter last)
  {
    for( ; first != last; ++first)
      std::cout << *first << ' ';
    std::cout << '\n';
  }

template<typename R>
  void print(R const& r)
  {
    print(r.begin(), r.end());
  }

template<typename Vec, typename Input_Stream>
  void read_vector(Input_Stream& is, Vec& v)
  {
    typedef typename Vec::value_type Value;
    
    Value value;
    while(is >> value) {
      v.push_back(value);
    }
  }

// Try to read a line from the vector. Return true if the vector was populated
// with data. False otherwise.
template<typename Str, typename Vec>
  bool read_vector_line(Str const& str, Vec& v)
  {
    // Don't read empty or comment lines.
    if(str[0] == '\n' || str[0] == '#')
      return false;
    
    // If there's a label, read that first.
    // FIXME: What if there are multiple labels?
    std::istringstream ss(str);
    if(std::isalpha(str[0])) {
      std::string label;
      ss >> label;
    }
    read_vector(ss, v);
    return true;
  }

template<typename Data, typename Input_Stream>
  void read_matrix(Input_Stream& is, Data& data)
  {
    typedef typename Data::value_type Vector;

    for(std::string line; std::getline(is, line); ) {
      Vector v;
      if(read_vector_line(line, v))
        data.push_back(v);
    }
  }

#endif
