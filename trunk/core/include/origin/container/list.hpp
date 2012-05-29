// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_CONTAINER_LIST_HPP
#define ORIGIN_CONTAINER_LIST_HPP

#include <list>

#include <origin/memory.hpp>

namespace origin
{
  // List node base
  //
  // The list node base provides basic linking structure for doubly linked
  // list. It includes a pair of pointers called next and prev. 
  struct list_node_base
  {
    list_node_base() 
      : next{this}, prev{this} 
    { }

    list_node_base(list_node_base* n, list_node_base* p)
      : next{n}, prev{p}
    { }

    list_node_base* next;
    list_node_base* prev;
  };

  // Link nodes
  //
  // Attach the linked range of nodes [f, l] into the linked list before the
  // node p. Returns a pointer to the first inserted node, f.
  inline list_node_base*
  link_nodes(list_node_base* p, list_node_base* f, list_node_base* l)
  {
    l->next = p;
    f->prev = p->prev;
    p->prev->next = f;
    p->prev = l;
    return f;
  }

  // Link node
  //
  // Attach the node q into the linked list before the node p. Returns a pointer
  // to the first inserted node, q.
  inline list_node_base*
  link_node(list_node_base* p, list_node_base* q)
  {
    return link_nodes(p, q, q);
  }

  // Unlink nodes
  //
  // Remove the linked nodes [f, l] from the list by linking the nodes before
  // and after f and l. Returns the node after l.
  inline list_node_base*
  unlink_nodes(list_node_base* f, list_node_base* l)
  {
    f->prev->next = l->next;
    l->next->prev = f->prev;
    return l->next;
  }

  // Unlink node
  //
  // Unlink the node p from a list by linking the nodes before and after p.
  // Returns the node after p.
  inline list_node_base*
  unlink_node(list_node_base* p)
  {
    return unlink_nodes(p, p);
  }



  // List node
  //
  // The list node class is a node in a linked list that stores a user-specified
  // data.
  template <typename T>
    struct list_node : list_node_base
    {
      // Forwarding constructors
      template <typename... Args>
        list_node(Args&&... args)
          : value{std::forward<Args>(args)...}
        { }

      T value;
    };



  // List node cast
  //
  // Return p cast as a typed list node.
  
  template <typename T>
    inline list_node<T>* list_node_cast(list_node_base* p)
    {
      return static_cast<list_node<T>*>(p);
    }

  template <typename T>
    inline const list_node<T>* list_node_cast(const list_node_base* p)
    {
      return static_cast<const list_node<T>*>(p);
    }

  // List base
  //
  // The list base class encapsulates the underlying storage and allocation
  // facilities for a linked list class. 
  //
  // The list base stores a single node base object, called its anchor, to which
  // allocated list nodes are attached. The anchor also serves as a list
  // specific "end" iterator. This class also stores a count, which is used to
  // record the number of nodes.
  template <typename T>
    class list_base
    {
    public:
      using node = list_node<T>;

      list_base()
        : alloc(default_allocator()), base{}, count{0}
      { }

      list_base(allocator& a)
        : alloc(a), base{}, count{0}
      { }


      // Observers
      
      // Returns true if there are no nodes linked to the list.
      //
      // Note: When count == 0, it must be the case that tail() == &base.
      bool empty() const { return count == 0; }

      // Returns the number of nodes linked to the list.
      std::size_t size() const { return count; }

      // Returns the anchor node.
      list_node_base* anchor()       { return &base; }
      list_node_base* anchor() const { return non_const(&base); }

      // Returns the first node attached to the list. Note that the return is
      // never a const pointer.
      list_node_base* head()       { return base.next; }
      list_node_base* head() const { return non_const(base.next); }

      // Returns the last node attached to the list. Note that the return is
      // never a const pointer.
      list_node_base* tail()       { return base.prev; }
      list_node_base* tail() const { return non_const(base.prev); }

      // Returns the object at the front of the list.
      T&       front();
      const T& front() const;

      // Returns the object at the back of the list.
      T&       back();
      const T& back() const;


      // Allocate and deallocate
      node* allocate();
      void deallocate(node* p);

    private:
      // Return p as a non-const node.
      static list_node_base* non_const(const list_node_base* p)
      {
        return const_cast<list_node_base*>(p);
      }

    public:
      allocator& alloc;
      list_node_base base;  // The anchor node
      std::size_t count;    // Number of linked nodes
    };

  template <typename T>
    inline T& list_base<T>::front()
    { 
      assert(!empty()); 
      return list_node_cast<T>(head())->value; 
    }

