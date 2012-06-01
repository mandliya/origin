// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_ORDINAL_MAP_HPP
#define ORIGIN_ORDINAL_MAP_HPP

#include <utility>

#include <origin/vector.hpp>
#include <origin/ordinal.hpp>
#include <origin/concepts.hpp>

namespace origin
{
  // FIXME: Define a variant that actually understands mapped value validity,
  // that is, which elements in the map are defined and which are not. This can 
  // be easily achieved using either an accompanying bitmap or by  making the 
  // elements optional (at greater cost of space and time). It should be 
  // called direct_map. The corresponding concept is Direct Map.
  //
  // The name "direct map" is intended to relate to the notion of direct-mapped
  // caches. The idea is that the key (or a part of it) contains the exact
  // location of the mapped value within the map. No search or transformation
  // is required for access.

  
  // If we consider maps to be functions (i.e., m[k] <=> f(x)), then 
  // Associative containers define partial functions. We can generalize that
  // notion to Associations and Partial Maps. More precisely an Association
  // is a Partial Map if, for any Map m and a key k, m.count(k) == 0. This is
  // our basic understanding of all associative containers.
  //
  // An Association is a Total Map if, for any Map m and all keys k in the
  // domain of m, m.count(k) > 0. Note that here, the domain is not simply 
  // all objects represented by the domain type, but rather the set of keys
  // over which a Map m is currently defined. Intuitively, a Total Map is one
  // where each key is implicitly mapped to some value. The value may or may
  // not have meaning, depending on the current state of the program, however.

  
  // FIXME: The ordinal_map is a Total, Direct, Unique Association.
  
  
  // NOTE: Total Maps don't have erase functions and their insert function is
  // more like a Multimap than a Map: it can't fail.

  // The ordinal map iterator provides an iterator type over the mappings in 
  // an ordinal map. Ordinal map iterators rae random access iterators.
  template <typename K, typename I>
    class ordinal_map_iterator
    {
    public:
      using value_type = std::pair<K, Value_type<I>>;
      using reference = std::pair<K, Iterator_reference<I>>;
      
      ordinal_map_iterator() = default;
      
      ordinal_map_iterator(std::size_t n, I i) 
        : pos{n}, iter{i}
      { }
      
      // Enable iterator interoperability
      template <typename J>
        ordinal_map_iterator(ordinal_map_iterator<K, J> i, 
                             Requires<Convertible<J, I>()>* = {}) 
          : pos{i.pos}, iter{i.iter}
        { }
      
      // Base iterator
      I base() const { return iter; }

      // Readable
      reference operator*() const;
      reference operator[](Difference_type<I> n) const;

      // Increment
      ordinal_map_iterator& operator++();
      ordinal_map_iterator operator++(int);
      
      // Decrement
      ordinal_map_iterator& operator--();
      ordinal_map_iterator operator--(int);

      // Advance
      ordinal_map_iterator& operator+=(Difference_type<I> n);
      ordinal_map_iterator& operator-=(Difference_type<I> n);
      
    private:
      std::size_t pos;
      I iter;
    };

  template <typename K, typename I>
    inline auto
    ordinal_map_iterator<K, I>::operator*() const -> reference
    { 
      return {ordinal_cast<K>(pos), *iter}; 
    }

  template <typename K, typename I>
    inline auto
    ordinal_map_iterator<K, I>::operator[](Difference_type<I> n) const -> reference
    { 
      return *(*this + n);
    }

  template <typename K, typename I>
    inline ordinal_map_iterator<K, I>&
    ordinal_map_iterator<K, I>::operator++()
    {
      ++pos;
      ++iter;
      return *this;
    }

  template <typename K, typename I>
    inline ordinal_map_iterator<K, I>&
    ordinal_map_iterator<K, I>::operator--()
    {
      --pos;
      --iter;
      return *this;
    }

  template <typename K, typename I>
    inline ordinal_map_iterator<K, I>&
    ordinal_map_iterator<K, I>::operator+=(Difference_type<I> n)
    {
      pos += n;
      iter += n;
      return *this;
    }

  template <typename K, typename I>
    inline ordinal_map_iterator<K, I>&
    ordinal_map_iterator<K, I>::operator-=(Difference_type<I> n)
    {
      pos -= n;
      iter -= n;
      return *this;
    }

  template <typename K, typename I>
    inline ordinal_map_iterator<K, I>
    operator+(ordinal_map_iterator<K, I> i, Difference_type<I> n)
    {
      return i += n;
    }

  template <typename K, typename I>
    inline ordinal_map_iterator<K, I>
    operator+(Difference_type<I> n, ordinal_map_iterator<K, I> i)
    {
      return i += n;
    }

  template <typename K, typename I>
    inline ordinal_map_iterator<K, I>
    operator-(ordinal_map_iterator<K, I> i, Difference_type<I> n)
    {
      return i -= n;
    }

  template <typename K, typename I>
    inline Difference_type<I>
    operator-(const ordinal_map_iterator<K, I>& a, const ordinal_map_iterator<K, I>& b)
    {
      return a - b;
    }

  // Equality comparable
  template <typename K, typename I>
    inline bool
    operator==(ordinal_map_iterator<K, I> const& a, const ordinal_map_iterator<K, I>& b)
    {
      return a.base() == b.base();
    }

