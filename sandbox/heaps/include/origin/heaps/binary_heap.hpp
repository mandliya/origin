// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_HEAPS_BINARY_HEAP_HPP
#define ORIGIN_HEAPS_BINARY_HEAP_HPP

#include <cassert>
#include <iosfwd>
#include <vector>
#include <algorithm>
#include <unordered_map>

#include <origin/utility/meta.hpp>

namespace origin
{
  /**
   * A mutable binary heap is a binary heap that allows the values in the to
   * be modified by an external program. The heap is updated to adjust the
   * location of the element within the data structure and preserve the heap
   * invariant.
   *
   * @tparam T      The value type
   * @tparam Comp   A Strict_weak_order on T
   * @tparam Map    A Mapping between T and an index
   * @tparam Alloc  An Allocator type
   */
  template<typename T, 
           typename Comp = std::less<T>, 
           typename Map = default_t, 
           typename Alloc = std::allocator<T>>
    class mutable_binary_heap
    {
    public:
      typedef T value_type;
      typedef Comp value_compare;   // FIXME: Should be called key compare?

      // FIXME: Should these be private?
      typedef std::vector<T> container_type;
      typedef std::unordered_map<value_type, size_t> map_type; //FIXME: Use traits class
      
      typedef Alloc allocator_type;
      typedef typename Alloc::size_type size_type;
    private:
      // Heapify after root is swapped with last element
      void heapify(size_type index);

      // Swap two elements in the heap by their indexes.
      void swap_elements(size_type index1, size_type index2);

      // Recursively print the heap as a tree
      // FIXME: Write this as an out-of body function.
      template<typename Char, typename Traits>
        void print_recur(size_type x, std::basic_ostream<Char, Traits>& os)
        {
          size_type sz = elements_.size();

          os << elements_[x];
          size_type i = 2 * x + 1;

          if (i < sz) {
            os << "(";
            print_recur (i, os);

            os << " ";
            i = i + 1;
            if (i < sz) {
              print_recur (i, os);
            }
            os << ")";
          }
        }

    public:
      /** @name Initialization */
      //@{
      /**
       * @brief Default constructor
       * Initialize an empty heap. The value compare function may be optionally 
       * given.
       *
       * @param comp  A value_compare function object
       */
      mutable_binary_heap(value_compare const& comp = value_compare{})
        : compare_{comp}
      { }

      // FIXME: Generalize this operation for input iterators.
      /**
       * @brief Range constructor
       * Initialize the heap with the elements in the range [first, last).
       * 
       * @tparam Iter   A Forward_Iterator whose reference type is convertible
       * the the value type of the heap.
       * 
       * @param first   An iterator referencing the first object in the range
       * @param last    An iterator past the end of the the range
       * @param comp    A value_compare function object
       */
      template<typename Iter>
        mutable_binary_heap(Iter first,
                            Iter last,
                            value_compare const& comp = value_compare{}) 
        : compare_{comp}
      {
        reserve(std::distance(first, last));
        while(first != last) {
          elements_.push_back(*first);
          index_[*first] = elements_.size() - 1;
          ++first;
        }
        size_type index = elements_.size() / 2;
        while (index > 0){
          index--;
          heapify(index);
        }
      }

      // FIXME: Implement me!
      /**
       * @brief Initializer list constructor
       * Initialize the heap with the elements from the given initializer list.
       * 
       * @param list  An initializer list of 
       * @param comp  A value_compare function object
       */
      mutable_binary_heap(std::initializer_list<T> list,
                          value_compare const& comp = value_compare{})
      { }
      //@}


      /** @name Properties */
      //@{
      /**
       * Return true if the heap has no elements.
       */
      bool empty() const
      {
        return elements_.empty();
      }

      /**
       * Return the number of elements in the heap.
       */
      size_type size() const
      {
        return elements_.size();
      }

      /**
       * Return the value comparison function.
       */
      value_compare value_comp() const
      {
        return compare_;
      }

      // FIXME: I think that this should probably go away.
      /**
       * Return a reference to the underlying container.
       */
      container_type const& data() const
      {
        return elements_;
      }
      //@}

      /** @name Capacity */
      //@{
      /**
       * Return the capacity allocated to the heap.
       */
      size_type capacity() const
      {
          return elements_.capacity();
      }

      /**
       * Reserve additional capacity for the heap.
       */
      void reserve(size_type n)
      {
          elements_.reserve(n);
      }
      //@}

