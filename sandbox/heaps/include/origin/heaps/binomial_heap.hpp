// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_HEAPS_BINOMIAL_HEAP_HPP
#define ORIGIN_HEAPS_BINOMIAL_HEAP_HPP

#include <cassert>
#include <vector>
#include <functional>

namespace origin
{

  // FIXME: This is not a typical binomial heap. It is not mergeable, so it's
  // name needs to be changed. I'm thinking flat_binomial_heap or something
  // like that.
  
  // FIXME: Is it possible to condense the non-mutable binomial heap operations 
  // into a single data structure?

  /**
   * @internal
   *
   * The binomial heap node defines the non-dependent link structure of a
   * binomial tree stored in a Random Access Container. Each node contains
   * a link to its parent node, it's child node, and its right subtree. The
   * degree of a node determines the number of nodes in the subtree. The size
   * of each subtree is 2^degree nodes.
   *
   * The heap node does not store its data object, either here, or in a derived
   * class. Instead, the heap node refers to the data element in an external
   * set of objects.
   */
  struct binomial_heap_node
  {
    public:
      typedef std::size_t size_type;

      binomial_heap_node()
        : index{-1}, parent{-1}, child{-1}, right{-1}, degree{0} 
      { }
      
      explicit binomial_heap_node(size_type ix)
        : index{ix}, parent{-1}, child{-1}, right{-1}, degree{0}
      { }
      
      // Unlink the node by resetting all of its indexes
      void reset()
      {
        index = parent = child = right = -1;
        degree = 0;
      }
      
      size_type index;    // Refers to the stored data object
      size_type parent;   // Index of the parent node
      size_type child;    // Index of the child node
      size_type right;    // Index of the right subtree
      size_type degree;   // Total degree of the subtree
  };

  /** 
   * @internal
   * Link the two nodes.
   */
  void binomial_link(binomial_heap_node& node1, 
                     binomial_heap_node& node2, 
                     size_t index1, 
                     size_t index2)
  {
    node1.parent = index2;
    node1.right = node2.child;
    node2.child = index1;
    node2.degree = node2.degree + 1;
  }


  /**
   * A binomial heap...
   *
   * @tparam T
   * @tparam Comp
   * @tparam Alloc
   */
  template<typename T,
           typename Comp = std::less<T>,
           typename Alloc = std::allocator<T>>
    class binomial_heap
    {
    public:
      typedef Alloc allocator_type;
      typedef typename allocator_type::size_type size_type;

      typedef T const value_type;
      typedef typename allocator_type::reference reference;
      typedef typename allocator_type::const_reference const_reference;
      
      typedef Comp value_compare;
    private:
      typedef std::vector<T, Alloc> element_vector;
      typedef std::vector<binomial_heap_node> node_vector;
      typedef std::vector<size_type> index_vector;
    public:

      /** @name Initialization */
      //@{
      /**
       * @brief Default constructor
       */
      binomial_heap(value_compare const& comp = value_compare{})
        : compare_{comp}, top_{-1}, head_{-1} 
      { }
      
      /**
       * @brief Range constructor
       */
      template<typename ForwardIterator>
        binomial_heap(ForwardIterator first, 
                      ForwardIterator last,
                      value_compare const& comp = value_compare{})
          : compare_{comp}, top_{-1}, head_{-1}
        {
          reserve(std::distance(first, last));
          while(first != last) {
            push(*first);
            ++first;
          }
        }
      
      /**
       * @brief Initializer list constructor
       */
      binomial_heap(std::initializer_list<T> lst, 
                    value_compare const& comp = value_compare{})
        : compare_{comp}, top_{-1}, head_{-1}
      {
        reserve(lst.size());
        for(auto &x : lst) {
          push(x);
        }
      }
      //@}


      /** @name Properties */
      //@{
      bool empty() const
      {
        return elements_.size()==0;
      }
        
      size_type size() const
      {
        return elements_.size();
      }
      
      value_compare value_comp() const
      {
        return compare_;
      }
      
      allocator_type allocator() const
      {
        return elements_.allocator();
      }
      //@}

