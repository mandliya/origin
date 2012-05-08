// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_MEMORY_ALLOCATION_HPP
#define ORIGIN_MEMORY_ALLOCATION_HPP

#include <cstddef>
#include <new>

#include <origin/memory/pointer.hpp>

namespace origin
{
  // Pointer (concept)
  // A pointer encapsulates the location of an object in memory. Note that
  // memory does not need to be in-process; it could easily be in secondary
  // storage, or it could be remote.
  //
  // A void pointer is a kind of generic pointer to which all types of pointers
  // can be implicitly converted. In main memory, this is called void*. A
  // garbage-collected pointer might be called gc_ptr<void>.



  // Void pointer type (alias)
  // The void pointer type alias names the result of Alloc's allocate method.
  // Alloc must be an allocator type.
  template <typename Alloc>
    using Void_pointer_type = decltype(std::declval<Alloc>().allocate(0));



  // Pointer type (alias)
  // The pointer type alias yields a pointer to an object of type T that 
  // would be allocated by Alloc.
  template <typename T, typename Alloc>
    using Pointer_type = Rebind_pointer<Void_pointer_type<Alloc>, T>;



  // Allocator (concept)
  //
  // An allocator is a facility that is responsible for allocating and
  // deallocating ojbects. The result of allocation is a void Pointer to the
  // allocated, but uninitialized object(s). Deallocation reverses that process,
  // releasing the allocated memory. Note that Pointer's are not required to
  // refer to in-process locations; they could refer to disk  locations or
  // objects on remote nodes. For example, a persistent memory allocator whose
  // Pointers are that can transfer values into and out of main memory. A
  // garbage collecting allocator returns Pointers that trace their 
  // reachability.



  // Default allocator
  // The default allocator describes allocates and deallocatese in-process 
  // memory using ::operator new and ::operator delete. Note that all allocators
  // for in-process memory should derive from this class.
  //
  // TODO: Should we also support placement allocation through this interface?
  //
  // TODO: Consider whether or not its worthwhile to support mechanisms for
  // copy and swap propagation that the standard uses, but in a more graceful
  // way (if possible). The mechanism is defined in [23.2.1/7-8]. A good 
  // default position seems to be that allocators should not be copied and
  // only swapped if they compare equal.
  class allocator
  {
  public:
    virtual ~allocator() { }

    // Allocate n bytes of memory. An exception may be thrown if the requested
    // number of bytes cannot be allocated.
    virtual void *allocate(std::size_t n)
    {
      return ::operator new(n);
    }

    // Deallocate the memory pointed to be p. The value p must have been
    // previously allocated using the corresponding allocate operation from the
    // same allocator object.
    virtual void deallocate(void *p, std::size_t n)
    {
      ::operator delete(p);
    }

    // Returns true if the the two allocators are equal. For all stateless
    // allocators, this is always true. Note that this operation *must* be
    // overloaded if the derived allocator is stateful.
    virtual bool equal(const allocator& x) const { return true; }
  };

  // Equality comparable
  // For all allocator-derived types, equality comparability is implemented
  // in terms of the allocator::equal method.
  bool operator==(const allocator& a, const allocator& b) 
  { 
    return a.equal(b);
  }

  bool operator!=(const allocator& a, const allocator& b)
  {
    return !a.equal(b);
  }



  // Auto allocator
  // The auto allocator maintains a buffer of N bytes on the stack. If an
  // allocation request exceeds N bytes, the fall-back allocator is used
  // instead.
  template <std::size_t N, typename Alloc = allocator>
  class auto_allocator : public allocator
  {
  public:

    virtual void *allocate(std::size_t n)
    {
      return 0;
    }

    virtual void deallocate(void *p, std::size_t n)
    {
    }

    // Every instance of an auto allocator is distinct from every other. No
    // two are the same.
    virtual bool equal(const auto_allocator& x) const
    {
      return false;
    }

  private:
    char buf[N];
  };


} // namespace origin

#endif
