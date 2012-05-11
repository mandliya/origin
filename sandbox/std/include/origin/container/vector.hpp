// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_CONTAINER_VECTOR_HPP
#define ORIGIN_CONTAINER_VECTOR_HPP

#include <origin/container/impl/vector_base.hpp>

namespace origin
{
  // Vector
  // A vector is a dynamically allocated and efficiently extendable array of
  // objects.
  //
  // FIXME: Finish documentation.
  template <typename T>
    class vector
    {
      vector_base<T> base;

    public:
      using iterator       = T*;
      using const_iterator = const T*;

      // Default constructible
      vector() noexcept : base() { }
      vector(allocator& alloc) noexcept : base(alloc) { }

      // Move semantics
      vector(vector&& x) noexcept;
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

      // Destructible
      ~vector() { clear(); }



      // Observers
      allocator&       get_allocator()       { return base.alloc; }
      const allocator& get_allocator() const { return base.alloc; }

      bool empty() const { return base.empty(); }
      bool full() const { return base.full(); }
      std::size_t size() const { return base.size(); }
      std::size_t capacity() const { return base.capacity(); }
      std::size_t residual() const { return base.residual(); }


      // Data access
      T&       operator[](std::size_t n)       { return *(base.first + n); }
      const T& operator[](std::size_t n) const { return *(base.first + n); }

      T&       front()       { *base.first; }
      const T& front() const { *base.first; }

      T&       back()       { return *(base.last - 1); }
      const T& back() const { return *(base.last - 1); }



      // Capacity
      void reserve();
      void reserve(std::size_t n);
      void resize(std::size_t n, const T& value = {});



      // Modifiers
      void push_back(T&& value);
      void push_back(const T& value);
      void pop_back();

      iterator insert(const_iterator pos, T&& value);
      iterator insert(const_iterator pos, const T& value);
      iterator insert(const_iterator pos, std::size_t n, const T& value);

      iterator erase(const_iterator pos);
      iterator erase(const_iterator first, const_iterator last);

      void clear();
      vector& swap(vector& tmp);



      // Iterators
      iterator begin() { return base.first; }
      iterator end()   { return base.last; }

      const_iterator begin() const { return base.first; }
      const_iterator end() const   { return base.last; }

      const_iterator cbegin() const { return base.first; }
      const_iterator cend() const   { return base.last; }
    };



  template <typename T>
    vector<T>::vector(vector&& x) noexcept
      : base(std::move(x.base)) 
    { }

  // If an alternative allocator is given for the move, then we must allocate
  // new memory using that allocator and move the elements of x into the
  // newly allocated memory. We can't just steal pointers since alloc was not
  // responsible for the creation of x's pointers.
  template <typename T>
    vector<T>::vector(vector&& x, allocator& alloc)
      : base(alloc, x.size())
    {
      vector_util::move_init(base, x.base);
    }

  template <typename T>  
    auto vector<T>::operator=(vector&& x) -> vector&
    { 
      vector tmp {std::move(x)};
      return swap(tmp); 
    }

  template <typename T>
    vector<T>::vector(const vector& x)
      : base(x.size())
    { 
      vector_util::copy_init(base, x.base);;
    }

  template <typename T>
    vector<T>::vector(const vector& x, allocator& alloc)
      : base(alloc, x.size())
    {
      vector_util::copy_init(base, x.base);;
    }

  template <typename T>
    auto vector<T>::operator=(const vector& x) -> vector&
    {
      vector tmp {x};
      return swap(tmp);
    }

  template <typename T>
    inline vector<T>::vector(std::size_t n, const T& value)
      : base(n)
    {
      vector_util::fill_init(base, n, value);
    }

  template <typename T>
    inline vector<T>::vector(std::size_t n, const T& value, allocator& alloc)
      : base(alloc, n)
    {
      vector_util::fill_init(base, n, value);
    }

  template <typename T>
    vector<T>::vector(std::initializer_list<T> list)
      : base(list.size())
    {
      vector_util::copy_init(base, list.begin(), list.size());
    }

  template <typename T>
    vector<T>::vector(std::initializer_list<T> list, allocator& alloc)
      : base(alloc, list.size())
    {
      vector_util::copy_init(base, list.begin(), list.size());
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
      if (full()) {
        vector_base<T> tmp(base.next_capacity(), base.alloc);
        vector_util::move_init(tmp, base);
        vector_util::insert_at_end(tmp, std::move(value));
        base.swap(tmp);
      } else {
        insert_at_end(base, std::move(value));
    }
  }

  template <typename T>
    void vector<T>::push_back(const T& value)
    {
      if (full()) {
        vector_base<T> tmp(base.next_capacity(), base.alloc);
        vector_util::move_init(tmp, base);
        vector_util::insert_at_end(tmp, value);
        base.swap(tmp);
      } else {
        vector_util::insert_at_end(base, value);
      }
    }