      /** @name Capacity */
      //@{
      size_type capacity() const
      {
        return elements_.capacity();
      }

      void reserve(size_type n)
      {
        reversemap_.reserve(n);
        elements_.reserve(n);
        forest_.reserve(n);
      }
      //@}
        
      /** @name Heap operations */
      //@{
      const_reference top() const
      {
        return get(top_);
      }
      
      void push(const value_type& d); 
        
      void pop();
      //@}

    private:
      // Return a "null" index.
      static constexpr size_type null()
      {
        return -1;
      }
    
      // Get the node at the given index in the forest vector. 
      binomial_heap_node& node(size_type n)
      {
        return forest_[n];
      }
      
      binomial_heap_node const& node(size_type n) const
      {
        return forest_[n];
      }
      
      // Return (a reference to) the parent index indicated by the given node.
      size_type& parent(size_type n)
      {
        return node(n).parent;
      }
      
      size_type parent(size_type n) const
      {
        return node(n).parent;
      }
      
      // Return (a reference to) the child index indicated by the given node.
      size_type& child(size_type n)
      {
        return node(n).child;
      }
      
      size_type child(size_type n) const
      {
        return node(n).child;
      }
      
      // Return (a reference to) the right subtree index of the indicated node.
      size_type& right(size_type n)
      {
        return node(n).right;
      }

      size_type right(size_type n) const
      {
        return node(n).right;
      }
      
      // Return the degree of the indicated node.
      size_type degree(size_type n) const
      {
        return node(n).degree;
      }
    
      // Return the element corresponding to the indicated node.
      const_reference get(size_type n) const
      {
        return elements_[node(n).index];
      }
      
      // Link the indicated nodes
      void link(size_type l, size_type r)
      {
        binomial_link(node(l), node(r), l, r);
      }
      
      // Compare the objects referenced by the indicatd nodes. Note that heap 
      // order is determined by !compare(x, y).
      bool compare(size_type m, size_type n) const
      {
        return compare_(get(m), get(n));
      }

      size_type find_top();       

      void forest_merge(size_type index);
      void heap_merge(size_type index);

    private:
      element_vector elements_;   // Stores the heap elements
      node_vector forest_;        // Stores tree nodes
      index_vector reversemap_;   // ???
        
      Comp compare_;

      size_type top_;     // The current top of the heap
      size_type head_;    // The head of the root list.
    };
  
  // Find and cache the new top element in the heap.
  template<typename T, typename Comp, typename Alloc>
    auto binomial_heap<T, Comp, Alloc>::find_top() -> size_type
    {
      // If the tree is empty, then there can be no top.
      if(empty())
        return null();

      // Find the subtree root that satisfies the heap property.
      size_type t = head_;      // current top
      size_type n = right(t);   // iterator
      while (n != null()) {
        if(!compare(n, t))
          t = n;
        n = right(n);
      }
      return t;
    }

  // Merge the root list of the indicated binomial tree with the maintained
  // root list. This is a standard sorted merge based on the degree of the
  // trees and completes in O(p + q) where p and q are the number of trees
  // in each root list. Note that both pa and q are O(lg n) where n is the
  // size() of the heap.
  template<typename T, typename Comp, typename Alloc>
    void binomial_heap<T, Comp, Alloc>::forest_merge(size_type index)
    {
      // Temporarily push a new heap node into the forest. This is used as a
      // placeholder for computing the new root list head.
      size_type current = forest_.size();
      forest_.push_back(binomial_heap_node());

      size_type p = head_;
      size_type q = index;

      // Perform the sorted merge.
      while(p != null() || q != null()) {
        if(q == null() || (p != null() && degree(p) < degree(q))) {
          right(current) = p;
          current = p;
          p = right(p);
        } else {
          right(current) = q ;
          current = q;
          q = right(q);
        }
      }
      
      // Get the new head
      head_ = forest_.back().right;
      forest_.pop_back();
    }

