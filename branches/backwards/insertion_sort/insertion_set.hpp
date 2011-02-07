// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_INSERTION_SET_HPP
#define ORIGIN_INSERTION_SET_HPP

#include "insertion_vector.hpp"

namespace origin
{
  // NOTE: The insertion_set and multiset only vary in the implementations
  // of TWO functions (the insert overloads). Is there really no better way
  // to build wrapper mostly similar wrapper types?
  //
  // I could privately inherit insertion_vector, but would prbably need to
  // wrap the calls to the obserers (since the base class functions are
  // templates.

  template<typename Value,
           typename Compare = std::less<Value>,
           typename Allocator = std::allocator<Value> >
  class insertion_set
  {
    typedef insertion_vector<
      Value, Compare, Allocator,
      binary_search_tag,      // Use binary search
      stable_sort_tag         // Use stable sorting
    > base_type;
  public:
    typedef Value value_type;
    typedef Value key_type;
    typedef Compare value_compare;
    typedef Compare key_compare;
    typedef Allocator allocator_type;
    
    typedef typename base_type::reference reference;
    typedef typename base_type::const_reference const_reference;
    typedef typename base_type::pointer pointer;
    typedef typename base_type::const_pointer const_pointer;

    typedef typename base_type::iterator iterator;
    typedef typename base_type::const_iterator const_iterator;
    typedef typename base_type::reverse_iterator reverse_iterator;
    typedef typename base_type::const_reverse_iterator const_reverse_iterator;

    typedef typename base_type::difference_type difference_type;
    typedef typename base_type::size_type size_type;

    /** @name Initialization */
    //@{
    // Default initialization
    insertion_set(value_compare const& comp = value_compare(),
                  allocator_type const& alloc = allocator_type())
      : _base(comp, alloc)
    { }
    
    // Allocator initialization
    explicit insertion_set(allocator_type const& alloc)
      : _base(alloc)
    { }

    // Copy initialization (with defaults)
    insertion_set(insertion_set const& x, allocator_type const& alloc)
      : _base(x, alloc)
    { }
    
    // Range initialization
    template<typename Iter>
    insertion_set(Iter first, Iter last,
                  value_compare const& comp = value_compare(),
                  allocator_type const& alloc = allocator_type())
      : _base(first, last, comp, alloc)
    { }

    template<typename Iter>
    void assign(Iter first, Iter last)
    { insertion_set tmp(first, last); swap(tmp); }
    //@}
    
    /** @name Properties */
    //@{
    allocator_type get_allocator() const
    { return _base.get_allocator(); }
    
    key_compare key_comp() const
    { return value_comp(); }
    
    value_compare value_comp() const
    { return _base.value_comp(); }
    //@}

    /** @name Size and Capacity */
    //@{
    bool empty() const
    { return _base.empty(); }
    
    size_type size() const
    { return _base.size(); }
    
    // FIXME: Make this a constexpr function.
    size_type max_size() const
    { return _base.max_size(); }
    
    size_type capacity() const
    { return _base.capacity();  }

    void reserve(size_type n)
    { _base.reserve(n); }
    
    void shrink_to_fit()
    { _base.shrink_to_fit(); }
    //@}
    
    /** @name Observers */
    //@{
    iterator find(value_type const& x)
    { return _base.find(x); }
    
    const_iterator find(value_type const& x) const
    { return _base.find(x); }
    
    iterator lower_bound(value_type const& x)
    { return _base.lower_bound(x); }
    
    const_iterator lower_bound(value_type const& x) const
    { return _base.lower_bound(x); }
    
    iterator upper_bound(value_type const& x) const
    { return _base.upper_bound(x); }
    
    std::pair<iterator, iterator> equal_range(value_type const& x)
    { return _base.equal_range(x); }
    
    std::pair<const_iterator, const_iterator> equal_range(value_type const& x) const
    { return _base.equal_range(x); }
    //@}

    /** @name Mutators */
    //@{
    // Insert
    std::pair<iterator, bool> insert(value_type const& x)
    { return _base.insert_unique(x); }

    // Insert (with hint)
    iterator insert(const_iterator hint, value_type const& x)
    { return _base.insert_unique(hint, x); }

    // Erase
    void erase(const_iterator pos)
    { _base.erase(pos); }

    void erase(const_iterator first, const_iterator last)
    { _base.erase(first, last); }

