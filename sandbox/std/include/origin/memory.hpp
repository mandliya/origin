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
  // Initialization and destruction
  // The construct(alloc, x, args...) and destroy(alloc, x) algorithms in this
  // module take an allocator argument so that they may be overloaded for
  // specific allocator types. For basic allocators, no overloads should be
  // required.



  // Construct
  // Initialize the allocated object pointed to by p using the given arguments,
  // which are forwarded to an appropriate constructor. This operation may be
  // overloaded for custom allocators or types.
  //
  // Template parameters:
  //    Alloc   - An Allocator type
  //    T       - The type of object being initialized
  //    Args... - The types of arguments forwarded to T's constructor
  //
  // Parameters:
  //    alloc   - An allocator object; unused in this definition
  //    x       - A reference to an uninitialized object being constructed
  //    args... - Arguments forwarded to T's constructor
  //
  // Note: The address of x is taken using operator &. Any type overloading that
  // operator to do something other than return the address cannot be
  // initialized using this operation. The preferred alternative is to overload
  // this operation specifically for that type.
  template <typename Alloc, typename T, typename... Args>
    void construct(Alloc& alloc, T& x, Args&&... args)
    {
      // static_assert(Constructible<T, Args...>(), "");
      new (&x) T(std::forward<Args>(args)...);
    }



  // Destroy
  // Destroy the object referred to by x, but do not deallocate its memory.
  // This operation may be overloaded for custom allocators or types.
  //
  // Template parameters:
  //    Alloc   - An Allocator type
  //    T       - The type of object being destroyed
  //
  // Parameters:
  //    alloc   - An allocator object; unused in this definition
  //    x       - A reference to an initialized object being destroyed
  template <typename Alloc, typename T>
    void destroy(Alloc& alloc, T& x)
    {
      // static_assert(Destructible<T>(), "");
      x.~T();
    }



  // NOTE: The following algorithms are not intended to be overloaded by library
  // users.



  // Destroy
  // Destroy the objects in the range [first, last). Note that this does not
  // deallocate memory.
  template <typename Alloc, typename I>
    void destroy(Alloc& alloc, I first, I last)
    {
      // static_assert(Input_iterator<I>(), "");
      // assert(is_bounded_range(first, last));
      while (first != last) {
        destroy(alloc, *first);
        ++first;
      }
    }



  // Uninitialized copy step
  // Construct *result with the value of *iter, and increment both iterators.
  template <typename Alloc, typename I, typename O>
    inline void uninitialized_copy_step(Alloc& alloc, I& iter, O& result)
    {
      construct(alloc, *result, *iter);
      ++iter;
      ++result;
    }



  // Uninitialized copy
  // Copy each value in the input range [first, last) into an uninitialized 
  // object in the output range [result, ...).
  //
  // FIXME: Optimize this for the case where we can use memmove.
  //
  // FIXME: This algorithm does not work on iterators whose result types are
  // not references. This should be reflected in the type requirements.
  template <typename Alloc, typename I, typename O>
    O uninitialized_copy(Alloc& alloc, I first, I last, O result)
    {
      // static_assert(Copy<I, O>(), "");
      while (first != last)
        uninitialized_copy_step(alloc, first, result);
      return result;
    }


  // Uninitialized copy n.
  // FIXME: This needs to return pair<I, O>.
  //
  // FIXME: Optimize for memmovable types.
  template <typename Alloc, typename I, typename O>
    O uninitialized_copy_n(Alloc& alloc, I first, std::size_t n, O result)
    {
      while(n != 0) {
        uninitialized_copy_step(alloc, first, result);
        --n;
      }
      return result;
    }



  // Uninitialized move
  //
  // FIXME: Document these algorithms...

  template <typename Alloc, typename I, typename O>
    inline void uninitialized_move_step(Alloc& alloc, I& iter, O& result)
    {
      construct(alloc, *result, std::move(*iter));
      ++iter;
      ++result;
    }

  template <typename Alloc, typename I, typename O>
    O uninitialized_move(Alloc& alloc, I first, I last, O result)
    {
      while (first != last)
        uninitialized_move_step(alloc, first, result);
      return result;
    }

  template <typename Alloc, typename I, typename O>
    O uninitialized_move_n(Alloc& alloc, I first, std::size_t n, O result)
    {
      while (n != 0) {
        uninitialized_move_step(alloc, first, result);
        --n;
      }
      return result;
    }



  // Uninitialized fill
  // Initialize each object in [first, last) with the specified value.
  //
  // FIXME: Optimize where we can use memset.
  // 
  // FIXME: This algorithm does not work on iterators whose result types are
  // not references. This should be reflected in the type requirements.
  template <typename Alloc, typename I, typename T>
    void uninitialized_fill(Alloc& alloc, I first, I last, const T& value)
    {
      while (first != last) {
        construct(alloc, *first, value);
        ++first;
      }
    }



  // Uninitialized fill n
  // Initialize each object in the range [first, first + n) with a copy of
  // value.
  //
  // FIXME: Optimize where we can use memset.
  template <typename Alloc, typename I, typename T>
    I uninitialized_fill_n(Alloc& alloc, I first, std::size_t n, const T& value)
    {
      while (n != 0) {
        construct(alloc, *first, value);
        ++first;
        --n;
      }
      return first;
    }
}

#endif
