// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#include <origin/blas/detail/matrix/matrix_impl_base.hpp>
#include <cassert>
#include <iostream>

using namespace origin;
using namespace std;
int main()
{
  using test_type = matrix_impl_base<float>;
  
  // Default Constructor.
  {
    test_type subject;
    assert(subject.size() == 0);
  }
  
  // Allocator constructor.
  {
    test_type::allocator_type alloc;
    test_type subject(alloc);
    assert(subject.size() == 0);
  }
  
  // matrix_impl_base(size_type n)
  {
    test_type subject(3);
    assert(subject.size() == 3);
    for(auto const& x : subject.matrix_data)
    {
      assert(x == test_type::value_type());
    }
  }
  
  // matrix_impl_base(size_type n, value_type const& val)
  {
    test_type subject(3, 3);
    assert(subject.size() == 3);
    for(auto const& x : subject.matrix_data)
    {
      assert(x == 3);
    }
  }
  
  // matrix_impl_base(size_type n, value_type const& val, allocator_type const& alloc)
  {
    test_type::allocator_type alloc;
    test_type subject(3, 3, alloc);
    assert(subject.size() == 3);
    for(auto const& x : subject.matrix_data)
    {
      assert(x == 3);
    }
  }
  
  // move constructor
  {
    test_type init_subject(3, 3);
    test_type subject(move(init_subject));
    assert(subject.size() == 3);
    for(auto const& x : subject.matrix_data)
    {
      assert(x == 3);
    }
  }
  
  // move + allocator constructor
  {
    test_type::allocator_type alloc;
    test_type init_subject(3, 3);
    test_type subject(move(init_subject), alloc);
    assert(subject.size() == 3);
    for(auto const& x : subject.matrix_data)
    {
      assert(x == 3);
    }
  }
  
  // copy constructor.
  {
    
  }
  
  // copy + allocator constructor
  {
    
  }
  
  // swap
  {
  }
 
  // clear
  {
  }
  
  // size
  {
  }
  
  // resize
  {
  }
  
  // apply
  {
  }
 
  return 0;
}