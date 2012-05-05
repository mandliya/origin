// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#include <new>

#include <origin/memory.hpp>

// Memory initializer
// The mem_initializer class controls the static initialization of the
// memory module.
struct mem_initializer
{
  origin::default_allocator alloc;
};

mem_initializer mem_init;



namespace origin
{
  // Default allocator

  void *default_allocator::allocate(size_t n)
  {
    return operator new(n);
  }

  void default_allocator::deallocate(void *p)
  {
    operator delete(p);
  }


  // Utility functions

  default_allocator &get_default_allocator() noexcept
  {
    return mem_init.alloc;
  }

} // namespace origin
