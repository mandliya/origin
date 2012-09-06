// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_CONTAINER_LIST_BASE_HPP
#define ORIGIN_CONTAINER_LIST_BASE_HPP

#include <cassert>

#include <origin/memory.hpp>

namespace origin
{
  // List node base
  //
  // The list node base provides basic linking structure for doubly linked
  // list. It includes a pair of pointers called next and prev. 
  //
  // A list node has two states: detached and attached. A detached node links
  // to no other nodes (in either direction). That is this->next == this and
  // this->prev == this. The attached state is the opposite of detached.
  // Neither this->next nor this->prev point to this.
  //
  // Invariants:
  //    this->next == this <=> this->prev == this
  struct list_node_base
  {
    list_node_base() 
      : next{this}, prev{this} 
    { }

    list_node_base(list_node_base* n, list_node_base* p)
      : next{n}, prev{p}
    {
      assert(!detached());
    }

    // Returns true when the node is detached, it links to no other nodes.
    bool detached() const { return next == this; }

    bool attached() const { return next != this; }

    list_node_base* next;
    list_node_base* prev;
  };


  // Reset node
  //
  // Reset the linked list node to its default, such that p->next == p and
  // p->prev == p.
  inline void
  reset_node(list_node_base* p)
  {
    p->next = p->prev = p;
  }

  // Relink node
  //
  // Modify the links of the noes before and after p so they point to p. This
  // is used in swap node to ensure that link structures are preserved after
  // swapping.
  inline void
  relink_node(list_node_base* p)
  {
    p->next->prev = p->prev->next = p;
  }

  // Move node
  //
  // Take links of q and transfer them to the detached node p. The node q is
  // reset after the links are taken. This is used in  swap nodes to transfer
  // link state when it is known that one node is detached. This is also used to
  // implement move semantics.
  inline void
  move_node(list_node_base* p, list_node_base* q)
  {
    assert(p->detached());
      p->next = q->next;
      p->prev = q->prev;
      relink_node(p);
      reset_node(q);
  }

  // Swap node
  //
  // Exchange the pointers in the list nodes p and q. The nodes before p and
  // q are relinked so the linked lists are preserved.
  //
  // Note that if neither p nor q are attached, then swapping the two nodes i
  // is idempotent (no action is taken).
  inline void
  swap_nodes(list_node_base* p, list_node_base* q)
  {
    if (p->attached()) {
      if (q->attached()) {
        // Both p and q are attached so we have to swap the pointers and
        // then re-link the nodes before and after q so they point to the
        // right elements.
        std::swap(p->next, q->next);
        std::swap(p->prev, q->prev);
        relink_node(p);
        relink_node(q);
      } else {
        // The node q is detached so we can move p into it.
        move_node(q, p);
      }
    } else if (q->attached()) {
      // the node p is detached so we can move q into it.
      move_node(p, q);
    }
  }


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

      list_base();
      list_base(allocator& a);
      list_base(list_base&& x);

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

      void swap(list_base& x);
      void reset();

    private:
      // A helo-per functions that returns p as a non-const node.
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
    inline 
    list_base<T>::list_base()
      : alloc(default_allocator()), base{}, count{0}
    { }

  template <typename T>
    inline
    list_base<T>::list_base(allocator& a)
      : alloc(a), base{}, count{0}
    { }

  // Move x into this object by copying x's node base and count. The object
  // x is reset to a default state.
  template <typename T>
    inline 
    list_base<T>::list_base(list_base&& x)
      : alloc(default_allocator()), base{}, count{x.count}
    {
      move_node(&base, &x.base);
      x.count = 0;
    }

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
    inline auto 
    list_base<T>::allocate() -> node*
    {
      node* p = origin::allocate<node>(alloc);
      ++count;
      return p;
    }

  template <typename T>
    inline void 
    list_base<T>::deallocate(node* p)
    {
      origin::deallocate(alloc, p);
      --count;
    }

  // Exchange this implementation with the other. This operation is only
  // defined when the allocators are the same.
  template <typename T>
    inline void
    list_base<T>::swap(list_base& x)
    {
      assert(alloc == x.alloc);
      swap_nodes(&base, &x.base);
      std::swap(count, x.count);
    }

  // Reset the list base to a default state.
  template <typename T>
    inline void
    list_base<T>::reset()
    {
      reset_node(&base);
      count = 0;
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
      assert(p->attached());
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

} // namespace origin


#endif