  // Merge the binomial tree rooted at the given index into the maintained
  // forest.
  template<typename T, typename Comp, typename Alloc>
    void binomial_heap<T, Comp, Alloc>::heap_merge(size_type index)
    {
      // Merge the root lists.
      forest_merge(index);
      if(right(head_) == null()) {
        return;
      }
        
      size_type x = head_;
      size_type next_x = right(x);
      size_type prev_x = null();
      
      // Iterate over the root list, trying to find the right place to insert
      // the new tree.
      while(next_x != null()) {
        if((degree(x) != degree(next_x)) 
           || (right(next_x) != null() && degree(right(next_x)) == degree(x))) 
        {
          prev_x = x;
          x = next_x;
        } else {
          if(!compare(x, next_x)) {
            right(x) = right(next_x);
            link(next_x, x);
          } else {
            if(prev_x == null()) {
              head_ = next_x;
            } else {
              right(prev_x) = next_x;
            }

            link(x, next_x);
            x = next_x;
          }
        }
        
        next_x = right(x);
      }
    }

  template<typename T, typename Comp, typename Alloc>
    void binomial_heap<T, Comp, Alloc>::push(const value_type& d)
    {
      binomial_heap_node obj;

      size_type last = elements_.size();
      elements_.push_back(d);
      forest_.emplace_back(last);
      
      reversemap_.push_back(last);

      if(head_ == null()) {
        // Initialize the new heap
        top_ = head_ = last;
      } else {
        // Merge the singleton binomial heap with the maintained forest heap
        // and maybe set the new top index.
        heap_merge(last);
        if(!compare(last, top_))
          top_ = last;
      }
    }
  
  template<typename T, typename Comp, typename Alloc>
    void binomial_heap<T, Comp, Alloc>::pop()
    {
      assert(( !empty() ));
    
      size_type new_head = null();
      size_type temp = top_;
  
      // Reverse the list
      // FIXME: What does this actually do?
      if(child(top_) != null()) {
        size_type tmp_head = child(top_);
        size_type tmp_sibling = null();
        while(tmp_head != null()) {
          tmp_sibling = right(tmp_head);
          parent(tmp_head) = null();
          right(tmp_head) = new_head;
          new_head = tmp_head;
          tmp_head = tmp_sibling;
        }
      }
      
      if(right(head_) == null()) {
        head_ = new_head;
      } else {
        if(head_ == top_) {
          head_ = right(head_);
        } else {
          size_type tmp = head_;
          while(right(tmp) != top_)
            tmp = right(tmp);
            
          right(tmp) = right(top_);
        }
        
        heap_merge(new_head);
      }
      
      // Copy the last element to location of old top element
      elements_[forest_[temp].index] = elements_.back();
      
      size_type index = reversemap_.back();
      reversemap_[forest_[temp].index] = index;

      // Point the index of the old element to correct location
      node(index).index = node(temp).index;
  
      // Invalidate the node.
      node(temp).reset();
  
      elements_.pop_back();
      reversemap_.pop_back();

      // Finally, compute the new top of the heap.
      top_ = find_top();
    }
  
#if 0

  /** 
   * The mutable binomial heap...
   *
   * @tparam T
   * @tparam Comp
   * @tparam Map
   * @tparam Alloc
   */
  template<typename T, 
           typename Comp = std::less<T>,
           typename Map = default_t,
           typename Alloc = std::allocator<T>> 
    class mutable_binomial_heap
    {
    private:
      typedef T value_type;
      typedef size_t size_type;
      typedef Comp value_compare;
      
      /* Random access container which holds the heap elements */
      std::vector<T, Alloc> elements_;
      std::vector<binomial_heap_node> forest_;
        
      value_compare compare_;
      std::unordered_map<T, size_type> id_;     // Index map
      
      size_type top_;   // Index of the root element
      size_type head_;  // Index of the top element
        
      /*
       * merge: Function to merge two binomial heaps
       * Input: 
       * size_type x : Index of the root element of the heap to be merged
       * Output:
       * Merges the heap pointed to by x in the main heap pointed to by head_
       * Return Value:
       * None       
       */
      void merge (size_type index);