  template <typename K, typename I>
    inline bool
    operator!=(ordinal_map_iterator<K, I> const& a, const ordinal_map_iterator<K, I>& b)
    {
      return a.base() != b.base();
    }

      
    // Totally ordered
  template <typename K, typename I>
    inline bool
    operator<(ordinal_map_iterator<K, I> const& a, const ordinal_map_iterator<K, I>& b)
    {
      return a.base() < b.base();
    }

  template <typename K, typename I>
    inline bool
    operator>(ordinal_map_iterator<K, I> const& a, const ordinal_map_iterator<K, I>& b)
    {
      return a.base() > b.base();
    }

  template <typename K, typename I>
    inline bool
    operator<=(ordinal_map_iterator<K, I> const& a, const ordinal_map_iterator<K, I>& b)
    {
      return a.base() <= b.base();
    }

  template <typename K, typename I>
    inline bool
    operator>=(ordinal_map_iterator<K, I> const& a, ordinal_map_iterator<K, I>& b)
    {
      return a.base() >= b.base();
    }


  // Ordinal map
  //
  // The ordinal map class is a container adaptor. It adapts a random access
  // container (e.g., a vector) into a direct, unique map. 
  //
  // The key parameter of the ordinal map must be an ordianl type; these values
  // are not stored in the map. Unlike a traditional map, these values are not
  // stored by the data structure. Key values are used to index into the
  // underlying random access data structure by their ordinal values.
  //
  // FIXME: Add real allocator support.
  //
  // FIXME: Cleanup and documentation
  template <typename K, typename T, typename Cont = vector<T>>
    class ordinal_map
    {
    public:
      using value_type  = std::pair<K, T>;

      using iterator       = ordinal_map_iterator<K, Iterator_type<Cont>>;
      using const_iterator = ordinal_map_iterator<K, Iterator_type<Cont const>>;

      // Default constructor
      ordinal_map() : data{} { }
      
      // Range initialization
      //
      // FIXME: Optimize for forward iterators?
      template <typename I>
        ordinal_map(I first, I last)
          : data{}
        {
          while (first != last) {
            (*this)[(*first).first] = (*first).second;
            ++first;
          }
        }

      ordinal_map(std::initializer_list<value_type> l)
        : ordinal_map(l.begin(), l.end())
      { }
      
      // Fill initialization
      //
      // FIXME: Really necessary?
      explicit ordinal_map(std::size_t n, const T& x = {})
        : data(n, x)
      { }


      // Obsrevers
      bool empty() const { return data.empty(); }

      std::size_t size() const { return data.size(); }
      
      std::size_t capacity() const { return data.capacity(); }

      // Resize the map to contain the specified number of mappings. If n
      // is greater than the current size, new mappings  will be added and
      // the mapped values initialized to x. If n is less than the current 
      // size, all mappings with keys greater than or equal to n will be 
      // destroyed.
      void resize(std::size_t n, const T& value = {}) { data.resize(n, value); }

      void reserve(std::size_t n) { data.reserve(n); }


      // Return a reference to the mapped value indicated by the given key.
      // If no such key exists, the map is resized in order to accomodate
      // the new value. This may result in the creation of ord(k) - size() 
      // new mappings.
      T& operator[](const K& k)
      {
        maybe_resize(ord(k));
        return get(k);
      }

      // Return a reference to the mapped value indicated by the given key.
      // Note that the key must be in the map.
      const T& operator[](const K& k) const
      { 
        assert(find(k) != end());
        return get(k); 
      }

      // Insert the given key/value pair into the map, returning an iterator to
      // the given position.
      iterator insert(const value_type& x)
      {
        maybe_resize(ord(x.first));
        get(x.first) = x.second;
        return begin() + ord(x.first);
      }


      // Clear and swap
      void clear() { data.clear(); }
      void swap(ordinal_map& x) { data.swap(x); }      

      
      // Return an iterator to the object indicated by the given key or an
      // iterator past the end if x is outside the bounds of the map.
      iterator find(const K& k)
      {
        return valid(k) ? begin() + ord(k) : end();
      }

      const_iterator find(const K& k) const
      {
        return valid(k) ? begin() + ord(k) : end();
      }
      
 
      // Return the number of objects associated with the given k. If the key
      // is in bounds, this is 1. If not, 0.
      std::size_t count(const K& k) const
      {
        return valid(k) ? 1 : 0;
      }

      // Iterators
      iterator begin() { return {0, data.begin()}; }
      iterator end()   { return {size(), data.end()}; }
      
      const_iterator begin() const { return {0, data.begin()}; }
      const_iterator end() const   { return {size(), data.end()}; }

    private:
      // Return true if the given key is valid (i.e., in bounds).
      bool valid(const K& k) const { return !empty() && ord(k) < size(); }

      // If the key is larger than the current size, allocate more mappings.
      // Note that the vector won't do this unless we push back. We're usually
      // just doing random-access insertion.
      void maybe_resize(std::size_t n)
      {
        if(n >= size()) {
          resize(n + 1);
        }
      }
      
      // Return the object at the mapped key.
      T&       get(const K& k)       { return data[ord(k)]; }
      const T& get(const K& k) const { return data[ord(k)]; }
      
    private:
      Cont data;
    };

} // namespace origin

#endif

