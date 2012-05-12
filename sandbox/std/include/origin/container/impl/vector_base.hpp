// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_CONTAINER_VECTOR_BASE_HPP
#define ORIGIN_CONTAINER_VECTOR_BASE_HPP

#include <origin/memory.hpp>

namespace origin
{
  // Vector base
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
      // Default constructible
      // Initialize the vector base with its default state; its first and
      // last pointers set to nullptr, and its size() is 0.
      vector_base() noexcept
        : alloc(default_allocator())
        , first {nullptr}, last {nullptr}, limit {nullptr}
      { }


      // Default with allocator
      // Default construct this object, using alloc as the allocator for
      // memory allocation and deallocation.
      explicit vector_base(allocator& a) noexcept
        : alloc(a), first {nullptr}, last {nullptr}, limit {nullptr}
      { }



      // Move constructible
      // Transfer the state of x into this vector base, and reset the state
      // of x to its default. Note that the vector base is not move-assignable.
      vector_base(vector_base&& x) noexcept
        : alloc(x.alloc), first {x.first}, last {x.last}, limit {x.limit}
      { 
        x.first = x.last = x.limit = nullptr;
      }

      vector_base(vector_base&& x, allocator& alloc)
        : alloc(alloc)
        , first {allocate(x.size())}, last {first}, limit {first + x.size()}
      {

      }

      vector_base& operator=(vector_base&&) = delete;



      // Copy constructible
      // A vector base is non-copyable. In order to copy a vector, allocate a
      // a new block of memory and then copy the values in the derived class.
      vector_base(const vector_base&) = delete;
      vector_base& operator=(const vector_base&) = delete;



      // Size construction
      // Initialize the vector base by allocating exactly n objects.
      vector_base(std::size_t n)
        : alloc(default_allocator())
        , first {allocate(n)}, last {first}, limit {first + n}
      { }

      // Size with allocator
      vector_base(std::size_t n, allocator& a)
        : alloc(a), first {allocate(n)}, last {first}, limit{first + n}
      { }



      // Destructible
      ~vector_base()
      {
        deallocate(first, capacity());
      }


      // Allocate and deallocate

      // Allocate n objects, returning a pointer to the first.
      T *allocate(std::size_t n)
      {
        return origin::allocate<T>(alloc, n);
      }

      // Deallocate n objects pointed to by p.
      void deallocate(T* p, std::size_t n)
      {
        origin::deallocate(alloc, p, n);
      }



      // Observers

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

      // Returns the next capacity of the vector base if the owning vector
      // is dynamically resized.
      //
      // TODO: Can I find a better name for this function?
      std::size_t next_capacity() const
      {
        return capacity() == 0 ? 4 : capacity() * 2;
      }


      // Mutators

      // Exchange the reresentation of x with this object. The operation is
      // only valid when the allocators of this and x compare equal.
      //
      // Requires:
      //    this->alloc == x.alloc
      void swap(vector_base& x);


      // Copy the values in [first, last) into the uninitialized range of
      // objects in this buffer indicated by pos.
      //
      // Requires:
      //    pos >= this->first
      //    pos + (last - first) <= this->limit
      //    is_uninitilized(pos, pos + (last - first))
      template <typename I>
        T* copy_at_pos(T* pos, I first, I last);


      // Copy the values in [first, last) into the uninitialized range of
      // objects at the end of this buffer. 
      //
      // Requires:
      //    this->last + (last - first) <= this->limit
      //    is_uninitialized(this->last, this->last + (last - first))
      template <typename I>
        void copy_at_end(I first, I last);


      // Copy the values in x into the uninitialized range of objects at the
      // end of this buffer.
      //
      // Requires:
      //    this->last + x.size() <= this->limit
      //    is_uninitialized(this->last, this->last + x.size())
      void copy_at_end(const vector_base<T>& x);



      template <typename I>
        T* move_at_pos(T* pos, I first, I last);

      template <typename I>
        void move_at_end(I first, I last);

      void move_at_end(vector_base<T>& x);



    // Fill the objects in the range [pos, pos + n) with the given value. Note
    // that this operation does not update the any of the pointers on the vector
    // base. This is assumed to have been done previously.
    //
    // Requires:
    //    Constructible<T, U>
    //    ???
    template <typename U>
      T* fill_at_pos(T* pos, std::size_t n, const U& value)
      {
        static_assert(Constructible<T, U>(), "");
        return uninitialized_fill_n(this->alloc, pos, n, value);
      }

    // Fill the objects of [this->last, this->last + n) with the given value and
    // update this->last to this->last + n.
    //
    // Requires:
    //    Constructible<T, U>
    //    ???
    template <typename U>
      void fill_at_end(std::size_t n, const U& value) 
      {
        static_assert(Constructible<T, U>(), "");
        this->last = fill_at_pos(this->last, n, value);
      }


