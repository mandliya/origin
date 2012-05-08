// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_VECTOR_HPP
#define ORIGIN_VECTOR_HPP

#include <origin/memory.hpp>

namespace origin
{
  // Vector base
  // The vector base implements the basic allocation logic for a dynamic
  // array. Note that the vector base is non-copyable. This is class 
  // represents a resource: a block of allocated memory.
  template <typename T, typename Alloc = allocator>
    class vector_base
    {
    public:
      // Default constructor
      // Initialize the vector base with its default state; its first and
      // last pointers set to nullptr, and its size() is 0.
      vector_base() noexcept
        : alloc(get_default_allocator()), first {nullptr}, last {nullptr}
      { }

      // Default-with-allocator constructor
      // Default-initialize the vector base, but use alloc as the memory
      // allocator.
      template <typename Alloc>
        explicit vector_base(Alloc& a)
          : alloc(&a), first {nullptr}, last {nullptr}
        { }

    private:
      Alloc& alloc;
    };
}

#endif
