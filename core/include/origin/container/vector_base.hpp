// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_CONTAINER_VECTOR_BASE_HPP
#define ORIGIN_CONTAINER_VECTOR_BASE_HPP

#include <origin/container/core.hpp>

namespace origin
{
  // The vector impl class stores three pointers that maintain the underlying
  // buffer of a vector base.
  //
  // FIXME: Would it be worthwhile to base this on void*? It means we could
  // some core functionality out of headers and into the binary. But how much?
  template <typename T>
    struct vector_core
    {
      // Default construction.
      vector_core()
        : first{nullptr}, last{nullptr}, limit{nullptr}
      { }

      // Move semantics
      vector_core(vector_core&& x)
        : first{x.first}, last{x.last}, limit{x.limit}
      {
        x.reset();
      }

      vector_core& operator=(vector_core&& x)
      {
        first = x.first;
        last = x.last;
        limit = x.limit;
        x.reset();
        return *this;
      }

      // Allocation
      vector_core(T* first, std::size_t n)
        : first{first}, last{first}, limit{first + n}
      { }


      // Returns true if there are no initialized values in the allocated block
      // of memory.
      bool empty() const { return first == last; }

      // Returns true when every allocated object is initialized.
      bool full() const { return last == limit; }

      // Returns the number of initialized valies in the allocated block of
      // memory.
      std::size_t size() const { return last - first; }

      // Returns the total number of allocated objects in the block of memory.
      std::size_t capacity() const { return limit - first; }

      // Returns the residual capacity of the vector; the number of allocated
      // but not initialized values.
      std::size_t residual() const { return limit - last; }

      // Reset the vector core making all of the pointers null.
      void reset() { first = last = limit = nullptr; }

      // Swap this core with x.
      void swap(vector_core& x)
      {
        std::swap(first, x.first);
        std::swap(last, x.last);
        std::swap(limit, x.limit);
      }


      T* first;
      T* last;
      T* limit;
    };



  // Vector base
  //
  // The vector base implements the basic allocation logic for a dynamic array.
  // Note that the vector base is non-copyable. This is class represents a
  // resource: a block of allocated memory.
  //
  // Invariants:
  //    this->first <= this->last
  //    this->last <= this->limit
  //    is_uninitilized(this->last, this->limit)
  template <typename T>
    class vector_base
    {
    public:
      // Default construction
      vector_base() noexcept;
      explicit vector_base(allocator& a) noexcept;

      // Move semantics
      vector_base(vector_base&& x) noexcept;
      vector_base(vector_base&& x, allocator& a)
        noexcept(Nothrow_move_constructible<T>());
      vector_base& operator=(vector_base&& x) noexcept;

      // Copy semantics (deleted)
      vector_base(const vector_base&) = delete;
      vector_base& operator=(const vector_base&) = delete;

      // Allocated construction
      vector_base(std::size_t n);
      vector_base(std::size_t n, allocator& a);

      ~vector_base();


      // Allocate and deallocate
      T *allocate(std::size_t n);
      void deallocate(T* p, std::size_t n);
      void deallocate();

      // Core manipulation
      vector_core<T> allocate_core();
      vector_core<T> allocate_core(std::size_t n);
      void deallocate_core(vector_core<T>& core);
      void take_core(vector_core<T>& core);


      // Observers

      // Returns true if there are no initialized values in the allocated block
      // of memory.
      bool empty() const { return core.empty(); }

      // Returns true when every allocated object is initialized.
      bool full() const { return core.full(); }

      // Returns the number of initialized valies in the allocated block of
      // memory.
      std::size_t size() const { return core.size(); }

      // Returns the total number of allocated objects in the block of memory.
      std::size_t capacity() const { return core.capacity(); }

      // Returns the residual capacity of the vector; the number of allocated
      // but not initialized values.
      std::size_t residual() const { return core.residual(); }

      // Returns the next capacity of the vector base if the owning vector
      // is dynamically resized.
      //
      // TODO: Can I find a better name for this function?
      std::size_t next_capacity() const
      {
        return capacity() == 0 ? 4 : capacity() * 2;
      }


      // Mutators
      void swap(vector_base& x);
      void clear();


      // FIXME: These have largely been deprecated. Remove them.
      template <typename I> 
        T* copy_at_pos(T* pos, I first, I last);
      template <typename I> 
        void copy_at_end(I first, I last);

      void copy_at_end(const vector_base<T>& x);


      template <typename U> 
        T* fill_at_pos(T* pos, std::size_t n, const U& value);
      template <typename U> 
        void fill_at_end(std::size_t n, const U& value);

      void erase_at_end(std::size_t n);


    public:
      allocator& alloc;
      vector_core<T> core;
    };

