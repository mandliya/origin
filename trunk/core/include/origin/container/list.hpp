// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_CONTAINER_LIST_HPP
#define ORIGIN_CONTAINER_LIST_HPP

#include <origin/container/list_base.hpp>

namespace origin
{
  // List iterator
  //
  // The list iterator class adapts a linked list node pointer into a 
  // bidirectional iterator.
  //
  // Note that list_iteator<const T> will generate constant iterator, which
  // will provide a constant reference to the underlying object.
  template <typename T>
    class list_iterator
    {
    public:
      list_iterator(list_node_base* p = {}) 
        : ptr{p} 
      { }

      // Conversion to const
      template <typename U, typename = Requires<Convertible<U, T>()>>
        list_iterator(const list_iterator<U>& x)
          : ptr{x.node()}
        { }

      template <typename U, typename = Requires<Convertible<U, T>()>>
        list_iterator& operator=(const list_iterator<U>& x)
        {
          ptr = x.node();
          return *this;
        }

      // Returns a pointer to the underlying node base pointer.
      list_node_base* node()       { return ptr; }
      list_node_base* node() const { return non_const(ptr); }

      // Read/write
      T& operator*() const;

      // Increment
      list_iterator& operator++();
      list_iterator operator++(int);

      // Decrement
      list_iterator& operator--();
      list_iterator operator--(int);

    private:
      // Returns true if ptr is "past the end".
      bool past_the_end() const { return ptr->next == ptr; }

      // Return p as a non-const node.
      static list_node_base* non_const(const list_node_base* p)
      {
        return const_cast<list_node_base*>(p);
      }

    private:
      list_node_base* ptr;
    };

  template <typename T>
    inline T& list_iterator<T>::operator*() const
    {
      assert(ptr && !past_the_end());
      return list_node_cast<T>(ptr)->value;
    }

  template <typename T>
    inline list_iterator<T>& list_iterator<T>::operator++()
    {
      assert(ptr && !past_the_end());
      ptr = ptr->next;
      return *this;
    }

  template <typename T>
    inline list_iterator<T> list_iterator<T>::operator++(int)
    {
      list_iterator tmp;
      operator++();
      return tmp;
    }

  template <typename T>
    inline list_iterator<T>& list_iterator<T>::operator--()
    {
      assert(ptr && !past_the_end());
      ptr = ptr->prev;
      return *this;
    }

  template <typename T>
    inline list_iterator<T> list_iterator<T>::operator--(int)
    {
      list_iterator tmp;
      operator--();
      return tmp;
    }

  template <typename T>
    bool operator==(const list_iterator<T>& a, const list_iterator<T>& b)
    {
      return a.node() == b.node();
    }

  template <typename T>
    bool operator!=(const list_iterator<T>& a, const list_iterator<T>& b)
    {
      return a.node() != b.node();
    }



  // List
  //
  // The linked list class...
  //
  // FIXME: Document this class.
  template <typename T>
    class list
    {
      using node = list_node<T>;
    public:
      using iterator = list_iterator<T>;
      using const_iterator = list_iterator<const T>;

      // Default constructible
      list();
      list(allocator& alloc);

      // Movable
      list(list&& x);
      list(list&& x, allocator& alloc);
      list& operator=(list&& x);

      // Copyable
      list(const list& x);
      list(const list& x, allocator& alloc);
      list& operator=(const list& x);

      // Iterator range constructible
      template <typename I>
        list(I first, I last);
      template <typename I>
        list(I first, I last, allocator& alloc);

      // Range constructible
      template <typename R, typename = Requires<Range<R>()>>
        explicit list(const R& range);
      template <typename R, typename = Requires<Range<R>()>>
        explicit list(const R& range, allocator& alloc);
      
      // Initializer list constructible
      list(std::initializer_list<T> l);
      list(std::initializer_list<T> l, allocator& alloc);
      list& operator=(std::initializer_list<T> l);

      // Fill constructible
      list(std::size_t n, const T& value = {});
      list(std::size_t n, const T& value, allocator& alloc);

      ~list();

      // Properties

      // Returns true if the list has no elements.
      bool empty() const { return base.empty(); }

      // Returns the number of elements in the list.
      std::size_t size() const { return base.size(); }

      // Returns the list's allocators.
      allocator& get_allocator() const { return base.alloc; }


