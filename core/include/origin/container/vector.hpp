// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_CONTAINER_VECTOR_HPP
#define ORIGIN_CONTAINER_VECTOR_HPP

#include <origin/container/vector_base.hpp>

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
    public:
      using iterator       = T*;
      using const_iterator = const T*;

      // Default constructible
      vector() noexcept;
      vector(allocator& alloc) noexcept;

      // Move semantics
      vector(vector&& x) noexcept;
      vector(vector&& x, allocator& alloc) noexcept;
      vector& operator=(vector&& x) noexcept;

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
      template <typename R, typename = Requires<Input_range<R>()>>
        explicit vector(const R& range);
      template <typename R, typename = Requires<Input_range<R>()>>
        vector& operator=(const R& range);



      vector(std::initializer_list<T> l);
      vector(std::initializer_list<T> l, allocator& alloc);
      vector& operator=(std::initializer_list<T> l);

      // Destructible
      ~vector();


      // Observers
      allocator&       get_allocator()       { return base.alloc; }
      const allocator& get_allocator() const { return base.alloc; }

      bool empty() const { return base.empty(); }
      bool full() const { return base.full(); }
      std::size_t size() const { return base.size(); }
      std::size_t capacity() const { return base.capacity(); }
      std::size_t residual() const { return base.residual(); }


      // Data access
      T&       operator[](std::size_t n)       { return *(base.core.first + n); }
      const T& operator[](std::size_t n) const { return *(base.core.first + n); }

      T&       front()       { return *base.core.first; }
      const T& front() const { return *base.first; }

      T&       back()       { return *(base.core.last - 1); }
      const T& back() const { return *(base.core.last - 1); }


      // Capacity
      void deflate();
      void reserve(std::size_t n);
      void resize(std::size_t n, const T& value = {});


      // Modifiers

      // Push back/pop back
      template <typename... Args>
        void emplace_back(Args&&... args);

      void push_back(T&& value);
      void push_back(const T& value);
      void pop_back();

      // Insert
      template <typename... Args>
        iterator emplace(const_iterator pos, Args&&... args);

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

      // Erase
      iterator erase(const_iterator pos);
      iterator erase(const_iterator first, const_iterator last);

      void clear();
      vector& swap(vector& tmp);


      // Iterators
      iterator begin() { return base.core.first; }
      iterator end()   { return base.core.last; }

      const_iterator begin() const { return base.core.first; }
      const_iterator end() const   { return base.core.last; }

      const_iterator cbegin() const { return base.core.first; }
      const_iterator cend() const   { return base.core.last; }

    private:
      vector_base<T> base;
    };


  // Default construction
  template <typename T>
    inline
    vector<T>::vector() noexcept 
      : vector(default_allocator())
    { }
    

  template <typename T>
    inline
    vector<T>::vector(allocator& alloc) noexcept 
      : base(alloc) 
    { }

  // Moveable
  template <typename T>
    inline
    vector<T>::vector(vector&& x) noexcept
      : base(std::move(x.base)) 
    { }

  // Move the objects of x into this object, using alloc to allocate new memory
  // for this vector.
  //
  // Note: The capacity of this object is preserved unless 
  // x.get_allocator() != alloc. In that case, new memory must be allocated.
  template <typename T>
    inline
    vector<T>::vector(vector&& x, allocator& alloc) noexcept
      : base(std::move(x), alloc)
    { }

  template <typename T>  
    inline vector<T>&
    vector<T>::operator=(vector&& x) noexcept
    {
      return move_assign_sequence(*this, std::move(x));
    }

  // Copyable
  template <typename T>
    vector<T>::vector(const vector& x)
      : vector(x, default_allocator())
    { }

  template <typename T>
    vector<T>::vector(const vector& x, allocator& alloc)
      : base(x.size(), alloc)
    {
      base.core.last = 
        uninitialized_copy(base.alloc, x.begin(), x.end(), begin());
    }

  template <typename T>
    auto vector<T>::operator=(const vector& x) -> vector&
    {
      return copy_assign_sequence(*this, x);
    }

  // Fillable
  template <typename T>
    inline vector<T>::vector(std::size_t n, const T& value)
      : vector(n, value, default_allocator())
    { }

  template <typename T>
    inline vector<T>::vector(std::size_t n, const T& value, allocator& alloc)
      : base(n, alloc)
    {
      base.core.last = uninitialized_fill_n(base.alloc, begin(), n, value);
    }

  template <typename T>
    inline auto vector<T>::assign(std::size_t n, const T& value) -> vector&
    {
      return fill_assign_sequence(*this, n, value);
    }

  // Iterator range constructible
  template <typename T>
    template <typename I>
      vector<T>::vector(I first, I last, Requires<Strict_input_iterator<I>()>*)
        : vector(first, last, default_allocator())
      { }

  template <typename T>
    template <typename I>
      vector<T>::vector(I first, I last, allocator& alloc, Requires<Strict_input_iterator<I>()>*)
        : base(alloc)
      {
        while (first != last) {
          push_back(*first);
          ++first;
        }
      }

  template <typename T>
    template <typename I>
      vector<T>::vector(I first, I last, Requires<Forward_iterator<I>()>*)
        : vector(first, last, default_allocator())
      { }

  template <typename T>
    template <typename I>
      vector<T>::vector(I first, I last, allocator& alloc, Requires<Forward_iterator<I>()>*)
        : base(distance(first, last), alloc)
      {
        base.core.last = 
          uninitialized_copy(get_allocator(), first, last, begin());
      }

  template <typename T>
    template <typename I>
      inline vector<T>&
      vector<T>::assign(I first, I last)
      {
        return copy_assign_sequence(*this, first, last);
      }

  // Range constructible
  template <typename T>
    template <typename R, typename X>
      inline
      vector<T>::vector(const R& range)
        : vector(begin(range), end(range), default_allocator())
      { }

  template <typename T>
    template <typename R, typename X>
      inline vector<T>&
      vector<T>::operator=(const R& range)
      {
        return copy_assign_sequence(*this, range);
      }

  // Initializer list constructible
  template <typename T>
    inline
    vector<T>::vector(std::initializer_list<T> l)
      : vector(l.begin(), l.end(), default_allocator())
    { }

  template <typename T>
    inline
    vector<T>::vector(std::initializer_list<T> l, allocator& alloc)
      : vector(l.begin(), l.end(), alloc)
    { }

  template <typename T>
    inline vector<T>&
    vector<T>::operator=(std::initializer_list<T> list)
    {
      return assign(list.begin(), list.end());
    }


  // Destructible
  template <typename T>
    inline
    vector<T>::~vector()
    {
      clear();
    }

  template <typename T>
    template <typename... Args>
      void vector<T>::emplace_back(Args&&... args)
      {
        emplace(end(), std::forward<Args>(args)...);
      }

  template <typename T>
    void vector<T>::push_back(T&& value)
    {
      emplace_back(std::move(value));
    }

  template <typename T>
    void vector<T>::push_back(const T& value)
    {
      emplace_back(value);
    }

  template <typename T>
    void vector<T>::pop_back()
    {
      assert(!empty());
      vector_erase_at_end(base, 1);
    }


  template <typename T>
    template <typename... Args>
      auto vector<T>::emplace(const_iterator pos, Args&&... args) -> iterator
      {
        T *mid = const_cast<T*>(pos);
        if (!full()) {
          // If the vector isn't empty, then we can just append or insert an
          // object with the given arguments.
          return vector_insert(base, mid, std::forward<Args>(args)...);
        } else {
          // Allocate a new buffer.
          vector_core<T> buf = base.allocate_core();
          T* result = buf.limit;
          try {
            // Transfer the elements of the current buffer into the newly
            // allocated one.
            buf.last = uninitialized_move(base.alloc, base.core.first, mid, buf.first);
            
            // Construct the new object in place.
            construct(base.alloc, buf.last, std::forward<Args>(args)...);
            result = buf.last++;
            
            // Transfer the rest of the elements into the buffer.
            buf.last = uninitialized_move(base.alloc, mid, base.core.last, buf.last);
          } catch(...) {
            base.deallocate_core(buf);;
            throw;
          }
          base.take_core(buf);
          return result;
        }
      }

  template <typename T>
    auto vector<T>::insert(const_iterator pos, T&& value) -> iterator
    {
      return emplace(pos, std::move(value));
    }

  template <typename T>
    auto vector<T>::insert(const_iterator pos, const T& value) -> iterator
    {
      return emplace(pos, value);
    }

  // FIXME: Optimize be removing unnecessary swaps.
  template <typename T>
    auto vector<T>::insert(const_iterator pos, std::size_t n, const T& value) 
      -> iterator
    {
      std::size_t m = pos - base.core.first;
      T* mid = base.core.first + m;
      if (size() + n <= capacity()) {
        // There is sufficient capacity to insert without reallocating.
        // If we're inserting at the end, we can fill uninitialized. Otherwise,
        // we have to shuffle elements around and then fill in the middle
        // of the vector.
        if (pos == end()) {
          base.core.last = uninitialized_fill_n(base.alloc, end(), n, value);
        } else {
          vector_shift_right(base, mid, n);
          fill_n(mid, n, value);
        }
        return mid;
      } else {
        // Allocate the new buffer.
        //
        // FIXME: Should I be doubling the buffer until I exceed size() + n?
        // Or is it sufficient to allocate to the precise size?
        vector_core<T> buf = base.allocate_core(size() + n);

        // Transfer the elements into the new buffer.
        try {
          buf.last = uninitialized_move(base.alloc, base.core.first, mid, buf.first);
          buf.last = uninitialized_fill_n(base.alloc, buf.last, n, value);
          buf.last = uninitialized_move(base.alloc, mid, base.core.last, buf.last);
        } catch(...) {
          base.deallocate_core(buf);
          throw;
        }
        base.take_core(buf);
        return base.core.first + m;
      }
    }

  // FIXME: The result of this operation is almost certainly incorrect.
  template <typename T>
    template <typename I>
      auto vector<T>::insert(const_iterator pos, I first, I last, Requires<Strict_input_iterator<I>()>*)
        -> iterator
      {
        T* p = const_cast<T*>(pos);
        while (first != last) {
          p = insert(p, *first);
          ++first;
        }
        return p;
      }

  template <typename T>
    template <typename I>
      auto vector<T>::insert(const_iterator pos, I first, I last, Requires<Forward_iterator<I>()>*)
        -> iterator
      {
        T* p = const_cast<T*>(pos);
        std::size_t n = distance(first, last);
        if (residual() >= n) {
          return vector_range_insert(base, p, first, last);
        } else {
          // Allocate a new buffer of an appropriate size.
          vector_core<T> buf = base.allocate_core();

          // Transfer the elements of the old base and those in [first, last)
          // into the new buffer. Note that we move from the old base, but
          // copy from [first, last).
          try {
            buf.last = uninitialized_move(base.alloc, base.core.first, p, buf.first);
            buf.last = uninitialized_copy(base.alloc, first, last, buf.last);
            buf.last = uninitialized_move(base.alloc, p, base.core.last, buf.last);
          } catch(...) {
            base.deallocate_core(buf);
            throw;
          }
          base.take_core(buf);
          return base.core.first + n;
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
    auto vector<T>::insert(const_iterator pos, std::initializer_list<T> list)
      -> iterator
    {
      return insert(pos, list.begin(), list.end());
    }

  template <typename T>
    auto vector<T>::erase(const_iterator pos) -> iterator
    {
      assert(pos >= base.core.first && pos < base.core.last);
      T* p = const_cast<T*>(pos);
      vector_shift_left(base, p);
      return p;
    }

  template <typename T>
    auto vector<T>::erase(const_iterator first, const_iterator last) -> iterator
    {
      assert(first >= base.core.first && last <= base.core.last);
      T* f = const_cast<T*>(first);
      T* l = const_cast<T*>(last);
      vector_shift_left(base, f, l);
      return f;
    }


  // Reserve only enough capacity required to store the initialized values
  // of the vector.
  //
  // Note that the standard claims that the corresponding shrink_to_fit method
  // is a non-binding request [23.3.6.3/9], meaning that we don't have to
  // implement this, or we can reserve more memory than just the size. However,
  // the implementation is easy enough.
  template <typename T>
    void vector<T>::deflate()
    {
      if (capacity() != size())
        relocate_vector(base, size());
    }

  // Reserve n objects of memory. Note that if n is less than the current 
  // capacity(), the operation has no effect.
  template <typename T>
    void vector<T>::reserve(std::size_t n)
    {
      if (capacity() < n)
        relocate_vector(base, n);
    }

  // FIXME: This can be made much, much simpler by writing it in terms of
  // insert and erase. See libstdc++.
  template <typename T>
    void vector<T>::resize(std::size_t n, const T& value)
    {
      if (n > size())
        insert(end(), n, value);
      else if (n < size());
        erase(begin() + n, end());
    }


  // Erase all of the initialized elemnts, but do not deallocate any memory.
  template <typename T>
    void vector<T>::clear()
    {
      base.clear();
    }


  // Two vectors may only be swapped when they have the same allocator (i.e.,
  // the allocators must compare equal).
  template <typename T>
    auto vector<T>::swap(vector& x) -> vector&
    {
      base.swap(x.base);
      return *this;
    }



  // Equality_comparable<vector<T>, vector<U>>
  template <typename T, typename U>
    bool operator==(const vector<T>& a, const vector<U>& b)
    {
      static_assert(Equality_comparable<T, U>(), "");
      return a.size() == b.size() 
          && lexicographical_equal(a.begin(), a.end(), b.begin(), b.end());
    }

  template <typename T, typename U>
    bool operator!=(const vector<T>& a, const vector<U>& b)
    {
      static_assert(Equality_comparable<T, U>(), "");
      return !(a == b);
    }

  // Equality comparable<vector<T>, initializer_list<U>>
  template <typename T, typename U>
    bool operator==(const vector<T>& a, std::initializer_list<U> b)
    {
      static_assert(Equality_comparable<T, U>(), "");
      return a.size() == b.size()
          && lexicographical_equal(a.begin(), a.end(), b.begin());
    }

  template <typename T, typename U>
    bool operator==(std::initializer_list<U> a, const vector<T>& b)
    {
      static_assert(Equality_comparable<T, U>(), "");
      return a.size() == b.size()
          && lexicographical_equal(a.begin(), a.end(), b.begin());
    }

  template <typename T, typename U>
    bool operator!=(const vector<T>& a, std::initializer_list<U> b)
    {
      static_assert(Equality_comparable<T, U>(), "");
      return !(a == b);
    }

  template <typename T, typename U>
    bool operator!=(std::initializer_list<U> a, const vector<T>& b)
    {
      static_assert(Equality_comparable<T, U>(), "");
      return !(a == b);
    }


  // Totally_ordered<vector<T>, vector<U>>
  template <typename T, typename U>
    bool operator<(const vector<T>& a, const vector<U>& b)
    {
      static_assert(Totally_ordered<T, U>(), "");
      return lexicographical_less(a.begin(), a.end(), b.begin(), b.end());
    }

  template <typename T, typename U>
    bool operator>(const vector<T>& a, const vector<U>& b)
    {
      static_assert(Totally_ordered<T, U>(), "");
      return b < a;
    }

  template <typename T, typename U>
    bool operator<=(const vector<T>& a, const vector<U>& b)
    {
      static_assert(Totally_ordered<T, U>(), "");
      return !(b < a);
    }

  template <typename T, typename U>
    bool operator>=(const vector<T>& a, const vector<U>& b)
    {
      static_assert(Totally_ordered<T, U>(), "");
      return (!a < b);
    }

} // namespace origin

#endif