      /*
       * mutable_binomial_heap_union: Unites the heap pointed to by head_ with 
       * the heap specified
       * Input: 
       * size_type index: Index of the root element of the heap to be united
       * Output:
       * United binomial heap
       * Return Value:
       * None       
       */
      void mutable_binomial_heap_union (size_type index);
          
      /*
       * get_new_top: Function to find the index of new root element
       * Input: 
       * None
       * Output:
       * Index of the new root
       * Return Value:
       * size_type: Index of the new root
       */
      size_type get_new_top(); 
        
    public:
      /**
       * @brief Default constructor
       */
      mutable_binomial_heap(value_compare const& comp = value_compare{}) 
        : compare_{comp}, top_{-1}, head_{-1} 
      { }
        
      /**
       * @brief Range constructor
       */
      template<typename Iter>
        mutable_binomial_heap(Iter first, 
                              Iter last,
                              value_compare const& comp = value_compare{}) 
          : compare_{comp}, top_{-1}, head_{-1}
        {
          while(first != last)
            push(*first++);
        }
        
      /**
       * @brief Initializer list constructor
       */
      mutable_binomial_heap(std::initializer_list<T> list,
                            value_compare const& comp = value_compare{}) 
        : compare_{comp}, top_{-1}, head_{-1}
      {
        for(auto const& x : list)
          push(x);
      }

      /** @name Properties */
      //@{
      bool empty() const
      {
        return elements_.size()==0;
      }
        
      size_type size() const
      {
        return elements_.size();
      }
        
      value_compare value_comp() const
      {
        return compare_;
      }
      //@}
        
      /** @name Capacity */
      //@{
      void reserve(size_type n)
      {
        elements_.reserve(n);
        forest_.reserve(n);
      }
        
      size_type capacity() const
      {
        return elements_.capacity();
      }
      //@}

      /** @name Heap operations */
      //@{
      const value_type& top() const
      {
        return elements_[forest_[top_].index];
      }

      void update(const value_type& d);
        
      void push(const value_type& d); 

      void pop();
      //@}
    };
  
