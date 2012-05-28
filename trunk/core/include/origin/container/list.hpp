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

    void hook(list_node_base* p) noexcept;
    void unhook() noexcept;

    list_node_base* next;
    list_node_base* prev;
  };

  // TODO: Would it be better to make these free algorithms like libc++ does?
  // It might be nice to support methods like link_node, which link a range
  // of nodes before a given position.

  // Attach this node to the linked list just before p.
  //
  // FIXME: Move this into the src dir.
  inline void list_node_base::hook(list_node_base* p) noexcept
  {
    next = p;
    prev = p->prev;
    p->prev->next = this;
    p->prev = this;
  }

  // Detach this node from the linked list by making the prev point to next
  // and next point to prev.
  //
  // FIXME: Move this into the src dir.
  inline void list_node_base::unhook() noexcept
  {
    list_node_base* p = prev;
    list_node_base* q = next;
    p->next = q;
    q->prev = p;
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



  // List iterator
  //
  // The list iterator class adapts a linked list node pointer into a 
  // bidirectional iterator.
  //
  // TODO: Implement const conversion requirements. A non-const iterator must
  // be convertible to a const iterator.
  template <typename T>
    class list_iterator
    {
    public:
      list_iterator() 
        : ptr{nullptr} 
      { }
      
      list_iterator(list_node_base* p) 
        : ptr{p} 
      { }

      // Returns a pointer to the underlying node pointer.
      list_node_base*       node()       { return ptr; }
      const list_node_base* node() const { return ptr; }

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
      : base()
    { }

  template <typename T>
    list<T>::list(allocator& alloc)
      : base(alloc)
    { }


  // TODO: This can probably be used for a single insert function.
  template <typename T>
    template <typename... Args>
    void list<T>::emplace_back(Args&&... args)
    {
      // Allocate and initialize the new new.
      node* p = base.allocate();
      try {
        construct(base.alloc, &p->value, std::forward<Args>(args)...);
      } catch(...) {
        base.deallocate(p);
      }

      // Link it at the back of the list.
      p->hook(base.anchor());
    }

  template <typename T>
    void list<T>::push_back(T&& value)
    {
      emplace_back(std::move(value));
    }

  template <typename T>
    void list<T>::push_back(const T& value)
    {
      emplace_back(value);
    }

} // namespace origin


#endif

