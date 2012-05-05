// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_MEMORY_ALLOCATION_HPP
#define ORIGIN_MEMORY_ALLOCATION_HPP

#include <cstddef>

namespace origin
{
  // Allocator (interface)
  // The allocator interface describes the interface by which memory is
  // allocated and de-allocated. The allocate and deallocate methods must be
  // reimplemnted by derived allocators.
  //
  // TODO: Should we also support placement allocation through this interface?
  //
  // TODO: Consider whether or not its worthwhile to support mechanisms for
  // copy and swap propagation that the standard uses, but in a more graceful
  // way (if possible). The mechanism is defined in [23.2.1/7-8]. A good 
  // default position seems to be that allocators should not be copied and
  // only swapped if they compare equal.
  class Allocator
  {
  public:
    virtual ~Allocator() { }

    // Allocate n bytes of memory. An exception may be thrown if the requested
    // number of bytes cannot be allocated.
    virtual void *allocate(std::size_t n) = 0;

    // Deallocate the memory pointed to be p. The value p must have been
    // previously allocated using the corresponding allocate operation from the
    // same allocator object.
    virtual void deallocate(void *p) = 0;
  };



  // Default allocator (allocator)
  // The default allocator is a general-purpose dynamic memory allocator.
  class default_allocator : public Allocator
  {
  public:
    virtual void *allocate(std::size_t n);
    virtual void deallocate(void *p);
  };



  // Get default allocator
  // Returns the default allocator for the system.
  //
  // TODO: Should this return Allocator? 
  //
  // TODO: Should I allow the default to be configured?
  default_allocator &get_default_allocator() noexcept;

} // namespace origin

#endif