  template <typename T>
    inline const T& list_base<T>::front() const 
    { 
      assert(!empty()); 
      return list_node_cast<T>(head())->value; 
    }

  template <typename T>
    inline T& list_base<T>::back() 
    { 
      assert(!empty()); 
      return list_node_cast<T>(tail())->value; 
    }

  template <typename T>
    inline const T& list_base<T>::back() const 
    { 
      assert(!empty()); 
      return list_node_cast<T>(tail())->value; 
    }

  template <typename T>
    inline auto list_base<T>::allocate() -> node*
    {
      node* p = origin::allocate<node>(alloc);
      ++count;
      return p;
    }

  template <typename T>
    inline void list_base<T>::deallocate(node* p)
    {
      origin::deallocate(alloc, p);
      --count;
    }


  // Create node
  //
  // Allocate and initialize a list with the given arguments.
  template <typename T, typename... Args>
    inline list_node<T>* 
    create_list_node(list_base<T>& base, Args&&... args)
    {
      list_node<T>* p = base.allocate();
      try {
        construct(base.alloc, &p->value, std::forward<Args>(args)...);
      } catch(...) {
        base.deallocate(p);
        throw;
      }
      return p;
    }

  // Destroy node
  //
  // Destroy the value stored by p and deallocate its memory and return a
  // pointer to the node after p. 
  template <typename T>
    inline void 
    destroy_list_node(list_base<T>& base, list_node_base* p)
    {
      assert(p->next != p);
      list_node<T>* n = static_cast<list_node<T>*>(p);
      origin::destroy(base.alloc, &n->value);
      base.deallocate(n);
    }

  // Destroy list nodes
  //
  // Destroy each of the linked list nodes in the range [first, last) and 
  // deallocate their memory.
  template <typename T>
    inline void 
    destroy_list_nodes(list_base<T>& base, list_node_base* first, list_node_base* last)
    {
      while (first != last) {
        list_node_base* p = first;
        first = first->next;
        destroy_list_node(base, p);
      }
      destroy_list_node(base, last);
    }

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
      template <typename U, typename = Requires<Convertible<U, T>()>*>
        list_iterator(const list_iterator<U>& x)
          : ptr{x.node()}
        { }

      template <typename U, typename = Requires<Convertible<U, T>()>*>
        list_iterator& operator=(const list_iterator<U>& x)
        {
          ptr = x.node();
          return *this;
        }

      // Returns a pointer to the underlying node base pointer.
      list_node_base* node()       { return ptr; }
      list_node_base* node() const { return non_const(ptr); }

      // Read/wrte
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
      ptr = ptr->next;
      return *this;
    }

  template <typename T>
    inline list_iterator<T> list_iterator<T>::operator--(int)
    {
      list_iterator tmp;
      operator++();
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
  template <typename T>
    class list
    {
      using node = list_node<T>;
    public:
      using iterator = list_iterator<T>;
      using const_iterator = list_iterator<const T>;

      list();
      list(allocator& alloc);

      template <typename I>
        list(I first, I last);
      template <typename I>
        list(I first, I last, allocator& alloc);

      // FIXME: Write a generalized range constructor
      
      list(std::initializer_list<T> l);
      list(std::initializer_list<T> l, allocator& alloc);

      // Observers
      bool empty() const { return base.empty(); }

      std::size_t size() const { return base.size(); }

      T&       front()       { return base.front(); }
      const T& front() const { return base.front(); }

      T&       back()       { return base.back(); }
      const T& back() const { return base.back(); }


      // Mutators

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

      // Iterators
      iterator       begin()       { return base.head(); }
      const_iterator begin() const { return base.head(); }
      
      iterator       end()       { return base.anchor(); }
      const_iterator end() const { return base.anchor(); }

    private:
      list_base<T> base;
    };


  template <typename T>
    list<T>::list()
      : list(default_allocator())
    { }

  template <typename T>
    list<T>::list(allocator& alloc)
      : base(alloc)
    { }


  template <typename T>
    template <typename I>
      list<T>::list(I first, I last)
        : base(first, last, default_allocator())
      { }

  // TODO: It would be more efficient to allocate a number of nodes in
  // one shot and then initialize and link them in a second pass.
  template <typename T>
    template <typename I>
      list<T>::list(I first, I last, allocator& alloc)
        : base(alloc)
      {
        while (first != last) {
          push_back(*first);
          ++first;
        }
      }

  template <typename T>
    list<T>::list(std::initializer_list<T> l)
      : list(l, default_allocator())
    { }

  template <typename T>
    list<T>::list(std::initializer_list<T> l, allocator& alloc)
      : list(l.begin(), l.end(), alloc)
    { }


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

} // namespace origin


#endif

