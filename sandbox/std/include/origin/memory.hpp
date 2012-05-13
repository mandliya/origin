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

#include <origin/concepts.hpp>
#include <origin/memory/allocation.hpp>

namespace origin
{
  // Initialization and destruction
  // The construct(alloc, x, args...) and destroy(alloc, x) algorithms in this
  // module take an allocator argument so that they may be overloaded for
  // specific allocator types. For basic allocators, no overloads should be
  // required.
  //
  // TODO: Should these allocators be in with allocation.hpp?



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
  //
  // FIXME: If T is "allocator-friendly", then we should propagate the
  // allocator reference to the initialized class. I believe, but am not 
  // certain that any allocator-friendly class will have, for every constructor,
  // an equivalent constructor that takes an additional allocator. 
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



  // Can memmove (constraint)
  // Returns true if the memmove function can be used to move memory from thes
  // I iterators into O iterators. In order for this to be the case, I and O
  // must be pointers to the same trivially move constructible value type.
  //
  // FIXME: GCC does not currently implement is_trivially_move_constructible,
  // so we have to rely on the Trivial constraint. There should be no
  // false positives.
  template <typename I, typename O>
    constexpr bool Can_memmove()
    {
      return Cxx_pointer<I>() && Cxx_pointer<O>()
          && Same<Value_type<I>, Value_type<O>>()
          && Trivial<Value_type<I>>();
    }


  // Memmove
  // Move n trivial objects pointed to by src into the memory pointed to by dst.
  //
  // FIXME: The correct type requirement is trivially move constructible, but
  // GCC does not yet implement that operation.
  template <typename T>
    inline T *origin_memmove(const T* src, std::size_t n, T* dest)
    {
      static_assert(Trivial<T>(), "");
      std::memmove(dest, src, n * sizeof(T));
      return dest + n;
    }


  // FIXME: These need to be in algorithm module (but not included by this
  // header). They might be included by containers in general.
  template <typename I, typename O>
    O move_backward(I first, I last, O result)
    {
      while (first != last) {
        --last;
        --result;
        *result = std::move(*last);
      }
      return result;
    }

  template <typename T>
    auto move_backward(const T* first, const T* last, T* result)
      -> Requires<Trivial<T>(), T*>
    {
      std::ptrdiff_t n = last - first;
      result -= n;
      std::memmove(result, first, n * sizeof(T));
      return result;
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
  // For these algorithms, Value_type<I> is not nothrow-constructible, but is
  // constructible, then the algorithm will actually copy instead of moving.
  // This helps preserve the strong exception safety. This algorithm is still
  // allowed to throw, but it won't invalidate the source data if it does.
  //
  // FIXME: Document these algorithms...

  template <typename Alloc, typename I, typename O>
    inline void uninitialized_move_step(Alloc& alloc, I& iter, O& result)
    {
      construct(alloc, *result, std::move_if_noexcept(*iter));
      ++iter;
      ++result;
    }



  // Uninitialized move
  // Move the elements in [first, last) into the uninitialized range of pointed
  // to by result.
  template <typename Alloc, typename I, typename O>
    inline O uninitialized_move(Alloc& alloc, I first, I last, O result)
    {
      while (first != last)
        uninitialized_move_step(alloc, first, result);
      return result;
    }

  // Optimization for mem-movable inputs.
  template <typename Alloc, typename T>
    inline auto uninitialized_move(Alloc& alloc, const T* first, const T* last, T* result)
      -> Requires<Trivial<T>(), T*>
    {
      return origin_memmove(first, last - first, result);
    }




  // Uninitialzed move n
  // Move the elemnts in [first, first + n) into the region of uninitialized
  // range pointed to by result.
  template <typename Alloc, typename I, typename O>
    inline O uninitialized_move_n(Alloc& alloc, I first, std::size_t n, O result)
    {
      while (n != 0) {
        uninitialized_move_step(alloc, first, result);
        --n;
      }
      return result;
    }

  // Optimization for mem-movable inputs.
  template <typename Alloc, typename T>
    inline auto uninitialized_move_n(Alloc& alloc, const T* first, std::size_t n, T* result)
      -> Requires<Trivial<T>(), T*>
    {
      return origin_memmove(first, n, result);
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
