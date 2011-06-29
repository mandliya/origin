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
#include <algorithm>

#include <origin/functional.hpp>
#include <origin/heaps/binomial_tree.hpp>

namespace origin 
{
  // FIXME: I shouldn't have to invert the order here. I should be able to
  // implenent the data strucure directly in terms of the base comparison.
  // For some reason changing the comparison in the link function gives
  // incorrect orders, and I can't figure out why. This works for now.
  /**
   * @internal
   * 
   * Compare two non-empty binomial nodes by the values at their roots.
   */
  template<typename T, typename Comp>
    struct binomial_tree_compare : inverse_compare<Comp>
    {
      typedef inverse_compare<Comp> base_comp;

      binomial_tree_compare(Comp c)
        : base_comp{c}
      { }

      bool operator()(T const& p, T const& q) const
      {
        return base_comp::operator()(p.root()->value, q.root()->value);
      }
    };

  /**
   * @internal
   * 
   * An extended comparison two binomial trees for empty trees. An empty tree
   * is treated as an extreme value under this comparison. For any tree t, and 
   * an empty tree e, comp(t, e) == true and comp(e, t) == false.
   */
  template<typename T, typename Comp>
    struct ext_binomial_tree_compare : binomial_tree_compare<T, Comp>
    {
      typedef binomial_tree_compare<T, Comp> base_comp;

      ext_binomial_tree_compare(Comp c)
        : base_comp{c}
      { }

      bool operator()(T const& p, T const& q) const
      {
        // Note that the first condition covers the case when both p and q
        // are nullptr. That's also false in order to preserve the strict weak
        // ordering.
        if(p.empty()) 
          return false;   // comp(nullptr, *) == false
        else if(q.empty()) 
          return true;    // comp(*, nullptr) == true
        else 
          return base_comp::operator()(p, q);
      }
    };

  // FIXME: Actually use the allocator
  /**
   * The binomial heap...
   * 
   * @tparam T      A Regular type; the value type
   * @tparam Comp   A Strict_weak_order on T
   * @tparam Alloc  An allocator of T
   */
  template<typename T, 
           typename Comp = std::less<T>, 
           typename Alloc = std::allocator<T>> 
  class binomial_heap {
    typedef binomial_tree<T, binomial_tree_node<T>, Alloc> tree_type;
    typedef typename tree_type::node_type node_type;
    typedef std::vector<tree_type> tree_list;
  public:
    typedef Alloc allocator_type;
    
    typedef T value_type;
    typedef typename Alloc::reference reference;
    typedef typename Alloc::const_reference const_reference;
    typedef typename Alloc::size_type size_type;
    
    typedef Comp value_compare;
  private:
    // We use two additional comparison operators. The root compare compares
    // two non-empty trees. The tree compare extends that ordering to empty
    // trees.
    typedef binomial_tree_compare<tree_type, value_compare> root_compare;
    typedef ext_binomial_tree_compare<tree_type, value_compare> tree_compare;
  public:

    /** @name Initialization */
    //@{
    /**
     * @brief Default constructor
     */
    binomial_heap(value_compare const& comp = value_compare{},
                  allocator_type const& alloc = allocator_type{})
      : trees_{alloc}, size_{0}, comp_{comp}
    { }

    /**
     * @brief Copy constructor
     */
    binomial_heap(binomial_heap const& x)
      : trees_{x.trees_}, size_{x.size_}, comp_{x.value_comp()}
    { }

    binomial_heap& operator=(binomial_heap const& x)
    {
      binomial_heap tmp{x};
      swap(x);
      return *this;
    }
    
    /**
     * @brief Move constructor
     */
    binomial_heap(binomial_heap&& x)
      : trees_{std::move(x.trees_)}, size_{x.size_}, comp_{x.comp_}
    {
      x.size_ = 0;  // x should be empty after moving
    }

    binomial_heap& operator=(binomial_heap&& x)
    {
      binomial_heap tmp{std::move(x)};
      swap(tmp);
      return *this;
    }
    //@}

