// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_MEMORY_HPP
#define ORIGIN_MEMORY_HPP

#include <cstdlib>
#include <utility>

#include <origin/concepts.hpp>
#include <origin/algorithm.hpp>
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
  //
  // FIXME: Should x be a pointer? Maybe? Maybe not.
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



  // Destroy
  // Destroy the objects in the range [first, last). Note that this does not
  // deallocate memory.
  template <typename Alloc, typename I>
    auto destroy(Alloc& alloc, I first, I last)
      -> Requires<!Trivial<Value_type<I>>(), void>
    {
      // static_assert(Input_iterator<I>(), "");
      // assert(is_bounded_range(first, last));
      while (first != last) {
        destroy(alloc, *first);
        ++first;
      }
    }

  // If the value type being destroyed is trivially destructible, then don't
  // do anything.
  template <typename Alloc, typename I>
    auto destroy(Alloc& alloc, I first, I last)
      -> Requires<Trivial<Value_type<I>>(), void>
    { }



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
  template <typename Alloc, typename I, typename O>
    O uninitialized_copy(Alloc& alloc, I first, I last, O result)
    {
      while (first != last)
        uninitialized_copy_step(alloc, first, result);
      return result;
    }

  // FIXME: The correct requirement is Trivially_copyable, not Trivial.
  //
  // FIXME: What guarantee do we have that alloc will not do anything  with the
  // initialization of trivially copyable types? It may be necessary to require
  // some static property of allocators w.r.t. to trivial types, or to make
  // a runtime check.
  template <typename Alloc, typename T>
    auto uninitialized_copy(Alloc& alloc, const T* first, const T* last, T* result)
      -> Requires<Trivial<T>(), T*>
    {
      return copy(first, last, result);
    }



  // Uninitialized copy n.
  //
  // FIXME: This needs to return pair<I, O>.
  template <typename Alloc, typename I, typename O>
    O uninitialized_copy_n(Alloc& alloc, I first, std::size_t n, O result)
    {
      while(n != 0) {
        uninitialized_copy_step(alloc, first, result);
        --n;
      }
      return result;
    }

  // FIXME: See comments uninitialized_copy.
  template <typename Alloc, typename T>
    auto uninitialized_copy_n(Alloc& alloc, const T* first, std::size_t n, T* result)
      -> Requires<Trivial<T>(), T*>
    {
      copy(first, first + n, result);
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

  // FIXME: See comments uninitialized_copy.
  template <typename Alloc, typename T>
    inline auto uninitialized_move(Alloc& alloc, const T* first, const T* last, T* result)
      -> Requires<Trivial<T>(), T*>
    {
      return move(first, last, result);
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

  // FIXME: See comments uninitialized_copy.
  template <typename Alloc, typename T>
    inline auto uninitialized_move_n(Alloc& alloc, const T* first, std::size_t n, T* result)
      -> Requires<Trivial<T>(), T*>
    {
      return move(first, first + n, result);
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
      I init = first;
      try {
        while (first != last) {
          construct(alloc, *first, value);
          ++first;
        }
      } catch(...) {
        destory(alloc, init, first);
        throw;
      }
    }

  template <typename Alloc, typename T>
    auto uninitialized_fill(Alloc& alloc, T* first, T* last, const T& value)
      -> Requires<Trivial<T>() && sizeof(T) == 1, void>
    {
      fill(first, last, value);
    }



  // Uninitialized fill n
  // Initialize each object in the range [first, first + n) with a copy of
  // value.
  template <typename Alloc, typename I, typename T>
    I uninitialized_fill_n(Alloc& alloc, I first, std::size_t n, const T& value)
    {
      I init = first;
      try {
        while (n != 0) {
          construct(alloc, *first, value);
          ++first;
          --n;
        }
      } catch(...) {
        destroy(alloc, init, first);
        throw;
      }
      return first;
    }

  template <typename Alloc, typename T>
    auto uninitialized_fill_n(Alloc& alloc, T* first, std::size_t n, const T& value)
      -> Requires<Trivial<T>() && sizeof(T) == 1, void>
    {
      fill(first, first + n, value);
    }
}

#endif
