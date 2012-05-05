// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_DYNARRAY_HPP
#define ORIGIN_DYNARRAY_HPP

#include <origin/memory.hpp>

namespace origin
{
  // Dynarray base
  // The dynarray base implements the basic allocation logic for a dynamic
  // array. Note that the dynarray base is non-copyable. This is class 
  // represents a resource: a block of allocated memory.
  template <typename T>
    class dynarray_base
    {
    public:
      // Default constructor
      // Initialize the dynarray base with its default state; its first and
      // last pointers set to nullptr, and its size() is 0.
      dynarray_base() noexcept
        : alloc(&get_default_allocator()), first {nullptr}, last {nullptr}
      { }



      // Default-with-allocator constructor
      // Default-initialize the dynarray base, but use alloc as the memory
      // allocator.
      explicit dynarray_base(Allocator& a)
        : alloc(&a), first {nullptr}, last {nullptr}
      { }



      // Move constructor
      // Moving x into a fresh dynarray base object causes the state of x
      // to be copied into this object, and the state of x to be set to its
      // default.
      dynarray_base(dynarray_base&& x) noexcept
        : alloc(x.alloc), first {x.first}, last {x.last}
      {
        x.first = x.last = nullptr;
      }



      // Copy semantics
      // The dynarray base is not copyable.
      dynarray_base(const dynarray_base&) = delete;
      dynarray_base& operator=(const dynarray_base&) = delete;



      // Size constructor
      // Initialize this object with n freshly allocated sub-objects.
      explicit dynarray_base(std::size_t n)
        : alloc(&get_default_allocator())
        , first {allocate<T>(*alloc, n)}
        , last {first + n}
      { }



      // Size-with-allocator construction
      // Initialize this object with n sub-objects using alloc for this and
      // all subsequent allocation operations.
      dynarray_base(size_t n, Allocator& a)
        : alloc(&a), first {allocate<T>(*alloc, n)}, last {first + n}
      { }



      // Destructor
      // Deallocates the memory owned by this object, if any.
      ~dynarray_base()
      {
        if (first)
          deallocate(*alloc, first);
      }



      // Observers

      // Returns the number of allocated elements.
      std::size_t size() const { return last - first; }

      // Returns true iff size() == 0.
      bool empty() const { return size() == 0; }

      // Returns the allocator associated with this object.
      Allocator& get_allocator() const { return *alloc; }

    private:
      Allocator *alloc;
      T *first;
      T *last;
    };
}

#endif