    /** @name Properties */
    //@{
    /**
     * Return true if there are no elements in the heap.
     */
    bool empty() const
    {
      return size_ == 0;
    }

    /**
     * Return the number of elements in the heap.
     */
    size_type size() const
    {
      return size_;
    }
    
    /**
     * Return the function used to compare heap values.
     */
    value_compare value_comp() const
    {
      return comp_;
    }
    
    allocator_type get_allocator() const
    {
      return allocator_type{};
    }
    //@}

    /** @name Heap operations */
    //@{
    /**
     * Return the value at the top of the heap.
     */
    const_reference top() const
    {
      return search()->root()->value;
    }

    /**
     * Insert a new value into the heap.
     */
    void push(value_type const&);
    void push(value_type&&);
   
    template<typename... Args>
      void emplace(Args&&... x);
    
    /**
     * Remove the top element from the heap.
     */
    void pop();
    value_type displace();

    void merge(binomial_heap& x);
    //@}

    /** @name Object operations */
    //@{
    void clear()
    {
      trees_.clear();
    }
    
    void swap(binomial_heap& x)
    {
      std::swap(trees_, x.trees_);
      std::swap(size_, x.size_);
      std::swap(comp_, x.comp_);
    }
    //@}

  private:
    // Return the root node comparison function
    root_compare get_root_compare() const
    {
      return root_compare{value_comp()};
    }
    
    // Return the extended tree comparison function
    tree_compare get_tree_compare() const
    {
      return tree_compare{value_comp()};
    }
    
    // Search for the top element, returning an iterator.
    typename tree_list::iterator search()
    {
      assert(( !empty() ));
      return std::min_element(begin(trees_), end(trees_), get_tree_compare());
    }

    typename tree_list::const_iterator search() const
    {
      assert(( !empty() ));
      return std::min_element(begin(trees_), end(trees_), get_tree_compare());
    }

    void link(tree_type& x, tree_type& y);
    void merge(tree_list& x, tree_list& y);

  private:
    tree_list trees_;     // A vector of trees indexed by tree order
    size_type size_;      // Number of objects in the heap
    value_compare comp_;  // The heap-ordering function
  };

  // Link the trees l and r such that l becomes the new root and that r is 
  // empty after the operation.
  template<typename T, typename Comp, typename Alloc>
    void binomial_heap<T, Comp, Alloc>::link(tree_type& l, tree_type& r) 
    {
      using std::swap;

      assert(( !l.empty() && !r.empty() ));

      // FIXME: Why do I have to explicitly invert the comparison?
      if(get_root_compare()(r, l))
        swap(l, r);
      l.link(r);
    }
  
  // Merge the root lists of two different trees. The tree list r should
  // contain no trees after merging.
  template<typename T, typename Comp, typename Alloc>
    void binomial_heap<T, Comp, Alloc>::merge(tree_list& l, tree_list& r) 
    {
      // Results are computed out-of-place and will be moved into the l.
      tree_list result;

      // Simplify the computation by ensuring the two forest have the same
      // order.
      // FIXME: Don't resize existing heaps! It shouldn't be too hard write
      // without resizing.
      size_t n = std::max(l.size(), r.size());
      l.resize(n);
      r.resize(n);

      // Accumulate addends. Initially reserve all the capacity it's going to
      // use so we don't repeatedly allocate memory.
      tree_list tmp;
      tmp.reserve(4);

      tree_type carry;
      for(size_t i = 0; i < n; ++i) {
        // Create a list of all non-null addends by actually moving them from
        // their source trees.
        tmp.clear();
        if(!carry.empty())
          tmp.push_back(std::move(carry));
        if(!l[i].empty())
          tmp.push_back(std::move(l[i]));
        if(!r[i].empty())
          tmp.push_back(std::move(r[i]));
      
        // Add the trees in the temporary buffer, possibly carrying any left
        // over. There are four cases.
        switch(tmp.size()) {
          case 0:
            // With 0 addends, push an empty tree at order i
            result.push_back(tree_type{});
            break;
          case 1:
            // With a single addend, the result is that tree at order i
            result.push_back(std::move(tmp[0]));
            break;
          case 2:
            // With two addends, the result is a null tree and a we carry the
            // linked result
            result.push_back(tree_type{});
            link(tmp[0], tmp[1]);
            carry = std::move(tmp[0]);
            break;
          case 3:
            // With three addends, the result is (any) one, and we carry the
            // merged remaining trees.
            result.push_back(std::move(tmp[0]));
            link(tmp[1], tmp[2]);
            carry = std::move(tmp[1]);
            break;
        }
      }

      // Finally, add the carry to the result.
      if(!carry.empty())
        result.push_back(std::move(carry));

      // Reset the previous r (we've moved everything out) and set the
      // the l to the new result.
      r.clear();
      l = std::move(result);
    }