  template <typename T>
    void vector<T>::pop_back()
    {
      assert(!empty());
      vector_util::erase_at_end(base, 1);
    }

  template <typename T>
    auto vector<T>::insert(const_iterator pos, const T& value) -> iterator
    {
      std::size_t n = pos - base.first;
      if (full()) {
        vector_base<T> tmp(base.next_capacity(), base.alloc);
        vector_util::move_to_end(tmp, base.first, pos);
        vector_util::insert_at_end(tmp, value);
        vector_util::move_to_end(tmp, pos, base.last).
        base.swap(tmp);
      } else {
        if (pos == end()) {
          vector_util::insert_at_end(base, value);
        } else {
          vector_util::shift_right(base, pos, 1);
          vector_util::insert_at(base, base.first + n, value);
        }
      }
      return base.first + n;
    }

  template <typename T>
    auto vector<T>::insert(const_iterator pos, T&& value) -> iterator
    {
      std::size_t n = pos - base.first;
      if (full()) {
        vector_base<T> tmp(base.next_capacity(), base.alloc);
        vector_util::move_to_end(tmp, base.first, pos);
        vector_util::insert_at_end(tmp, std::move(value));
        vector_util::move_to_end(tmp, pos, base.last);
        base.swap(tmp);
      } else {
        if (pos == end()) {
          vector_util::insert_at_end(base, std::move(value));
        } else {
          vector_util::shift_right(base, pos, 1);
          vector_util::insert_at(base, base.first + n, std::move(value));
        }
      }
      return base.first + n;
    }

  template <typename T>
    auto vector<T>::insert(const_iterator pos, std::size_t n, const T& value) 
      -> iterator
    {
      std::size_t m = pos - base.first;
      if (size() + n >= capacity()) {
        vector_base<T> tmp(base.next_capacity(), base.alloc);
        vector_util::move_to_end(tmp, base.first, pos);
        vector_util::fill_at_end(tmp, n, value);
        vector_util::move_to_end(tmp, pos, base.last);
        base.swap(tmp);
      } else {
        if (pos == end()) {
          vector_util::fill_at_end(base, n, value);
        } else {
          vector_util::shift_right(base, pos, n);
          vector_util::fill_at(base, base.first + m, n, value);
        }
      }
      return base.first + m;
    }


  template <typename T>
    auto vector<T>::erase(const_iterator pos) -> iterator
    {
      assert(pos >= base.first && pos < base.last);
      std::size_t n = pos - base.first;
      vector_util::shift_left(base, base.first + n, 1);
      return base.first + n;
    }

  template <typename T>
    auto vector<T>::erase(const_iterator first, const_iterator last) -> iterator
    {
      assert(first >= base.first && last <= base.last);
      std::size_t n = first - base.first;
      vector_util::shift_left(base, base.first + n, last - first);
      return base.first + n;
    }


  // Reserve only enough capacity required to store the initialized values
  // of the vector.
  //
  // Note that the standard claims that the corresponding shrink_to_fit method
  // is a non-binding request [23.3.6.3/9], meaning that we don't have to
  // implement this, or we can reserve more memory than just the size. However,
  // the implementation is easy enough.
  template <typename T>
    void vector<T>::reserve()
    {
      if (capacity() != size()) {
        vector_base<T> tmp(size(), base.alloc);
        vector_util::move_init(tmp, base);
        base.swap(tmp);
      }
    }

  // Reserve n objects of memory. Note that if n is less than the current 
  // capacity(), the operation has no effect.
  template <typename T>
    void vector<T>::reserve(std::size_t n)
    {
      if (n > capacity()) {
        vector_base<T> tmp(n, base.alloc);
        vector_util::move_init(tmp, base);
        base.swap(tmp);
      }
    }

  // FIXME: Thiis can be made much, much simpler by writing it in terms of
  // insert.
  template <typename T>
    void vector<T>::resize(std::size_t n, const T& value)
    {
      if (n > size()) {
        if (n > capacity()) {
          vector_base<T> tmp(n, base.alloc);
          vector_util::move_init(tmp, base);
          vector_util::fill_at_end(tmp, n - size(), value);
          base.swap(tmp);
        } else {
          vector_util::fill_at_end(base, n - size(), value);
        }
      } else if (n < size()) {
        vector_util::erase_at_end(base, size() - n);
      }
    }


  // Erase all of the initialized elemnts, but do not deallocate any memory.
  template <typename T>
    void vector<T>::clear()
    {
      vector_util::erase_all(base);
    }


  // Two vectors may only be swapped when they have the same allocator (i.e.,
  // the allocators must compare equal).
  template <typename T>
    auto vector<T>::swap(vector& x) -> vector&
    {
      base.swap(x.base);
      return *this;
    }





} // namespace origin

#endif