    void erase(value_type const& x)
    { _base.erase(x); }
    //@}
    
    /** @name Iterators */
    //@{
    // Forward iterators
    iterator begin()
    { return _base.begin(); }
    
    iterator end()
    { return _base.end(); }
    
    const_iterator begin() const
    { return _base.begin(); }
    
    const_iterator end() const
    { return _base.end(); }
    
    const_iterator cbegin()
    { return _base.begin(); }
    
    const_iterator cend()
    { return _base.end(); }
    
    // Reverse iterators
    reverse_iterator rbegin()
    { return _base.rbegin(); }
    
    reverse_iterator rend()
    { return _base.rend(); }
    
    const_reverse_iterator rbegin() const
    { return _base.rbegin(); }
    
    const_reverse_iterator rend() const
    { return _base.rend(); }
    
    const_reverse_iterator crbegin()
    { return _base.crbegin(); }
    
    const_reverse_iterator crend()
    { return _base.crend(); }
    //@}
    
    /** @name Special Functions */
    //@{
    bool equal(insertion_set const& x) const
    { return _base.equal(x._base); }
    
    bool less(insertion_set const& x) const
    { return _base.less(x._base); }
    
    void swap(insertion_set& x)
    { _base.swap(x); }
    //@}

  private:
    base_type _base;
  };
  
  template<typename V, typename C, typename A>
  bool operator==(insertion_set<V, C, A> const& x, insertion_set<V, C, A> const& y)
  { return x.equal(y); }
  
  template<typename V, typename C, typename A>
  bool operator!=(insertion_set<V, C, A> const& x, insertion_set<V, C, A> const& y)
  { return !x.equal(y); }
  
  template<typename V, typename C, typename A>
  bool operator<(insertion_set<V, C, A> const& x, insertion_set<V, C, A> const& y)
  { return x.less(y); }

  template<typename V, typename C, typename A>
  bool operator>(insertion_set<V, C, A> const& x, insertion_set<V, C, A> const& y)
  { return y.less(x); }

  template<typename V, typename C, typename A>
  bool operator<=(insertion_set<V, C, A> const& x, insertion_set<V, C, A> const& y)
  { return !y.less(x); }

  template<typename V, typename C, typename A>
  bool operator>=(insertion_set<V, C, A> const& x, insertion_set<V, C, A> const& y)
  { return !x.less(y); }
  
  template<typename V, typename C, typename A>
  void swap(insertion_set<V, C, A>& x, insertion_set<V, C, A>& y)
  { x.swap(y); }

  template<typename Value,
           typename Compare = std::less<Value>,
           typename Allocator = std::allocator<Value> >
  class insertion_multiset
  {
    typedef insertion_vector<
      Value, Compare, Allocator,
      binary_search_tag,      // Use binary search
      stable_sort_tag         // Use stable sorting
    > base_type;
  public:
    typedef Value value_type;
    typedef Value key_type;
    typedef Compare value_compare;
    typedef Compare key_compare;
    typedef Allocator allocator_type;
    
    typedef typename base_type::reference reference;
    typedef typename base_type::const_reference const_reference;
    typedef typename base_type::pointer pointer;
    typedef typename base_type::const_pointer const_pointer;

    typedef typename base_type::iterator iterator;
    typedef typename base_type::const_iterator const_iterator;
    typedef typename base_type::reverse_iterator reverse_iterator;
    typedef typename base_type::const_reverse_iterator const_reverse_iterator;

    typedef typename base_type::difference_type difference_type;
    typedef typename base_type::size_type size_type;

    /** @name Initialization */
    //@{
    // Default initialization
    insertion_multiset(value_compare const& comp = value_compare(),
                       allocator_type const& alloc = allocator_type())
      : _base(comp, alloc)
    { }
    
    // Allocator initialization
    explicit insertion_multiset(allocator_type const& alloc)
      : _base(alloc)
    { }

    // Copy initialization (with defaults)
    insertion_multiset(insertion_multiset const& x, allocator_type const& alloc)
      : _base(x, alloc)
    { }
    
    // Range initialization
    template<typename Iter>
    insertion_multiset(Iter first, Iter last,
                       value_compare const& comp = value_compare(),
                       allocator_type const& alloc = allocator_type())
      : _base(first, last, comp, alloc)
    { }

    template<typename Iter>
    void assign(Iter first, Iter last)
    { insertion_multiset tmp(first, last); swap(tmp); }
    //@}
    
