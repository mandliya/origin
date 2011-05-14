// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_HEAPS_INDEX_HPP
#define ORIGIN_HEAPS_INDEX_HPP

#include <cassert>
#include <vector>
#include <unordered_map>

namespace origin
{
  template<typename Key, 
           typename Hash = std::hash<Key>, 
           typename Eq = std::equal_to<Key>, 
           typename Alloc = std::allocator<Key>>
    class hash_index
    {
    public:
      typedef Key key_type;
      typedef std::size_t mapped_type;
      typedef Hash hasher;
      typedef Eq key_equal;

      // Rebind the hash index over the given allocator.
      template<typename A>
        struct rebind
        {
          typedef hash_index<Key, Hash, Eq, A> type;
        };
      
      /**
       * Return true if the map has the given key.
       */
      bool has(key_type const& x) const
      {
        return map_.find(x) != map_.end();
      }
      
      /**
       * Return the index associated with the given key.
       */
      mapped_type get(key_type const& k) const
      {
        assert(( has(k) ));
        return map_.find(k)->second;
      }
      
      /**
       * Associate the the given key with the specified value.
       */
      void set(key_type const& k, mapped_type v)
      {
        // assert(( !has(k) ));
        map_.insert({k, v});
      }
      
    private:
      std::unordered_map<Key, mapped_type, Hash, Eq, Alloc> map_;
    };

  template<typename Key, 
           typename Alloc = std::allocator<Key>>
    class vector_index
    {
    public:
      typedef Key key_type;
      typedef std::size_t mapped_type;

      // Rebind the index over the given allocator.
      template<typename A>
        struct rebind
        {
          typedef hash_index<Key, A> type;
        };
      
      /**
       * Return true if the map has the given key. This is true if the key
       * is a valid index of the vector.
       */
      bool has(key_type const& x) const
      {
        return !map.empty() && x < map_.size();
      }
      
      /**
       * Return the index associated with the given key.
       */
      mapped_type get(key_type const& k) const
      {
        assert(( has(k) ));
        return map_[k];
      }
      
      /**
       * Associate the the given key with the specified value. The vector is
       * resized if the key is out of bounds.
       */
      void set(key_type const& k, mapped_type v)
      {
        if(k >= map_.size()) {
          map_.resize(2 * k, -1u);
        }
        map_[k] = v;
      }
      
    private:
      std::vector<mapped_type> map_;
    };

} // namespace origin

#endif

