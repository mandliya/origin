// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_INSERTION_VECTOR_HPP
#define ORIGIN_INSERTION_VECTOR_HPP

#include <vector>

#include "linear_search.hpp"

namespace origin
{
  // Search tags
  struct linear_search_tag { };
  struct binary_search_tag { };

  // sort tags
  struct unstable_sort_tag { };
  struct stable_sort_tag { };

  /** 
   * @internal
   * The pair associative comparison function delegates to key comparison 
   * function, which is given as the Compare parameter. This comparator offers 
   * several overloads for "partial comparisons" the key component of the
   * of pair as well.
   */
  template<typename Pair, typename Compare>
  struct pair_assoc_compare
  {
    typedef typename Pair::first_type Key;
    typedef typename Pair::second_type Value;
  
    pair_assoc_compare()
      : comp()
    { }

    pair_assoc_compare(Compare comp)
      : comp(comp)
    { }
    
    bool operator()(Pair const& x, Pair const& y) const
    { return comp(x.first, y.first); }
    
    // Convenience overloads.
    bool operator()(Key const& x, Pair const& y) const
    { return comp(x, y.first); }
    
    bool operator()(Pair const& x, Key const& y) const
    { return comp(x.first, y); }
    
    Compare comp;
  };
  

  // FIXME: Write some kind of description of where you might want to use
  // this data structure as opposed to a binary heap or selection sort.
  
  /**
   * @internal
   * The insertion vector class implements a vector whose values are sorted
   * during insertion. This class provides the generic implementation for
   * insertion sets, maps, and queues. Unlike a standard vector, the insertion
   * vector does not support arbitrary insertions.
   *
   * @tparam Value      The value type contained by the sorted vector. This may
   *                    be a key value pair.
   * @tparam Compare    A function object that compares value types. This must
   *                    be a strict weak order.
   * @tparam Allocator  An allocator of Value types.
   * @tparam Unique     True if only unique values are allowed. False will
   *                    generate a multiset.
   * @tparam Search     True if using binaray search. False will cause the
   *                    data structure to search linearly.
   * @tparam Sort     True if preserving insertion order for equivalent keys.
   *                    False will result in a reversed insertion order for
   *                    multisets. This has no affect on sets.
   */
  template<typename Value,
           typename Compare,
           typename Allocator,
           typename Search,
           typename Sort>
  class insertion_vector
  {
    typedef insertion_vector<
      Value, Compare, Allocator, Search, Sort
    > this_type;
  public:
    typedef Value value_type;
    typedef Compare value_compare;
    typedef Allocator allocator_type;

    typedef typename allocator_type::reference reference;
    typedef typename allocator_type::const_reference const_reference;
    typedef typename allocator_type::pointer pointer;
    typedef typename allocator_type::const_pointer const_pointer;
    typedef typename allocator_type::size_type size_type;
    typedef typename allocator_type::difference_type difference_type;

  private:
    typedef std::vector<Value> vector_type;
  public:

    typedef typename vector_type::iterator iterator;
    typedef typename vector_type::const_iterator const_iterator;
    typedef typename vector_type::reverse_iterator reverse_iterator;
    typedef typename vector_type::const_reverse_iterator const_reverse_iterator;

    /** @name Initialization */
    //@{
    // Default constructor
    insertion_vector(value_compare const& comp = value_compare(),
                     allocator_type const& alloc = allocator_type())
      : _vec(alloc), _comp(comp)
    { }
    
    // Allocator constructor
    explicit insertion_vector(allocator_type const& alloc)
      : _vec(alloc)
    { }

    // Copy semantics (uses defaults)
    insertion_vector(insertion_vector const& x, allocator_type const& alloc)
      : _vec(x, alloc), _comp()
    { }
    
    // Range constructors
    // FIXME: Specialize on noexcept move
    template<typename Iter>
    insertion_vector(Iter first, Iter last,
                     value_compare const& comp = value_compare(),
                     allocator_type const& alloc = allocator_type())
      : _vec(first, last, alloc), _comp()
    { std::sort(_vec.begin(), _vec.end(), _comp); }
    
    template<typename Iter>
    void assign(Iter first, Iter last)
    { insertion_vector tmp(first, last); swap(tmp); }
    //@}

    /** @name Size and Capacity */
    //@{
    bool empty() const
    { return _vec.empty(); }

    size_type size() const
    { return _vec.size(); }

    size_type max_size() const
    { return _vec.max_size(); }

