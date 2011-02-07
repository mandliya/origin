// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_INSERTION_MAP_HPP
#define ORIGIN_INSERTION_MAP_HPP

#include <stdexcept>
#include <functional>
#include <algorithm>
#include <iterator>
#include <memory>

#include "insertion_vector.hpp"

namespace origin
{
  // NOTE: In other map types, the value type is pair<Key const, Value>, which
  // prevents the programmer from changing the key values when a reference to
  // the value type is returned (e.g., by an iterator). This is not easily
  // done when value types are stored in a vector because the objects *move* 
  // when the underlying vector is resized. This necessitates a non-const key.
  //
  // This property can be worked around by internally storing keys as non-const
  // but converting them to const-keyed types wherever a user can get a 
  // reference to the value type. This requires that we customize the iterators
  // for the data type.
  //
  // FIXME: To make this type correct, the value type must have a constany key
  // component.

  /**
   * The insertion_map implements a sorted, simple, pair-associative container
   * implemented using an insertion-sorted vector.
   */
  template<typename Key,
           typename Value,
           typename Compare = std::less<Key>,
           typename Allocator = std::allocator<std::pair<Key, Value> > >
  class insertion_map
  {
  public:
    typedef Key key_type;
    typedef Value mapped_type; 
    typedef std::pair<Key, Value> value_type;
    typedef Compare key_compare;
    typedef pair_assoc_compare<value_type, key_compare> value_compare;
    typedef Allocator allocator_type;
  private:
    typedef insertion_vector<
      value_type, value_compare, allocator_type,
      binary_search_tag,      // Use binary search
      stable_sort_tag         // Use stable sorting
    > base_type;
  public:
    
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
    // Default constructor
    insertion_map(key_compare const& comp = key_compare(),
                  allocator_type const& alloc = allocator_type())
      : _base(comp, alloc)
    { }
    
    // Allocator constructor
    explicit insertion_map(allocator_type const& alloc)
      : _base(alloc)
    { }

    // Copy semantics (uses defaults)
    insertion_map(insertion_map const& x, allocator_type const& alloc)
      : _base(x, alloc)
    { }
    
    // Range initialization
    template<typename Iter>
    insertion_map(Iter first, Iter last,
                  key_compare const& comp = key_compare(),
                  allocator_type const& alloc = allocator_type())
      : _base(first, last, comp, alloc)
    { }
    
    template<typename Iter>
    void assign(Iter first, Iter last)
    { insertion_map tmp(first, last); swap(tmp); }
    //@}
    
    /** @name Properties */
    //@{
    allocator_type get_allocator() const
    { return _base.get_allocator(); }
    
    key_compare key_comp() const
    { return value_comp().comp; }
    
    value_compare value_comp() const
    { return _base.value_comp(); }
    //@}

    /** @name Size and Capacity */
    //@{
    bool empty() const
    { return _base.empty(); }
    
    size_type size() const
    { return _base.size(); }
    
    // FIXME: Make this a constexpr function?
    size_type max_size() const
    { return _base.max_size(); }
    
    size_type capacity() const
    { return _base.capacity();  }
    
    void reserve(size_type n)
    { _base.reserve(n); }
    
    void shrink_to_fit()
    { _base.shrink_to_fit(); }
    //@}
    
    /** @name Operations */
    //@{
    iterator find(key_type const& x)
    { return _base.find(x); }
    
    const_iterator find(key_type const& x) const
    { return _base.find(x); }
    
    size_type count(key_type const& x) const
    {
      std::pair<const_iterator, const_iterator> rng = equal_range(x);
      return std::distance(rng.first, rng.second);
    }
    
    iterator lower_bound(key_type const& x)
    { return _base.lower_bound(x); }
    
    const_iterator lower_bound(key_type const& x) const
    { return _base.lower_bound(x); }
    
    iterator upper_bound(key_type const& x) const
    { return _base.upper_bound(x); }
    
    // FIXME: use origin::range?
    std::pair<iterator, iterator> equal_range(key_type const& x)
    { return _base.equal_range(x); }
    
    std::pair<const_iterator, const_iterator> equal_range(key_type const& x) const
    { return _base.equal_range(x); }
    
    mapped_type& operator[](key_type const& x);
    
    mapped_type& at(key_type const& x);
    mapped_type const& at(key_type const& x) const;
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
    // Iterators
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
    { return _base.begin(); }
    
    const_reverse_iterator rend() const
    { return _base.rend(); }
    
    const_reverse_iterator crbegin()
    { return _base.rbegin(); }
    