  template<typename T, typename C, typename A>
    void binomial_heap<T, C, A>::push(value_type const& value) 
    {
      // Create a singleton forest and merge it into the heap.
      tree_list tmp = {tree_type{value}};
      merge(trees_, tmp);
      ++size_;
    }

  template<typename T, typename C, typename A>
    void binomial_heap<T, C, A>::push(value_type&& value) 
    {
      // Create a singleton forest and merge it into the heap.
      tree_list tmp = {tree_type{std::move(value)}};
      merge(trees_, tmp);
      ++size_;
    }

  template<typename T, typename C, typename A>
    template<typename... Args>
      void binomial_heap<T, C, A>::emplace(Args&&... args) 
      {
        // FIXME: This is currently broken, pending a fix in binomial_tree.
        assert(( false ));
      }

  template<typename T, typename C, typename A>
    void binomial_heap<T, C, A>::merge(binomial_heap& x) 
    {
      merge(trees_, x.trees_);
      size_ += x.size_;
      x.size_ = 0;
    }

  template<typename T, typename C, typename A>
    void binomial_heap<T, C, A>::pop() 
    {
      // Find the top of the heap and explode the tree into a list of children.
      // Note that we have to reverse the list in order to have it in the
      // right order. Then, reset the tree at this level.
      auto iter = search();
      auto kids = iter->explode();
      std::reverse(begin(kids), end(kids));
      iter->clear();

      // FIXME: Note we aren't actually reducing memory usage here because
      // a) shrinking the vector won't actually release used memory, and b)
      // it's a relatively small amount of memory compared to the number of
      // objects that could otherwise be contained (i.e., 2^n). If the vector
      // is 32 elements wide (32 pointers), it could hold 4 billion objects.

      // Merge the children back into the heap and decrement the size.
      merge(trees_, kids);
      --size_;
    }


  // FIXME: Actually use the allocator
  /**
   * The mutable binomial heap...
   * 
   * @tparam T      A Regular type; the value type
   * @tparam Comp   A Strict_weak_order on T
   * @tparam Alloc  An allocator of T
   */
  template<typename T, 
           typename Comp = std::less<T>, 
           typename Alloc = std::allocator<T>> 
  class mutable_binomial_heap {
    typedef binomial_tree<T, binomial_tree_node<T>, Alloc> tree_type;
    typedef typename tree_type::node_type node_type;
    typedef std::vector<tree_type> tree_list;
  public:
    typedef Alloc allocator_type;
    
    typedef T value_type;
    typedef typename Alloc::reference reference;
    typedef typename Alloc::const_reference const_reference;
    typedef typename Alloc::size_type size_type;
    
    typedef Comp value_compare;
  private:
    // We use two additional comparison operators. The root compare compares
    // two non-empty trees. The tree compare extends that ordering to empty
    // trees.
    typedef binomial_tree_compare<tree_type, value_compare> root_compare;
    typedef ext_binomial_tree_compare<tree_type, value_compare> tree_compare;
  public:

