// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_MEMORY_HPP
#define ORIGIN_MEMORY_HPP

#include <utility>
#include <iostream>

#include <origin/memory/allocation.hpp>

namespace origin
{
  // Allocate
  // The allocate operation allocates n objects of the specified type using
  // the given allocator. Note that the allocated memory is uninitialized. Its 
  // use is as follows:
  //
  //    X *ptr = allocate<X>(alloc, 3);
  //
  // Which returns a pointer to 3 objects of type X. This operation is
  // primarily provided as helper for untyped allocators.
  //
  // FIXME: What are the requirements on T? It cannot be void.
  template <typename T, typename Alloc>
    inline Pointer_type<T, Alloc> allocate(Alloc& alloc, std::size_t n = 1)
    {
      using Ptr = Pointer_type<T, Alloc>;
      return static_ptr_cast<Ptr>(alloc.allocate(n * sizeof(T)));
    }



  // Deallocate
  // The deallocate operation deallocate the memory pointed to by p. The value
  // p must have been previously allocated using a corresponding allocate
  // operation on the same alloc object.
  //
  // FIXME: Write the type requirements for this algorithm. Here, Alloc must
  // be an Allocator and Ptr must be in the family of allocated pointer types.
  template <typename Alloc, typename Ptr>
    void deallocate(Alloc& alloc, Ptr p, std::size_t n = 1)
    {
      alloc.deallocate(p, n);
    }



  // Construct
  // Initialize the allocated object pointed to by p using the given arguments,
  // which are forwarded to an appropriate constructor.
  template <typename T, typename... Args>
    void construct(T& p, Args&&... args)
    {
      // static_assert(Constructible<T, Args...>(), "");
      new (addressof(p)) T(std::forward<Args>(args)...);
    }



  // Destroy
  // Uninitialize the object pointed to by p, but do not deallocate its memory.
  template <typename T>
    void destroy(T& p)
    {
      // static_assert(Destructible<T>(), "");
      p.~T();
    }



  // Uninitialized copy
  // Copy each value in the input range [first, last) into an uninitialized 
  // object in the output range [result, ...).
  //
  // FIXME: Optimize this for the case where we can use memmove.
  //
  // FIXME: This algorithm does not work on iterators whose result types are
  // not references. This should be reflected in the type requirements.
  template <typename I, typename O>
    O uninitialized_copy(I first, I last, O result)
    {
      // static_assert(Copy<I, O>(), "");
      while (first != last) {
        construct(*result, *first);
        ++first;
        ++result;
      }
      return result;
    }



  // Uninitialized fill
  // Initialize each unitialized object in [first, last) with the specified
  // value.
  //
  // FIXME: Optimize for the case where we can use memset.
  // 
  // FIXME: This algorithm does not work on iterators whose result types are
  // not references. This should be reflected in the type requirements.
  template <typename I, typename T>
    void uninitialized_fill(I first, I last, const T& value)
    {
      while (first != last) {
        construct(*first, value);
        ++first;
      }
    }
}

#endif