    const_reverse_iterator crend()
    { return _base.rend(); }
    //@}
    
    /** Special Functions */
    //@{
    bool equal(insertion_map const& x) const
    { return _base == x._base; }
    
    bool less(insertion_map const& x) const
    { return _base < x._base; }
    
    void swap(insertion_map& x)
    { _base.swap(x); }
    //@}

  private:
    base_type _base;
  };
  
  template<typename K, typename V, typename C, typename A>
  typename insertion_map<K, V, C, A>::mapped_type&
  insertion_map<K, V, C, A>::operator[](key_type const& x)
  {
    iterator iter = find(x);
    if(iter == end) {
      iter = insert(std::make_pair(x, mapped_type()));
    }
    return iter->second;
  }

  template<typename K, typename V, typename C, typename A>
  typename insertion_map<K, V, C, A>::mapped_type&
  insertion_map<K, V, C, A>::at(key_type const& x)
  {
    iterator iter = find(x);
    if(iter == end())
      throw std::out_of_range("key not in map");
    return iter->second;
  }

  template<typename K, typename V, typename C, typename A>
  typename insertion_map<K, V, C, A>::mapped_type const&
  insertion_map<K, V, C, A>::at(key_type const& x) const
  {
    iterator iter = find(x);
    if(iter == end())
      throw std::out_of_range("key not in map");
    return iter->second;
  }
  
  // Operators
  template<typename K, typename V, typename C, typename A>
  bool operator==(insertion_map<K, V, C, A> const& x,
                  insertion_map<K, V, C, A> const& y)
  { return x.equal(y); }
  
  template<typename K, typename V, typename C, typename A>
  bool operator!=(insertion_map<K, V, C, A> const& x,
                  insertion_map<K, V, C, A> const& y)
  { return !x.equal(y); }

  template<typename K, typename V, typename C, typename A>
  bool operator<(insertion_map<K, V, C, A> const& x,
                 insertion_map<K, V, C, A> const& y)
  { return x.less(y); }

  template<typename K, typename V, typename C, typename A>
  bool operator>(insertion_map<K, V, C, A> const& x,
                 insertion_map<K, V, C, A> const& y)
  { return y.less(x); }

  template<typename K, typename V, typename C, typename A>
  bool operator<=(insertion_map<K, V, C, A> const& x,
                  insertion_map<K, V, C, A> const& y)
  { return !y.less(x); }

  template<typename K, typename V, typename C, typename A>
  bool operator>=(insertion_map<K, V, C, A> const& x,
                  insertion_map<K, V, C, A> const& y)
  { return !x.less(y); }

  // Special functions
  template<typename K, typename V, typename C, typename A>
  void swap(insertion_map<K, V, C, A>& x, insertion_map<K, V, C, A>& y)
  { x.swap(y); }


  /**
   * The insertion multimap implements a sorted, pair-associative container
   * that is implemented using an insertion-sorted vector.
   */
  template<typename Key,
           typename Value,
           typename Compare = std::less<Key>,
           typename Allocator = std::allocator<std::pair<Key, Value> > >
  class insertion_multimap
  {
  public:
    typedef Key key_type;
    typedef Value mapped_type; 
    typedef std::pair<Key, Value> value_type;
    typedef Compare key_compare;
    typedef pair_assoc_compare<value_type, key_compare> value_compare;
    typedef Allocator allocator_type;
  private:
    typedef insertion_vector<
      value_type, value_compare, allocator_type,
      binary_search_tag,      // Use binary search
      stable_sort_tag         // Use stable sorting
    > base_type;
  public:
    
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
    // Default constructor
    insertion_multimap(key_compare const& comp = key_compare(),
                  allocator_type const& alloc = allocator_type())
      : _base(comp, alloc)
    { }
    
    // Allocator constructor
    explicit insertion_multimap(allocator_type const& alloc)
      : _base(alloc)
    { }

    // Copy semantics (uses defaults)
    insertion_multimap(insertion_multimap const& x, allocator_type const& alloc)
      : _base(x, alloc)
    { }

    // Range constructors
    template<typename Iter>
    insertion_multimap(Iter first, Iter last,
                  key_compare const& comp = key_compare(),
                  allocator_type const& alloc = allocator_type())
      : _base(first, last, comp, alloc)
    { }
    
    template<typename Iter>
    void assign(Iter first, Iter last)
    { insertion_multimap tmp(first, last); swap(tmp); }
    //@}
    
    /** @name Properties */
    //@{
    allocator_type get_allocator() const
    { return _base.get_allocator(); }
    