    size_type capacity() const
    { return _vec.capacity(); }

    void reserve(size_type n)
    { _vec.reserve(n); }

    void shrink_to_fit()
    { _vec.shrink_to_fit(); }
    //@}
    
    /** @name Properties */
    //@{
    allocator_type get_allocator() const
    { return _vec.get_allocator(); }
    
    value_compare value_comp() const
    { return _comp; }
    //@}

    /**
     * @name Search Operations
     *
     * @note The following are member function templtaes to facilitate the
     * difference between simple and pair associative structures.
     */
    //@{
    template<typename Key>
    iterator find(Key const& x)
    {
      std::pair<iterator, iterator> rng = do_equal_range(x, Search());
      return rng.first != rng.second ? rng.first : _vec.end();
    }

    template<typename Key>
    const_iterator find(Key const& x) const
    { return self()->find(x); }

    template<typename Key>
    iterator lower_bound(Key const& x)
    { return do_lower_bound(x, Search()); }

    template<typename Key>
    const_iterator lower_bound(Key const& x) const
    { return self()->lower_bound(x); }

    template<typename Key>
    iterator upper_bound(Key const& x)
    { return do_upper_bound(x, Search()); }

    template<typename Key>
    const_iterator upper_bound(Key const& x) const
    { return self()->upper_bound(x); }

    template<typename Key>
    std::pair<iterator, iterator> equal_range(Key const& x)
    { return do_equal_range(x, Search()); }

    template<typename Key>
    std::pair<const_iterator, const_iterator> equal_range(Key const& x) const
    { return self()->equal_range(x); }
    //@}

    /**
     * @name Mutators
     *
     * @note the insert_unique algorithms are provided because they encapsulate
     * an insertion call dispatched on stability parameter. It's easier to
     * provide that functionality here than to access it in the wrapper classes.
     *
     * @todo Implement range insertion
     */
    //@{
    // Mulit-key insert
    iterator insert_multi(value_type const& x)
    { return do_insert_multi(Sort(), x); }

    // Multi-key hint insert
    // FIXME: This is probably sub-optimal.
    iterator insert_multi(const_iterator hint, value_type const& x)
    { insert_multi(x); }
        
    // Unique insert
    std::pair<iterator, bool> insert_unique(value_type const& x);

    // Unique hint insert
    iterator insert_unique(const_iterator hint, value_type const& x);

    // Erase
    void erase(const_iterator pos)
    { _vec.erase(pos); }

    void erase(const_iterator first, const_iterator last)
    { _vec.erase(first, last); }

    // Erase Key
    template<typename Key>
    void erase(Key const& key)
    { erase(equal_range(key));  }
    
    // Clear
    void clear()
    { _vec.clear(); }    
    //@}

    /** @name Iterators */
    //@{
    // Forward iterators
    iterator begin()
    { return _vec.begin(); }

    iterator end()
    { return _vec.end(); }

    const_iterator begin() const
    { return _vec.begin(); }

    const_iterator end() const
    { return _vec.end(); }

    const_iterator cbegin() const
    { return _vec.begin(); }

    const_iterator cend() const
    { return _vec.end(); }
    
    // Reverse iterators
    reverse_iterator rbegin()
    { return _vec.rbegin(); }
    
    reverse_iterator rend()
    { return _vec.rend(); }
    
    const_reverse_iterator rbegin() const
    { return _vec.rbegin(); }
    
    const_reverse_iterator rend() const
    { return _vec.rend(); }
    
    const_reverse_iterator crbegin()
    { return _vec.crbegin(); }
    
    const_reverse_iterator crend()
    { return _vec.crbegin(); }
    //@}
    
    /** @name Special Functions 
     *
     * These functions essentially provide named methods for overload support.
     * Rather than directly implement the operators or algorithms in an overload
     * we provide named versions that might be detected at compile time.
     */
    //@{
    bool equal(insertion_vector const& x) const
    { return _vec == x._vec; }
    
    bool less(insertion_vector const& x) const
    { return _vec < x._vec; }
    
    void swap(insertion_vector& x)
    {
      using std::swap;
      swap(_vec, x._vec);
      swap(_comp, x._comp);
    }
    //}

  private:
    // Return this without a const
    this_type* self() const
    { return const_cast<this_type*>(this); }

    // NOTE: The following observers are non-const on purpose. The const
    // observers simply delegate to the non-const versions. This reduces the
    // number of duplicate implementations.
    