      // Element access

      // Returns the first element in the list.
      T&       front()       { return base.front(); }
      const T& front() const { return base.front(); }

      // Returns the last element in the list.
      T&       back()       { return base.back(); }
      const T& back() const { return base.back(); }


      // Mutators
      void resize(std::size_t n, const T& value = {});

      // Push front
      template <typename... Args>
        void emplace_front(Args&&... args);
      void push_front(T&& x);
      void push_front(const T& x);

      // Push back
      template <typename... Args>
        void emplace_back(Args&&... args);
      void push_back(T&& value);
      void push_back(const T& value);

      // Pop front and back
      void pop_front();
      void pop_back();

      // Insert
      template <typename... Args>
        iterator emplace(const_iterator pos, Args&&... args);
      iterator insert(const_iterator pos, T&& value);
      iterator insert(const_iterator pos, const T& value);

      // Erase
      iterator erase(const_iterator pos);
      iterator erase(const_iterator first, const_iterator last);

      void swap(list& x);
      void clear();

      // Iterators
      iterator       begin()       { return base.head(); }
      const_iterator begin() const { return base.head(); }
      
      iterator       end()       { return base.anchor(); }
      const_iterator end() const { return base.anchor(); }

    private:
      list_base<T> base;
    };


  template <typename T>
    inline 
    list<T>::list()
      : list(default_allocator())
    { }

  template <typename T>
    inline
    list<T>::list(allocator& alloc)
      : base(alloc)
    { }

  template <typename T>
    inline 
    list<T>::list(list&& x)
      : base(std::move(x.base))
    { }

  // Move construction with a separate allocator requires us to allocate new
  // nodes, but we can move the values form x into the new nodes.
  //
  // FIXME: If T is also an Allocated type, the we need to construct node values
  // with alloc (i.e., push_back(t, alloc)). What are the considerations when
  // x's allocator is different than alloc? I suspect that we have to copy in
  // those cases.
  template <typename T>
    inline
    list<T>::list(list&& x, allocator& alloc)
      : list(alloc)
    {
      for (iterator i = x.begin(); i != x.end(); ++i)
        push_back(std::move_if_noexcept(*i));
    }

  template <typename T>
    inline list<T>&
    list<T>::operator=(list&& x)
    {
      list tmp {std::move(x)};
      swap(tmp);
      return *this;
    }

  template <typename T>
    inline
    list<T>::list(const list& x)
      : list(x.begin(), x.end())
    { }

  template <typename T>
    inline
    list<T>::list(const list& x, allocator& alloc)
      : list(x.begin(), x.end(), alloc)
    { }

  // FIXME: This can be optimized by copying the first [begin, end) values from
  // x without reallocating.
  template <typename T>
    inline list<T>&
    list<T>::operator=(const list& x)
    {
      /*
      if (x.size() <= size()) {
        iterator i = copy(x.begin(), x.end(), begin());
        erase(i, end());
      } else {
        resize(x.size());
        copy(x.begin(), x.end(), begin());
      }
      */
      list tmp{x};
      swap(tmp);
      return *this;
    }


  template <typename T>
    template <typename I>
      inline
      list<T>::list(I first, I last)
        : list(first, last, default_allocator())
      { }

  // TODO: It would be more efficient to allocate a number of nodes in
  // one shot and then initialize and link them in a second pass.
  template <typename T>
    template <typename I>
      inline
      list<T>::list(I first, I last, allocator& alloc)
        : base(alloc)
      {
        while (first != last) {
          push_back(*first);
          ++first;
        }
      }

  template <typename T>
    inline
    list<T>::list(std::initializer_list<T> l)
      : list(l, default_allocator())
    { }

  template <typename T>
    inline
    list<T>::list(std::initializer_list<T> l, allocator& alloc)
      : list(l.begin(), l.end(), alloc)
    { }

  // FIXME: Optimize this guy.
  template <typename T>
    inline list<T>&
    list<T>::operator=(std::initializer_list<T> l)
    {
      list tmp {l};
      swap(tmp);
      return *this;
    }

  template <typename T>
    inline
    list<T>::list(std::size_t n, const T& value)
      : list(n, value, default_allocator())
    { }