      /** @name Heap operations */
      //@{
      /**
       * Return the element on the top of the heap.
       */
      value_type const& top() const
      {
          return elements_[0];
      }

      /**
       * Update the given key within the heap.
       */
      void update(const value_type& x);

      /**
       * Push the given element onto the heap.
       */
      void push(const value_type& x);

      /**
       * Pop the top element from the heap. This operation completes in
       * O(lg n) time.
       */
      void pop();
      //@}

      // FIXME: Go away?
      template<typename Char, typename Traits>
        void print(std::basic_ostream<Char, Traits>& os);

    private:
      // Return the results of comparing the mth element with the nth element.
      // Note that the comparison is inverted in order to define the correct
      // heap order.
      bool compare_elems(size_type m, size_type n)
      {
        return compare_(elements_[n], elements_[m]);
      }

    private:
      container_type elements_;
      value_compare compare_;
      map_type index_;
    };

  template<typename T, typename Comp, typename Map, typename Alloc>
    void
    mutable_binary_heap<T, Comp, Map, Alloc>::swap_elements(size_type m,
                                                            size_type n)
    {
      // Swap two elements in the heap structure
      std::swap(elements_[m], elements_[n]);

      // And update their external map values accordingly
      index_[elements_[m]] = m;
      index_[elements_[n]] = n;
    }

  template<typename T, typename Comp, typename Map, typename Alloc>
    void mutable_binary_heap<T, Comp, Map, Alloc>::push(value_type const& x)
    {
      // Push element into the heap structure
      elements_.push_back(x);
      size_type index = elements_.size() - 1;
      index_[x] = index;

      // Move the element up the heap until condition satisfied
      while(index > 0) {
        size_type parent = (index - 1) / 2;
        if(compare_elems(index, parent)) {
          swap_elements(index, parent);
          index = parent;
        } else {
          break;
        }
      }
    }

  template<typename T, typename Comp, typename Map, typename Alloc>
    void mutable_binary_heap<T, Comp, Map, Alloc>::heapify(size_type parent)
    {
      size_type total_size = elements_.size();
      size_type new_parent = parent;

      do {
        swap_elements(parent, new_parent);
        parent = new_parent;

        size_type left = 2 * parent + 1;
        size_type right = 2 * parent + 2;

        if((left < total_size) && compare_elems(left, parent))
          new_parent = left;
        if((right < total_size) && compare_elems(right, new_parent))
          new_parent = right;
      } while(parent != new_parent);
    }

  template<typename T, typename Comp, typename Map, typename Alloc>
    void mutable_binary_heap<T, Comp, Map, Alloc>::pop()
    {
      //swap root with last element and delete old root
      swap_elements(0, elements_.size() - 1);
      elements_.pop_back();

      // Preserve the heap invariant.
      if(!empty()) {
        heapify(0);
      }
    }

  template<typename T, typename Comp, typename Map, typename Alloc>
    void mutable_binary_heap<T, Comp, Map, Alloc>::update(value_type const& x)
    {
//       assert(( elements_[index_(x)] == x ));

      //update the element with the new value
      size_type index = index_[x];

      // FIXME: This seems wrong. The heap should always observe the
      // precondition above. The object at the index associated with the key
      // x must be equal to x. Re-inserting the value should be a no-op.
      elements_[index] = x;
      size_type parent = (index - 1) / 2;

      // After update, element may need to move up the heap, except for root element
      if ((index > 0) && (compare_elems(index, parent))) {
        while(index > 0){
          parent = (index - 1) / 2;
          if(compare_elems(index, parent)){
            swap_elements(index, parent);
            index = parent;
          } else {
            break;
          }
        }
      } else {
        //else, element may need to move down the heap
        heapify(index);
      }
    }

  template<typename T, typename Comp, typename Map, typename Alloc>
    template<typename Char, typename Traits>
      void 
      mutable_binary_heap<T, Comp, Map, Alloc>::
        print(std::basic_ostream<Char, Traits>& os)
      {
        if(elements_.size() > 0)
            print_recur(0, os);
      }


  // FIXME: Find a nice data structure reference for the documentation (CLR?).
  // FIXME: This is basically std::priority_queue. Why not reuse that
  // implementation?
  /**
   * The binary heap data structure implements a classic, generic binary heap.
   * The order (min or max) of the heap is determined by a compare operation on
   * the value type. This defaults to std::less, resulting in max-heap.
   *
   * @tparam T      Type of data to be stored in the heap
   * @tparam Comp   A Strict Weak Ordering over T
   */
  template<typename T, typename Comp = std::less<T>>
    class binary_heap
    {
      typedef std::vector<T> container_type;
    public:
      typedef T value_type;
      typedef Comp value_compare;
      typedef typename container_type::size_type size_type;

