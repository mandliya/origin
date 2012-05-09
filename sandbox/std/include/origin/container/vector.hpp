// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_CONTAINER_VECTOR_HPP
#define ORIGIN_CONTAINER_VECTOR_HPP

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
      explicit vector_base(allocator& a)
        : alloc(a), first {nullptr}, last {nullptr}, limit {nullptr}
      { }



      // Move constructible
      // Transfer the state of x into this vector base, and reset the state
      // of x to its default. Note that the vector base is not move-assignable.
      vector_base(vector_base&& x)
        : alloc(x.alloc), first {x.first}, last {x.last}, limit {x.limit}
      { 
        x.first = x.last = x.limit = nullptr;
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




  // Vector
  // A vector is a dynamically allocated and efficiently extendable array of
  // objects.
  //
  // FIXME: Finish documentation.
  template <typename T>
    class vector : private vector_base<T>
    {
      using Base = vector_base<T>;

      // Return a reference to the vector base.
      Base&       base()       { return *this; }
      const Base& base() const { return *this; }

    public:
      // Default constructible
      vector() : Base() { }
      vector(allocator& alloc) : Base(alloc) { }

      // Move semantics
      vector(vector&& x);
      vector(vector&& x, allocator& alloc);
      vector& operator=(vector&& x);

      // Copy semantics
      vector(const vector& x);
      vector(const vector& x, allocator& alloc);
      vector& operator=(const vector& x);

      // Fill constructible
      vector(std::size_t n, const T& value = {});
      vector(std::size_t n, const T& value, allocator& alloc);

      // Range constructible
      vector(std::initializer_list<T> list);
      vector(std::initializer_list<T> list, allocator& alloc);
      vector& operator=(std::initializer_list<T>& list);

      // Destructible.
      ~vector() { clear(); }


      // Observers
      allocator&       get_allocator()       { return this->alloc; }
      const allocator& get_allocator() const { return this->alloc; }

      bool empty() const { return Base::empty(); }
      std::size_t size() const { return Base::size(); }
      std::size_t capacity() const { return Base::capacity(); }



      // Data access
      T&       operator[](std::size_t n)       { return *(this->first + n); }
      const T& operator[](std::size_t n) const { return *(this->first + n); }

      T&       front()       { *this->first; }
      const T& front() const { *this->first; }

      T&       back()       { return *(this->last - 1); }
      const T& back() const { return *(this->last - 1); }


      // Capacity
      void reserve();
      void reserve(std::size_t n);
      void resize(std::size_t n, const T& value = {});

      // Modifiers
      void push_back(T&& value);
      void push_back(const T& value);
      void pop_back();

      void clear();
      vector& swap(vector& tmp);



      // Iterators
      T *begin() { return this->first; }
      T *end()   { return this->last; }

      const T* begin() const { return this->first; }
      const T* end() const   { return this->last; }
    };


  template <typename T> struct vector_util;


  template <typename T>
    vector<T>::vector(vector&& x) 
      : Base(std::move(x)) 
    { }

  // If an alternative allocator is given for the move, then we must allocate
  // new memory using that allocator and move the elements of x into the
  // newly allocated memory. We can't just steal pointers since alloc was not
  // responsible for the creation of x's pointers.
  template <typename T>
    vector<T>::vector(vector&& x, allocator& alloc)
      : Base(alloc, x.size())
    {
      vector_util<T>::move_init(*this, x.begin(), x.size(), begin());
    }
    
  template <typename T>  
    auto vector<T>::operator=(vector&& x) -> vector&
    { 
      vector tmp {std::move(x)};
      return swap(tmp); 
    }

  template <typename T>
    vector<T>::vector(const vector& x)
      : Base(x.size())
    { 
      vector_util<T>::copy_init(*this, x.begin(), x.size(), begin());
    }

  template <typename T>
    vector<T>::vector(const vector& x, allocator& alloc)
      : Base(alloc, x.size())
    {
      vector_util<T>::copy_init(*this, x.begin(), x.size(), begin());
    }

  template <typename T>
    auto vector<T>::operator=(const vector& x) -> vector&
    { 
      vector tmp {x};
      return swap(tmp);
    }

  template <typename T>
    inline vector<T>::vector(std::size_t n, const T& value)
      : Base(n)
    {
      vector_util<T>::fill_init(*this, n, value);
    }

  template <typename T>
    inline vector<T>::vector(std::size_t n, const T& value, allocator& alloc)
      : Base(alloc, n)
    {
      vector_util<T>::fill_init(*this, n, value);
    }

  template <typename T>
    vector<T>::vector(std::initializer_list<T> list)
      : Base(list.size())
    {
      vector_util<T>::copy_init(*this, list.begin(), list.size(), begin());
    }

  template <typename T>
    vector<T>::vector(std::initializer_list<T> list, allocator& alloc)
      : Base(alloc, list.size())
    {
      vector_util<T>::copy_init(*this, list.begin(), list.size(), begin());
    }

  template <typename T>
    auto vector<T>::operator=(std::initializer_list<T>& list) -> vector&
    {
      vector tmp {list};
      return swap(tmp);
    }


  template <typename T>
    void vector<T>::push_back(T&& value)
    {
      vector_util<T>::maybe_grow(*this);
      construct(this->alloc, *this->last, std::move(value));
      ++this->last;
    }

  // FIXME: Consider refactoring this into the util class.
  template <typename T>
    void vector<T>::push_back(const T& value)
    {
      vector_util<T>::maybe_grow(*this);
      construct(this->alloc, *this->last, value);
      ++this->last;
    }

  // FIXME: Consider refactoring to the util class.
  template <typename T>
    void vector<T>::pop_back()
    {
      assert(!empty());
      --this->last;
      destroy(this->alloc, *this->last);
    }

  // Reserve only enough capacity required to store the initialized values
  // of the vector.
  //
  // NOTE: The standard claims that the corresponding shrink_to_fit method
  // is a non-binding request, meaning that we don't have to implement this,
  // or we can reserve more memory than just the size.
  template <typename T>
    void vector<T>::reserve()
    {
      if (capacity() == size())
        return;
      vector_util<T>::shrink(*this, size());
    }

  // Reserve n objects of memory. Note that if n is less than the current 
  // capacity(), the operation has no effect.
  template <typename T>
    void vector<T>::reserve(std::size_t n)
    {
      if (n > capacity())
        vector_util<T>::grow(*this, n);
    }

  // FIXME: Refactor into the util class?
  template <typename T>
    void vector<T>::resize(std::size_t n, const T& value)
    {
      if (n > size()) {
        // If more capacity is needed, ensure that we have it.
        if (n > capacity())
          reserve(n);

        // FIXME: We can't separate the reserve and fill calls. If an exception
        // thrown here, then we've lost the strong exception guarantee, which
        // is actually pretty easy to achieve. We have to move and fill inside
        // a single try block.
        this->last = 
          uninitialized_fill_n(this->alloc, this->last, n - size(), value);

      } else if (n < size()) {
        destroy(this->alloc, this->first + n, this->last);
        this->last = this->first + n;
      }
    }


  // Erase all of the initialized elemnts, but do not deallocate any memory.
  template <typename T>
    void vector<T>::clear()
    {
      vector_util<T>::clear(*this);
    }


  // Two vectors may only be swapped when they have the same allocator (i.e.,
  // the allocators must compare equal).
  template <typename T>
    auto vector<T>::swap(vector& tmp) -> vector&
    {
      Base::swap(tmp.base());
      return *this;
    }






  // Vector (utility)
  // The vector util class implements common algorithms for the vector and
  // vector base classes. The purpose of this class is to encapsulate the
  // actual logic used by the data structure, hopefully allowing us to reuse
  // aspects of it in other data structures.
  template <typename T>
    struct vector_util
    {
      // Fill the [base.first, base.first + n) elements with a copy of value.
      //
      // TODO: optimize for noexcept copies?
      static void 
      fill_init(vector_base<T>& base, std::size_t n, const T& value)
      {
        try {
          base.last = uninitialized_fill_n(base.alloc, base.first, n, value);
        } catch (...) {
          base.deallocate(base.first, n);
          throw;
        }
      }



      // FIXME: If move T is not nothrow-movable, then we can't actually move;
      // we have to copy values. In fact, this holds for *every* move operation
      // in the utility class.
      static void
      move_init(vector_base<T>& base, const T* first, std::size_t n, T* result)
      {
        try {
          base.last = uninitialized_move_n(base.alloc, first, n, result);
        } catch (...) {
          base.deallocate(base.first, n);
          throw;
        }
      }



      // Copy the values in [first, first + n) into the uninitialized
      // objects of [result, result + n).
      //
      // TODO: Optimize for no-except copies?
      static void 
      copy_init(vector_base<T>& base, const T* first, std::size_t n, T* result)
      {
        try {
          base.last = uninitialized_copy_n(base.alloc, first, n, result);
        } catch (...) {
          base.deallocate(base.first, n);
          throw;
        }
      }


      // Grow the capacity of the vector base to n elements.
      static void grow(vector_base<T>& base, std::size_t n)
      {
        T* first = base.allocate(n);
        T* last = first;
        T* limit = first + n;

        // Move data into the newly allocated memory.
        try {
          last = uninitialized_move(base.alloc, base.first, base.last, first);
        } catch(...) {
          base.deallocate(first, n);
          throw;
        }

        // Release the previously used memory block and point to the new
        // block.
        base.deallocate(base.first, base.capacity());
        base.first = first;
        base.last = last;
        base.limit = limit;
      }


      // If the vector is full, the reserve additional space. The first
      // reallocation reserves 4 bytes; all subsequent allocations reserve
      // double the previous capacity.
      //
      // FIXME: Make these values (4, 2) depend on vector_base<T>?.
      static void maybe_grow(vector_base<T>& base)
      {
        if (base.full()) {
          std::size_t n = base.capacity() == 0 ? 4 : 2 * base.capacity();
          grow(base, n);
        }
      }


      // Reduce the size of the vector base to n elements. If n < size(),
      // only n elements are reserved. The remaining size() - n are destroyed.
      static void shrink(vector_base<T>&  base, std::size_t n)
      {
        T* first = base.allocate(n);
        T* last = first;
        T* limit = first + n;

        // We may need to destroy some data.
        // NOTE: This must not throw! Do I need to do something special to
        // guarantee that?
        if (n < base.size()) {
          destroy(base.alloc, base.first + n, base.last);
        }

        // Move values from the old memory segment into the new one. Note that
        // we may not be moving n elements if base.size() is smaller.
        // FIXME: Use min() if we can do so without inducing a dependency.
        std::size_t sz = n < base.size() ? n : base.size();
        try {
          last = uninitialized_move_n(base.alloc, base.first, sz, first);
        } catch (...) {
          base.deallocate(first, n);
          throw;
        }

        // Deallocate old block of memory and reset the vector base.
        base.deallocate(base.first, base.capacity());
        base.first = first;
        base.last = last;
        base.limit = limit;
      }



      // Erase all of the initialized elements in the vector base.
      static void clear(vector_base<T>& base)
      {
        destroy(base.alloc, base.first, base.last);
        base.last = base.first;
      }
    };



} // namespace origin

#endif