  // FIXME: This could be optimized by allocating all nodes at the same time,
  // and initializing and linking them at the same time.
  template <typename T>
    inline
    list<T>::list(std::size_t n, const T& value, allocator& alloc)
      : list(alloc)
    {
      while(n != 0) {
        push_back(value);
        --n;
      }
    }

  template <typename T>
    inline
    list<T>::~list()
    {
      clear();
    }

  // Resize the list so that it contains exactly n elements. If n is greater
  // than this->size(), then we add n - this->size() nodes initialized to
  // value. If n < this->size(), we erase the trailing size() - n nodes.
  template <typename T>
    inline void
    list<T>::resize(std::size_t n, const T& value)
    {
      if (n > size()) {
        while (size() != n)
          push_back(value);
      } else if (n < size()) {
        // TODO: If size() - n > size() / 2, then it would be more efficient
        // to count backwards from end.
        erase(next(begin(), n), end());
      }
    }


  template <typename T>
    template <typename... Args>
    inline void 
    list<T>::emplace_back(Args&&... args)
    {
      insert(end(), std::forward<Args>(args)...);
    }

  template <typename T>
    inline void 
    list<T>::push_back(T&& value)
    {
      emplace_back(std::move(value));
    }

  template <typename T>
    inline void 
    list<T>::push_back(const T& value)
    {
      emplace_back(value);
    }

  template <typename T>
    template <typename... Args>
      inline void
      list<T>::emplace_front(Args&&... args)
      {
        emplace(begin(), std::forward<Args>(args)...);
      }

  template <typename T>
    inline void
    list<T>::push_front(T&& value)
    {
      emplace(begin(), std::move(value));
    }

  template <typename T>
    inline void
    list<T>::push_front(const T& value)
    {
      emplace(begin(), value);
    }

  template <typename T>
    inline void
    list<T>::pop_front()
    {
      erase(--end());
    }

  template <typename T>
    inline void
    list<T>::pop_back()
    {
      erase(begin());
    }

  template <typename T>
    template <typename... Args>
      auto 
      list<T>::emplace(const_iterator pos, Args&&... args) -> iterator
      {
        node* p = create_list_node(base, std::forward<Args>(args)...);
        return link_node(pos.node(), p);
      }

  template <typename T>
    inline auto 
    list<T>::insert(const_iterator pos, T&& value) -> iterator
    {
      return emplace(pos, std::move(value));
    }

  template <typename T>
    inline auto 
    list<T>::insert(const_iterator pos, const T& value) -> iterator
    {
      return emplace(pos, value);
    }

  template <typename T>
    inline auto
    list<T>::erase(const_iterator pos) -> iterator
    {
      list_node_base* p = pos.node();
      list_node_base* q = unlink_node(p);
      destroy_list_node(base, p);
      return q;
    }

  template <typename T>
    inline auto
    list<T>::erase(const_iterator first, const_iterator last) -> iterator
    {
      list_node_base* f = first.node();
      list_node_base* l = last.node()->prev;
      list_node_base* p = unlink_nodes(f, l);
      destroy_list_nodes(base, f, l);
      return p;
    }

  template <typename T>
    inline void
    list<T>::swap(list& x)
    {
      base.swap(x.base);
    }

  template <typename T>
    inline void
    list<T>::clear()
    {
      if (!empty())
        erase(begin(), end());
    }


  // Equality comparable
  //
  // Two lists a and compare equal when they have the same elements. That is,
  // a == b if and only if lexicographical_equal(a, b) returns true.
  template <typename T>
    inline bool
    operator==(const list<T>& a, const list<T>& b)
    {
      return lexicographical_equal(a, b);
    }

  template <typename T>
    inline bool
    operator!=(const list<T>& a, const list<T>& b)
    {
      return !(a == b);
    }

  // Totally ordered
  //
  // A list a compares less than a list b when a's elements are 
  // lexicographically less than b's.
  template <typename T>
    inline bool
    operator<(const list<T>& a, const list<T>& b)
    {
      return lexicographical_less(a, b);
    }

  template <typename T>
    inline bool
    operator>(const list<T>& a, const list<T>& b)
    {
      return b < a;
    }

  template <typename T>
    inline bool
    operator<=(const list<T>& a, const list<T>& b)
    {
      return !(b < a);
    }

  template <typename T>
    inline bool
    operator>=(const list<T>& a, const list<T>& b)
    {
      return !(a < b);
    }

} // namespace origin


#endif

