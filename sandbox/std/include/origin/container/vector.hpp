// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_CONTAINER_VECTOR_HPP
#define ORIGIN_CONTAINER_VECTOR_HPP

#include <initializer_list>

#include <origin/iterator.hpp>
#include <origin/range.hpp>
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
      vector& assign(std::size_t n, const T& value);

      // Iterator range constructible
      template <typename I>
        vector(I first, I last, Requires<Strict_input_iterator<I>()>* = {});
      template <typename I>
        vector(I first, I last, allocator& alloc, Requires<Strict_input_iterator<I>()>* = {});
      template <typename I>
        vector(I first, I last, Requires<Forward_iterator<I>()>* = {});
      template <typename I>
        vector(I first, I last, allocator& alloc, Requires<Forward_iterator<I>()>* = {});
      template <typename I>
        vector& assign(I first, I last);



      // Range constructible
      template <typename R>
        explicit vector(const R& range, Requires<Strict_input_range<R>()>* = {});
      template <typename R>
        vector(const R& range, allocator& alloc, Requires<Strict_input_range<R>()>* = {});
      template <typename R>
        explicit vector(const R& range, Requires<Forward_range<R>()>* = {});
      template <typename R>
        vector(const R& range, allocator& alloc, Requires<Forward_range<R>()>* = {});
      template <typename R>
        auto operator=(const R& range) -> Requires<Range<R>(), vector&>;



      vector(std::initializer_list<T> list);
      vector(std::initializer_list<T> list, allocator& alloc);
      vector& operator=(std::initializer_list<T> list);

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
      void collapse();
      void reserve(std::size_t n);
      void resize(std::size_t n, const T& value = {});



      // Modifiers
      void push_back(T&& value);
      void push_back(const T& value);
      void pop_back();

      iterator insert(const_iterator pos, T&& value);
      iterator insert(const_iterator pos, const T& value);
      iterator insert(const_iterator pos, std::size_t n, const T& value);
      iterator insert(const_iterator pos, std::initializer_list<T> list);

      template <typename I>
        iterator insert(const_iterator pos, I first, I last, Requires<Strict_input_iterator<I>()>* = {});
      template <typename I>
        iterator insert(const_iterator pos, I first, I last, Requires<Forward_iterator<I>()>* = {});
      template <typename R>
        iterator insert(const_iterator pos, const R& range, Requires<Range<R>()>* = {});

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
  //
  // Note that memory allocated to x is not released by this operation. However,
  // x is returned to an empty state.
  template <typename T>
    vector<T>::vector(vector&& x, allocator& alloc)
      : base(x.size(), alloc)
    {
      base.move_at_end(x.base);
      x.base.last = x.base.first;
    }

  // NOTE: Move assignment results in allocation. Because the allocator is
  // held by reference, it can't be replaced. That's fine since we've said
  // that allocators can't be replaced after initialization anyways.
  template <typename T>  
    auto vector<T>::operator=(vector&& x) -> vector&
    { 
      vector tmp {std::move(x), base.alloc};
      return swap(tmp); 
    }

  template <typename T>
    vector<T>::vector(const vector& x)
      : base(x.size())
    {
      base.copy_at_end(x.base);
    }

  template <typename T>
    vector<T>::vector(const vector& x, allocator& alloc)
      : base(alloc, x.size())
    {
      base.copy_at_end(x.base);
    }

  template <typename T>
    auto vector<T>::operator=(const vector& x) -> vector&
    {
      vector tmp {x, base.alloc};
      return swap(tmp);
    }

  template <typename T>
    inline vector<T>::vector(std::size_t n, const T& value)
      : base(n)
    {
      base.fill_at_end(n, value);
    }

  template <typename T>
    inline vector<T>::vector(std::size_t n, const T& value, allocator& alloc)
      : base(alloc, n)
    {
      base.fill_at_end(n, value);
    }

  template <typename T>
    inline auto vector<T>::assign(std::size_t n, const T& value) -> vector&
    {
      vector tmp {n, value, base.alloc};
      return swap(tmp);
    }

  template <typename T>
    template <typename I>
      vector<T>::vector(I first, I last, Requires<Strict_input_iterator<I>()>*)
        : base()
      {
        while (first != last)
          push_back(*first);
      }

  template <typename T>
    template <typename I>
      vector<T>::vector(I first, I last, allocator& alloc, Requires<Strict_input_iterator<I>()>*)
        : base(alloc)
      {
        while (first != last)
          push_back(*first);
      }

  template <typename T>
    template <typename I>
      vector<T>::vector(I first, I last, Requires<Forward_iterator<I>()>*)
        : base(distance(first, last))
      {
        base.copy_at_end(first, last);
      }

  template <typename T>
    template <typename I>
      vector<T>::vector(I first, I last, allocator& alloc, Requires<Forward_iterator<I>()>*)
        : base(alloc, distance(first, last))
      {
        base.copy_at_end(first, last);
      }

  template <typename T>
    template <typename I>
      auto vector<T>::assign(I first, I last) -> vector&
      {
        vector tmp {first, last, base.alloc};
        return swap(tmp);
      }

  template <typename T>
    template <typename R>
      vector<T>::vector(const R& range, Requires<Strict_input_range<R>()>*)
        : base()
      {
        for (const auto& x : range)
          push_back(x);
      }

  template <typename T>
    template <typename R>
      vector<T>::vector(const R& range, allocator& alloc, Requires<Strict_input_range<R>()>*)
        : base(alloc)
      {
        for (const auto& x : range)
          push_back(x);
      }

  template <typename T>
    template <typename R>
      vector<T>::vector(const R& range, Requires<Forward_range<R>()>*)
        : base(origin::size(range))
      {
        base.copy_at_end(origin::begin(range), origin::end(range));
      }

  template <typename T>
    template <typename R>
      vector<T>::vector(const R& range, allocator& alloc, Requires<Forward_range<R>()>*)
        : base(alloc, origin::size(range))
      {
        base.copy_at_end(origin::begin(range), origin::end(range));
      }

  template <typename T>
    template <typename R>
      auto vector<T>::operator=(const R& range) -> Requires<Range<R>(), vector&>
      {
        vector tmp {range, base.alloc};
        swap(tmp);
      }

  template <typename T>
    vector<T>::vector(std::initializer_list<T> list)
      : base(list.size())
    {
      base.copy_at_end(list.begin(), list.end());
    }

  template <typename T>
    vector<T>::vector(std::initializer_list<T> list, allocator& alloc)
      : base(alloc, list.size())
    {
      base.copy_at_end(list.begin(), list.end());
    }

  template <typename T>
    auto vector<T>::operator=(std::initializer_list<T> list) -> vector&
    {
      vector tmp {list, base.alloc};
      return swap(tmp);
    }

  template <typename T>
    void vector<T>::push_back(T&& value)
    {
      if (full()) {
        vector_base<T> tmp(base.next_capacity(), base.alloc);
        tmp.move_at_end(tmp, base);
        tmp.append(std::move(value));
        base.swap(tmp);
      } else {
        base.append(std::move(value));
    }
  }

  template <typename T>
    void vector<T>::push_back(const T& value)
    {
      if (full()) {
        vector_base<T> tmp(base.next_capacity(), base.alloc);
        tmp.move_at_end(base);
        tmp.append(value);
        base.swap(tmp);
      } else {
        base.append(value);
      }
    }

  template <typename T>
    void vector<T>::pop_back()
    {
      assert(!empty());
      base.erase_at_end(1);
    }

  template <typename T>
    auto vector<T>::insert(const_iterator pos, T&& value) -> iterator
    {
      std::size_t n = pos - base.first;
      T* mid = base.first + n;
      if (full()) {
        vector_base<T> tmp(base.next_capacity(), base.alloc);
        tmp.move_at_end(base.first, mid);
        tmp.append(std::move(value));
        tmp.move_at_end(mid, base.last);
        base.swap(tmp);
        return base.first + n;
      } else {
        if (pos == end()) {
          base.append(std::move(value));
        } else {
          base.insert(mid, std::move(value));
        }
        return mid;
      }
    }

  template <typename T>
    auto vector<T>::insert(const_iterator pos, const T& value) -> iterator
    {
      std::size_t n = pos - base.first;
      T* mid = base.first + n;
      if (full()) {
        vector_base<T> tmp(base.next_capacity(), base.alloc);
        tmp.move_at_end(base.first, mid);
        tmp.append(value);
        tmp.move_at_end(mid, base.last);
        base.swap(tmp);
        return base.first + n;
      } else {
        if (pos == end()) {
          base.append(value);
        } else {
          base.insert(mid, value);
        }
        return mid;
      }
    }

  template <typename T>
    auto vector<T>::insert(const_iterator pos, std::size_t n, const T& value) 
      -> iterator
    {
      std::size_t m = pos - base.first;
      T* mid = base.first + m;
      if (size() + n >= capacity()) {
        vector_base<T> tmp(base.next_capacity(), base.alloc);
        tmp.move_at_end(base.first, mid);
        tmp.fill_at_end(n, value);
        tmp.move_at_end(base.first, mid);
        base.swap(tmp);
        return base.first + m;
      } else {
        if (pos == end()) {
          base.fill_at_end(n, value);
        } else {
          base.shift_right(mid, n);
          base.fill_at_pos(mid, n, value);
        }
        return mid;
      }
    }

  template <typename T>
    template <typename I>
      auto vector<T>::insert(const_iterator pos, I first, I last, Requires<Strict_input_iterator<I>()>*)
        -> iterator
      {
        while (first != last) {
          pos = insert(pos, *first);
          ++first;
        }
      }

  template <typename T>
    template <typename I>
      auto vector<T>::insert(const_iterator pos, I first, I last, Requires<Forward_iterator<I>()>*)
        -> iterator
      {
        std::size_t n = distance(first, last);
        T* mid = const_cast<T*>(pos);
        if (size() + n >= capacity) {
          vector_base<T> tmp(base.next_capacity(), base.alloc);
          tmp.move_at_end(base.first, mid);
          tmp.copy_at_end(first, last);
          tmp.move_at_end(mid, base.last);
        } else {
          base.shift_right(pos, n);
          base.copy_at_pos(base.first + n, first, last);
        }
      }

  template <typename T>
    template <typename R>
      auto vector<T>::insert(const_iterator pos, const R& range, Requires<Range<R>()>*)
        -> iterator
      {
        return insert(pos, begin(range), end(range));
      }

  template <typename T>
    auto vector<T>::erase(const_iterator pos) -> iterator
    {
      assert(pos >= base.first && pos < base.last);
      std::size_t n = pos - base.first;
      base.shift_left(base.first + n, 1);
      return base.first + n;
    }

  template <typename T>
    auto vector<T>::erase(const_iterator first, const_iterator last) -> iterator
    {
      assert(first >= base.first && last <= base.last);
      std::size_t n = first - base.first;
      base.shift_left(base.first + n, last - first);
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
    void vector<T>::collapse()
    {
      if (capacity() != size()) {
        vector_base<T> tmp(size(), base.alloc);
        tmp.move_at_end(base);
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
        tmp.move_at_end(base);
        base.swap(tmp);
      }
    }

  // FIXME: Thiis can be made much, much simpler by writing it in terms of
  // insert and erase. See libstdc++.
  template <typename T>
    void vector<T>::resize(std::size_t n, const T& value)
    {
      if (n > size()) {
        if (n > capacity()) {
          vector_base<T> tmp(n, base.alloc);
          tmp.move_at_end(base);
          tmp.fill_at_end(n - size(), value);
          base.swap(tmp);
        } else {
          base.fill_at_end(n - size(), value);
        }
      } else if (n < size()) {
        base.erase_at_end(size() - n);
      }
    }


  // Erase all of the initialized elemnts, but do not deallocate any memory.
  template <typename T>
    void vector<T>::clear()
    {
      base.erase();
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