  template <class T, class Comp, class Map, class Alloc>
    auto mutable_binomial_heap<T, Comp, Map, Alloc>::get_new_top() -> size_type
    {
      size_type top_index;
      size_type tmp;
      if (head_ == size_type(-1)) {
        return -1;
      }
      
      top_index = head_;
      tmp = forest_[top_index].right;
      
      while (tmp != size_type(-1)) {
        if (!compare_(elements_[forest_[tmp].index], elements_[forest_[top_index].index])) {
          top_index = tmp;
        }
        
        tmp = forest_[tmp].right;
      }
      
      return top_index;
    }
  
  
  template <class T, class Comp, class Map, class Alloc>
    void mutable_binomial_heap<T, Comp, Map, Alloc>::push(const value_type& d)
    {
      binomial_heap_node obj;
      size_type index;
    
      elements_.push_back(d);

      obj.index = elements_.size() - 1;
      obj.parent = size_type(-1);
      obj.right = size_type(-1);
      obj.child = size_type(-1);
      obj.degree = 0;
      
      forest_.push_back(obj);
      
      index = forest_.size() - 1;
      
      id_[d] = index;
      if (head_ == size_type(-1)) {
        /* New heap */
        head_ = index;
        top_ = head_;
      } else {
        /* Unite the 1 element heap with the existing heap */
        mutable_binomial_heap_union(index);
        if (!compare_(elements_[forest_[index].index], elements_[forest_[top_].index]))
            top_ = index;
      }
    }
  
  
  template <class T, class Comp, class Map, class Alloc>
    void mutable_binomial_heap<T, Comp, Map, Alloc>::update(const value_type& d)
    {
      size_type index = id_[d];
      size_type parent = forest_[index].parent;
      size_type temp;
      elements_[forest_[index].index] = d;
      
      while (parent != size_type(-1) && 
            !compare_(d, elements_[forest_[parent].index])) {
        elements_[forest_[index].index] = elements_[forest_[parent].index];
        elements_[forest_[parent].index] = d;

        id_[elements_[forest_[index].index]] = index;
        
        index = parent;
        parent = forest_[parent].parent;
      }

      id_[d] = index;
      
      if (!compare_(d, elements_[forest_[top_].index])) {
        top_ = index;
      }
    }
  
  
  // Mege the root lists of two subtrees
  template <class T, class Comp, class Map, class Alloc>
    void mutable_binomial_heap<T, Comp, Map, Alloc>::merge(size_type index)
    {
      size_type p = head_;
      size_type q = index;
      
      size_type current = forest_.size();
      size_type initial_size = -1;
      forest_.push_back(binomial_heap_node());
      
      while (p != initial_size || q != initial_size) {
        if (q == initial_size || 
              (p != initial_size && forest_[p].degree < forest_[q].degree)) {
          forest_[current].right = p;
          current = p;
          p = forest_[p].right;
        } else {
          forest_[current].right = q ;
          current = q;
          q = forest_[q].right;
        }
      }
      
      head_ = forest_.back().right;
      forest_.pop_back();
    }
  
  
  // Merge two subtrees in the heap.
  template <class T, class Comp, class Map, class Alloc>
    void mutable_binomial_heap<T, Comp, Map, Alloc>::mutable_binomial_heap_union(size_type index)
    {
      /* Merge the root lists */
      merge(index);
      if (forest_[head_].right == size_type(-1)) {
        return;
      }
      
      size_type x = head_;
      size_type next_x = forest_[head_].right;
      size_type prev_x = size_type(-1);
      
      while (next_x != size_type(-1)) {
        if ((forest_[x].degree != forest_[next_x].degree) 
              || (forest_[next_x].right != size_type(-1) 
                  && forest_[forest_[next_x].right].degree 
                  == forest_[x].degree)) {
            prev_x = x;
            x = next_x;
        } else {
          if (!compare_ (elements_[forest_[x].index], elements_[forest_[next_x].index])) {
            forest_[x].right = forest_[next_x].right;
            binomial_link(forest_[next_x], forest_[x], next_x, x);
          } else {
            if (prev_x == size_type (-1)) {
              head_ = next_x;
            } else {
              forest_[prev_x].right = next_x;
            }
            
            binomial_link(forest_[x], forest_[next_x], x, next_x);
            x = next_x;
          }
        }
        
        next_x = forest_[x].right;
      }
    }
  
  
  template <class T, class Comp, class Map, class Alloc>
    void mutable_binomial_heap<T, Comp, Map, Alloc>::pop()
    {
      size_type new_head = size_type(-1);
      
      if (head_ == size_type(-1)) {
        return;
      }
    
      size_type temp = top_;
  
      /* Reverse the list */
      if (forest_[top_].child != size_type(-1)) {
        size_type tmp_head = forest_[top_].child;
        size_type tmp_sibling;
        
        while (tmp_head != size_type(-1)) {
          tmp_sibling = forest_[tmp_head].right;
          forest_[tmp_head].parent = size_type(-1);
          forest_[tmp_head].right = new_head;
          new_head = tmp_head;
          tmp_head = tmp_sibling;
        }
      }
      
      if (forest_[head_].right == size_type(-1)) {
        head_ = new_head;
      } else {
        if (head_ == top_) {
          head_ = forest_[head_].right;
        } else {
          size_type tmp = head_;
          while (forest_[tmp].right != top_) {
            tmp = forest_[tmp].right;
          }
            
          forest_[tmp].right = forest_[top_].right;
        }
        
        mutable_binomial_heap_union(new_head);
      }
      
  
      // where in forest_ old last element is stored
      size_type index = id_[elements_[elements_.size()-1]];
  
      // copy the last element to location of old top element
      elements_[forest_[temp].index] = elements_[elements_.size()-1];
  
      //point the index of the old element to correct location
      forest_[index].index = forest_[temp].index;
  
      // Invalidating the entries of node
      forest_[temp].parent = -1;
      forest_[temp].child = -1;
      forest_[temp].right = -1;
      forest_[temp].degree = 0;
      forest_[temp].index = -1;
  
      elements_.pop_back();
  
      top_ = get_new_top();
    }
  
#endif  


} // namespace origin

#endif