    /** @name Initialization */
    //@{
    /**
     * @brief Default constructor
     */
    mutable_binomial_heap(value_compare const& comp = value_compare{},
                  allocator_type const& alloc = allocator_type{})
      : trees_{alloc}, size_{0}, comp_{comp}
    { }

    /**
     * @brief Copy constructor
     */
    mutable_binomial_heap(mutable_binomial_heap const& x)
      : trees_{x.trees_}, size_{x.size_}, comp_{x.value_comp()}
    { }

    mutable_binomial_heap& operator=(mutable_binomial_heap const& x)
    {
      mutable_binomial_heap tmp{x};
      swap(x);
      return *this;
    }
    
    /**
     * @brief Move constructor
     */
    mutable_binomial_heap(mutable_binomial_heap&& x)
      : trees_{std::move(x.trees_)}, size_{x.size_}, comp_{x.comp_}
    {
      x.size_ = 0;  // x should be empty after moving
    }

    mutable_binomial_heap& operator=(mutable_binomial_heap&& x)
    {
      mutable_binomial_heap tmp{std::move(x)};
      swap(tmp);
      return *this;
    }
    //@}

    /** @name Properties */
    //@{
    /**
     * Return true if there are no elements in the heap.
     */
    bool empty() const
    {
      return size_ == 0;
    }

    /**
     * Return the number of elements in the heap.
     */
    size_type size() const
    {
      return size_;
    }
    
    /**
     * Return the function used to compare heap values.
     */
    value_compare value_comp() const
    {
      return comp_;
    }
    
    allocator_type get_allocator() const
    {
      return allocator_type{};
    }
    //@}

    /** @name Heap operations */
    //@{
    /**
     * Return the value at the top of the heap.
     */
    const_reference top() const
    {
      return search()->root()->value;
    }

    /**
     * Insert a new value into the heap.
     */
    void push(value_type const&);
    void push(value_type&&);
   
    template<typename... Args>
      void emplace(Args&&... x);
    
    /**
     * Remove the top element from the heap.
     */
    void pop();
    value_type displace();

    void merge(mutable_binomial_heap& x);
    //@}

    /** @name Object operations */
    //@{
    void clear()
    {
      trees_.clear();
    }
    
    void swap(mutable_binomial_heap& x)
    {
      std::swap(trees_, x.trees_);
      std::swap(size_, x.size_);
      std::swap(comp_, x.comp_);
    }
    //@}

  private:
    // Return the root node comparison function
    root_compare get_root_compare() const
    {
      return root_compare{value_comp()};
    }
    
    // Return the extended tree comparison function
    tree_compare get_tree_compare() const
    {
      return tree_compare{value_comp()};
    }
    
    // Search for the top element, returning an iterator.
    typename tree_list::iterator search()
    {
      assert(( !empty() ));
      return std::min_element(begin(trees_), end(trees_), get_tree_compare());
    }

    typename tree_list::const_iterator search() const
    {
      assert(( !empty() ));
      return std::min_element(begin(trees_), end(trees_), get_tree_compare());
    }

    void link(tree_type& x, tree_type& y);
    void merge(tree_list& x, tree_list& y);

  private:
    tree_list trees_;     // A vector of trees indexed by tree order
    size_type size_;      // Number of objects in the heap
    value_compare comp_;  // The heap-ordering function
  };

  // Link the trees l and r such that l becomes the new root and that r is 
  // empty after the operation.
  template<typename T, typename Comp, typename Alloc>
    void mutable_binomial_heap<T, Comp, Alloc>::link(tree_type& l, tree_type& r) 
    {
      using std::swap;

      assert(( !l.empty() && !r.empty() ));

      // FIXME: Why do I have to explicitly invert the comparison?
      if(get_root_compare()(r, l))
        swap(l, r);
      l.link(r);
    }
  