  template <typename T>
    inline 
    vector_base<T>::vector_base() noexcept
      : vector_base(default_allocator())
    { }

  template <typename T>
    inline
    vector_base<T>::vector_base(allocator& a) noexcept
      : alloc(a), core{}
    { }

  // Transfer the state of x into this vector base, and reset the state of x to
  // its default. Note that the vector base is not move-assignable.
  template <typename T>
    vector_base<T>::vector_base(vector_base&& x) noexcept
      : alloc(x.alloc), core{std::move(x.core)}
    { }

  // Move the elements of x into this object, using a to allocate memory. If
  // a != x.alloc, then we must allocate new memory and transfer each element
  // x into the newly allocated buffer.
  //
  // NOTE: This constructor violates the ideal layering because it includes
  // a value operation (uninitialized_move).
  template <typename T>
    vector_base<T>::vector_base(vector_base&& x, allocator& a)
      noexcept(Nothrow_move_constructible<T>())
      : alloc(x.alloc), core{}
    {
      if (alloc == a) {
        core = std::move(x.core);
      } else {
        core.first = allocate(x.size());
        try {
          core.last = 
            uninitialized_move(alloc, x.core.first, x.core.last, core.first);
        } catch(...) {
          dealloate(core.first, size());
          throw;
        }
        core.limit = core.last;
      }
    }

  // Move the state of x into this object, resetting x afterwards.
  template <typename T>
    inline vector_base<T>&
    vector_base<T>::operator=(vector_base&& x) noexcept
    {
      assert(alloc == x.alloc);
      core = std::move(x.core);
      return *this;
    }

  template <typename T>
    inline 
    vector_base<T>::vector_base(std::size_t n)
      : vector_base(n, default_allocator())
    { }

  // Initialize the vector base with n allocated objects.
  template <typename T>
    inline
    vector_base<T>::vector_base(std::size_t n, allocator& a)
      : alloc(a), core{allocate(n), n}
    { }

  template <typename T>
    vector_base<T>::~vector_base()
    {
      deallocate();
    }



  // Allocate n objects, returning a pointer to the first.
  template <typename T>
    inline T*
    vector_base<T>::allocate(std::size_t n)
    {
      // return origin::allocate<T>(alloc, n);
      T* p = origin::allocate<T>(alloc, n);
      return p;
    }

  // Deallocate n objects pointed to by p.
  template <typename T>
    inline void
    vector_base<T>::deallocate(T* p, std::size_t n)
    {
      origin::deallocate(alloc, p, n);
    }

  // Deallocate the memory currently allocated to this vector base.
  template <typename T>
    inline void
    vector_base<T>::deallocate()
    {
      deallocate(core.first, capacity());
    }


  // Allocate a new core with the (at least) twice the capacity as the
  // this vector base.
  template <typename T>
    inline vector_core<T>
    vector_base<T>::allocate_core()
    {
      return allocate_core(next_capacity());
    }

  // Allocate a new core with the given size.
  template <typename T>
    inline vector_core<T>
    vector_base<T>::allocate_core(std::size_t n)
    {
      return {allocate(n), n};
    }

  // Deallocate the memory used by a vector core. This is only called when
  // an exception is thrown after allocation.
  template <typename T>
    inline void
    vector_base<T>::deallocate_core(vector_core<T>& core)
    {
      deallocate(core.first, core.capacity());
    }



  // Assume the ownership of the other core. This destroys the elements in
  // [first, last) and deallocates the associated memory prior to assuming
  // ownership.
  //
  // NOTE: This operation breaks the layering because it includes a value
  // operation (destroy).
  template <typename T>
    inline void
    vector_base<T>::take_core(vector_core<T>& core)
    {
      destroy(alloc, this->core.first, this->core.last);
      deallocate();
      this->core.first = core.first;
      this->core.last = core.last;
      this->core.limit = core.limit;
    }


  // Exchange the reresentation of x with this object. The operation is only
  // valid when the allocators of this and x compare equal.
  //
  // Requires:
  //    this->alloc == x.alloc
  template <typename T>
    inline void 
    vector_base<T>::swap(vector_base& x)
    {
      assert(alloc == x.alloc);
      core.swap(x.core);
    }


  // Erase all the elements in this buffer, updating the buffer so that it is
  // empty.
  template <typename T>
    inline void vector_base<T>::clear()
    {
      destroy(alloc, core.first, core.last);
      core.last = core.first;
    }



  // Vector base operations