    // NOTE: For pair-associative containers, the Key parameter for the
    // following algorithms may be either value_type or key_type. Note that the
    // map_value_compare function provides overloads for both keys and pairs
    // that makes these instantiations feasible. Also note, that only the
    // insert algorithms will instantiate these functions over the value type.

    // Lower bound dispatch targets
    template<typename Key>
    iterator do_lower_bound(Key const& key, binary_search_tag)
    { return std::lower_bound(begin(), end(), key, _comp); }

    template<typename Key>
    iterator do_lower_bound(Key const& key, linear_search_tag)
    { return linear_lower_bound(begin(), end(), key, _comp); }

    // Upper bound dispatch targets
    template<typename Key>
    iterator do_upper_bound(Key const& key, binary_search_tag)
    { return std::upper_bound(begin(), end(), key, _comp); }

    template<typename Key>
    iterator do_upper_bound(Key const& key, linear_search_tag)
    { return linear_upper_bound(begin(), end(), key, _comp); }

    // Equal range dispatch targets
    template<typename Key>
    std::pair<iterator, iterator> do_equal_range(Key const& key, binary_search_tag)
    { return std::equal_range(begin(), end(), key, _comp); }

    template<typename Key>
    std::pair<iterator, iterator> do_equal_range(Key const& key, linear_search_tag)
    { return linear_equal_range(begin(), end(), key, _comp); }

    // Multi-key insert (copy) dispatch targets
    iterator do_insert_multi(unstable_sort_tag, value_type const& x)
    { return _vec.insert(lower_bound(x), x); }

    iterator do_insert_multi(stable_sort_tag, value_type const& x)
    { return _vec.insert(upper_bound(x), x); }
        
    // NOTE: The unique variants accept a range parameter that gives the
    // positions for insertion. These are established when initially 
    // if the set actually contains a unique value. The Range parameter is
    // a pair of iterators.

    // Unique insert (copy) dispatch
    template<typename Range>
    iterator do_insert_unique(unstable_sort_tag, Range rng, value_type const& x)
    { return _vec.insert(rng.first, x); }    

    template<typename Range>
    iterator do_insert_unique(stable_sort_tag, Range rng, value_type const& x)
    { return _vec.insert(rng.second, x); }    

  private:
    vector_type _vec;
    value_compare _comp;
  };

  // Insert Unique (copy)
  template<typename V, typename C, typename A, typename S, typename T>
  inline std::pair<typename insertion_vector<V, C, A, S, T>::iterator, bool>
  insertion_vector<V, C, A, S, T>::insert_unique(value_type const& x)
  {
    std::pair<iterator, iterator> rng = equal_range(x);
    if(rng.first == rng.second)
      return std::make_pair(do_insert_unique(T(), rng, x), true);
    else
      return std::make_pair(rng.first, false);
  }

  // Operators
  template<typename V, typename C, typename A, typename S, typename T>
  bool operator==(insertion_vector<V, C, A, S, T> const& x,
                  insertion_vector<V, C, A, S, T> const& y)
  { return x.equal(y); }
  
  template<typename V, typename C, typename A, typename S, typename T>
  bool operator!=(insertion_vector<V, C, A, S, T> const& x,
                  insertion_vector<V, C, A, S, T> const& y)
  { return !x.equal(y); }
  
  template<typename V, typename C, typename A, typename S, typename T>
  bool operator<(insertion_vector<V, C, A, S, T> const& x,
                 insertion_vector<V, C, A, S, T> const& y)
  { return x.less(y); }

  template<typename V, typename C, typename A, typename S, typename T>
  bool operator>(insertion_vector<V, C, A, S, T> const& x,
                 insertion_vector<V, C, A, S, T> const& y)
  { return y.less(x); }

  template<typename V, typename C, typename A, typename S, typename T>
  bool operator<=(insertion_vector<V, C, A, S, T> const& x,
                  insertion_vector<V, C, A, S, T> const& y)
  { return !y.less(x); }

  template<typename V, typename C, typename A, typename S, typename T>
  bool operator>=(insertion_vector<V, C, A, S, T> const& x,
                  insertion_vector<V, C, A, S, T> const& y)
  { return !x.less(y); }

  // Special functions
  template<typename V, typename C, typename A, typename S, typename T>
  void swap(insertion_vector<V, C, A, S, T>& x, 
            insertion_vector<V, C, A, S, T>& y)
  { return x.swap(y); }  
}

#endif