    key_compare key_comp() const
    { return value_comp().comp; }
    
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
    
    // FIXME: Use better default initialization syntax.
    void reserve(size_type n)
    { _base.reserve(n); }
    
    void shrink_to_fit()
    { _base.shrink_to_fit(); }
    //@}
    
    /** @name Operations */
    //@{
    iterator find(key_type const& x)
    { return _base.find(x); }
    
    const_iterator find(key_type const& x) const
    { return _base.find(x); }
    
    size_type count(key_type const& x) const
    {
      std::pair<iterator, iterator> rng = equal_range(x);
      return std::distance(rng.first, rng.second);
    }
    
    iterator lower_bound(key_type const& x)
    { return _base.lower_bound(x); }
    
    const_iterator lower_bound(key_type const& x) const
    { return _base.lower_bound(x); }
    
    iterator upper_bound(key_type const& x) const
    { return _base.upper_bound(x); }
    
    // FIXME: use origin::range?
    std::pair<iterator, iterator> equal_range(key_type const& x)
    { return _base.equal_range(x); }
    
    std::pair<const_iterator, const_iterator> equal_range(key_type const& x) const
    { return _base.equal_range(x); }
    
    mapped_type& operator[](key_type const& x);
    
    mapped_type& at(key_type const& x);
    mapped_type const& at(key_type const& x) const;
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
    // Iterators
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
    { return _base.begin(); }
    
    const_reverse_iterator rend() const
    { return _base.rend(); }
    
    const_reverse_iterator crbegin()
    { return _base.rbegin(); }
    
    const_reverse_iterator crend()
    { return _base.rend(); }
    //@}
    
    /** Special Functions */
    //@{
    bool equal(insertion_multimap const& x) const
    { return _base.equal(x._base); }
    
    bool less(insertion_multimap const& x) const
    { return _base.less(x._base); }
    
    void swap(insertion_multimap& x)
    { _base.swap(x); }
    //@}

  private:
    base_type _base;
  };
  
  template<typename K, typename V, typename C, typename A>
  typename insertion_multimap<K, V, C, A>::mapped_type&
  insertion_multimap<K, V, C, A>::operator[](key_type const& x)
  {
    iterator iter = find(x);
    if(iter == end) {
      iter = insert(std::make_pair(x, mapped_type()));
    }
    return iter->second;
  }

  template<typename K, typename V, typename C, typename A>
  typename insertion_multimap<K, V, C, A>::mapped_type&
  insertion_multimap<K, V, C, A>::at(key_type const& x)
  {
    iterator iter = find(x);
    if(iter == end())
      throw std::out_of_range("key not in map");
    return iter->second;
  }

  template<typename K, typename V, typename C, typename A>
  typename insertion_multimap<K, V, C, A>::mapped_type const&
  insertion_multimap<K, V, C, A>::at(key_type const& x) const
  {
    const_iterator iter = find(x);
    if(iter == end())
      throw std::out_of_range("key not in map");
    return iter->second;
  }
  
  // Operators
  template<typename K, typename V, typename C, typename A>
  bool operator==(insertion_multimap<K, V, C, A> const& x,
                  insertion_multimap<K, V, C, A> const& y)
  { return x.equal(y); }
  
  template<typename K, typename V, typename C, typename A>
  bool operator!=(insertion_multimap<K, V, C, A> const& x,
                  insertion_multimap<K, V, C, A> const& y)
  { return !x.equal(y); }

  template<typename K, typename V, typename C, typename A>
  bool operator<(insertion_multimap<K, V, C, A> const& x,
                 insertion_multimap<K, V, C, A> const& y)
  { return x.less(y); }

  template<typename K, typename V, typename C, typename A>
  bool operator>(insertion_multimap<K, V, C, A> const& x,
                 insertion_multimap<K, V, C, A> const& y)
  { return y.less(x); }

  template<typename K, typename V, typename C, typename A>
  bool operator<=(insertion_multimap<K, V, C, A> const& x,
                  insertion_multimap<K, V, C, A> const& y)
  { return !y.less(x); }

  template<typename K, typename V, typename C, typename A>
  bool operator>=(insertion_multimap<K, V, C, A> const& x,
                  insertion_multimap<K, V, C, A> const& y)
  { return !x.less(y); }

  // Special functions
  template<typename K, typename V, typename C, typename A>
  void swap(insertion_multimap<K, V, C, A>& x, insertion_multimap<K, V, C, A>& y)
  { x.swap(y); }

} // namespace origin

#endif
