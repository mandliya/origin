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
      // TODO: Better name for this function?
      std::size_t next_capacity() const
      {
        return capacity() == 0 ? 4 : capacity() * 2;
      }


      // Mutators
      void swap(vector_base& x)
      {
        assert(alloc == x.alloc);
        std::swap(first, x.first);
        std::swap(last, x.last);
        std::swap(limit, x.limit);
      }

    public:
      allocator& alloc;
      T *first;
      T *last;
      T *limit;
    };



  // Vector (utility)
  // The vector util class implements common algorithms for the vector and
  // vector base classes. The purpose of this class is to encapsulate the
  // actual logic used by the data structure, hopefully allowing us to reuse
  // aspects of it in other data structures.
  //
  // FIXME: Move these methods into the vector base class, but do a better job
  // designing them. They need some serious attention.
  struct vector_util
  {
    // Move values from the input base to the output base where
    // out.capacity() >= in.size().
    //
    // This function only supports the strong exception guarantee when called
    // from a constructor. A propagated exception will cause the vector's base
    // to be destroyed.
    template <typename Base>
      static void move_init(Base& out, const Base& in) 
      {
        out.last = uninitialized_move_n(out.alloc, in.first, in.size(), out.first);
      }



    // Move the elemnts in the range [first, last) to the end of output base.
    // The output base must have out.capacity() > out.size() + (last - first).
    template <typename Base, typename T>
      static void move_to_end(Base& out, const T* first, const T* last)
      {
        out.last = uninitialized_move(out.alloc, first, last, out.last);
      }



    // Copy the values from the range [first, first + n) into the output base
    // where out.capacity() >= n.
    template <typename T, typename I>
      static void copy_init(vector_base<T>& out, I first, std::size_t n)
      {
        out.last = uninitialized_copy_n(out.alloc, first, n, out.first);
      }


    // Copy the values in [first, last) into the uninitialized range of objects
    // indicated by pos.
    template <typename T, typename I>
      static T* copy_at_pos(vector_base<T>& out, T* pos, I first, I last)
      {
        return uninitialized_copy(out.alloc, first, last, pos);
      }

    // Copy the values in [first, last) into the uninitialized range of
    // objects at the end of the buffer.
    template <typename T, typename I>
      void copy_at_end(vector_base<T>& out, I first, I last)
      {
        out.last = copy_at_pos(out, first, last, out.last);
      }


    // FIXME: This algorithm is redundant with copy_at_end but only when
    // out.first == out.last.
    template <typename T, typename I>
      static void copy_init(vector_base<T>& out, I first, I last)
      {
        out.last = copy_at_pos(out, out.first, first, last);
      }


    // Copy values from the input base to the output base where 
    // out.capacity() >= in.size(). 
    template <typename Base>
      static void copy_init(Base& out, const Base& in)
      {
        copy_init(out, in.first, in.size());
      }



    // Fill the objects of the output base with the given value where
    // out.capacity() >= n.
    template <typename Base, typename T>
      static void fill_init(Base& out, std::size_t n, const T& value)
      {
        out.last = uninitialized_fill_n(out.alloc, out.first, n, value);
      }



    // Destroy the n objects at the end of the vector where n <= size().
    template <typename Base>
      static void erase_at_end(Base& out, std::size_t n)
      {
        destroy(out.alloc, out.last - n, out.last);
        out.last -= n;
      }



    // Erase all of the initialized elements in the vector base.
    template <typename Base>
      static void erase_all(Base& out)
      {
        erase_at_end(out, out.size());
      }



    // Fill the values in the range [pos, pos + n) with the given value. The
    // out base must have out.capaicty() >= out.size() + n. Note that this
    // operation does not update the any of the pointers on the vector base.
    // This is assumed to have been done previously.
    template <typename Base, typename I, typename T>
      static void fill_at(Base& out, I pos, std::size_t n, const T& value)
      {
        uninitialized_fill_n(out.alloc, pos, n, value);
      }

    // Fill the objects of [out.last, out.last + n) with the given value
    // where out.capacity() >= out.size() + n.
    template <typename Base, typename T>
      static void fill_at_end(Base& out, std::size_t n, const T& value) 
      {
        out.last = uninitialized_fill_n(out.alloc, out.last, n, value);
      }



    // Insert or emplace the given arguments into the position indicated by
    // pos. Note that this does not update out's last pointer. We assume that
    // is done when data is shifted to accommodate the insertion.
    template <typename Base, typename I, typename... Args>
      static void insert_at(Base& out, I pos, Args&&... args)
      {
        construct(out.alloc, *pos, std::forward<Args>(args)...);
      }


    // Insert or emplace the given arguments into the last object of the base,
    // assuming that out.capacity() > out.size().
    template <typename Base, typename... Args>
      static void insert_at_end(Base& out, Args&&... args)
      {
        construct(out.alloc, *out.last, std::forward<Args>(args)...);
        ++out.last;
      }



    // Shift the elements of the output base to the right, creating a range
    // of invalid values in the middle of the vector base. Note that
    // out.capacity() >= out.size() + n.
    //
    // Note that this operation leaves the resulting base in a state that
    // violates its invariants; there are partially formed objects in the middle
    // of the vector on termination. A subsequent insert or fill is required
    // to re-initialize those values.
    template <typename T>
      static void shift_right(vector_base<T>& out, const T* pos, std::size_t n)
      {
        T* p = out.last - 1;
        T* q = p + n;

        // Shift the last elements of the vector base into the uninitialized
        // segment of memory.
        // This is effectively unitialized_move_backward.
        while (q != out.last) {
          construct(out.alloc, *q, std::move(*p));
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
        
        out.last += n;
      }


    // Shift the elements of the vector to the left so that those in the
    // range [pos, pos + n) are overwritten by the elements to the right of
    // pos + n. The remaining n elements at the end of the vector are destroyed.
    // The function returns an iterator pointing to the first element to the
    // right of pos that was not destroyed.
    template <typename T>
      static void shift_left(vector_base<T>& out, T* pos, std::size_t n)
      {
        // This should be equivalent to move(pos + n, out.last, pos).
        T* p = pos + n;
        T* q = pos;
        while (p != out.last) {
          *q = std::move(*p);
          ++q;
          ++p;
        }
        assert(out.last - q == n);
        destroy(out.alloc, q, out.last);
        out.last = q;
      }

  };
} // namespace origin

#endif