      // Erase n elements at the end of this buffer, updating this->last to
      // point to the new last initialized value.
      //
      // Requires:
      //    n < this->size()
      void erase_at_end(std::size_t n);


      // Erase all the elements in this buffer, updating the buffer so that
      // it is empty.
      void erase();


      // Insert or emplace args into the position indicated by pos. Note that
      // this does not update out's last pointer; it is assumed that is done
      // when data is shifted to accommodate the insertion.
      //
      // Requires:
      //    is_uninitialized(pos)
      template <typename... Args>
        void insert(T* pos, Args&&... args);


      // Insert or emplace the given arguments into the last object of the base
      // and update the last pointer to reflect the insertion.
      //
      // Requires:
      //    this->last < this->limit
      template <typename... Args>
        void append(Args&&... args);


      // TODO: These are great candidates for general purpose memory algorithms.

      // Shift the elements of the buffer to the "right", creating an n-object
      // range of uninitialized objects at pos., That is, [pos, pos + n) is an
      // uninitialized after the shift operation and [pos + n, pos + 2 * n)
      // hold the elements of the original range.
      void shift_right(T* pos, std::size_t n);

      // Shift the elements of the buffer to the "left", effectively erasing the
      // n elements pointed to by pos. That is, the elements [pos, pos + n) are
      // replaced by those in [pos + n, pos + 2 * n).
      void shift_left(T* pos, std::size_t n);

    public:
      allocator& alloc;
      T *first;
      T *last;
      T *limit;
    };



  template <typename T>
    void vector_base<T>::swap(vector_base& x)
    {
      assert(alloc == x.alloc);
      std::swap(first, x.first);
      std::swap(last, x.last);
      std::swap(limit, x.limit);
    }

  template <typename T>
    template <typename I>
      inline T* vector_base<T>::copy_at_pos(T* pos, I first, I last)
      {
        assert(pos >= this->first && pos <= this->last);
        return uninitialized_copy(this->alloc, first, last, pos);
      }

  template <typename T>
    template <typename I>
      inline void vector_base<T>::copy_at_end(I first, I last)
      {
        this->last = copy_at_pos(this->last, first, last);
      }

  template <typename T>
    inline void vector_base<T>::copy_at_end(const vector_base& x)
    {
      assert(this->last + x.size() <= this->limit);
      copy_at_end(x.first, x.last);
    }

  template <typename T>
    template <typename I>
      inline T* vector_base<T>::move_at_pos(T* pos, I first, I last)
      {
        assert(pos >= this->first && pos <= this->last);
        return uninitialized_move(this->alloc, first, last, pos);
      }

  template <typename T>
    template <typename I>
      inline void vector_base<T>::move_at_end(I first, I last)
      {
        this->last = move_at_pos(this->last, first, last);
      }

  template <typename T>
    inline void vector_base<T>::move_at_end(vector_base& x)
    {
      assert(this->last + x.size() <= this->limit);
      move_at_end(x.first, x.last);
    }

  template <typename T>
    template <typename... Args>
      inline void vector_base<T>::insert(T* pos, Args&&... args)
      {
        this->shift_right(pos, 1);
        construct(this->alloc, *pos, std::forward<Args>(args)...);
      }

  template <typename T>
    template <typename... Args>
      inline void vector_base<T>::append(Args&&... args)
      {
        construct(this->alloc, *this->last, std::forward<Args>(args)...);
        ++this->last;
      }

  template <typename T>
    inline void vector_base<T>::erase_at_end(std::size_t n)
    {
      destroy(this->alloc, this->last - n, this->last);
      this->last -= n;
    }

  template <typename T>
    inline void vector_base<T>::erase()
    {
      destroy(this->alloc, this->first, this->last);
      this->last = this->first;
    }

  template <typename T>
    void vector_base<T>::shift_right(T* pos, std::size_t n)
    {
      T* p = this->last - 1;
      T* q = p + n;

      // Shift the last elements of the vector base into the uninitialized
      // segment of memory. This is effectively unitialized_move_backward.
      while (q != this->last) {
        construct(this->alloc, *q, std::move(*p));
        --p;
        --q;
      }

      // Then shift the remaining elements from [pos, p) using standard
      // move assignment, guaranteeing destruction of initialized elements.
      // This is effectively move_backward.
      while (p != pos) {
        *q = std::move(*p);
        --p;
        --q;
      }
      *q = std::move(*p);

      this->last += n;
    }

  template <typename T>
    void vector_base<T>::shift_left(T* pos, std::size_t n)
    {
      // This should be equivalent to move(pos + n, out.last, pos).
      T* p = pos + n;
      T* q = pos;
      while (p != this->last) {
        *q = std::move(*p);
        ++q;
        ++p;
      }
      destroy(this->alloc, q, this->last);
      this->last = q;
    }

} // namespace origin

#endif