  // Relocate vector
  //
  // Relocate the elements of base into a newly allocated buffer of size n
  // and make base point to the new buffer.
  template <typename T>
    inline void
    relocate_vector(vector_base<T>& base, std::size_t n)
    {
      assert(n >= base.size());
      vector_core<T> buf = base.allocate_core(n);
      try {
        buf.last = uninitialized_move(base.alloc, 
                                      base.core.first, 
                                      base.core.last, 
                                      buf.first);
      } catch(...) {
        base.deallocate(buf);
        throw;
      }
      base.deallocate();
      base.core.first = buf.first;
      base.core.last = buf.last;
      base.core.limit = buf.limit;
    }



  // Shift the elements of the buffer to the "right", creating an n-object range
  // of uninitialized objects at pos. That is, [pos, pos + n) is uninitialized
  // after the shift operation and [pos + n, pos + 2 * n) hold the elements of
  // the original range.
  template <typename T>
    inline void 
    vector_shift_right(vector_base<T>& base, T* pos, std::size_t n)
    {
      // Move the last - n - 1 into the uninitalized region of the buffer and
      // update last so it points to the new end.
      T* last = base.core.last;
      T* mid = last - n;
      base.core.last = uninitialized_move(base.alloc, mid, last, last);
      move_backward(pos, mid, last);
    }



  // Shift the elements of the buffer to the left, so that the eleemnt pointed
  // at by pos is overwritten and the resulting buffer is one element smaller.
  template <typename T>
    inline void
    vector_shift_left(vector_base<T>& base, T* pos)
    {
      T* mid = pos + 1;
      if (mid != base.core.last)
        move(mid, base.core.last, pos);
      --base.core.last;
      destroy(base.alloc, base.core.last);
    }



  // Shift the elements of the vector left so that the eleemnts of the range
  // [first, last) are overwritten and the resulting buffer is last - first
  // elements smaller.
  template <typename T>
    inline void 
    vector_shift_left(vector_base<T>& base, T* first, T* last)
    {
      if (first != last) {
        if (last != base.core.last) {
          auto i = move(last, base.core.last, first);
          vector_erase_to_end(base, i);
          base.core.last = i;
        } else {
          vector_erase_to_end(base, first);
          base.core.last = first;
        }
      }
    }



  // Erase n elements at the end of this buffer, updating this->last to point to
  // the new last initialized value.
  //
  // Requires:
  //    n < base.size()
  template <typename T>
    inline void 
    vector_erase_at_end(vector_base<T>& base, std::size_t n)
    {
      assert(n < base.size());
      destroy(base.alloc, base.core.last - n, base.core.last);
      base.core.last -= n;
    }


  // Erase all the elements form [pos, last) in base and adjust base so that
  // it now describes the range [first, pos).
  //
  // Requires:
  //    pos is in base
  template <typename T>
    inline void
    vector_erase_to_end(vector_base<T>& base, T* pos)
    {
      destroy(base.alloc, pos, base.core.last);
      base.core.last = pos;
    }


  // Insert or emplace args into the position indicated by pos. Note that this
  // does not update out's last pointer; it is assumed that is done when data is
  // shifted to accommodate the insertion.
  //
  // Requires:
  //    Constructible<T, Args...>
  //    is_uninitialized(pos)
  template <typename T, typename... Args>
    inline T* 
    vector_insert(vector_base<T>& base, T* pos, Args&&... args)
    {
      static_assert(Constructible<T, Args...>(), "");
      if (pos == base.core.last) {
        construct(base.alloc, base.core.last, std::forward<Args>(args)...);
        ++base.core.last;
      } else {
        vector_shift_right(base, pos, 1);
        construct(base.alloc, pos, std::forward<Args>(args)...);
      }
      return pos;
    }


  // Copy the values in [first, last) into the buffer. Note that the buffer
  // must have sufficient capacity to accommodate the insertion.
  //
  // Requires:
  //    Forward_iterator<I> 
  //    this->size() + distance(first, last) <= capacity
  template <typename T, typename I>
    inline T*
    vector_range_insert(vector_base<T>& base, T* pos, I first, I last)
    {
      T* end = base.core.last;               // The original end of the buffer
      std::size_t k = end - pos;             // The size of [pos, end)
      std::size_t n = distance(first, last); // The size of [first, last)
      if (k > n) {
        // There are fewer elements being inserted than in the remainder of
        // the buffer, so we can simply shift and copy.
        vector_shift_right(base, pos, k);
        copy(first, last, pos);
      } else {
        // There are more elements being inserted than in the remainder so
        // we break up the sequence of elements being inserted, copying some
        // elements into the uninitilized buffer, moving the remainder, and
        // copying the rest over the moved elements.
        I mid = next(first, k);
        base.core.last = uninitialized_copy(base.alloc, mid, last, base.core.last);
        base.core.last = uninitialized_move(base.alloc, pos, end, base.core.last);
        copy(first, mid, pos);
      }
      return pos;
    }



} // namespace origin

#endif