      /**
       * @brief Default constructor
       * Initialize an empty heap. A comparison function may optionally be given.
       *
       * @param comp  A value comparison Operation
       */
      binary_heap(value_compare const& comp = value_compare{})
        : elements_{}, compare_{comp}
      { }

      // FIXME: This needs to be specialized for input and forwrad iterators.
      /**
       * @brief Range constructor
       * Initialize the heap over the sequence of elements in the range
       * [first, last). A comparison function may be optional given.
       *
       *
       * @param first   The first iterator in a range of objects
       * @param last    An iterator past the end of a range of objects
       * @param comp    A Comp function object.
       */
      template<typename ForwardIterator>
        binary_heap(ForwardIterator first,
                    ForwardIterator last,
                    value_compare const& comp = value_compare{})
          : elements_{first, last}, compare_{comp}
        {
          std::make_heap(elements_.begin(), elements_.end(), compare_);
        }

      /**
       * @brief Initializer list constructor
       * Initialize the heap from a seuqence of objects. A comparison function
       * may be optionally given.
       *
       * @param list  An initializer list of objects of type T
       * @param comp  A Comp function object.
       */
      binary_heap(std::initializer_list<T> list, value_compare const& comp)
        : elements_{list.begin(), list.end()}, compare_{comp}
        {
          std::make_heap(elements_.begin(), elements_.end(), compare_);
        }

      /** @name Properties */
      //@{
      /**
       * Return true if the heap has no elements.
       */
      bool empty() const
      {
        return elements_.size() == 0;
      }

      /**
       * Return the number of elements in the heap.
       */
      size_type size() const
      {
        return elements_.size();
      }

      /**
       * Return the value comparison operation.
       */
      value_compare value_comp() const
      {
        return compare_;
      }
      //@}

      /** @name Capacity */
      //@{
      /**
       * Return the current capacity of the heap. This is the number of objects
       * for which memory has been allocated, but not initialized.
       */
      size_type capacity() const
      {
        return elements_.capacity();
      }

      // FIXME: What if n < size()? Do you truncate the heap? Is the heap still
      // valid after truncation? Should you never give up capacity once required?
      /**
       * Reserve additional memory for the heap.
       */
      void reserve(size_type n)
      {
        elements_.reserve(n);
      }
      //@}


      /** @name Operations */
      //@{
      /**
       * Return the object on the top of the heap.
       */
      const value_type& top() const
      {
          return elements_[0];
      }

      /**
       * Push an object onto the heap. This operation completes in O(lg n) time.
       */
      void push(value_type const& x)
      {
        elements_.push_back(x);
        std::push_heap(elements_.begin(), elements_.end(), compare_);
      }

      /**
       * Pop the top object off the heap. This operation completes in O(lg n)
       * time.
       */
      void pop()
      {
        std::pop_heap(elements_.begin(), elements_.end(), compare_);
        elements_.pop_back();
      }
      //@}

      // FIXME: Go away?
      /**
       * Write the contents of the heap to the given output stream.
       */
      template<typename Char, typename Traits>
        void print(std::basic_ostream<Char, Traits>& os);

    private:

      // FIXME: Go away?
      // Helper function for displaying the binary heap
      template<typename Char, typename Traits>
        void print_recur(size_type n, std::basic_ostream<Char, Traits>& os);

    private:
      // FIXME: Use EBO to remove the overhead of the value compare function.
      container_type elements_;
      value_compare compare_;
    };


  template<typename T, typename Comp>
    template<typename Char, typename Traits>
      void binary_heap<T, Comp>::print(std::basic_ostream<Char, Traits>& os)
      {
        if(!empty()) {
          print_recur(0, os);
        }
      }

  template<typename T, typename Comp>
    template<typename Char, typename Traits>
      void binary_heap<T, Comp>::print_recur(size_type n,
                                             std::basic_ostream<Char, Traits>& os)
      {
        os << elements_[n];
        size_type i = 2 * n + 1;

        size_type sz = elements_.size();
        if(i < sz) {
          os << "(";
          print_recur(i, os);

          os << " ";
          i = i + 1;
          if(i < sz) {
            print_recur(i, os);
          }
          os << ")";
        }
      }

} // namespace origin


#endif // ORIGIN_HEAPS_BINARY_HEAP_HPP