    /** @name Properties */
    //@{
    allocator_type get_allocator() const
    { return _base.get_allocator(); }
    
    key_compare key_comp() const
    { return value_comp(); }
    
    value_compare value_comp() const
    { return _base.value_comp(); }
    //@}

    /** @name Size and Capacity */
    //@{
    bool empty() const
    { return _base.empty(); }
    
    size_type size() const
    { return _base.size(); }
    
    // FIXME: Make this a constexpr function.
    size_type max_size() const
    { return _base.max_size(); }
    
    size_type capacity() const
    { return _base.capacity();  }

    void reserve(size_type n)
    { _base.reserve(n); }
    
    void shrink_to_fit()
    { _base.shrink_to_fit(); }
    //@}
    
    /** @name Observers */
    //@{
    iterator find(value_type const& x)
    { return _base.find(x); }
    
    const_iterator find(value_type const& x) const
    { return _base.find(x); }
    
    iterator lower_bound(value_type const& x)
    { return _base.lower_bound(x); }
    
    const_iterator lower_bound(value_type const& x) const
    { return _base.lower_bound(x); }
    
    iterator upper_bound(value_type const& x) const
    { return _base.upper_bound(x); }
    
    std::pair<iterator, iterator> equal_range(value_type const& x)
    { return _base.equal_range(x); }
    
    std::pair<const_iterator, const_iterator> equal_range(value_type const& x) const
    { return _base.equal_range(x); }
    //@}

    /** @name Mutators */
    //@{
    // Insert
    std::pair<iterator, bool> insert(value_type const& x)
    { return _base.insert_multi(x); }

    // Insert (with hint)
    iterator insert(const_iterator hint, value_type const& x)
    { return _base.insert_multi(hint, x); }

    // Erase
    void erase(const_iterator pos)
    { _base.erase(pos); }

    void erase(const_iterator first, const_iterator last)
    { _base.erase(first, last); }

    void erase(value_type const& x)
    { _base.erase(x); }
    //@}
    
    /** @name Iterators */
    //@{
    // Forward iterators
    iterator begin()
    { return _base.begin(); }
    
    iterator end()
    { return _base.end(); }
    
    const_iterator begin() const
    { return _base.begin(); }
    
    const_iterator end() const
    { return _base.end(); }
    
    const_iterator cbegin()
    { return _base.begin(); }
    
    const_iterator cend()
    { return _base.end(); }
    
    // Reverse iterators
    reverse_iterator rbegin()
    { return _base.rbegin(); }
    
    reverse_iterator rend()
    { return _base.rend(); }
    
    const_reverse_iterator rbegin() const
    { return _base.rbegin(); }
    
    const_reverse_iterator rend() const
    { return _base.rend(); }
    
    const_reverse_iterator crbegin()
    { return _base.crbegin(); }
    
    const_reverse_iterator crend()
    { return _base.crend(); }
    //@}

  private:
    base_type _base;
  };

  template<typename V, typename C, typename A>
  bool operator==(insertion_multiset<V, C, A> const& x, 
                  insertion_multiset<V, C, A> const& y)
  { return x.equal(y); }
  
  template<typename V, typename C, typename A>
  bool operator!=(insertion_multiset<V, C, A> const& x, 
                  insertion_multiset<V, C, A> const& y)
  { return !x.equal(y); }
  
  template<typename V, typename C, typename A>
  bool operator<(insertion_multiset<V, C, A> const& x, 
                 insertion_multiset<V, C, A> const& y)
  { return x.less(y); }

  template<typename V, typename C, typename A>
  bool operator>(insertion_multiset<V, C, A> const& x, 
                 insertion_multiset<V, C, A> const& y)
  { return y.less(x); }

  template<typename V, typename C, typename A>
  bool operator<=(insertion_multiset<V, C, A> const& x, 
                  insertion_multiset<V, C, A> const& y)
  { return !y.less(x); }

  template<typename V, typename C, typename A>
  bool operator>=(insertion_multiset<V, C, A> const& x, 
                  insertion_multiset<V, C, A> const& y)
  { return !x.less(y); }
  
  template<typename V, typename C, typename A>
  void swap(insertion_multiset<V, C, A>& x, insertion_multiset<V, C, A>& y)
  { x.swap(y); }

} // namespace origin

#endif