  // Merge the root lists of two different trees. The tree list r should
  // contain no trees after merging.
  template<typename T, typename Comp, typename Alloc>
    void mutable_binomial_heap<T, Comp, Alloc>::merge(tree_list& l, tree_list& r) 
    {
      // Results are computed out-of-place and will be moved into the l.
      tree_list result;

      // Simplify the computation by ensuring the two forest have the same
      // order.
      // FIXME: Don't resize existing heaps! It shouldn't be too hard write
      // without resizing.
      size_t n = std::max(l.size(), r.size());
      l.resize(n);
      r.resize(n);

      // Accumulate addends. Initially reserve all the capacity it's going to
      // use so we don't repeatedly allocate memory.
      tree_list tmp;
      tmp.reserve(4);

      tree_type carry;
      for(size_t i = 0; i < n; ++i) {
        // Create a list of all non-null addends by actually moving them from
        // their source trees.
        tmp.clear();
        if(!carry.empty())
          tmp.push_back(std::move(carry));
        if(!l[i].empty())
          tmp.push_back(std::move(l[i]));
        if(!r[i].empty())
          tmp.push_back(std::move(r[i]));
      
        // Add the trees in the temporary buffer, possibly carrying any left
        // over. There are four cases.
        switch(tmp.size()) {
          case 0:
            // With 0 addends, push an empty tree at order i
            result.push_back(tree_type{});
            break;
          case 1:
            // With a single addend, the result is that tree at order i
            result.push_back(std::move(tmp[0]));
            break;
          case 2:
            // With two addends, the result is a null tree and a we carry the
            // linked result
            result.push_back(tree_type{});
            link(tmp[0], tmp[1]);
            carry = std::move(tmp[0]);
            break;
          case 3:
            // With three addends, the result is (any) one, and we carry the
            // merged remaining trees.
            result.push_back(std::move(tmp[0]));
            link(tmp[1], tmp[2]);
            carry = std::move(tmp[1]);
            break;
        }
      }

      // Finally, add the carry to the result.
      if(!carry.empty())
        result.push_back(std::move(carry));

      // Reset the previous r (we've moved everything out) and set the
      // the l to the new result.
      r.clear();
      l = std::move(result);
    }

  template<typename T, typename C, typename A>
    void mutable_binomial_heap<T, C, A>::push(value_type const& value) 
    {
      // Create a singleton forest and merge it into the heap.
      tree_list tmp = {tree_type{value}};
      merge(trees_, tmp);
      ++size_;
    }

  template<typename T, typename C, typename A>
    void mutable_binomial_heap<T, C, A>::push(value_type&& value) 
    {
      // Create a singleton forest and merge it into the heap.
      tree_list tmp = {tree_type{std::move(value)}};
      merge(trees_, tmp);
      ++size_;
    }

  template<typename T, typename C, typename A>
    template<typename... Args>
      void mutable_binomial_heap<T, C, A>::emplace(Args&&... args) 
      {
        // FIXME: This is currently broken, pending a fix in binomial_tree.
        assert(( false ));
      }

  template<typename T, typename C, typename A>
    void mutable_binomial_heap<T, C, A>::merge(mutable_binomial_heap& x) 
    {
      merge(trees_, x.trees_);
      size_ += x.size_;
      x.size_ = 0;
    }

  template<typename T, typename C, typename A>
    void mutable_binomial_heap<T, C, A>::pop() 
    {
      // Find the top of the heap and explode the tree into a list of children.
      // Note that we have to reverse the list in order to have it in the
      // right order. Then, reset the tree at this level.
      auto iter = search();
      auto kids = iter->explode();
      std::reverse(begin(kids), end(kids));
      iter->clear();

      // FIXME: Note we aren't actually reducing memory usage here because
      // a) shrinking the vector won't actually release used memory, and b)
      // it's a relatively small amount of memory compared to the number of
      // objects that could otherwise be contained (i.e., 2^n). If the vector
      // is 32 elements wide (32 pointers), it could hold 4 billion objects.

      // Merge the children back into the heap and decrement the size.
      merge(trees_, kids);
      --size_;
    }


} // namespace origin

#endif
