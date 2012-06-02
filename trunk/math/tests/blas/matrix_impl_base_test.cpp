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
  
  // move constructor
  {
  }
  
  // move + allocator constructor
  {
  }
  
  // copy constructor.
  {
  }
  
  // copy + allocator constructor
  {
  }
  
  // matrix_impl_base(size_type n)
  {
  }
  
  // matrix_impl_base(size_type n, value_type const& val)
  {
  }
  
  // matrix_impl_base(size_type n, value_type const& val, allocator_type const& alloc)
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